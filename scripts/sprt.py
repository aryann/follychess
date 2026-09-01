#!/usr/bin/env python3
"""Runs an SPRT self-play match to test whether an engine change gains Elo.

The match is played between the engine in the current working tree (the
"candidate") and the engine at a baseline git revision (the "baseline") using
fastchess. An SPRT (sequential probability ratio test) stops the match as
soon as the result is statistically conclusive: H1 accepted means the
candidate is stronger (or, for --mode simplify, not weaker); H0 accepted
means it is not.

Games are played with a fixed node budget per move, which is deterministic
and machine-independent. Openings come from an opening book so that game
pairs differ; without one, the deterministic engine would repeat the same
game forever.

Baseline binaries are cached per commit under testing/bins, so each baseline
is only ever built once. Games, logs, and the summary of each run are
archived under testing/results.

If the FOLLYTEST_API_URL and FOLLYTEST_API_KEY environment variables are
set, runs are also uploaded to FollyTest (see web/) in the background as
they progress. Uploads are best-effort: failures never interrupt a match,
and --backfill re-uploads a finished run from its results directory.
"""

import argparse
import datetime
import json
import os
import pathlib
import re
import shutil
import subprocess
import sys
import tempfile
import threading
import time
import urllib.request

# SPRT Elo bounds (elo0, elo1) by mode.
BOUNDS = {
    "gain": (0, 5),
    "simplify": (-5, 0),
}

BOOK_FORMATS = {
    ".epd": "epd",
    ".pgn": "pgn",
}

# Patterns for the statistics block that fastchess prints after each game.
ELO_PATTERN = re.compile(r"^Elo: ([-\d.]+) \+/- ([-\d.]+)")
GAMES_PATTERN = re.compile(
    r"^Games: (\d+), Wins: (\d+), Losses: (\d+), Draws: (\d+)")
LLR_PATTERN = re.compile(r"^LLR: ([-\d.]+) \(.*\) \(([-\d.]+), ([-\d.]+)\)")

PGN_GAME_START = re.compile(r"^(?=\[Event )", re.MULTILINE)
PGN_HEADER = re.compile(r"^\[(\w+) \"(.*)\"\]", re.MULTILINE)
PGN_RESULTS = ("1-0", "0-1", "1/2-1/2", "*")
# The reason fastchess appends to the final move's comment, e.g.
# "{-8.74/5 0.066s, Black wins by adjudication}".
PGN_REASON = re.compile(r", ([^{}]+)\}\s*(?:1-0|0-1|1/2-1/2|\*)\s*$")


def find_repo_root() -> pathlib.Path:
    # When invoked via `bazel run`, the original working directory is only
    # available through this environment variable.
    if workspace := os.environ.get("BUILD_WORKSPACE_DIRECTORY"):
        return pathlib.Path(workspace)

    for parent in pathlib.Path(__file__).resolve().parents:
        if (parent / "MODULE.bazel").exists():
            return parent

    raise RuntimeError("Could not find the repository root.")


def run(args: list[str], **kwargs) -> subprocess.CompletedProcess:
    return subprocess.run(args, check=True, **kwargs)


def capture(args: list[str]) -> str:
    return run(args, capture_output=True, text=True).stdout.strip()


def build_baseline(commit: str, output: pathlib.Path) -> None:
    worktree = tempfile.mkdtemp(prefix="follychess-baseline-")
    run(["git", "worktree", "add", "--quiet", "--detach", worktree, commit])
    try:
        run(["bazel", "build", "-c", "opt", "//cli:follychess"], cwd=worktree)
        output.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(pathlib.Path(worktree) / "bazel-bin/cli/follychess",
                     output)
    finally:
        run(["git", "worktree", "remove", "--force", worktree])


def parse_pgn_games(path: pathlib.Path) -> list[dict]:
    """Parses a fastchess PGN file into FollyTest game records.

    The file is append-only, so a game's position in the file is a stable
    identifier. The last game is skipped if it is still being written.
    """
    if not path.is_file():
        return []

    games = []
    chunks = [c for c in PGN_GAME_START.split(path.read_text()) if c.strip()]
    for index, chunk in enumerate(chunks, start=1):
        if not chunk.rstrip().endswith(PGN_RESULTS):
            continue  # Partially written.

        headers = dict(PGN_HEADER.findall(chunk))
        reason = PGN_REASON.search(chunk)
        games.append({
            "n": index,
            "white": headers.get("White", ""),
            "black": headers.get("Black", ""),
            "result": headers.get("Result", "*"),
            "termination": (reason.group(1) if reason else
                            headers.get("Termination", "")),
            "pgn": chunk.strip() + "\n",
        })
    return games


class StatsTracker:
    """Extracts the latest match statistics from fastchess output lines."""

    def __init__(self):
        self.stats = {}
        self.llr = None
        self.llr_bounds = None

    def consume(self, line: str) -> bool:
        """Returns True when the line completes a statistics block."""
        line = line.strip()
        if match := ELO_PATTERN.match(line):
            self.stats["elo"] = float(match.group(1))
            self.stats["elo_error"] = float(match.group(2))
        elif match := GAMES_PATTERN.match(line):
            self.stats["games"] = int(match.group(1))
            self.stats["wins"] = int(match.group(2))
            self.stats["losses"] = int(match.group(3))
            self.stats["draws"] = int(match.group(4))
        elif match := LLR_PATTERN.match(line):
            # The LLR line is the last line of the statistics block.
            self.llr = float(match.group(1))
            self.llr_bounds = (float(match.group(2)), float(match.group(3)))
            self.stats["llr"] = self.llr
            return True
        return False

    def final_status(self) -> str:
        if self.llr is None or self.llr_bounds is None:
            return "inconclusive"
        lower, upper = self.llr_bounds
        if self.llr >= upper:
            return "passed"
        if self.llr <= lower:
            return "failed"
        return "inconclusive"


class Uploader:
    """Best-effort background uploader to the FollyTest API.

    Every few seconds the PGN file is re-parsed and new games are posted
    along with the latest statistics. Failures are logged and retried on the
    next cycle; they never interrupt the match.
    """

    INTERVAL_SECONDS = 5

    def __init__(self, api_url: str, api_key: str):
        self.api_url = api_url.rstrip("/")
        self.api_key = api_key
        self.run_id = None
        self.pgn_path = None
        self.uploaded = 0
        self.stats = None
        self.sent_stats = None
        self.lock = threading.Lock()
        self.stop_event = threading.Event()
        self.thread = None

    def _request(self, method: str, path: str, payload: dict) -> dict | None:
        request = urllib.request.Request(
            f"{self.api_url}{path}",
            data=json.dumps(payload).encode(),
            headers={"Content-Type": "application/json",
                     "X-Api-Key": self.api_key},
            method=method)
        try:
            with urllib.request.urlopen(request, timeout=10) as response:
                return json.load(response)
        except Exception as error:  # Best-effort by design.
            print(f"follytest: upload failed: {error}", file=sys.stderr)
            return None

    def create_run(self, metadata: dict) -> bool:
        response = self._request("POST", "/runs", metadata)
        if response:
            self.run_id = response["id"]
            print(f"follytest: recording run {self.run_id}")
        return self.run_id is not None

    def start(self, pgn_path: pathlib.Path) -> None:
        self.pgn_path = pgn_path
        self.thread = threading.Thread(target=self._loop, daemon=True)
        self.thread.start()

    def _loop(self) -> None:
        while not self.stop_event.wait(self.INTERVAL_SECONDS):
            self.sync()

    def set_stats(self, stats: dict) -> None:
        with self.lock:
            self.stats = dict(stats)

    def sync(self) -> None:
        if not self.run_id:
            return

        games = parse_pgn_games(self.pgn_path)
        new_games = games[self.uploaded:]
        with self.lock:
            stats = self.stats
        if not new_games and stats == self.sent_stats:
            return

        payload = {"games": new_games, "stats": stats}
        if self._request("POST", f"/runs/{self.run_id}/batch", payload):
            self.uploaded += len(new_games)
            self.sent_stats = stats

    def finish(self, status: str) -> None:
        self.stop_event.set()
        if self.thread:
            self.thread.join()
        self.sync()
        if self.run_id:
            self._request("PATCH", f"/runs/{self.run_id}", {"status": status})
            print(f"follytest: run {self.run_id} recorded as {status}")


def make_uploader() -> Uploader | None:
    api_url = os.environ.get("FOLLYTEST_API_URL")
    api_key = os.environ.get("FOLLYTEST_API_KEY")
    if not api_url or not api_key:
        return None
    return Uploader(api_url, api_key)


def run_match(args: list[str], summary_path: pathlib.Path,
              tracker: StatsTracker, uploader: Uploader | None) -> int:
    with (open(summary_path, "w") as out,
          subprocess.Popen(args, stdout=subprocess.PIPE, text=True) as proc):
        for line in proc.stdout:
            sys.stdout.write(line)
            out.write(line)
            if tracker.consume(line) and uploader:
                uploader.set_stats(tracker.stats)
    return proc.returncode


def backfill(run_dir: pathlib.Path, uploader: Uploader | None) -> int:
    if uploader is None:
        print("Set FOLLYTEST_API_URL and FOLLYTEST_API_KEY to backfill.",
              file=sys.stderr)
        return 1

    games = parse_pgn_games(run_dir / "games.pgn")
    if not games:
        print(f"No finished games found in {run_dir}.", file=sys.stderr)
        return 1

    tracker = StatsTracker()
    summary = run_dir / "summary.txt"
    if summary.is_file():
        for line in summary.read_text().splitlines():
            tracker.consume(line)

    # Reconstruct what we can from the directory name:
    # {timestamp}-{mode}-vs-{short_commit}.
    name_match = re.match(r"\d{8}-\d{6}-(\w+)-vs-(\w+)", run_dir.name)
    mode = name_match.group(1) if name_match else "gain"
    baseline = name_match.group(2) if name_match else ""
    elo0, elo1 = BOUNDS.get(mode, BOUNDS["gain"])

    metadata = {
        "mode": mode,
        "elo0": elo0,
        "elo1": elo1,
        "nodes": 0,
        "book": "",
        "candidate_commit": "",
        "candidate_dirty": False,
        "baseline_ref": baseline,
        "baseline_commit": baseline,
        "note": f"Backfilled from {run_dir.name}.",
    }
    if not uploader.create_run(metadata):
        return 1

    uploader.pgn_path = run_dir / "games.pgn"
    uploader.set_stats(tracker.stats)
    uploader.sync()
    uploader.finish(tracker.final_status())
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument(
        "--baseline", default="main",
        help="Git revision to test against (default: %(default)s).")
    parser.add_argument(
        "--mode", choices=BOUNDS, default="gain",
        help="gain: the change should gain strength; SPRT bounds [0, 5] Elo. "
             "simplify: the change should not lose strength; SPRT bounds "
             "[-5, 0] Elo. (default: %(default)s)")
    parser.add_argument(
        "--nodes", type=int, default=30_000,
        help="Node budget per move (default: %(default)s).")
    parser.add_argument(
        "--max-rounds", type=int, default=500,
        help="Maximum opening pairs to play before giving up on a conclusive "
             "result; two games per round (default: %(default)s).")
    parser.add_argument(
        "--book", type=pathlib.Path,
        default=pathlib.Path("testing/books/UHO_4060_v4.epd"),
        help="Opening book, .epd or .pgn (default: %(default)s).")
    parser.add_argument(
        "--concurrency", type=int, default=max(1, (os.cpu_count() or 3) - 2),
        help="Concurrent games (default: number of CPUs minus two).")
    parser.add_argument(
        "--note", default="",
        help="Description of the change being tested, shown in FollyTest.")
    parser.add_argument(
        "--backfill", type=pathlib.Path, metavar="RUN_DIR",
        help="Upload an existing testing/results directory to FollyTest "
             "instead of running a new match.")
    args = parser.parse_args()

    root = find_repo_root()
    os.chdir(root)

    uploader = make_uploader()

    if args.backfill:
        return backfill(args.backfill, uploader)

    elo0, elo1 = BOUNDS[args.mode]

    book_format = BOOK_FORMATS.get(args.book.suffix)
    if book_format is None:
        parser.error(f"Cannot infer book format from: {args.book}")
    if not args.book.is_file():
        parser.error(f"Opening book not found: {args.book}")

    print("Building candidate (working tree) and fastchess...")
    run(["bazel", "build", "-c", "opt", "//cli:follychess",
         "@fastchess//:fastchess"])
    candidate_bin = root / "bazel-bin/cli/follychess"
    fastchess_bin = capture(["bazel", "cquery", "-c", "opt",
                             "--output=files", "@fastchess//:fastchess"])

    commit = capture(
        ["git", "rev-parse", "--verify", f"{args.baseline}^{{commit}}"])
    short = capture(["git", "rev-parse", "--short", commit])
    baseline_bin = root / "testing/bins" / f"follychess-{commit}"

    if baseline_bin.is_file():
        print(f"Using cached baseline binary for {args.baseline} ({short}).")
    else:
        print(f"Building baseline {args.baseline} ({short})...")
        build_baseline(commit, baseline_bin)

    timestamp = datetime.datetime.now().strftime("%Y%m%d-%H%M%S")
    run_dir = root / "testing/results" / f"{timestamp}-{args.mode}-vs-{short}"
    run_dir.mkdir(parents=True)

    if uploader:
        if uploader.create_run({
            "mode": args.mode,
            "elo0": elo0,
            "elo1": elo1,
            "nodes": args.nodes,
            "book": args.book.name,
            "candidate_commit": capture(["git", "rev-parse", "HEAD"]),
            "candidate_dirty": bool(
                capture(["git", "status", "--porcelain", "-uno"])),
            "baseline_ref": args.baseline,
            "baseline_commit": commit,
            "note": args.note,
        }):
            uploader.start(run_dir / "games.pgn")
        else:
            uploader = None

    print()
    print(f"SPRT: candidate (working tree) vs baseline {args.baseline} "
          f"({short})")
    print(f"Bounds: [{elo0}, {elo1}] Elo, nodes/move: {args.nodes}, "
          f"book: {args.book}")
    print(f"Results: {run_dir}")
    print()

    tracker = StatsTracker()
    status = "interrupted"
    returncode = 1
    try:
        returncode = run_match(
            [
                fastchess_bin,
                "-engine", f"cmd={candidate_bin}", "name=candidate",
                "-engine", f"cmd={baseline_bin}", f"name=baseline-{short}",
                "-each", "tc=inf", f"nodes={args.nodes}",
                "-openings", f"file={args.book}", f"format={book_format}",
                "order=random",
                "-repeat", "-games", "2", "-rounds", str(args.max_rounds),
                "-sprt", f"elo0={elo0}", f"elo1={elo1}", "alpha=0.05",
                "beta=0.05",
                "-draw", "movenumber=40", "movecount=8", "score=10",
                "-resign", "movecount=3", "score=600", "twosided=true",
                "-concurrency", str(args.concurrency),
                "-config", f"outname={run_dir}/config.json",
                "-pgnout", f"file={run_dir}/games.pgn",
                "-log", f"file={run_dir}/fastchess.log",
            ],
            run_dir / "summary.txt", tracker, uploader)
        status = tracker.final_status()
    finally:
        if uploader:
            uploader.finish(status)

    return returncode


if __name__ == "__main__":
    sys.exit(main())

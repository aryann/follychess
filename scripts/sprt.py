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
"""

import argparse
import datetime
import os
import pathlib
import shutil
import subprocess
import sys
import tempfile

# SPRT Elo bounds (elo0, elo1) by mode.
BOUNDS = {
    "gain": (0, 5),
    "simplify": (-5, 0),
}

BOOK_FORMATS = {
    ".epd": "epd",
    ".pgn": "pgn",
}


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


def run_and_tee(args: list[str], output: pathlib.Path) -> int:
    with (open(output, "w") as out,
          subprocess.Popen(args, stdout=subprocess.PIPE, text=True) as proc):
        for line in proc.stdout:
            sys.stdout.write(line)
            out.write(line)
    return proc.returncode


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
    args = parser.parse_args()

    root = find_repo_root()
    os.chdir(root)

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

    print()
    print(f"SPRT: candidate (working tree) vs baseline {args.baseline} "
          f"({short})")
    print(f"Bounds: [{elo0}, {elo1}] Elo, nodes/move: {args.nodes}, "
          f"book: {args.book}")
    print(f"Results: {run_dir}")
    print()

    return run_and_tee(
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
        run_dir / "summary.txt")


if __name__ == "__main__":
    sys.exit(main())

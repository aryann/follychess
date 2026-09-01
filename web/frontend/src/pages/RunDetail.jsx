import { collection, doc, onSnapshot, orderBy, query } from "firebase/firestore";
import { useEffect, useState } from "react";
import { Link, useNavigate, useParams } from "react-router-dom";

import Breadcrumbs from "../Breadcrumbs";
import { db } from "../firebase";
import { formatElo, formatScore, formatTimestamp, shortCommit } from "../format";

function Stat({ label, value }) {
  return (
    <div className="cell">
      <div className="label">{label}</div>
      <div className="value">{value}</div>
    </div>
  );
}

export default function RunDetail() {
  const { runId } = useParams();
  const [run, setRun] = useState(null);
  const [games, setGames] = useState([]);
  const navigate = useNavigate();

  useEffect(() => onSnapshot(doc(db, "runs", runId), (snapshot) => {
    setRun(snapshot.exists() ? snapshot.data() : undefined);
  }), [runId]);

  useEffect(() => {
    const q = query(collection(db, "runs", runId, "games"), orderBy("n"));
    return onSnapshot(q, (snapshot) => {
      setGames(snapshot.docs.map((d) => ({ id: d.id, ...d.data() })));
    });
  }, [runId]);

  if (run === null) return <p className="meta">Loading…</p>;
  if (run === undefined) return <p>No such run.</p>;

  const stats = run.stats || {};

  return (
    <>
      <Breadcrumbs items={[
        { label: "Runs", to: "/" },
        { label: runId },
      ]} />
      <h1>
        {run.mode} vs <code>{shortCommit(run.baseline_commit)}</code>{" "}
        <span className={`status ${run.status}`}>{run.status}</span>
      </h1>
      {run.note && <p className="lead">{run.note}</p>}

      <div className="statgrid">
        <Stat label="Elo" value={formatElo(stats)} />
        <Stat label="LLR" value={stats.llr !== undefined
          ? `${stats.llr} (${run.elo0}, ${run.elo1})` : "—"} />
        <Stat label="Score" value={formatScore(stats)} />
        <Stat label="Games" value={run.games_count} />
        <Stat label="Nodes/move" value={run.nodes} />
      </div>

      <p className="meta">
        Started {formatTimestamp(run.created_at)} · candidate{" "}
        <code>{shortCommit(run.candidate_commit)}{run.candidate_dirty ? "+" : ""}</code>
        {" "}vs baseline {run.baseline_ref}{" "}
        (<code>{shortCommit(run.baseline_commit)}</code>) · book {run.book}
        {run.candidate_dirty &&
          " · + means the working tree had uncommitted changes"}
      </p>

      <h2>Games</h2>
      {games.length === 0 && <p>No games uploaded yet.</p>}
      {games.length > 0 && (
        <table>
          <thead>
            <tr>
              <th>#</th>
              <th>White</th>
              <th>Black</th>
              <th>Result</th>
              <th>Termination</th>
            </tr>
          </thead>
          <tbody>
            {games.map((game) => (
              <tr key={game.id} className="rowlink"
                onClick={() => navigate(`/runs/${runId}/games/${game.id}`)}>
                <td><Link to={`/runs/${runId}/games/${game.id}`}>{game.n}</Link></td>
                <td>{game.white}</td>
                <td>{game.black}</td>
                <td>{game.result}</td>
                <td>{game.termination}</td>
              </tr>
            ))}
          </tbody>
        </table>
      )}
    </>
  );
}

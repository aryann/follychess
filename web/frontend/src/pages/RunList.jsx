import { collection, limit, onSnapshot, orderBy, query } from "firebase/firestore";
import { useEffect, useState } from "react";
import { useNavigate } from "react-router-dom";

import { db } from "../firebase";
import { formatElo, formatScore, formatTimestamp, shortCommit } from "../format";

export default function RunList() {
  const [runs, setRuns] = useState(null);
  const navigate = useNavigate();

  useEffect(() => {
    const q = query(collection(db, "runs"), orderBy("created_at", "desc"),
      limit(100));
    return onSnapshot(q, (snapshot) => {
      setRuns(snapshot.docs.map((doc) => ({ id: doc.id, ...doc.data() })));
    });
  }, []);

  if (runs === null) return <p className="meta">Loading…</p>;

  return (
    <>
      <h1>Runs</h1>
      {runs.length === 0 && <p>No runs recorded yet.</p>}
      {runs.length > 0 && (
        <table>
          <thead>
            <tr>
              <th>Started</th>
              <th>Status</th>
              <th>Mode</th>
              <th>Candidate</th>
              <th>Baseline</th>
              <th>Elo</th>
              <th>Score</th>
              <th>Games</th>
              <th>Note</th>
            </tr>
          </thead>
          <tbody>
            {runs.map((run) => (
              <tr key={run.id} className="rowlink"
                onClick={() => navigate(`/runs/${run.id}`)}>
                <td>{formatTimestamp(run.created_at)}</td>
                <td><span className={`status ${run.status}`}>{run.status}</span></td>
                <td>{run.mode} [{run.elo0}, {run.elo1}]</td>
                <td><code>{shortCommit(run.candidate_commit)}{run.candidate_dirty ? "+" : ""}</code></td>
                <td><code>{shortCommit(run.baseline_commit)}</code></td>
                <td>{formatElo(run.stats)}</td>
                <td>{formatScore(run.stats)}</td>
                <td>{run.games_count}</td>
                <td>{run.note}</td>
              </tr>
            ))}
          </tbody>
        </table>
      )}
    </>
  );
}

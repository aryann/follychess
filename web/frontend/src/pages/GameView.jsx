import { Chess } from "chess.js";
import { doc, getDoc } from "firebase/firestore";
import { useEffect, useMemo, useState } from "react";
import { Chessboard } from "react-chessboard";
import { Link, useParams } from "react-router-dom";

import { db } from "../firebase";

// Parses the PGN into the starting FEN (games start from book openings) and
// the list of FENs after each move, so stepping through the game is an array
// lookup.
function parseGame(pgn) {
  const chess = new Chess();
  chess.loadPgn(pgn);
  const moves = chess.history();

  const headers = chess.header();
  const start = headers.FEN;

  const replay = start ? new Chess(start) : new Chess();
  const fens = [replay.fen()];
  for (const move of moves) {
    replay.move(move);
    fens.push(replay.fen());
  }
  return { moves, fens, headers };
}

export default function GameView() {
  const { runId, gameId } = useParams();
  const [game, setGame] = useState(null);
  const [ply, setPly] = useState(0);

  useEffect(() => {
    getDoc(doc(db, "runs", runId, "games", gameId)).then((snapshot) => {
      setGame(snapshot.exists() ? snapshot.data() : undefined);
    });
  }, [runId, gameId]);

  const parsed = useMemo(() => {
    if (!game) return null;
    try {
      return parseGame(game.pgn);
    } catch (error) {
      return { error: String(error) };
    }
  }, [game]);

  useEffect(() => {
    if (!parsed || parsed.error) return undefined;
    const onKey = (event) => {
      if (event.key === "ArrowLeft") setPly((p) => Math.max(0, p - 1));
      if (event.key === "ArrowRight") {
        setPly((p) => Math.min(parsed.moves.length, p + 1));
      }
    };
    window.addEventListener("keydown", onKey);
    return () => window.removeEventListener("keydown", onKey);
  }, [parsed]);

  if (game === null) return <p className="meta">Loading…</p>;
  if (game === undefined) return <p>No such game.</p>;
  if (parsed.error) {
    return (
      <>
        <h1>Game {game.n}</h1>
        <p>Could not parse this game&apos;s PGN: {parsed.error}</p>
        <pre>{game.pgn}</pre>
      </>
    );
  }

  const { moves, fens } = parsed;

  return (
    <>
      <h1>
        Game {game.n}{" "}
        <span className="meta" style={{ fontSize: 16, fontWeight: 400 }}>
          <Link to={`/runs/${runId}`}>back to run</Link>
        </span>
      </h1>
      <p className="lead">
        {game.white} vs {game.black} · {game.result}
        {game.termination ? ` · ${game.termination}` : ""}
      </p>

      <div className="gameview">
        <div className="board">
          <Chessboard position={fens[ply]} arePiecesDraggable={false} />
          <div className="controls">
            <button onClick={() => setPly(0)}>⏮</button>
            <button onClick={() => setPly((p) => Math.max(0, p - 1))}>◀</button>
            <button onClick={() => setPly((p) => Math.min(moves.length, p + 1))}>▶</button>
            <button onClick={() => setPly(moves.length)}>⏭</button>
          </div>
          <p className="meta">
            Move {ply} of {moves.length}. Use the arrow keys to step.
          </p>
        </div>

        <div className="moves">
          {moves.map((move, index) => (
            <span key={index}>
              {index % 2 === 0 && `${Math.floor(index / 2) + 1}. `}
              <span
                className={`mv ${ply === index + 1 ? "current" : ""}`}
                onClick={() => setPly(index + 1)}
              >
                {move}
              </span>{" "}
            </span>
          ))}
        </div>
      </div>
    </>
  );
}

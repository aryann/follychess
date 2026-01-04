import asyncio
import datetime
import json
import os
import random
import shutil
import sqlite3
import sys

import chess
import chess.pgn
import chess.engine


def _bazel_bin_dir():
    return sys.argv[0].split('/bazel-bin/')[0] + '/bazel-bin/'


def _create_pgn(engines, board):
    game = chess.pgn.Game()
    game.headers["White"] = engines[0].id.get("name")
    game.headers["Black"] = engines[1].id.get("name")
    game.headers["Date"] = datetime.date.today().strftime("%Y.%m.%d")
    game.headers["Result"] = board.result()

    node = game
    for move in board.move_stack:
        node = node.add_variation(move)

    return game


async def start_stockfish():
    stockfish = shutil.which("stockfish")
    transport, engine = await chess.engine.popen_uci(stockfish)

    await engine.configure({
        "UCI_Elo": 1600,
        "UCI_LimitStrength": True,
    })
    return engine


async def start_follychess():
    bin = os.path.join(_bazel_bin_dir(), 'cli', 'follychess')
    transport, engine = await chess.engine.popen_uci(bin)
    return engine


async def run_game(engines, db) -> None:
    random.shuffle(engines)
    curr = 0

    start = datetime.datetime.now()
    board = chess.Board()
    moves = []
    while not board.is_game_over(claim_draw=True):
        engine = engines[curr % len(engines)]
        result = await engine.play(board, chess.engine.Limit(time=0.1))
        curr += 1
        print(result.move, end=" ", flush=True)
        moves.append(str(result.move))
        board.push(result.move)

    end = datetime.datetime.now()

    pgn = _create_pgn(engines, board)
    print(pgn)

    db.execute(f"""
        INSERT INTO results (
            start_time,
            end_time,
            white,
            black,
            white_params,
            black_params,
            outcome,
            uci_moves,
            pgn
        ) VALUES (
            ?,
            ?,
            ?,
            ?,
            jsonb(?),
            jsonb(?),
            ?,
            ?,
            ?);
            """,
               (
                   start.isoformat(),
                   end.isoformat(),
                   engines[0].id.get("name"),
                   engines[1].id.get("name"),
                   json.dumps(dict(engines[0].config.items())),
                   json.dumps(dict(engines[1].config.items())),
                   board.result(),
                   ' '.join(moves),
                   str(pgn),
               ))
    db.commit()

    return board.result()


def init_db():
    conn = sqlite3.connect("results.db")
    cursor = conn.cursor()

    cursor.execute("""
        CREATE TABLE IF NOT EXISTS results (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            start_time TEXT,
            end_time TEXT,
            white TEXT,
            black TEXT,
            white_params BLOB,
            black_params BLOB,
            outcome TEXT,
            uci_moves TEXT,
            pgn TEXT
        );
    """)

    return conn


async def main() -> None:
    db = init_db()
    engines = []
    try:
        engines = [await start_stockfish(), await start_follychess()]

        while True:
            result = await run_game(engines, db)
            print()
            print(f'{engines[0].id.get("name")}, {engines[1].id.get("name")}, {result}')

            print()
            print("---")
            print()

    finally:
        for engine in engines:
            await engine.quit()


asyncio.run(main())

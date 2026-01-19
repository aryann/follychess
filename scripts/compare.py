import asyncio
import dataclasses
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


@dataclasses.dataclass
class Engine:
    name: str
    bin: str
    engine: chess.engine.Protocol

    def __init__(self, engine: str):
        self.name, bin = engine.split(":")
        self.bin = os.path.expanduser(bin)

    async def start(self):
        transport, self.engine = await chess.engine.popen_uci(self.bin)


def create_pgn(engines, board):
    game = chess.pgn.Game()
    game.headers["White"] = engines[0].engine.id.get("name")
    game.headers["Black"] = engines[1].engine.id.get("name")
    game.headers["Date"] = datetime.date.today().strftime("%Y.%m.%d")
    game.headers["Result"] = board.result()

    node = game
    for move in board.move_stack:
        node = node.add_variation(move)

    return game


async def run_game(engines, db, invocation_id) -> None:
    random.shuffle(engines)
    curr = 0

    start = datetime.datetime.now()
    board = chess.Board()
    moves = []
    while not board.is_game_over(claim_draw=True):
        engine = engines[curr % len(engines)].engine
        result = await engine.play(board, chess.engine.Limit(time=0.1, depth=5))
        curr += 1
        print(result.move, end=" ", flush=True)
        moves.append(str(result.move))
        board.push(result.move)

    end = datetime.datetime.now()

    pgn = create_pgn(engines, board)
    print(pgn)

    winner = ''
    if board.result() == '1-0':
        winner = engines[0].name
    elif board.result() == '0-1':
        winner = engines[1].name

    db.execute(f"""
        INSERT INTO results (
            invocation_id,
            start_time,
            end_time,
            white,
            black,
            white_name,
            black_name,
            white_params,
            black_params,
            outcome,
            winner,
            uci_moves,
            pgn
        ) VALUES (
            ?,
            ?,
            ?,
            ?,
            ?,
            ?,
            ?,
            jsonb(?),
            jsonb(?),
            ?,
            ?,
            ?,
            ?);
            """,
               (
                   invocation_id,
                   start.isoformat(),
                   end.isoformat(),
                   engines[0].name,
                   engines[1].name,
                   engines[0].engine.id.get("name"),
                   engines[1].engine.id.get("name"),
                   json.dumps(dict(engines[0].engine.config.items())),
                   json.dumps(dict(engines[1].engine.config.items())),
                   board.result(),
                   winner,
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
            invocation_id TEXT,
            start_time TEXT,
            end_time TEXT,
            white TEXT,
            black TEXT,
            white_name TEXT,
            black_name TEXT,
            white_params BLOB,
            black_params BLOB,
            outcome TEXT,
            winner TEXT,
            uci_moves TEXT,
            pgn TEXT
        );
    """)

    return conn


async def main() -> None:
    if len(sys.argv) != 4:
        print(f"Usage: ${sys.argv[0]} INVOCATION_ID NAME_1:ENGINE_BIN_1 NAME_2: ENGINE_BIN_2", file=sys.stderr)
        return

    invocation_id, engine1, engine2 = sys.argv[1:4]
    engines = [Engine(engine1), Engine(engine2)]
    await engines[0].start()
    await engines[1].start()

    db = init_db()
    try:
        while True:
            result = await run_game(engines, db, invocation_id)
            print()
            print(f'{engines[0].name}, {engines[1].name}, {result}')

            print()
            print("---")
            print()

    finally:
        for engine in engines:
            await engine.engine.quit()


asyncio.run(main())

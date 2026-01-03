import asyncio
import datetime
import os
import random
import shutil
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


async def run_game(engines) -> None:
    random.shuffle(engines)
    curr = 0

    board = chess.Board()
    while not board.is_game_over(claim_draw=True):
        engine = engines[curr % len(engines)]
        result = await engine.play(board, chess.engine.Limit(time=0.1))
        curr += 1
        print(result.move, end=" ", flush=True)
        board.push(result.move)

    print(_create_pgn(engines, board))
    return board.result()


async def main() -> None:
    engines = []
    try:
        engines = [await start_stockfish(), await start_follychess()]

        while True:
            result = await run_game(engines)
            print()
            print(f'{engines[0].id.get("name")}, {engines[1].id.get("name")}, {result}')

            print()
            print("---")
            print()

    finally:
        for engine in engines:
            await engine.quit()


asyncio.run(main())

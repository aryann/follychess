import asyncio
import datetime
import os
import random
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
    transport, engine = await chess.engine.popen_uci("/opt/homebrew/bin/stockfish")

    await engine.configure({"UCI_Elo": 1320})
    return engine


async def start_follychess():
    bin = os.path.join(_bazel_bin_dir(), 'cli', 'follychess')
    transport, engine = await chess.engine.popen_uci(bin)
    return engine


async def run_game(engines) -> None:
    random.shuffle(engines)
    curr = 0

    board = chess.Board()
    while not board.is_game_over():
        engine = engines[curr % len(engines)]
        result = await engine.play(board, chess.engine.Limit(time=0.1))
        curr += 1
        board.push(result.move)

    print(_create_pgn(engines, board))


async def main() -> None:
    engines = [await start_stockfish(), await start_follychess()]

    for _ in range(100):
        await run_game(engines)
        print()
        print("---")
        print()

    for engine in engines:
        await engine.quit()


asyncio.run(main())

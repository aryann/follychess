// FollyChess is a UCI-compatible chess engine written in C++23.
//
// Copyright (C) 2025-2026 Aryan Naraghi <aryan.naraghi@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include "perft.h"

#include <map>
#include <thread>
#include <vector>

#include "absl/log/log.h"
#include "move.h"
#include "move_generator.h"
#include "position.h"
#include "scoped_move.h"

namespace follychess {
namespace {

std::size_t RunPerft(std::size_t depth, std::size_t current_depth,
                     Position &position, Move start_move,
                     std::vector<std::size_t> &depth_counts) {
  ++depth_counts[current_depth];

  if (depth == current_depth) {
    return 1;
  }

  std::vector<Move> moves = GenerateMoves(position);
  std::size_t final_move_count = 0;

  for (const Move &move : moves) {
    ScopedMove scoped_move(move, position);

    if (position.GetCheckers(~position.SideToMove())) {
      continue;
    }
    final_move_count +=
        RunPerft(depth, current_depth + 1, position, start_move, depth_counts);
  }

  return final_move_count;
}

}  // namespace

void RunPerft(std::size_t depth, const Position &position,
              std::vector<std::size_t> &final_depth_counts,
              std::map<Move, std::size_t> &final_move_counts) {
  if (depth == 0) {
    return;
  }

  std::vector<Move> initial_moves = GenerateMoves(position);

  std::vector<std::vector<std::size_t>> all_depth_counts(
      initial_moves.size(), std::vector<std::size_t>(depth + 1, 0));
  std::vector<std::size_t> all_move_counts(initial_moves.size(), 0);

  std::vector<std::thread> threads;

  for (int i = 0; i < initial_moves.size(); ++i) {
    threads.emplace_back([&, i]() {
      Move move = initial_moves[i];

      Position new_position = position;
      new_position.Do(move);
      if (new_position.GetCheckers(~new_position.SideToMove())) {
        return;
      }

      all_move_counts[i] =
          RunPerft(depth, 1, new_position, move, all_depth_counts[i]);
    });
  }

  for (std::thread &thread : threads) {
    thread.join();
  }

  for (int i = 0; i < initial_moves.size(); ++i) {
    final_move_counts[initial_moves[i]] = all_move_counts[i];
  }

  final_depth_counts.resize(depth + 1, 0);
  final_depth_counts[0] = 1;
  for (const std::vector<std::size_t> &depth_counts : all_depth_counts) {
    for (int i = 0; i < depth_counts.size(); ++i) {
      final_depth_counts[i] += depth_counts[i];
    }
  }
}

}  // namespace follychess

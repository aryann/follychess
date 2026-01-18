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

#ifndef FOLLYCHESS_SEARCH_SEARCH_H_
#define FOLLYCHESS_SEARCH_SEARCH_H_

#include "engine/game.h"
#include "engine/move.h"
#include "engine/position.h"
#include "search/evaluation.h"
#include "search/principal_variation.h"

namespace follychess {

struct SearchInfo {
  int depth;

  // The evaluation score in centipawns, relative to the side to move.
  //
  //   * Positive values indicate an advantage for the side to move.
  //   * Negative values indicate an advantage for the opponent.
  int score;

  // If set, specifies the number of moves until checkmate. The score is
  // relative to the side to move:
  //
  //   * Positive means the side to move can force a win (delivering mate).
  //   * Negative means the side to move is forced to lose (getting mated).
  //q
  std::optional<int> mate_in;

  std::int64_t nodes;
  std::int64_t node_per_second;
  std::int64_t tbhits;
  std::string principal_variation;
};

}  // namespace follychess

template <>
struct std::formatter<follychess::SearchInfo> : std::formatter<std::string> {
  static auto format(const follychess::SearchInfo& info,
                     std::format_context& context) {
    std::string score = std::format("cp {}", info.score);
    if (info.mate_in) {
      score = std::format("mate {}", *info.mate_in);
    }

    auto out = context.out();
    return std::format_to(
        out, "info depth {} score {} nodes {} nps {} tbhits {} pv {}",
        info.depth, score, info.nodes, info.node_per_second, info.tbhits,
        info.principal_variation);
  }
};

namespace follychess {

struct SearchOptions {
  SearchOptions& SetDepth(int value) {
    depth = value;
    return *this;
  }

  int depth = 5;

  SearchOptions& SetInfoObserver(
      const std::function<void(const SearchInfo&)>& value) {
    info_observer = value;
    return *this;
  }

  std::function<void(const SearchInfo&)> info_observer = [](const SearchInfo&) {
  };
};

Move Search(const Game& game, const SearchOptions& options = SearchOptions());

}  // namespace follychess

#endif  // FOLLYCHESS_SEARCH_SEARCH_H_

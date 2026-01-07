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

#ifndef FOLLYCHESS_SEARCH_TRANSPOSITION_H_
#define FOLLYCHESS_SEARCH_TRANSPOSITION_H_

#include <array>
#include <optional>

#include "absl/container/flat_hash_map.h"
#include "engine/position.h"

namespace follychess {

class TranspositionTable {
 public:
  enum class BoundType : std::int8_t {
    Exact,
    UpperBound,
    LowerBound,
  };

  explicit TranspositionTable() : hits_{0} {}

  constexpr std::optional<int> Probe(const Position& position, int alpha,
                                     int beta, int remaining_depth,
                                     Move* best_move);

  constexpr void Record(const Position& position, int score,
                        int remaining_depth, BoundType type, Move best_move);

  [[nodiscard]] constexpr std::int64_t GetHits() const { return hits_; }

 private:
  static constexpr auto KEntries = 1 << 24;

  struct Entry {
    Move best_move;
    int remaining_depth{0};
    int score{0};
    BoundType type{BoundType::Exact};
  };

  absl::flat_hash_map<std::uint64_t, Entry> table_;
  std::int64_t hits_;
};

constexpr std::optional<int> TranspositionTable::Probe(const Position& position,
                                                       int alpha, int beta,
                                                       int remaining_depth,
                                                       Move* best_move) {
  auto it = table_.find(position.GetKey());
  if (it == table_.end()) {
    return std::nullopt;
  }

  const Entry& entry = it->second;
  *best_move = entry.best_move;

  if (entry.remaining_depth < remaining_depth) {
    return std::nullopt;
  }

  switch (entry.type) {
    case BoundType::Exact:
      ++hits_;
      return entry.score;
    case BoundType::UpperBound:
      if (entry.score <= alpha) {
        ++hits_;
        return alpha;
      }
    case BoundType::LowerBound:
      if (entry.score >= beta) {
        ++hits_;
        return beta;
      }
    default:
      return std::nullopt;
  }
}

constexpr void TranspositionTable::Record(const Position& position, int score,
                                          int remaining_depth, BoundType type,
                                          Move best_move) {
  table_[position.GetKey()] = {
      .best_move = best_move,
      .remaining_depth = remaining_depth,
      .score = score,
      .type = type,
  };
}

}  // namespace follychess

#endif  // FOLLYCHESS_SEARCH_TRANSPOSITION_H_

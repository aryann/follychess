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

#include <optional>

#include "absl/container/flat_hash_map.h"
#include "engine/position.h"
#include "search/evaluation.h"

namespace follychess {

class TranspositionTable {
 public:
  enum class BoundType : std::int8_t {
    Exact,
    UpperBound,
    LowerBound,
  };

  struct ProbeParams {
    int alpha;
    int beta;
    int ply;
    int depth;
  };

  struct RecordParams {
    int ply;
    int depth;
  };

  // Allocates the transposition table to fit within the specified memory limit.
  // The final number of entries is rounded down to the nearest power of two.
  // This enables fast bitwise indexing (`key & (size - 1)`) rather than slower
  // modulo arithmetic (`key % size`).
  explicit TranspositionTable(std::size_t size_mb = 256)
      : entries_(std::bit_floor(size_mb * (2 << 20) / sizeof(Bucket))),
        hits_(0) {}

  constexpr std::optional<int> Probe(const Position& position,
                                     ProbeParams probe_params, Move* best_move);

  constexpr void Record(const Position& position, int score,
                        RecordParams record_params, BoundType type,
                        Move best_move);

  [[nodiscard]] constexpr std::int64_t GetHits() const { return hits_; }

  [[nodiscard]] std::size_t size() const { return entries_.size(); }

 private:
  [[nodiscard]] static int NormalizeScore(const int score, const int ply) {
    if (score > kCheckMateThreshold) {
      return score + ply;
    }
    if (score < -kCheckMateThreshold) {
      return score - ply;
    }
    return score;
  }

  [[nodiscard]] static int DenormalizeScore(const int score, const int ply) {
    if (score > kCheckMateThreshold) {
      return score - ply;
    }
    if (score < -kCheckMateThreshold) {
      return score + ply;
    }
    return score;
  }

  struct Entry {
    Move best_move;
    int remaining_depth{0};
    int score{0};
    BoundType type{BoundType::Exact};
  };

  struct Bucket {
    // On a hash collision, this entry is always overwritten by the newest
    // evaluation.
    Entry always_entry;

    // On a hash collision, this entry is overwritten if and only if the new
    // remaining_depth >= the stored remaining_depth.
    Entry deep_entry;
  };

  // TODO(aryann): Replace this with a more efficient data structure.
  absl::flat_hash_map<std::uint64_t, Entry> table_;
  std::vector<Bucket> entries_;
  std::int64_t hits_;
};

constexpr std::optional<int> TranspositionTable::Probe(const Position& position,
                                                       ProbeParams probe_params,
                                                       Move* best_move) {
  auto it = table_.find(position.GetKey());
  if (it == table_.end()) {
    return std::nullopt;
  }

  const Entry& entry = it->second;
  const int score = DenormalizeScore(entry.score, probe_params.ply);
  *best_move = entry.best_move;

  if (entry.remaining_depth < probe_params.depth) {
    return std::nullopt;
  }

  switch (entry.type) {
    case BoundType::Exact:
      ++hits_;
      return score;

    case BoundType::UpperBound:
      if (score <= probe_params.alpha) {
        ++hits_;
        return probe_params.alpha;
      }
      break;

    case BoundType::LowerBound:
      if (score >= probe_params.beta) {
        ++hits_;
        return probe_params.beta;
      }
      break;
  }

  return std::nullopt;
}

constexpr void TranspositionTable::Record(const Position& position, int score,
                                          RecordParams record_params,
                                          BoundType type, Move best_move) {
  table_[position.GetKey()] = {
      .best_move = best_move,
      .remaining_depth = record_params.depth,
      .score = NormalizeScore(score, record_params.ply),
      .type = type,
  };
}

}  // namespace follychess

#endif  // FOLLYCHESS_SEARCH_TRANSPOSITION_H_

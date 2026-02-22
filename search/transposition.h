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

#include <bit>
#include <optional>
#include <utility>
#include <vector>

#include "engine/move.h"
#include "engine/zobrist.h"

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
      : table_(std::bit_floor(size_mb * (1 << 20) / sizeof(Bucket))),
        probes_(0),
        hits_(0) {}

  std::optional<int> Probe(ZobristKey key, ProbeParams probe_params,
                           Move* best_move);

  void Record(ZobristKey key, int score, RecordParams record_params,
              BoundType type, Move best_move);

  [[nodiscard]] std::int64_t GetHits() const { return hits_; }

  [[nodiscard]] std::int64_t GetMisses() const { return probes_ - hits_; }

  [[nodiscard]] double GetHitRate() const {
    if (probes_ == 0) {
      return 0;
    }
    return static_cast<double>(hits_) / static_cast<double>(probes_);
  }

  [[nodiscard]] std::size_t size() const { return table_.size(); }

 private:
  [[nodiscard]] static int NormalizeScore(int score, int ply);

  [[nodiscard]] static int DenormalizeScore(int score, int ply);

  struct Entry {
    ZobristKey key;
    Move best_move;
    int remaining_depth{0};
    int score{0};
    BoundType type{BoundType::Exact};
  };

  static_assert(sizeof(Entry) == 24);

  struct Bucket {
    // On a hash collision, this entry is always overwritten by the newest
    // evaluation.
    Entry always_entry;

    // On a hash collision, this entry is overwritten if and only if the new
    // remaining_depth >= the stored remaining_depth.
    Entry deep_entry;
  };

  static_assert(sizeof(Bucket) == 48);

  template <typename Self>
  [[nodiscard]] auto&& GetBucket(this Self&& self, const ZobristKey key) {
    const std::size_t index = key.GetValue() & (self.table_.size() - 1);
    return std::forward<Self>(self).table_[index];
  }

  [[nodiscard]] const Entry* GetEntry(ZobristKey key) const;

  std::vector<Bucket> table_;

  std::int64_t probes_;
  std::int64_t hits_;
};

}  // namespace follychess

#endif  // FOLLYCHESS_SEARCH_TRANSPOSITION_H_

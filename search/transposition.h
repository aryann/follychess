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

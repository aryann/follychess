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

  explicit TranspositionTable(const Position& position)
      : position_{position}, hits_{0} {}

  [[nodiscard]] constexpr std::optional<int> Probe(int alpha, int beta,
                                                   int depth);

  constexpr void Record(int score, int depth, BoundType type);

  [[nodiscard]] constexpr std::int64_t GetHits() const { return hits_; }

  constexpr void Clear() {
    table_.clear();
    hits_ = 0;
  }

 private:
  static constexpr auto KEntries = 1 << 24;

  struct Entry {
    int depth{0};
    int score{0};
    BoundType type{BoundType::Exact};
  };

  const Position& position_;

  absl::flat_hash_map<std::uint64_t, Entry> table_;
  std::int64_t hits_;
};

[[nodiscard]] constexpr std::optional<int> TranspositionTable::Probe(
    int alpha, int beta, int depth) {
  auto it = table_.find(position_.GetKey());
  if (it == table_.end()) {
    return std::nullopt;
  }

  const Entry& entry = it->second;
  if (entry.depth < depth) {
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

constexpr void TranspositionTable::Record(int score, int depth,
                                          BoundType type) {
  table_[position_.GetKey()] = {
      .depth = depth,
      .score = score,
      .type = type,
  };
}

}  // namespace follychess

#endif  // FOLLYCHESS_SEARCH_TRANSPOSITION_H_

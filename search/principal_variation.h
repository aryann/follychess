#ifndef FOLLYCHESS_SEARCH_PRINCIPAL_VARIATION_H_
#define FOLLYCHESS_SEARCH_PRINCIPAL_VARIATION_H_

#include "engine/position.h"

namespace follychess {

class PrincipalVariationTable {
 public:
  explicit PrincipalVariationTable() : data_(kMaxDepth * kMaxDepth) {}

  void RecordMove(int ply, Move move) {
    DCHECK_GE(ply, 0);
    DCHECK_LT(ply, kMaxDepth);

    int index = GetIndex(ply);
    DCHECK_LT(index, data_.size());
    data_[index] = move;

    int child_index = GetIndex(ply + 1);
    DCHECK_LT(child_index, data_.size());

    while (data_[child_index] != Move::NullMove()) {
      ++index;
      DCHECK_LT(index, data_.size());

      data_[index] = data_[child_index];
      ++child_index;
      DCHECK_LT(child_index, data_.size());
    }

    // Null-terminate the current ply. This is necessary in case the new
    // principal variation for this ply has a smaller length than the one being
    // replaced.
    ++index;
    data_[index] = Move::NullMove();
  }

  [[nodiscard]] std::span<const Move> GetPrincipalVariation() const {
    std::size_t size = 0;
    while (data_[size] != Move::NullMove()) {
      ++size;
    }

    return {data_.data(), size};
  }

 private:
  [[nodiscard]] static int GetIndex(int ply) { return ply * kMaxDepth; }

  static constexpr int kMaxDepth = 128;

  std::vector<Move> data_;
};

}  // namespace follychess

#endif  // FOLLYCHESS_SEARCH_PRINCIPAL_VARIATION_H_

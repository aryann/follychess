#ifndef FOLLYCHESS_SEARCH_EVALUATION_H_
#define FOLLYCHESS_SEARCH_EVALUATION_H_

#include "engine/position.h"

namespace follychess {

[[nodiscard]] int GetMaterialScore(const Position& position);

[[nodiscard]] int GetPlacementScore(const Position& position);

template <Side Side>
[[nodiscard]] int CountDoubledPawns(const Position& position) {
  const Bitboard pawns = position.GetPieces(Side, kPawn);
  int count = 0;
  for (Bitboard file : file::kFileMasks) {
    const int current = (pawns & file).GetCount();

    // If a file has N pawns, we count N - 1 doubled pawns.
    if (current > 1) {
      count += current - 1;
    }
  }

  return count;
}

[[nodiscard]] int Evaluate(const Position& position);

}  // namespace follychess

#endif  // FOLLYCHESS_SEARCH_EVALUATION_H_

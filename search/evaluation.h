#ifndef FOLLYCHESS_SEARCH_EVALUATION_H_
#define FOLLYCHESS_SEARCH_EVALUATION_H_

#include "engine/position.h"
#include "phase.h"

namespace follychess {

struct Score {
  int middle = 0;
  int end = 0;

  constexpr Score operator+(const Score other) const {
    return {.middle = middle + other.middle, .end = end + other.end};
  }
};

template <Side Side>
[[nodiscard]] Score GetPlacementScore(const Position& position);

template <Side Side>
[[nodiscard]] int GetMaterialScore(const Position& position);

template <Side Side>
[[nodiscard]] int CountDoubledPawns(const Position& position);

template <Side Side>
[[nodiscard]] int CountBlockedPawns(const Position& position);

[[nodiscard]] int Evaluate(const Position& position, int phase);

}  // namespace follychess

#endif  // FOLLYCHESS_SEARCH_EVALUATION_H_

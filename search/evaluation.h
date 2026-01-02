#ifndef FOLLYCHESS_SEARCH_EVALUATION_H_
#define FOLLYCHESS_SEARCH_EVALUATION_H_

#include "engine/position.h"
#include "phase.h"

namespace follychess {

template <Side Side>
[[nodiscard]] int GetPlacementScore(const Position& position, int phase);

template <Side Side>
[[nodiscard]] int GetMaterialScore(const Position& position);

template <Side Side>
[[nodiscard]] int CountDoubledPawns(const Position& position);

template <Side Side>
[[nodiscard]] int CountBlockedPawns(const Position& position);

[[nodiscard]] int Evaluate(const Position& position, int phase);

}  // namespace follychess

#endif  // FOLLYCHESS_SEARCH_EVALUATION_H_

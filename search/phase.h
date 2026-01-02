#ifndef FOLLYCHESS_SEARCH_PHASE_H_
#define FOLLYCHESS_SEARCH_PHASE_H_

#include "engine/position.h"

namespace follychess {

// Calculates the game phase based on material. Returns 0 for the starting
// position and 256 for a position with only kings. All other positions
// return an interpolated value.
//
// The implementation is based on
// https://www.chessprogramming.org/Tapered_Eval.
[[nodiscard]] int CalculatePhase(const Position& position);

}  // namespace follychess

#endif  // FOLLYCHESS_SEARCH_PHASE_H_

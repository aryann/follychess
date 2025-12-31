#ifndef FOLLYCHESS_SEARCH_PHASE_H_
#define FOLLYCHESS_SEARCH_PHASE_H_

#include "engine/position.h"

namespace follychess {

[[nodiscard]] int CalculatePhase(const Position& position);

}  // namespace follychess

#endif  // FOLLYCHESS_SEARCH_PHASE_H_

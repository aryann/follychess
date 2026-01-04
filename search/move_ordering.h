#ifndef FOLLYCHESS_SEARCH_MOVE_ORDERING_H_
#define FOLLYCHESS_SEARCH_MOVE_ORDERING_H_

#include <vector>

#include "engine/move.h"
#include "engine/position.h"

namespace follychess {

void OrderMoves(const Position& position, Move priority_move,
                std::vector<Move>& moves);

}  // namespace follychess

#endif  // FOLLYCHESS_SEARCH_MOVE_ORDERING_H_

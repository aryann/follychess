#ifndef FOLLYCHESS_MOVE_GENERATOR_H_
#define FOLLYCHESS_MOVE_GENERATOR_H_

#include <vector>

#include "move.h"
#include "position.h"
#include "types.h"

namespace follychess {

template <MoveType MoveType>
std::vector<Move> GenerateMoves(const Position &position);

std::vector<Move> GenerateMoves(const Position &position);

template <Side Side>
std::vector<Move> GenerateLegalMoves(Position position);

template <MoveType MoveType>
std::vector<Move> GenerateLegalMoves(const Position &position);

std::vector<Move> GenerateLegalMoves(const Position &position);

}  // namespace follychess

#endif  // FOLLYCHESS_MOVE_GENERATOR_H_

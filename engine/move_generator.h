// FollyChess is a UCI-compatible chess engine written in C++23.
//
// Copyright (C) 2025-2026 Aryan Naraghi <aryan.naraghi@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

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

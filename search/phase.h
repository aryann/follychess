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

#ifndef FOLLYCHESS_SEARCH_PHASE_H_
#define FOLLYCHESS_SEARCH_PHASE_H_

#include "engine/position.h"

namespace follychess {

constexpr int kStartPhaseValue = 0;
constexpr int kEndPhaseValue = 256;

// Calculates the game phase based on material. Returns 0 for the starting
// position and 256 for a position with only kings. All other positions
// return an interpolated value.
//
// The implementation is based on
// https://www.chessprogramming.org/Tapered_Eval.
[[nodiscard]] int CalculatePhase(const Position& position);

}  // namespace follychess

#endif  // FOLLYCHESS_SEARCH_PHASE_H_

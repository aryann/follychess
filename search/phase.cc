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

#include "search/phase.h"

#include "engine/position.h"

namespace follychess {
namespace {

constexpr int kKnightMaterialScore = 1;
constexpr int kBishopMaterialScore = 1;
constexpr int kRookMaterialScore = 2;
constexpr int kQueenMaterialScore = 4;

constexpr int kStartMaterialScore = 4 * kKnightMaterialScore +  //
                                    4 * kBishopMaterialScore +  //
                                    4 * kRookMaterialScore +    //
                                    2 * kQueenMaterialScore;

}  // namespace

int CalculatePhase(const Position& position) {
  int phase = kStartMaterialScore;
  phase -= position.GetPieces(kKnight).GetCount() * kKnightMaterialScore;
  phase -= position.GetPieces(kBishop).GetCount() * kBishopMaterialScore;
  phase -= position.GetPieces(kRook).GetCount() * kRookMaterialScore;
  phase -= position.GetPieces(kQueen).GetCount() * kQueenMaterialScore;

  phase = phase * kEndPhaseValue + kStartMaterialScore / 2;
  phase /= kStartMaterialScore;
  return phase;
}

}  // namespace follychess

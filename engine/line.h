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

#ifndef FOLLYCHESS_LINE_H_
#define FOLLYCHESS_LINE_H_

#include "bitboard.h"
#include "engine/types.h"

namespace follychess {

template <Direction Direction>
consteval void FillFromOrigin(Square from,
                              std::array<Bitboard, kNumSquares> &lines) {
  Bitboard result(from);
  Bitboard curr(from);

  while (curr) {
    curr = curr.Shift<Direction>();
    if (!curr) {
      break;
    }

    Square to = curr.LeastSignificantBit();
    lines[to] = result;
    result |= curr;
  }
}

consteval auto GenerateLineTable() {
  std::array<std::array<Bitboard, kNumSquares>, kNumSquares> lines;

  for (int square = A8; square < kNumSquares; ++square) {
    const auto from = static_cast<Square>(square);
    std::array<Bitboard, kNumSquares> &curr = lines[square];

    FillFromOrigin<kNorth>(from, curr);
    FillFromOrigin<kNorthEast>(from, curr);
    FillFromOrigin<kEast>(from, curr);
    FillFromOrigin<kSouthEast>(from, curr);
    FillFromOrigin<kSouth>(from, curr);
    FillFromOrigin<kSouthWest>(from, curr);
    FillFromOrigin<kWest>(from, curr);
    FillFromOrigin<kNorthWest>(from, curr);
  }

  return lines;
}

// Gets a bitboard of squares on the line from `from` to `to`. The range is
// inclusive of `from` and exclusive of `to`, i.e., [`from`, `to`).
constexpr Bitboard GetLine(const Square from, const Square to) {
  static constexpr std::array<std::array<Bitboard, kNumSquares>, kNumSquares>
      kLineTable = GenerateLineTable();
  return kLineTable[from][to];
}

}  // namespace follychess

#endif  // FOLLYCHESS_LINE_H_

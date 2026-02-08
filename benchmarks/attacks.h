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

#ifndef FOLLYCHESS_BENCHMARKS_ATTACKS_H_
#define FOLLYCHESS_BENCHMARKS_ATTACKS_H_

#include "engine/attacks.h"
#include "engine/bitboard.h"
#include "engine/magic.generated.h"

namespace follychess {

template <Piece Piece>
[[nodiscard]] Bitboard GenerateAttacksOnTheFly(Square square,
                                               Bitboard occupied) {
  static_assert(Piece == kBishop || Piece == kRook || Piece == kQueen);

  if constexpr (Piece == kBishop) {
    return GenerateSlidingAttacks<kNorthEast, kSouthEast, kSouthWest,
                                  kNorthWest>(square, occupied);
  }
  if constexpr (Piece == kRook) {
    return GenerateSlidingAttacks<kNorth, kEast, kSouth, kWest>(square,
                                                                occupied);
  }
  return GenerateAttacksOnTheFly<kBishop>(square, occupied) |
         GenerateAttacksOnTheFly<kRook>(square, occupied);
}

template <template <typename...> class Map, Piece Piece>
[[nodiscard]] auto GenerateAttacksMap() {
  std::array<Map<Bitboard, Bitboard>, kNumSquares> result;
  for (int square = kFirstSquare; square < kNumSquares; ++square) {
    const Square from = static_cast<Square>(square);

    Bitboard mask;
    if constexpr (Piece == kBishop || Piece == kQueen) {
      mask |= kSlidingAttackTables.bishop_magic_squares[square].mask;
    }
    if constexpr (Piece == kRook || Piece == kQueen) {
      mask |= kSlidingAttackTables.rook_magic_squares[square].mask;
    }

    std::vector<Bitboard> occupancies = MakePowerSet(mask);
    for (Bitboard occupied : occupancies) {
      result[from][occupied] = GenerateAttacks<Piece>(from, occupied);
    }
  }

  return result;
}

template <template <typename...> class Map, Piece Piece>
[[nodiscard]] Bitboard GetAttacksFromMap(Square square, Bitboard occupied) {
  static_assert(Piece == kBishop || Piece == kRook || Piece == kQueen);

  static const std::array<Map<Bitboard, Bitboard>, kNumSquares> kBishopAttacks =
      GenerateAttacksMap<Map, kBishop>();
  static const std::array<Map<Bitboard, Bitboard>, kNumSquares> kRookAttacks =
      GenerateAttacksMap<Map, kRook>();
  static const std::array<Map<Bitboard, Bitboard>, kNumSquares> kQueenAttacks =
      GenerateAttacksMap<Map, kQueen>();

  Bitboard bishop_mask = kSlidingAttackTables.bishop_magic_squares[square].mask;
  Bitboard rook_mask = kSlidingAttackTables.rook_magic_squares[square].mask;

  if constexpr (Piece == kBishop) {
    occupied &= bishop_mask;
    return kBishopAttacks[square].find(occupied)->second;
  } else if constexpr (Piece == kRook) {
    occupied &= rook_mask;

    return kRookAttacks[square].find(occupied)->second;
  } else {
    Bitboard queen_mask = bishop_mask | rook_mask;
    occupied &= queen_mask;
    return kQueenAttacks[square].find(occupied)->second;
  }
}

}  // namespace follychess

#endif  // FOLLYCHESS_BENCHMARKS_ATTACKS_H_
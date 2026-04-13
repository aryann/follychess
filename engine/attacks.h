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

#ifndef FOLLYCHESS_ATTACKS_H_
#define FOLLYCHESS_ATTACKS_H_

#include <array>
#include <random>

#include "absl/log/check.h"
#include "bitboard.h"
#include "engine/magic.generated.h"
#include "engine/magic.h"

namespace follychess {

template <Direction... Directions>
consteval void MakePawnAttacks(std::array<Bitboard, kNumSquares> &attacks) {
  for (int square = kFirstSquare; square < kNumSquares; ++square) {
    Bitboard start(static_cast<Square>(square));
    attacks[square] = (start.Shift<Directions>() | ...);
  }
}

consteval auto MakePawnAttacks() {
  std::array<std::array<Bitboard, kNumSquares>, kNumSides> attacks;
  MakePawnAttacks<kNorthEast, kNorthWest>(attacks[kWhite]);
  MakePawnAttacks<kSouthEast, kSouthWest>(attacks[kBlack]);
  return attacks;
}

constexpr Bitboard GetPawnAttacks(Square square, Side side) {
  static constexpr std::array<std::array<Bitboard, kNumSquares>, kNumSides>
      kPawnAttacks = MakePawnAttacks();
  return kPawnAttacks[side][square];
}

consteval auto GeneratePawnAttacks() {
  std::array<Bitboard, kNumSquares> attacks;
  for (int square = kFirstSquare; square < kNumSquares; ++square) {
    Bitboard start(static_cast<Square>(square));
    attacks[square] = kEmptyBoard                  //
                      | start.Shift<kNorthEast>()  //
                      | start.Shift<kSouthEast>()  //
                      | start.Shift<kSouthWest>()  //
                      | start.Shift<kNorthWest>();
  }
  return attacks;
}

consteval std::array<Bitboard, kNumSquares> GenerateKnightAttacks() {
  std::array<Bitboard, kNumSquares> attacks;
  for (int square = kFirstSquare; square < kNumSquares; ++square) {
    Bitboard start(static_cast<Square>(square));
    attacks[square] = kEmptyBoard                                  //
                      | start.Shift<kNorth>().Shift<kNorthEast>()  //
                      | start.Shift<kEast>().Shift<kNorthEast>()   //
                      | start.Shift<kEast>().Shift<kSouthEast>()   //
                      | start.Shift<kSouth>().Shift<kSouthEast>()  //
                      | start.Shift<kSouth>().Shift<kSouthWest>()  //
                      | start.Shift<kWest>().Shift<kSouthWest>()   //
                      | start.Shift<kWest>().Shift<kNorthWest>()   //
                      | start.Shift<kNorth>().Shift<kNorthWest>();
  }
  return attacks;
}

consteval std::array<Bitboard, kNumSquares> GenerateKingAttacks() {
  std::array<Bitboard, kNumSquares> attacks;
  for (int square = kFirstSquare; square < kNumSquares; ++square) {
    Bitboard start(static_cast<Square>(square));
    attacks[square] = kEmptyBoard                  //
                      | start.Shift<kNorth>()      //
                      | start.Shift<kNorthEast>()  //
                      | start.Shift<kEast>()       //
                      | start.Shift<kSouthEast>()  //
                      | start.Shift<kSouth>()      //
                      | start.Shift<kSouthWest>()  //
                      | start.Shift<kWest>()       //
                      | start.Shift<kNorthWest>();
  }
  return attacks;
}

class MagicSliderAttacks {
 public:
  static constexpr Bitboard GetBishopAttacks(Square square,
                                                  Bitboard occupied) {
    const MagicEntry &magic = kSliderAttacks.bishop_magic_squares[square];
    occupied &= magic.mask;
    std::size_t index = (magic.magic * occupied.Data()) >> magic.shift;
    return kSliderAttacks.attacks[magic.attack_table_index + index];
  }

  static constexpr Bitboard GetRookAttacks(Square square,
                                                Bitboard occupied) {
    const MagicEntry &magic = kSliderAttacks.rook_magic_squares[square];
    occupied &= magic.mask;
    std::size_t index = (magic.magic * occupied.Data()) >> magic.shift;
    return kSliderAttacks.attacks[magic.attack_table_index + index];
  }
};

class LazySliderAttacks {
  public:
    static constexpr Bitboard GetBishopAttacks(Square square,
                                                  Bitboard occupied) {
    return GenerateSlidingAttacks<kNorthEast, kSouthEast, kSouthWest,
                                  kNorthWest>(square, occupied);
  }

  static constexpr Bitboard GetRookAttacks(Square square, Bitboard occupied) {
    return GenerateSlidingAttacks<kNorth, kEast, kSouth, kWest>(square,
                                                                occupied);
  }
};

template <Piece Piece, typename SliderAttacks = MagicSliderAttacks>
constexpr Bitboard GenerateAttacks(Square square, Bitboard occupied) {
  static_assert(Piece != kPawn);

  if constexpr (Piece == kKnight) {
    static constexpr std::array<Bitboard, kNumSquares> kKnightAttacks =
        GenerateKnightAttacks();
    return kKnightAttacks[square];
  }

  if constexpr (Piece == kKing) {
    static constexpr std::array<Bitboard, kNumSquares> kKingAttacks =
        GenerateKingAttacks();
    return kKingAttacks[square];
  }

  if constexpr (Piece == kBishop) {
    return SliderAttacks::GetBishopAttacks(square, occupied);
  }

  if constexpr (Piece == kRook) {
    return SliderAttacks::GetRookAttacks(square, occupied);
  }

  if constexpr (Piece == kQueen) {
    return SliderAttacks::GetBishopAttacks(square, occupied) |
           SliderAttacks::GetRookAttacks(square, occupied);
  }

  return kEmptyBoard;
}

}  // namespace follychess

#endif  // FOLLYCHESS_ATTACKS_H_

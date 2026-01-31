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

#ifndef FOLLYCHESS_TYPES_H_
#define FOLLYCHESS_TYPES_H_

#include <format>
#include <optional>
#include <string>
#include <string_view>

#include "absl/log/check.h"

namespace follychess {

// See `Bitboard` comment.
enum Square : std::uint8_t {
  // clang-format off
  A8, B8, C8, D8, E8, F8, G8, H8,
  A7, B7, C7, D7, E7, F7, G7, H7,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A1, B1, C1, D1, E1, F1, G1, H1,
  // clang-format on
};

using enum Square;

constexpr Square kFirstSquare = A8;

constexpr std::size_t kRanks = 8;
constexpr std::size_t kFiles = 8;
constexpr std::size_t kNumSquares = kRanks * kFiles;

// Parses a string of the form "a1" as a Square. Returns `std::nullopt`
// if and only if the input is invalid.
std::optional<Square> ParseSquare(std::string_view input);

constexpr std::uint8_t GetRank(Square square) { return square / 8; }

constexpr std::uint8_t GetFile(Square square) { return square % 8; }

constexpr Square MakeSquare(std::uint8_t rank, std::uint8_t file) {
  DCHECK_GE(rank, 0);
  DCHECK_LT(rank, 8);
  DCHECK_GE(file, 0);
  DCHECK_LT(file, 8);

  return static_cast<Square>(rank * 8 + file);
}

constexpr Square Reflect(const Square square) {
  const int new_tank = 7 - GetRank(square);
  return static_cast<Square>(new_tank * 8 + GetFile(square));
}

std::string ToString(Square square);

// Required for GoogleTest to print Square in error messages.
inline void PrintTo(const Square &square, std::ostream *os) {
  *os << ToString(square);
}

enum Side : std::uint8_t {
  kWhite,
  kBlack,
  kEmptySide,
};

constexpr std::size_t kNumSides = 2;

constexpr Side operator~(Side side) { return side == kWhite ? kBlack : kWhite; }

enum Direction : std::int8_t {
  kNorth = -8,
  kSouth = -kNorth,
  kEast = 1,
  kWest = -kEast,

  kNorthEast = kNorth + kEast,
  kNorthWest = kNorth + kWest,
  kSouthEast = kSouth + kEast,
  kSouthWest = kSouth + kWest,
};

enum Piece : std::uint8_t {
  kPawn,
  kKnight,
  kBishop,
  kRook,
  kQueen,
  kKing,
  kEmptyPiece,
};

// Required for GoogleTest to print Piece in error messages.
inline void PrintTo(const Piece &piece, std::ostream *os) {
  static std::string_view kPieceNames[] = {"P", "N", "B", "R", "Q", "K", "-"};
  *os << kPieceNames[piece];
}

constexpr std::size_t kNumPieces = 6;

enum class MoveType : std::uint8_t {
  kQuiet,
  kCapture,
  kEvasion,
};

using enum MoveType;

}  // namespace follychess

template <>
struct std::formatter<follychess::Square> : std::formatter<std::string> {
  static auto format(follychess::Square square, std::format_context &context) {
    return std::format_to(context.out(), "{}", ToString(square));
  }
};

#endif  // FOLLYCHESS_TYPES_H_

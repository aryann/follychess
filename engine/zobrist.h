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

#ifndef FOLLYCHESS_ZOBRIST_H_
#define FOLLYCHESS_ZOBRIST_H_

#include <optional>
#include <random>

#include "engine/castling.h"
#include "engine/types.h"

namespace follychess {

struct ZobristKeys {
  ZobristKeys();

  std::array<std::array<std::array<std::uint64_t, kNumSides>, kNumPieces>,
             kNumSquares>
      elements;
  std::array<std::uint64_t, kFiles> en_passant_files;
  std::array<std::uint64_t, kNumCastlingCombinations> castling;

  std::uint64_t black_to_move;
};

inline ZobristKeys::ZobristKeys() : elements(), en_passant_files(), castling() {
  std::mt19937 engine(std::random_device{}());
  std::uniform_int_distribution<std::uint64_t> dist(0);

  for (int i = 0; i < kNumSquares; ++i) {
    DCHECK_EQ(elements.size(), kNumSquares);

    for (int j = 0; j < kNumPieces; ++j) {
      DCHECK_EQ(elements[i].size(), kNumPieces);

      for (int k = 0; k < kNumSides; ++k) {
        DCHECK_EQ(elements[i][j].size(), kNumSides);
        elements[i][j][k] = dist(engine);
      }
    }
  }

  for (std::uint64_t& file : en_passant_files) {
    file = dist(engine);
  }

  for (std::uint64_t& combination : castling) {
    combination = dist(engine);
  }

  black_to_move = dist(engine);
}

// N.B.: ZobristKeys relies on a random number generator, so it cannot be
// defined as a constexpr variable.
inline const ZobristKeys kZobristKeys;

class ZobristKey {
 public:
  ZobristKey() : key_(0ULL) {}

  constexpr void Update(const Square square, const Piece piece,
                        const Side side) {
    key_ ^= kZobristKeys.elements[square][piece][side];
  }

  constexpr void UpdateSideToMove() { key_ ^= kZobristKeys.black_to_move; }

  constexpr void ToggleEnPassantTarget(const std::optional<Square> target) {
    if (target) {
      key_ ^= kZobristKeys.en_passant_files[GetFile(*target)];
    }
  }

  constexpr void ToggleCastlingRights(const CastlingRights& castling_rights) {
    DCHECK(castling_rights.Get() < kNumCastlingCombinations);
    key_ ^= kZobristKeys.castling[castling_rights.Get()];
  }

  [[nodiscard]] std::uint64_t GetValue() const { return key_; }

  constexpr auto operator<=>(const ZobristKey& other) const = default;

  constexpr explicit operator bool() const { return key_ != 0; }

 private:
  std::uint64_t key_;
};

}  // namespace follychess

#endif  // FOLLYCHESS_ZOBRIST_H_

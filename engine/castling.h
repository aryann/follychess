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

#ifndef FOLLYCHESS_CASTLING_RIGHTS_H_
#define FOLLYCHESS_CASTLING_RIGHTS_H_

#include <format>

#include "bitboard.h"
#include "types.h"

namespace follychess {

template <Side Side>
constexpr Bitboard GetKingSideCastlingPath() {
  static_assert(Side == kWhite || Side == kBlack);

  if constexpr (Side == kWhite) {
    return Bitboard(F1) | Bitboard(G1);
  } else {
    return Bitboard(F8) | Bitboard(G8);
  }
}

template <Side Side>
constexpr Bitboard GetQueenSideCastlingPath() {
  static_assert(Side == kWhite || Side == kBlack);

  if constexpr (Side == kWhite) {
    return Bitboard(B1) | Bitboard(C1) | Bitboard(D1);
  } else {
    return Bitboard(B8) | Bitboard(C8) | Bitboard(D8);
  }
}

class CastlingRights {
 public:
  enum Flags : std::uint8_t {
    kNoCastlingRights = /* */ 0b0000,
    kWhiteKing = /*        */ 0b0001,
    kWhiteQueen = /*       */ 0b0010,
    kBlackKing = /*        */ 0b0100,
    kBlackQueen = /*       */ 0b1000,
    kAllCastlingRights = /**/ 0b1111,
  };

  explicit CastlingRights(std::uint8_t rights = kNoCastlingRights)
      : rights_(rights) {}

  template <Side Side>
  [[nodiscard]] constexpr bool HasKingSide() const {
    static_assert(Side == kWhite || Side == kBlack);

    if constexpr (Side == kWhite) {
      return rights_ & kWhiteKing;
    } else {
      return rights_ & kBlackKing;
    }
  }

  template <Side Side>
  [[nodiscard]] constexpr bool HasQueenSide() const {
    static_assert(Side == kWhite || Side == kBlack);

    if constexpr (Side == kWhite) {
      return rights_ & kWhiteQueen;
    } else {
      return rights_ & kBlackQueen;
    }
  }

  constexpr void InvalidateOnMove(Square square) {
    static constexpr std::array<std::uint8_t, kNumSquares> kCastlingMasks = [] {
      std::array<std::uint8_t, kNumSquares> masks{};
      masks.fill(0b1111);

      // White masks:
      masks[A1] = ~kWhiteQueen;
      masks[E1] = ~(kWhiteKing | kWhiteQueen);
      masks[H1] = ~kWhiteKing;

      // Black masks:
      masks[A8] = ~kBlackQueen;
      masks[E8] = ~(kBlackKing | kBlackQueen);
      masks[H8] = ~kBlackKing;

      return masks;
    }();

    rights_ &= kCastlingMasks[square];
  }

  constexpr void Set(Flags flags) {
    rights_ |= static_cast<std::uint8_t>(flags);
  }

  [[nodiscard]] constexpr std::uint8_t Get() const { return rights_; }

  constexpr bool operator==(const CastlingRights &other) const = default;

  constexpr explicit operator bool() const {
    return rights_ != Flags::kNoCastlingRights;
  }

 private:
  std::uint8_t rights_;
};

using enum CastlingRights::Flags;

constexpr std::size_t kNumCastlingCombinations = 1 << 4;

}  // namespace follychess

template <>
struct std::formatter<follychess::CastlingRights>
    : std::formatter<std::string> {
  static auto format(follychess::CastlingRights rights,
                     std::format_context &context) {
    auto out = context.out();
    if (!rights) {
      return std::format_to(out, "-");
    }

    using enum follychess::Side;

    std::format_to(out, "{}", rights.HasKingSide<kWhite>() ? "K" : "");
    std::format_to(out, "{}", rights.HasQueenSide<kWhite>() ? "Q" : "");
    std::format_to(out, "{}", rights.HasKingSide<kBlack>() ? "k" : "");
    std::format_to(out, "{}", rights.HasQueenSide<kBlack>() ? "q" : "");
    return out;
  }
};

#endif  // FOLLYCHESS_CASTLING_RIGHTS_H_

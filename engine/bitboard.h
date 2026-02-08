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

#ifndef FOLLYCHESS_BITBOARD_H_
#define FOLLYCHESS_BITBOARD_H_

#include <bit>
#include <format>
#include <initializer_list>
#include <string>

#include "absl/log/check.h"
#include "engine/types.h"

namespace follychess {

// Represents an 8x8 chess board.
//
// Squares map to bits in the bitboard as follows, with square A8 corresponding
// to the least significant bit (LSB), and H1 to the most significant bit (MSB).
//
//   8:   0   1   2   3   4   5   6   7
//   7:   8   9  10  11  12  13  14  15
//   6:  16  17  18  19  20  21  22  23
//   5:  24  25  26  27  28  29  30  31
//   4:  32  33  34  35  36  37  38  39
//   3:  40  41  42  43  44  45  46  47
//   2:  48  49  50  51  52  53  54  55
//   1:  56  57  58  59  60  61  62  63
//       a   b   c   d   e   f   g   h
//
// This layout matches the typical visual representation of a board, improving
// code readability and debuggability.
class Bitboard {
  // ...
 public:
  constexpr explicit Bitboard(std::uint64_t data) : data_(data) {}

  constexpr Bitboard() : data_(0ULL) {}

  constexpr explicit Bitboard(Square square) : data_(1ULL << square) {}

  explicit constexpr Bitboard(std::string_view input) : data_(0ULL) {
    int square = 0;
    for (const char curr : input) {
      if (curr != '.' && curr != 'X') {
        continue;
      }
      if (curr == 'X') {
        Set(static_cast<Square>(square));
      }
      ++square;
    }
  }

  constexpr bool operator==(const Bitboard &other) const = default;

  constexpr Bitboard operator-(const Bitboard &other) const {
    return Bitboard(data_ - other.data_);
  }

  constexpr Bitboard operator&(const Bitboard &other) const {
    return Bitboard(data_ & other.data_);
  }

  constexpr Bitboard operator&(Square square) const {
    return Bitboard(data_ & (1ULL << square));
  }

  constexpr Bitboard operator|(const Bitboard &other) const {
    return Bitboard(data_ | other.data_);
  }

  constexpr Bitboard operator^(const Bitboard &other) const {
    return Bitboard(data_ ^ other.data_);
  }

  constexpr Bitboard operator~() const { return Bitboard(~data_); }

  constexpr Bitboard &operator&=(const Bitboard &other) {
    data_ &= other.data_;
    return *this;
  }

  constexpr Bitboard &operator|=(const Bitboard &other) {
    data_ |= other.data_;
    return *this;
  }

  constexpr Bitboard &operator^=(const Bitboard &other) {
    data_ ^= other.data_;
    return *this;
  }

  constexpr Bitboard operator<<(int bits) const {
    return Bitboard(data_ << bits);
  }

  constexpr Bitboard operator>>(int bits) const {
    return Bitboard(data_ >> bits);
  }

  constexpr Bitboard operator<<=(int bits) {
    data_ <<= bits;
    return *this;
  }

  constexpr Bitboard operator>>=(int bits) {
    data_ >>= bits;
    return *this;
  }

  constexpr operator bool() const { return data_ != 0; }

  constexpr auto operator<=>(const Bitboard &other) const = default;

  [[nodiscard]] constexpr bool Get(Square square) const {
    return data_ & 1ULL << square;
  }

  constexpr void Set(Square square) { data_ |= 1ULL << square; }

  constexpr void Clear(Square square) { data_ &= ~(1ULL << square); }

  // Gets the square of the least significant bit (LSB).
  //
  // Precondition: The Bitboard must not be empty.
  [[nodiscard]] constexpr Square LeastSignificantBit() const {
    if !consteval {
      DCHECK(data_ != 0ULL);
    }
    return static_cast<Square>(std::countr_zero(data_));
  }

  // Finds the least significant bit (LSB), clears it from the board,
  // and returns its corresponding square.
  //
  // Precondition: The Bitboard must not be empty.
  constexpr Square PopLeastSignificantBit() {
    Square square = LeastSignificantBit();
    data_ &= data_ - 1;
    return square;
  }

  // Returns the number of set bits (aka, population count).
  [[nodiscard]] constexpr int GetCount() const { return std::popcount(data_); }

  template <Direction D>
  [[nodiscard]] constexpr Bitboard Shift() const;

  [[nodiscard]] constexpr auto Data() const { return data_; }

 private:
  std::uint64_t data_;
};

namespace rank {

// N.B.: Integer literals used with Bitboard must be at least 64-bit to
// prevent overflow during bitwise operations. The ULL suffix ensures the
// literal is unsigned long long, which is guaranteed to be at least 64 bits.
constexpr Bitboard k8(0xFFULL);
constexpr Bitboard k7 = k8 << 8;
constexpr Bitboard k6 = k7 << 8;
constexpr Bitboard k5 = k6 << 8;
constexpr Bitboard k4 = k5 << 8;
constexpr Bitboard k3 = k4 << 8;
constexpr Bitboard k2 = k3 << 8;
constexpr Bitboard k1 = k2 << 8;

constexpr std::array kRankMasks = {k8, k7, k6, k5, k4, k3, k2, k1};

}  // namespace rank

namespace file {

constexpr Bitboard kA(0x101010101010101ULL);
constexpr Bitboard kB = kA << 1;
constexpr Bitboard kC = kB << 1;
constexpr Bitboard kD = kC << 1;
constexpr Bitboard kE = kD << 1;
constexpr Bitboard kF = kE << 1;
constexpr Bitboard kG = kF << 1;
constexpr Bitboard kH = kG << 1;

constexpr std::array kFileMasks = {kA, kB, kC, kD, kE, kF, kG, kH};

}  // namespace file

constexpr Bitboard kEmptyBoard;
constexpr Bitboard kEdges = file::kA | file::kH | rank::k1 | rank::k8;

template <Direction Direction>
constexpr Bitboard Bitboard::Shift() const {
  if constexpr (Direction == kNorth) {
    return *this >> 8;
  }

  if constexpr (Direction == kNorthEast) {
    return *this >> 7 & ~file::kA;
  }

  if constexpr (Direction == kEast) {
    return *this << 1 & ~file::kA;
  }

  if constexpr (Direction == kSouthEast) {
    return *this << 9 & ~file::kA;
  }
  if constexpr (Direction == kSouth) {
    return *this << 8;
  }

  if constexpr (Direction == kSouthWest) {
    return *this << 7 & ~file::kH;
  }

  if constexpr (Direction == kWest) {
    return *this >> 1 & ~file::kH;
  }

  if constexpr (Direction == kNorthWest) {
    return *this >> 9 & ~file::kH;
  }

  return kEmptyBoard;
}

}  // namespace follychess

// A std::formatter specialization for printing a Bitboard.
//
// See https://en.cppreference.com/w/cpp/utility/format/formatter.html for
// more details.
template <>
struct std::formatter<follychess::Bitboard> : std::formatter<std::string> {
  static auto format(follychess::Bitboard bitboard,
                     std::format_context &context) {
    auto out = context.out();

    for (int row = 0; row < 8; ++row) {
      out = std::format_to(out, "{}:", 8 - row);
      for (int col = 0; col < 8; ++col) {
        auto square = static_cast<follychess::Square>(row * 8 + col);
        if (bitboard.Get(square)) {
          out = std::format_to(out, " X");
        } else {
          out = std::format_to(out, " .");
        }
      }
      out = std::format_to(out, "\n");
    }

    out = std::format_to(out, "  ");
    for (int col = 0; col < 8; ++col) {
      out = std::format_to(out, " {:c}", 'a' + col);
    }
    out = std::format_to(out, "\n");

    return out;
  }
};

template <>
struct std::hash<follychess::Bitboard> {
  std::size_t operator()(const follychess::Bitboard &b) const noexcept {
    return std::hash<uint64_t>()(b.Data());
  }
};

#endif  // FOLLYCHESS_BITBOARD_H_

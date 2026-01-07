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

#include "move.h"

#include <ostream>

#include "absl/strings/str_split.h"

namespace follychess {
namespace {

std::optional<Move::Flags> ParsePromotionPiece(char c) {
  switch (c) {
    case 'n':
      return Move::Flags::kKnightPromotion;
    case 'b':
      return Move::Flags::kBishopPromotion;
    case 'r':
      return Move::Flags::kRookPromotion;
    case 'q':
      return Move::Flags::kQueenPromotion;
    default:
      return std::nullopt;
  }
}

std::optional<Move::Flags> ParseFlags(std::string_view input) {
  if (input == "c") {
    return Move::Flags::kCapture;
  }
  if (input == "dpp") {
    return Move::Flags::kDoublePawnPush;
  }
  if (input == "ep") {
    return Move::Flags::kEnPassantCapture;
  }
  if (input == "oo") {
    return Move::Flags::kKingCastle;
  }
  if (input == "ooo") {
    return Move::Flags::kQueenCastle;
  }
  return std::nullopt;
}

}  // namespace

std::expected<Move, std::string> Move::FromUCI(std::string_view input) {
  auto error = std::unexpected(std::format("Invalid UCI move: {}", input));
  if (input.empty()) {
    return error;
  }

  if (input == "0000") {
    return NullMove();
  }

  auto parts = std::views::split(input, '#') |
               std::ranges::to<std::vector<std::string>>();

  if (parts.size() > 2) {
    return error;
  }
  std::string_view head = parts[0];

  if (head.size() < 4) {
    return error;
  }
  if (head.size() > 5) {
    return error;
  }

  std::optional<Square> from = ParseSquare(head.substr(0, 2));
  std::optional<Square> to = ParseSquare(head.substr(2, 2));
  if (!from || !to) {
    return error;
  }

  Flags flags = kNone;
  if (parts.size() == 2) {
    std::optional<Flags> parsed = ParseFlags(parts.back());
    if (!parsed) {
      return error;
    }
    flags = *parsed;
  }

  if (head.size() == 5) {
    std::optional<Flags> promotion_flag = ParsePromotionPiece(head[4]);
    if (!promotion_flag) {
      return error;
    }
    flags = static_cast<Flags>(flags | *promotion_flag);
  }

  return Move(*from, *to, flags);
}

std::ostream &operator<<(std::ostream &os, const Move &move) {
  static bool kFull = true;
  move.FormatTo(std::ostream_iterator<char>(os), kFull);
  return os;
}

}  // namespace follychess

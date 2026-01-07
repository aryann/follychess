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

#include "testing.h"

#include <format>
#include <ostream>
#include <source_location>

#include "absl/log/check.h"
#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "bitboard.h"

namespace follychess {
namespace {

using namespace ::std::literals;

std::string TestPositionToFen(std::string_view input) {
  std::vector<std::string_view> parts =
      absl::StrSplit(input, "a b c d e f g h");

  CHECK_EQ(parts.size(), 2) << "Invalid position input: " << input;

  std::string fen_board;

  std::string_view board = parts[0];
  int square = 0;
  int empty_count = 0;
  for (char curr : board) {
    if ("PNBRQKpnbrqk."s.find(curr) == std::string::npos) {
      continue;
    }

    if (curr == '.') {
      empty_count++;
    } else {
      if (empty_count > 0) {
        fen_board += std::to_string(empty_count);
        empty_count = 0;
      }
      fen_board += curr;
    }

    if ((square + 1) % 8 == 0) {
      if (empty_count > 0) {
        fen_board += std::to_string(empty_count);
        empty_count = 0;
      }
      if (square != kNumSquares - 1) {
        fen_board += '/';
      }
    }

    ++square;
  }

  return absl::StrCat(fen_board, " ", absl::StripAsciiWhitespace(parts[1]));
}

}  // namespace

Move MakeMove(std::string_view input, std::source_location location) {
  std::expected<Move, std::string> move = Move::FromUCI(input);
  if (!move.has_value()) {
    LOG(FATAL) << location.file_name() << ":" << location.line() << ": "
               << "Invalid move: " << move.error();
  }

  return move.value();
}

std::vector<Move> MakeMoves(std::initializer_list<std::string_view> input,
                            std::source_location location) {
  std::vector<Move> moves;
  for (std::string_view curr : input) {
    moves.push_back(MakeMove(curr, location));
  }
  return moves;
}

std::expected<Position, std::string> TryMakePosition(std::string_view input) {
  return Position::FromFen(TestPositionToFen(input));
}

Position MakePosition(std::string_view input, std::source_location location) {
  std::expected<Position, std::string> position = TryMakePosition(input);
  if (!position.has_value()) {
    LOG(FATAL) << location.file_name() << ":" << location.line() << ": "
               << "Invalid position: " << position.error();
  }

  return position.value();
}

void PrintTo(const Bitboard &bitboard, std::ostream *os) {
  *os << std::format("Bitboard(0x{:})", bitboard.Data());
}

}  // namespace follychess

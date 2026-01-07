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

#include "engine/types.h"

#include <format>
#include <string>

namespace follychess {

std::optional<Square> ParseSquare(std::string_view input) {
  if (input.size() != 2) {
    return std::nullopt;
  }

  char file = input[0];
  char rank = input[1];

  if (file < 'a' || file > 'h' || rank < '1' || rank > '8') {
    return std::nullopt;
  }

  int file_index = file - 'a';
  int rank_index = rank - '0';
  int square = (8 - rank_index) * 8 + file_index;
  return static_cast<Square>(square);
}

std::string ToString(Square square) {
  int rank = 8 - square / 8;
  char file = 'a' + square % 8;
  return file + std::to_string(rank);
}

}  // namespace follychess

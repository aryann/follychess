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

#ifndef FOLLYCHESS_TESTING_H_
#define FOLLYCHESS_TESTING_H_

#include <gmock/gmock.h>

#include <format>
#include <iostream>
#include <source_location>
#include <string>
#include <variant>

#include "bitboard.h"
#include "position.h"

namespace follychess {

std::expected<Position, std::string> TryMakePosition(std::string_view input);

Position MakePosition(
    std::string_view input,
    std::source_location location = std::source_location::current());

Move MakeMove(std::string_view input,
              std::source_location location = std::source_location::current());

std::vector<Move> MakeMoves(
    std::initializer_list<std::string_view> input,
    std::source_location location = std::source_location::current());

void PrintTo(const Bitboard &bitboard, std::ostream *os);

MATCHER_P(EqualsBitboard, expected,
          std::format("Bitboard(0x{:x})", Bitboard(expected).Data())) {
  Bitboard expected_bitboard = Bitboard(expected);
  Bitboard actual_bitboard = arg;
  if (actual_bitboard == expected_bitboard) {
    return true;
  }

  auto expected_parts =
      std::views::split(std::format("{}", expected_bitboard), '\n') |
      std::ranges::to<std::vector<std::string>>();
  auto actual_parts =
      std::views::split(std::format("{}", actual_bitboard), '\n') |
      std::ranges::to<std::vector<std::string>>();

  *result_listener << "\n\n"
                   << "      Expected:                Actual:\n"
                   << "      ---------                -------";

  for (int i = 0; i < expected_parts.size(); ++i) {
    *result_listener << '\n'
                     << expected_parts[i] << "      " << actual_parts[i];
    if (expected_parts[i] != actual_parts[i]) {
      *result_listener << "  <-";
    }
  }

  return false;
}

MATCHER_P(EqualsPosition, expected, "") {
  std::expected<Position, std::string> result = TryMakePosition(expected);
  if (!result.has_value()) {
    *result_listener << "could not parse position: " << result.error();
    return false;
  }

  const Position &expected_position = result.value();
  const Position &actual_position = arg;

  if (expected_position == actual_position) {
    return true;
  }

  auto expected_parts =
      std::views::split(std::format("{:k}", expected_position), '\n') |
      std::ranges::to<std::vector<std::string>>();
  auto actual_parts =
      std::views::split(std::format("{:k}", actual_position), '\n') |
      std::ranges::to<std::vector<std::string>>();

  *result_listener << "\n\n"
                   << "      Expected:                  Actual:\n"
                   << "      ---------                  -------";

  int max_line_length = std::ssize(expected_parts.front()) + 2;
  for (int i = 0; i < expected_parts.size(); ++i) {
    *result_listener << '\n' << expected_parts[i];

    for (int j = 0; j < max_line_length - std::ssize(expected_parts[i]); ++j) {
      *result_listener << ' ';
    }

    *result_listener << "      " << actual_parts[i];
    if (expected_parts[i] != actual_parts[i]) {
      *result_listener << "  <-";
    }
  }

  return false;
}

}  // namespace follychess

#endif  // FOLLYCHESS_TESTING_H_

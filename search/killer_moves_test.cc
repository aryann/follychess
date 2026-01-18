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

#include "search/killer_moves.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "engine/move.h"
#include "engine/testing.h"

namespace follychess {
namespace {
using ::testing::Eq;

TEST(KillerMoves, Basic) {
  KillerMoves moves;

  {
    auto [first, second] = moves[20];
    EXPECT_THAT(first, Eq(MakeMove("0000")));
    EXPECT_THAT(second, Eq(MakeMove("0000")));
  }

  {
    moves.Set(1, MakeMove("d2d4"));
    auto [first, second] = moves[1];
    EXPECT_THAT(first, Eq(MakeMove("d2d4")));
    EXPECT_THAT(second, Eq(MakeMove("0000")));
  }

  {
    moves.Set(1, MakeMove("b1c3"));
    auto [first, second] = moves[1];
    EXPECT_THAT(first, Eq(MakeMove("b1c3")));
    EXPECT_THAT(second, Eq(MakeMove("d2d4")));
  }

  {
    moves.Set(1, MakeMove("g1f3"));
    auto [first, second] = moves[1];
    EXPECT_THAT(first, Eq(MakeMove("g1f3")));
    EXPECT_THAT(second, Eq(MakeMove("b1c3")));
  }
}

}  // namespace
}  // namespace follychess

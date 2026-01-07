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

#include "search/phase.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "engine/position.h"
#include "engine/testing.h"

namespace follychess {
namespace {

using ::testing::ElementsAreArray;
using ::testing::Eq;

TEST(CalculatePhase, StartingPosition) {
  EXPECT_THAT(CalculatePhase(Position::Starting()), Eq(0));
}

TEST(CalculatePhase, Mid) {
  Position position = MakePosition(
      "8: . . . k . . . ."
      "7: r . . . . . r ."
      "6: . . . . . . . ."
      "5: . . . . . . . ."
      "4: . . . . . . . ."
      "3: . R . . . . . ."
      "2: . . . . . . . ."
      "1: . R . . . . . K"
      "   a b c d e f g h"
      //
      "   w - - 0 1");

  EXPECT_THAT(CalculatePhase(position), Eq(171));
}

TEST(CalculatePhase, KingsOnly) {
  Position position = MakePosition(
      "8: . . . k . . . ."
      "7: . . . . . . . ."
      "6: . . . . . . . ."
      "5: . . . . . . . ."
      "4: . . . . . . . ."
      "3: . . . . . . . ."
      "2: . . . . . . . ."
      "1: . . . . . . . K"
      "   a b c d e f g h"
      //
      "   w - - 0 1");

  EXPECT_THAT(CalculatePhase(position), Eq(256));
}

}  // namespace
}  // namespace follychess
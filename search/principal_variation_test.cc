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

#include "search/principal_variation.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "engine/move.h"
#include "engine/testing.h"

namespace follychess {
namespace {

using ::testing::ElementsAreArray;
using ::testing::Eq;

TEST(PrincipalVariationTable, SinglePly) {
  PrincipalVariationTable table;
  table.RecordMove(0, MakeMove("e2e4"));

  EXPECT_THAT(table.GetPrincipalVariation(),
              ElementsAreArray(MakeMoves({"e2e4"})));
}

TEST(PrincipalVariationTable, MultiplePlies) {
  PrincipalVariationTable table;
  table.RecordMove(3, MakeMove("b5e3"));
  table.RecordMove(2, MakeMove("b1c3"));
  table.RecordMove(1, MakeMove("d7d5"));
  table.RecordMove(0, MakeMove("e2e4"));

  EXPECT_THAT(table.GetPrincipalVariation(),
              ElementsAreArray(MakeMoves({"e2e4", "d7d5", "b1c3", "b5e3"})));
}

TEST(PrincipalVariationTable, Replacement) {
  PrincipalVariationTable table;
  table.RecordMove(3, MakeMove("b5e3"));
  table.RecordMove(2, MakeMove("b1c3"));
  table.RecordMove(1, MakeMove("d7d5"));
  table.RecordMove(0, MakeMove("e2e4"));

  EXPECT_THAT(table.GetPrincipalVariation(),
              ElementsAreArray(MakeMoves({"e2e4", "d7d5", "b1c3", "b5e3"})));

  // Overwrite the previous sequence.
  table.RecordMove(2, Move::NullMove());
  table.RecordMove(1, MakeMove("d7d5"));
  table.RecordMove(0, MakeMove("d2d4"));

  EXPECT_THAT(table.GetPrincipalVariation(),
              ElementsAreArray(MakeMoves({"d2d4", "d7d5"})));
}

}  // namespace
}  // namespace follychess

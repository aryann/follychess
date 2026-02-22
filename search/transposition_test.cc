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

#include "search/transposition.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace follychess {
namespace {
using ::testing::ElementsAreArray;
using ::testing::Eq;
using ::testing::HasSubstr;
using ::testing::Optional;
using ::testing::SizeIs;

TEST(TranspositionTable, TableSize) {
  EXPECT_THAT(TranspositionTable(1).size(), Eq(1 << 14));

  EXPECT_THAT(TranspositionTable(94).size(), Eq(1 << 20));
  EXPECT_THAT(TranspositionTable(95).size(), Eq(1 << 20));

  EXPECT_THAT(TranspositionTable(96).size(), Eq(1 << 21));
  EXPECT_THAT(TranspositionTable(97).size(), Eq(1 << 21));
  EXPECT_THAT(TranspositionTable(125).size(), Eq(1 << 21));
  EXPECT_THAT(TranspositionTable(126).size(), Eq(1 << 21));
  EXPECT_THAT(TranspositionTable(127).size(), Eq(1 << 21));
  EXPECT_THAT(TranspositionTable(128).size(), Eq(1 << 21));
  EXPECT_THAT(TranspositionTable(130).size(), Eq(1 << 21));

  EXPECT_THAT(TranspositionTable(256).size(), Eq(1 << 22));
}

}  // namespace
}  // namespace follychess

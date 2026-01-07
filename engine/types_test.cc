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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>

#include "engine/testing.h"

namespace follychess {
namespace {

using ::testing::Eq;
using ::testing::Optional;

TEST(Square, ParseSquare) {
  EXPECT_THAT(ParseSquare("a8"), Optional(Eq(A8)));
  EXPECT_THAT(ParseSquare("h8"), Optional(Eq(H8)));
  EXPECT_THAT(ParseSquare("d5"), Optional(Eq(D5)));
  EXPECT_THAT(ParseSquare("a1"), Optional(Eq(A1)));
  EXPECT_THAT(ParseSquare("d1"), Optional(Eq(D1)));
  EXPECT_THAT(ParseSquare("h1"), Optional(Eq(H1)));

  EXPECT_THAT(ParseSquare("aa8"), Eq(std::nullopt));
  EXPECT_THAT(ParseSquare("h11"), Eq(std::nullopt));
  EXPECT_THAT(ParseSquare("A1"), Eq(std::nullopt));
  EXPECT_THAT(ParseSquare("a"), Eq(std::nullopt));
  EXPECT_THAT(ParseSquare(""), Eq(std::nullopt));
}

TEST(Square, ToString) {
  EXPECT_THAT(ToString(A8), Eq("a8"));
  EXPECT_THAT(ToString(H1), Eq("h1"));
  EXPECT_THAT(ToString(D5), Eq("d5"));
  EXPECT_THAT(ToString(A1), Eq("a1"));
  EXPECT_THAT(ToString(H1), Eq("h1"));
}

TEST(Square, Reflect) {
  EXPECT_THAT(Reflect(A8), A1);
  EXPECT_THAT(Reflect(H1), H8);
  EXPECT_THAT(Reflect(D5), D4);
  EXPECT_THAT(Reflect(A1), A8);
  EXPECT_THAT(Reflect(H3), H6);
}

}  // namespace
}  // namespace follychess

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

#include "engine/castling.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <format>

#include "engine/testing.h"

namespace follychess {
namespace {

using ::testing::Eq;
using ::testing::IsFalse;
using ::testing::IsTrue;

TEST(Constructor, NoRights) {
  CastlingRights rights;

  EXPECT_THAT(std::format("{}", rights), Eq("-"));
  EXPECT_THAT(rights.HasKingSide<kWhite>(), IsFalse());
  EXPECT_THAT(rights.HasQueenSide<kWhite>(), IsFalse());
  EXPECT_THAT(rights.HasKingSide<kBlack>(), IsFalse());
  EXPECT_THAT(rights.HasQueenSide<kBlack>(), IsFalse());
}

TEST(Constructor, AllRights) {
  CastlingRights rights(kAllCastlingRights);

  EXPECT_THAT(std::format("{}", rights), Eq("KQkq"));
  EXPECT_THAT(rights.HasKingSide<kWhite>(), IsTrue());
  EXPECT_THAT(rights.HasQueenSide<kWhite>(), IsTrue());
  EXPECT_THAT(rights.HasKingSide<kBlack>(), IsTrue());
  EXPECT_THAT(rights.HasQueenSide<kBlack>(), IsTrue());
}

TEST(InvalidateOnMove, RookMoves) {
  CastlingRights rights(kAllCastlingRights);

  EXPECT_THAT(std::format("{}", rights), Eq("KQkq"));

  rights.InvalidateOnMove(A1);
  EXPECT_THAT(std::format("{}", rights), Eq("Kkq"));

  rights.InvalidateOnMove(H1);
  EXPECT_THAT(std::format("{}", rights), Eq("kq"));

  rights.InvalidateOnMove(A8);
  EXPECT_THAT(std::format("{}", rights), Eq("k"));

  rights.InvalidateOnMove(H8);
  EXPECT_THAT(std::format("{}", rights), Eq("-"));
}

TEST(InvalidateOnMove, KingMoves) {
  CastlingRights rights(kAllCastlingRights);

  EXPECT_THAT(std::format("{}", rights), Eq("KQkq"));

  rights.InvalidateOnMove(E1);
  EXPECT_THAT(std::format("{}", rights), Eq("kq"));

  rights.InvalidateOnMove(E8);
  EXPECT_THAT(std::format("{}", rights), Eq("-"));
}

TEST(GetKingSideCastlingPath, White) {
  EXPECT_THAT(GetKingSideCastlingPath<kWhite>(),
              EqualsBitboard("8: . . . . . . . ."
                             "7: . . . . . . . ."
                             "6: . . . . . . . ."
                             "5: . . . . . . . ."
                             "4: . . . . . . . ."
                             "3: . . . . . . . ."
                             "2: . . . . . . . ."
                             "1: . . . . . X X ."
                             "   a b c d e f g h"));
}

TEST(GetKingSideCastlingPath, Black) {
  EXPECT_THAT(GetKingSideCastlingPath<kBlack>(),
              EqualsBitboard("8: . . . . . X X ."
                             "7: . . . . . . . ."
                             "6: . . . . . . . ."
                             "5: . . . . . . . ."
                             "4: . . . . . . . ."
                             "3: . . . . . . . ."
                             "2: . . . . . . . ."
                             "1: . . . . . . . ."
                             "   a b c d e f g h"));
}

TEST(GetQueenSideCastlingPath, White) {
  EXPECT_THAT(GetQueenSideCastlingPath<kWhite>(),
              EqualsBitboard("8: . . . . . . . ."
                             "7: . . . . . . . ."
                             "6: . . . . . . . ."
                             "5: . . . . . . . ."
                             "4: . . . . . . . ."
                             "3: . . . . . . . ."
                             "2: . . . . . . . ."
                             "1: . X X X . . . ."
                             "   a b c d e f g h"));
}

TEST(GetQueenSideCastlingPath, Black) {
  EXPECT_THAT(GetQueenSideCastlingPath<kBlack>(),
              EqualsBitboard("8: . X X X . . . ."
                             "7: . . . . . . . ."
                             "6: . . . . . . . ."
                             "5: . . . . . . . ."
                             "4: . . . . . . . ."
                             "3: . . . . . . . ."
                             "2: . . . . . . . ."
                             "1: . . . . . . . ."
                             "   a b c d e f g h"));
}

}  // namespace
}  // namespace follychess

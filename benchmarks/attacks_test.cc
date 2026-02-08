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

#include "benchmarks/attacks.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <map>

#include "engine/testing.h"

namespace follychess {
namespace {

using testing::Eq;

//================================================================================
// GenerateAttacksOnTheFly Tests
//================================================================================

TEST(GenerateAttacksOnTheFly, Bishop) {
  {
    Bitboard blockers = kEmptyBoard;

    EXPECT_THAT(GenerateAttacksOnTheFly<kBishop>(D5, blockers),
                EqualsBitboard("8: X . . . . . X ."
                               "7: . X . . . X . ."
                               "6: . . X . X . . ."
                               "5: . . . . . . . ."
                               "4: . . X . X . . ."
                               "3: . X . . . X . ."
                               "2: X . . . . . X ."
                               "1: . . . . . . . X"
                               "   a b c d e f g h"));
  }

  {
    Bitboard blockers(
        "8: . . . . . . . X"
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . X . ."
        "3: X . . . X . . ."
        "2: . . . . . . . ."
        "1: . . . . . . . X"
        "   a b c d e f g h");

    EXPECT_THAT(GenerateAttacksOnTheFly<kBishop>(C1, blockers),
                EqualsBitboard("8: . . . . . . . ."
                               "7: . . . . . . . ."
                               "6: . . . . . . . ."
                               "5: . . . . . . . ."
                               "4: . . . . . . . ."
                               "3: X . . . X . . ."
                               "2: . X . X . . . ."
                               "1: . . . . . . . ."
                               "   a b c d e f g h"));
  }
}

TEST(GenerateAttacksOnTheFly, Rook) {
  {
    Bitboard blockers(
        "8: . . . . . . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: X . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: . . . X . . . ."
        "   a b c d e f g h");

    EXPECT_THAT(GenerateAttacksOnTheFly<kRook>(A1, blockers),
                EqualsBitboard("8: . . . . . . . ."
                               "7: . . . . . . . ."
                               "6: . . . . . . . ."
                               "5: . . . . . . . ."
                               "4: X . . . . . . ."
                               "3: X . . . . . . ."
                               "2: X . . . . . . ."
                               "1: . X X X . . . ."
                               "   a b c d e f g h"));
  }

  {
    Bitboard blockers(
        "8: . . . . . . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . X . X . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: . . . . . . . ."
        "   a b c d e f g h");

    EXPECT_THAT(GenerateAttacksOnTheFly<kRook>(A4, blockers),
                EqualsBitboard("8: X . . . . . . ."
                               "7: X . . . . . . ."
                               "6: X . . . . . . ."
                               "5: X . . . . . . ."
                               "4: . X X . . . . ."
                               "3: X . . . . . . ."
                               "2: X . . . . . . ."
                               "1: X . . . . . . ."
                               "   a b c d e f g h"));
  }

  {
    Bitboard blockers(
        "8: . . . X . . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . X . X . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: . . . . . . . ."
        "   a b c d e f g h");

    EXPECT_THAT(GenerateAttacksOnTheFly<kRook>(D1, blockers),
                EqualsBitboard("8: . . . X . . . ."
                               "7: . . . X . . . ."
                               "6: . . . X . . . ."
                               "5: . . . X . . . ."
                               "4: . . . X . . . ."
                               "3: . . . X . . . ."
                               "2: . . . X . . . ."
                               "1: X X X . X X X X"
                               "   a b c d e f g h"));
  }
}

TEST(GenerateAttacksOnTheFly, Queen) {
  {
    Bitboard blockers(
        "8: . . . . . . . ."
        "7: . . . . . . . ."
        "6: . . X . . . . ."
        "5: . . . . . . . ."
        "4: . . . X . . . ."
        "3: . . . . . X . ."
        "2: . . . . . . . ."
        "1: . . . . . . . ."
        "   a b c d e f g h");

    EXPECT_THAT(GenerateAttacksOnTheFly<kQueen>(D5, blockers),
                EqualsBitboard("8: . . . X . . X ."
                               "7: . . . X . X . ."
                               "6: . . X X X . . ."
                               "5: X X X . X X X X"
                               "4: . . X X X . . ."
                               "3: . X . . . X . ."
                               "2: X . . . . . . ."
                               "1: . . . . . . . ."
                               "   a b c d e f g h"));
  }

  {
    Bitboard blockers(
        "8: X X X X X X X X"
        "7: X X X X X X X X"
        "6: X X X X X X X X"
        "5: X X X X X X X X"
        "4: X X X X X X X X"
        "3: X X X X X X X X"
        "2: X X X X X X X X"
        "1: X X X X X X X X"
        "   a b c d e f g h");

    EXPECT_THAT(GenerateAttacksOnTheFly<kQueen>(E4, blockers),
                EqualsBitboard("8: . . . . . . . ."
                               "7: . . . . . . . ."
                               "6: . . . . . . . ."
                               "5: . . . X X X . ."
                               "4: . . . X . X . ."
                               "3: . . . X X X . ."
                               "2: . . . . . . . ."
                               "1: . . . . . . . ."
                               "   a b c d e f g h"));
  }
}

//================================================================================
// GetAttacksFromMap Tests
//================================================================================

TEST(GetAttacksFromMap, Bishop) {
  {
    Bitboard blockers = kEmptyBoard;

    EXPECT_THAT((GetAttacksFromMap<std::map, kBishop>(D5, blockers)),
                EqualsBitboard("8: X . . . . . X ."
                               "7: . X . . . X . ."
                               "6: . . X . X . . ."
                               "5: . . . . . . . ."
                               "4: . . X . X . . ."
                               "3: . X . . . X . ."
                               "2: X . . . . . X ."
                               "1: . . . . . . . X"
                               "   a b c d e f g h"));
  }

  {
    Bitboard blockers(
        "8: . . . . . . . X"
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . X . ."
        "3: X . . . X . . ."
        "2: . . . . . . . ."
        "1: . . . . . . . X"
        "   a b c d e f g h");

    EXPECT_THAT((GetAttacksFromMap<std::map, kBishop>(C1, blockers)),
                EqualsBitboard("8: . . . . . . . ."
                               "7: . . . . . . . ."
                               "6: . . . . . . . ."
                               "5: . . . . . . . ."
                               "4: . . . . . . . ."
                               "3: X . . . X . . ."
                               "2: . X . X . . . ."
                               "1: . . . . . . . ."
                               "   a b c d e f g h"));
  }
}

TEST(GetAttacksFromMap, Rook) {
  {
    Bitboard blockers(
        "8: . . . . . . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: X . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: . . . X . . . ."
        "   a b c d e f g h");

    EXPECT_THAT((GetAttacksFromMap<std::map, kRook>(A1, blockers)),
                EqualsBitboard("8: . . . . . . . ."
                               "7: . . . . . . . ."
                               "6: . . . . . . . ."
                               "5: . . . . . . . ."
                               "4: X . . . . . . ."
                               "3: X . . . . . . ."
                               "2: X . . . . . . ."
                               "1: . X X X . . . ."
                               "   a b c d e f g h"));
  }

  {
    Bitboard blockers(
        "8: . . . . . . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . X . X . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: . . . . . . . ."
        "   a b c d e f g h");

    EXPECT_THAT((GetAttacksFromMap<std::map, kRook>(A4, blockers)),
                EqualsBitboard("8: X . . . . . . ."
                               "7: X . . . . . . ."
                               "6: X . . . . . . ."
                               "5: X . . . . . . ."
                               "4: . X X . . . . ."
                               "3: X . . . . . . ."
                               "2: X . . . . . . ."
                               "1: X . . . . . . ."
                               "   a b c d e f g h"));
  }

  {
    Bitboard blockers(
        "8: . . . X . . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . X . X . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: . . . . . . . ."
        "   a b c d e f g h");

    EXPECT_THAT((GetAttacksFromMap<std::map, kRook>(D1, blockers)),
                EqualsBitboard("8: . . . X . . . ."
                               "7: . . . X . . . ."
                               "6: . . . X . . . ."
                               "5: . . . X . . . ."
                               "4: . . . X . . . ."
                               "3: . . . X . . . ."
                               "2: . . . X . . . ."
                               "1: X X X . X X X X"
                               "   a b c d e f g h"));
  }
}

TEST(GetAttacksFromMap, Queen) {
  {
    Bitboard blockers(
        "8: . . . . . . . ."
        "7: . . . . . . . ."
        "6: . . X . . . . ."
        "5: . . . . . . . ."
        "4: . . . X . . . ."
        "3: . . . . . X . ."
        "2: . . . . . . . ."
        "1: . . . . . . . ."
        "   a b c d e f g h");

    EXPECT_THAT((GetAttacksFromMap<std::map, kQueen>(D5, blockers)),
                EqualsBitboard("8: . . . X . . X ."
                               "7: . . . X . X . ."
                               "6: . . X X X . . ."
                               "5: X X X . X X X X"
                               "4: . . X X X . . ."
                               "3: . X . . . X . ."
                               "2: X . . . . . . ."
                               "1: . . . . . . . ."
                               "   a b c d e f g h"));
  }

  {
    Bitboard blockers(
        "8: X X X X X X X X"
        "7: X X X X X X X X"
        "6: X X X X X X X X"
        "5: X X X X X X X X"
        "4: X X X X X X X X"
        "3: X X X X X X X X"
        "2: X X X X X X X X"
        "1: X X X X X X X X"
        "   a b c d e f g h");

    EXPECT_THAT((GetAttacksFromMap<std::map, kQueen>(E4, blockers)),
                EqualsBitboard("8: . . . . . . . ."
                               "7: . . . . . . . ."
                               "6: . . . . . . . ."
                               "5: . . . X X X . ."
                               "4: . . . X . X . ."
                               "3: . . . X X X . ."
                               "2: . . . . . . . ."
                               "1: . . . . . . . ."
                               "   a b c d e f g h"));
  }
}

}  // namespace
}  // namespace follychess

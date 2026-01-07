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

#include "engine/move_generator.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "engine/position.h"
#include "engine/testing.h"

namespace follychess {
namespace {

using ::testing::AllOf;
using ::testing::Contains;
using ::testing::Eq;
using ::testing::IsEmpty;
using ::testing::Not;
using ::testing::UnorderedElementsAre;
using ::testing::UnorderedElementsAreArray;

TEST(GenerateMoves, QuietMoves_StartingPosition) {
  auto position = Position::Starting();

  EXPECT_THAT(GenerateMoves<kQuiet>(position),
              UnorderedElementsAreArray(MakeMoves({
                  "a2a3",      //
                  "a2a4#dpp",  //
                  "b1a3",      //
                  "b1c3",      //
                  "b2b3",      //
                  "b2b4#dpp",  //
                  "c2c3",      //
                  "c2c4#dpp",  //
                  "d2d3",      //
                  "d2d4#dpp",  //
                  "e2e3",      //
                  "e2e4#dpp",  //
                  "f2f3",      //
                  "f2f4#dpp",  //
                  "g1f3",      //
                  "g1h3",      //
                  "g2g3",      //
                  "g2g4#dpp",  //
                  "h2h3",      //
                  "h2h4#dpp",  //
              })));
}

TEST(GenerateMoves, QuietMoves_StartingPosition2) {
  Position position = MakePosition(
      "8: r n b q k b n r"
      "7: p p . p p p p p"
      "6: . . . . . . . ."
      "5: . . p . . . . ."
      "4: . . . . P . . ."
      "3: . . . . . . . ."
      "2: P P P P . P P P"
      "1: R N B Q K B N R"
      "   a b c d e f g h"
      //
      "   w KQkq c6 0 2");

  EXPECT_THAT(GenerateMoves<kQuiet>(position),
              UnorderedElementsAreArray(MakeMoves({
                  "a2a3",      //
                  "a2a4#dpp",  //
                  "b1a3",      //
                  "b1c3",      //
                  "b2b3",      //
                  "b2b4#dpp",  //
                  "c2c3",      //
                  "c2c4#dpp",  //
                  "d1e2",      //
                  "d1f3",      //
                  "d1g4",      //
                  "d1h5",      //
                  "d2d3",      //
                  "d2d4#dpp",  //
                  "e1e2",      //
                  "e4e5",      //
                  "f1a6",      //
                  "f1b5",      //
                  "f1c4",      //
                  "f1d3",      //
                  "f1e2",      //
                  "f2f3",      //
                  "f2f4#dpp",  //
                  "g1e2",      //
                  "g1f3",      //
                  "g1h3",      //
                  "g2g3",      //
                  "g2g4#dpp",  //
                  "h2h3",      //
                  "h2h4#dpp",  //
              })));
}

TEST(Pawns, QuietPromotions) {
  {
    Position position = MakePosition(
        "8: . . . . . . . ."
        "7: . . . . P . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: . . . . . . . ."
        "   a b c d e f g h"
        //
        "   w - - 0 1");

    EXPECT_THAT(GenerateMoves<kQuiet>(position),
                UnorderedElementsAreArray(MakeMoves({
                    "e7e8n",
                    "e7e8b",
                    "e7e8r",
                    "e7e8q",
                })));
  }
  {
    Position position = MakePosition(
        "8: . . . . . . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: p . . . . . . ."
        "1: . . . . . . . ."
        "   a b c d e f g h"
        //
        "   b - - 0 1");

    EXPECT_THAT(GenerateMoves<kQuiet>(position),
                UnorderedElementsAreArray(MakeMoves({
                    "a2a1n",
                    "a2a1b",
                    "a2a1r",
                    "a2a1q",
                })));
  }
}

TEST(Pawns, Captures) {
  {
    Position position = MakePosition(
        "8: . . . . . . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . p . r . ."
        "3: . . . . P . . ."
        "2: . . . . . . . ."
        "1: . . . . . . . ."
        "   a b c d e f g h"
        //
        "   w - - 0 1");

    EXPECT_THAT(GenerateMoves<kCapture>(position),
                UnorderedElementsAreArray(MakeMoves({
                    "e3d4#c",
                    "e3f4#c",
                })));
  }
  {
    Position position = MakePosition(
        "8: . . . . . . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . p . . . ."
        "4: . . P . R . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: . . . . . . . ."
        "   a b c d e f g h"
        //
        "   b - - 0 1");

    EXPECT_THAT(GenerateMoves<kCapture>(position),
                UnorderedElementsAreArray(MakeMoves({
                    "d5c4#c",
                    "d5e4#c",
                })));
  }
  {
    Position position = MakePosition(
        "8: . . . r . r . ."
        "7: . . . . P . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: . . . . . . . ."
        "   a b c d e f g h"
        //
        "   w - - 0 1");

    EXPECT_THAT(GenerateMoves<kCapture>(position),
                UnorderedElementsAreArray(MakeMoves({
                    "e7d8n#c",
                    "e7d8b#c",
                    "e7d8r#c",
                    "e7d8q#c",
                    "e7f8n#c",
                    "e7f8b#c",
                    "e7f8r#c",
                    "e7f8q#c",
                })));
  }
}

TEST(Pawns, EnPassant) {
  {
    Position position = MakePosition(
        "8: . . . . . . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . p P . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: . . . . . . . ."
        "   a b c d e f g h"
        //
        "   w - e6 0 1");

    EXPECT_THAT(GenerateMoves<kQuiet>(position),
                UnorderedElementsAreArray(MakeMoves({
                    "f5f6",
                })));
    EXPECT_THAT(GenerateMoves<kCapture>(position),
                UnorderedElementsAreArray(MakeMoves({
                    "f5e6#ep",
                })));
  }
  {
    Position position = MakePosition(
        "8: . . . . . . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . P p . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: . . . . . . . ."
        "   a b c d e f g h"
        //
        "   w - e6 0 1");

    EXPECT_THAT(GenerateMoves<kQuiet>(position),
                UnorderedElementsAreArray(MakeMoves({
                    "d5d6",
                })));
    EXPECT_THAT(GenerateMoves<kCapture>(position),
                UnorderedElementsAreArray(MakeMoves({
                    "d5e6#ep",
                })));
  }

  {
    Position position = MakePosition(
        "8: . . . . . . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: P p . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: . . . . . . . ."
        "   a b c d e f g h"
        //
        "   b - a3 1 1");

    EXPECT_THAT(GenerateMoves<kQuiet>(position),
                UnorderedElementsAreArray(MakeMoves({
                    "b4b3",
                })));
    EXPECT_THAT(GenerateMoves<kCapture>(position),
                UnorderedElementsAreArray(MakeMoves({
                    "b4a3#ep",
                })));
  }

  {
    Position position = MakePosition(
        "8: . . . . . . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . . p P"
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: . . . . . . . ."
        "   a b c d e f g h"
        //
        "   b - h3 1 1");

    EXPECT_THAT(GenerateMoves<kQuiet>(position),
                UnorderedElementsAreArray(MakeMoves({
                    "g4g3",
                })));
    EXPECT_THAT(GenerateMoves<kCapture>(position),
                UnorderedElementsAreArray(MakeMoves({
                    "g4h3#ep",
                })));
  }
}

TEST(Knights, QuietMoves) {
  {
    Position position = MakePosition(
        "8: . . . . . . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . N . . ."
        "2: . . . . . . . ."
        "1: . . . . . . . ."
        "   a b c d e f g h"
        //
        "   w - - 0 1");

    EXPECT_THAT(GenerateMoves<kQuiet>(position),
                UnorderedElementsAreArray(MakeMoves({
                    "e3d5",
                    "e3f5",
                    "e3c4",
                    "e3g4",
                    "e3c2",
                    "e3g2",
                    "e3d1",
                    "e3f1",
                })));
  }
  {
    Position position = MakePosition(
        "8: . . . . . . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . r . r . ."
        "4: . . . . . . . ."
        "3: . . . . N . . ."
        "2: . . . . . . . ."
        "1: . . . r . r . ."
        "   a b c d e f g h"
        //
        "   w - - 0 1");

    EXPECT_THAT(GenerateMoves<kQuiet>(position),
                UnorderedElementsAreArray(MakeMoves({
                    "e3c2",
                    "e3c4",
                    "e3g2",
                    "e3g4",
                })));
  }
}

TEST(Knights, Captures) {
  {
    Position position = MakePosition(
        "8: . . . . . . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . N . . ."
        "2: . . . . . . . ."
        "1: . . . . . . . ."
        "   a b c d e f g h"
        //
        "   w - - 0 1");

    EXPECT_THAT(GenerateMoves<kCapture>(position), IsEmpty());
  }
  {
    Position position = MakePosition(
        "8: . . . . . . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . b . b . ."
        "4: . . . . . . . ."
        "3: . . . . N . . ."
        "2: . . . . . . . ."
        "1: . . . r . r . ."
        "   a b c d e f g h"
        //
        "   w - - 12 20");

    EXPECT_THAT(GenerateMoves<kCapture>(position),
                UnorderedElementsAreArray(MakeMoves({
                    "e3d1#c",
                    "e3d5#c",
                    "e3f1#c",
                    "e3f5#c",
                })));
  }
}

TEST(Bishops, QuietMoves) {
  Position position = MakePosition(
      "8: . . . . . . . ."
      "7: . . . . . . . ."
      "6: . p . . . p . ."
      "5: . . . . . . . ."
      "4: . . . B . . . ."
      "3: . . . . . . . ."
      "2: . p . . . r . ."
      "1: . . . . . . . ."
      "   a b c d e f g h"
      //
      "   w - - 0 1");

  EXPECT_THAT(GenerateMoves<kQuiet>(position),
              UnorderedElementsAreArray(MakeMoves({
                  "d4c5",
                  "d4c3",
                  "d4e5",
                  "d4e3",
              })));
}

TEST(Bishops, Captures) {
  Position position = MakePosition(
      "8: . . . . . . . ."
      "7: . . . . . . . ."
      "6: . p . . . p . ."
      "5: . . . . . . . ."
      "4: . . . B . . . ."
      "3: . . . . . . . ."
      "2: . p . . . r . ."
      "1: . . . . . . . ."
      "   a b c d e f g h"
      //
      "   w - - 12 20");

  EXPECT_THAT(GenerateMoves<kCapture>(position),
              UnorderedElementsAreArray(MakeMoves({
                  "d4b6#c",
                  "d4b2#c",
                  "d4f6#c",
                  "d4f2#c",
              })));
}

// N.B.: King quiet moves can place the king in check. The engine must undo
// moves that place the king in check.
TEST(King, QuietMoves) {
  {
    Position position = MakePosition(
        "8: . . . . . . . ."
        "7: . . . . . . . ."
        "6: . . . . . p . ."
        "5: . . . . . . . ."
        "4: . . . K . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: . n . . . . . ."
        "   a b c d e f g h"
        //
        "   w - - 0 1");

    EXPECT_THAT(GenerateMoves<kQuiet>(position),
                UnorderedElementsAreArray(MakeMoves({
                    "d4d5",
                    "d4e4",
                    "d4e5",
                    "d4e3",
                    "d4d3",
                    "d4c3",
                    "d4c4",
                    "d4c5",
                })));
  }
  {
    Position position = MakePosition(
        "8: . . . . . . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . k . . . ."
        "3: . . . . . . . ."
        "2: . . . . . P . ."
        "1: . N . . . . . ."
        "   a b c d e f g h"
        //
        "   b - - 0 1");

    EXPECT_THAT(GenerateMoves<kQuiet>(position),
                UnorderedElementsAreArray(MakeMoves({
                    "d4d5",
                    "d4e5",
                    "d4e4",
                    "d4e3",
                    "d4d3",
                    "d4c3",
                    "d4c4",
                    "d4c5",
                })));
  }
}

// N.B.: Evasions can still leave the king in check. The engine must undo
// moves that place the king in check.
TEST(King, Evasions) {
  {
    Position position = MakePosition(
        "8: . . . . . . . ."
        "7: . . . r . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . K . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: . . . . . . . ."
        "   a b c d e f g h"
        //
        "   w - - 0 1");

    EXPECT_THAT(GenerateMoves<kEvasion>(position),
                UnorderedElementsAreArray(MakeMoves({
                    "d4d5",
                    "d4c5",
                    "d4c4",
                    "d4c3",
                    "d4d3",
                    "d4e5",
                    "d4e4",
                    "d4e3",
                })));
  }
}

TEST(Castling, WhiteCastling) {
  Position position = MakePosition(
      "8: r . . . k . . r"
      "7: . . . . . . . ."
      "6: . . . . . . . ."
      "5: . . . . . . . ."
      "4: . . . . . . . ."
      "3: . . . . . . . ."
      "2: . . . . . . . ."
      "1: R . . . K . . R"
      "   a b c d e f g h"
      //
      "   w KQ - 0 1");

  EXPECT_THAT(
      GenerateMoves<kQuiet>(position),
      AllOf(Contains(MakeMove("e1g1#oo")), Contains(MakeMove("e1c1#ooo"))));
}

TEST(Castling, WhiteKingSideOnly) {
  {
    Position position = MakePosition(
        "8: . . . . k . . r"
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . r . . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: R . . . K . . R"
        "   a b c d e f g h"
        //
        "   w KQ - 0 1");

    EXPECT_THAT(GenerateMoves<kQuiet>(position),
                AllOf(Contains(MakeMove("e1g1#oo")),
                      Not(Contains(MakeMove("e1c1#ooo")))));
  }
  {
    Position position = MakePosition(
        "8: . . . . k . . r"
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: R n . . K . . R"
        "   a b c d e f g h"
        //
        "   w KQ - 0 1");

    EXPECT_THAT(GenerateMoves<kQuiet>(position),
                AllOf(Contains(MakeMove("e1g1#oo")),
                      Not(Contains(MakeMove("e1c1#ooo")))));
  }
}

TEST(Castling, WhiteQueenSideOnly) {
  {
    Position position = MakePosition(
        "8: r . . . k . . ."
        "7: . . . . . . . ."
        "6: . . . . . . r ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: R . . . K . . R"
        "   a b c d e f g h"
        //
        "   w KQ - 0 1");

    EXPECT_THAT(GenerateMoves<kQuiet>(position),
                AllOf(Contains(MakeMove("e1c1#ooo")),
                      Not(Contains(MakeMove("e1g1#oo")))));
  }
  {
    Position position = MakePosition(
        "8: r . . . k . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: R . . . K . n R"
        "   a b c d e f g h"
        //
        "   w KQ - 0 1");

    EXPECT_THAT(GenerateMoves<kQuiet>(position),
                AllOf(Contains(MakeMove("e1c1#ooo")),
                      Not(Contains(MakeMove("e1g1#oo")))));
  }
  {
    Position position = MakePosition(
        "8: r . . . k . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: R . . . K n . R"
        "   a b c d e f g h"
        //
        "   w KQ - 0 1");

    EXPECT_THAT(GenerateMoves<kQuiet>(position),
                AllOf(Contains(MakeMove("e1c1#ooo")),
                      Not(Contains(MakeMove("e1g1#oo")))));
  }
  {
    Position position = MakePosition(
        "8: r . . . k . . ."
        "7: . . . . . . . ."
        "6: . . . . . . r ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . r . . . . . ."
        "2: . . . . . . . ."
        "1: R . . . K . . R"
        "   a b c d e f g h"
        //
        "   w KQ - 0 1");

    EXPECT_THAT(GenerateMoves<kQuiet>(position),
                AllOf(Contains(MakeMove("e1c1#ooo")),
                      Not(Contains(MakeMove("e1g1#oo")))));
  }
}

TEST(Castling, Black) {
  Position position = MakePosition(
      "8: r . . . k . . r"
      "7: . . . . . . . ."
      "6: . . . . . . . ."
      "5: . . . . . . . ."
      "4: . . . . . . . ."
      "3: . . . . . . . ."
      "2: . . . . . . . ."
      "1: R . . . K . . R"
      "   a b c d e f g h"
      //
      "   b kq - 0 1");

  EXPECT_THAT(
      GenerateMoves<kQuiet>(position),
      AllOf(Contains(MakeMove("e8g8#oo")), Contains(MakeMove("e8c8#ooo"))));
}

TEST(Castling, BlackKingSideOnly) {
  Position position = MakePosition(
      "8: r . . . k . . r"
      "7: . . . . . . . ."
      "6: . . . . . . . ."
      "5: . . . . . B . ."
      "4: . . . . . . . ."
      "3: . . . . . . . ."
      "2: . . . . . . . ."
      "1: R . . . K . . R"
      "   a b c d e f g h"
      //
      "   b kq - 0 1");

  EXPECT_THAT(GenerateMoves<kQuiet>(position),
              AllOf(Contains(MakeMove("e8g8#oo")),
                    Not(Contains(MakeMove("e8c8#ooo")))));
}

TEST(Castling, BlackQueenSideOnly) {
  Position position = MakePosition(
      "8: r . . . k . . r"
      "7: . . . . . . . ."
      "6: . . . . . . . N"
      "5: . . . . . . . ."
      "4: . . . . . . . ."
      "3: . . . . . . . ."
      "2: . . . . . . . ."
      "1: R . . . K . . R"
      "   a b c d e f g h"
      //
      "   b kq - 0 1");

  EXPECT_THAT(GenerateMoves<kQuiet>(position),
              AllOf(Contains(MakeMove("e8c8#ooo")),
                    Not(Contains(MakeMove("e8g8#oo")))));
}

TEST(Castling, NoCastlingRights) {
  {
    Position position = MakePosition(
        "8: r . . . k . . r"
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: R . . . K . . R"
        "   a b c d e f g h"
        //
        "   w - - 0 1");

    EXPECT_THAT(GenerateMoves<kQuiet>(position),
                AllOf(Not(Contains(MakeMove("e1g1#oo"))),
                      Not(Contains(MakeMove("e1c1#ooo")))));
  }

  {
    Position position = MakePosition(
        "8: r . . . k . . r"
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: R . . . K . . R"
        "   a b c d e f g h"
        //
        "   b - - 0 1");

    EXPECT_THAT(GenerateMoves<kQuiet>(position),
                AllOf(Not(Contains(MakeMove("e8g8#oo"))),
                      Not(Contains(MakeMove("e8c8#ooo")))));
  }
}

TEST(Castling, WhiteKingSidePassesThroughCheck) {
  {
    Position position = MakePosition(
        "8: r . . . k . . r"
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . r . ."
        "1: R . . . K . . R"
        "   a b c d e f g h"
        //
        "   w KQ - 0 1");

    EXPECT_THAT(GenerateMoves<kQuiet>(position),
                AllOf(Not(Contains(MakeMove("e1g1#oo"))),
                      Contains(MakeMove("e1c1#ooo"))));
  }

  {
    Position position = MakePosition(
        "8: r . . . k . . r"
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . r ."
        "1: R . . . K . . R"
        "   a b c d e f g h"
        //
        "   w KQ - 0 1");

    EXPECT_THAT(GenerateMoves<kQuiet>(position),
                AllOf(Not(Contains(MakeMove("e1g1#oo"))),
                      Contains(MakeMove("e1c1#ooo"))));
  }
}

TEST(Castling, WhiteQueenSidePassesThroughCheck) {
  {
    Position position = MakePosition(
        "8: r . . . k . . r"
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . r . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: R . . . K . . R"
        "   a b c d e f g h"
        //
        "   w KQ - 0 1");

    EXPECT_THAT(GenerateMoves<kQuiet>(position),
                AllOf(Contains(MakeMove("e1g1#oo")),
                      Not(Contains(MakeMove("e1c1#ooo")))));
  }

  {
    Position position = MakePosition(
        "8: r . . . k . . r"
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: . . r . . . . ."
        "1: R . . . K . . R"
        "   a b c d e f g h"
        //
        "   w KQ - 0 1");

    EXPECT_THAT(GenerateMoves<kQuiet>(position),
                AllOf(Contains(MakeMove("e1g1#oo")),
                      Not(Contains(MakeMove("e1c1#ooo")))));
  }
}

TEST(Castling, BlackKingSidePassesThroughCheck) {
  Position position = MakePosition(
      "8: r . . . k . . r"
      "7: . . . . . . . ."
      "6: . . . . . . . ."
      "5: . . . . . . . ."
      "4: . . . . . R . ."
      "3: . . . . . . . ."
      "2: . . . . . . . ."
      "1: R . . . K . . R"
      "   a b c d e f g h"
      //
      "   b kq - 0 1");

  // Queenside should still be allowed
  EXPECT_THAT(GenerateMoves<kQuiet>(position),
              AllOf(Not(Contains(MakeMove("e8g8#oo"))),
                    Contains(MakeMove("e8c8#ooo"))));
}

TEST(Castling, BlackQueenSidePassesThroughCheck) {
  Position position = MakePosition(
      "8: r . . . k . . r"
      "7: . . . R . . . ."
      "6: . . . . . . . ."
      "5: . . . . . . . ."
      "4: . . . . . . . ."
      "3: . . . . . . . ."
      "2: . . . . . . . ."
      "1: R . . . K . . R"
      "   a b c d e f g h"
      //
      "   b kq - 0 1");

  EXPECT_THAT(GenerateMoves<kQuiet>(position),
              AllOf(Contains(MakeMove("e8g8#oo")),
                    Not(Contains(MakeMove("e8c8#ooo")))));
}

TEST(Castling, WhiteQueenSideB1AttackedIsLegal) {
  Position position = MakePosition(
      "8: r . . . k . . r"
      "7: . . . . . . . ."
      "6: . . . . . . . ."
      "5: . . . . . . . ."
      "4: . r . . . . . ."
      "3: . . . . . . . ."
      "2: . . . . . . . ."
      "1: R . . . K . . R"
      "   a b c d e f g h"
      //
      "   w KQ - 0 1");

  EXPECT_THAT(
      GenerateMoves<kQuiet>(position),
      AllOf(Contains(MakeMove("e1g1#oo")), Contains(MakeMove("e1c1#ooo"))));
}

TEST(Castling, BlackQueenSideB8AttackedIsLegal) {
  Position position = MakePosition(
      "8: r . . . k . . r"
      "7: . R . . . . . ."
      "6: . . . . . . . ."
      "5: . . . . . . . ."
      "4: . . . . . . . ."
      "3: . . . . . . . ."
      "2: . . . . . . . ."
      "1: R . . . K . . R"
      "   a b c d e f g h"
      //
      "   b kq - 0 1");

  EXPECT_THAT(
      GenerateMoves<kQuiet>(position),
      AllOf(Contains(MakeMove("e8g8#oo")), Contains(MakeMove("e8c8#ooo"))));
}

TEST(Evasion, CheckResolvedViaKnightCapture) {
  Position position = MakePosition(
      "8: . . . k . . . ."
      "7: . . . . . N . ."
      "6: . . . . . . b ."
      "5: . . . . . . . ."
      "4: . . . . . . . ."
      "3: . . . . . . . ."
      "2: . . . . . . . ."
      "1: . . . . . . . ."
      "   a b c d e f g h"
      //
      "   b - - 0 2");

  EXPECT_THAT(GenerateMoves<kEvasion>(position), Contains(MakeMove("g6f7#c")));
}

}  // namespace
}  // namespace follychess

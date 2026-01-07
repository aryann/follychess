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

#include "engine/testing.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "engine/testing.h"

namespace follychess {
namespace {

using ::testing::Eq;
using ::testing::IsTrue;
using ::testing::Optional;

TEST(TryMakePosition, Ok) {
  EXPECT_THAT(TryMakePosition("8: r n b q k b n r\n"
                              "7: p p p p p p p p\n"
                              "6: . . . . . . . .\n"
                              "5: . . . . . . . .\n"
                              "4: . . . . . . . .\n"
                              "3: . . . . . . . .\n"
                              "2: P P P P P P P P\n"
                              "1: R N B Q K B N R\n"
                              "   a b c d e f g h\n"
                              "\n"
                              "   w KQkq - 0 1\n")
                  .has_value(),
              IsTrue());

  EXPECT_THAT(TryMakePosition("8: r . . . k . . r"
                              "7: p . p p q p b ."
                              "6: b n . . p n p ."
                              "5: . . . P N . . ."
                              "4: . p . . P . . ."
                              "3: . . N . . Q . p"
                              "2: P P P B B P P P"
                              "1: R . . . K . . R"
                              "   a b c d e f g h"
                              ""
                              "   w KQkq - 0 1")
                  .has_value(),
              IsTrue());

  EXPECT_THAT(TryMakePosition("8: . . . . . . . ."
                              "7: . . p . . . . ."
                              "6: . . . p . . . ."
                              "5: K P . . . . . r"
                              "4: . R . . . p . k"
                              "3: . . . . . . . ."
                              "2: . . . . P . P ."
                              "1: . . . . . . . ."
                              "   a b c d e f g h"
                              //
                              "   w - a3 0 1")
                  .has_value(),
              IsTrue());
}

TEST(MakePosition, Failures) {
  ASSERT_DEATH(MakePosition(""), "Invalid position input: ");
  ASSERT_DEATH(MakePosition("KQkq"), "Invalid position input: KQkq");
}

}  // namespace
}  // namespace follychess

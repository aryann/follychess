#include "engine/game.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "testing.h"

namespace follychess {
namespace {

using ::testing::Eq;
using ::testing::IsEmpty;
using ::testing::IsFalse;
using ::testing::IsTrue;
using ::testing::Ne;
using ::testing::Not;

TEST(GetRepetitionCount, Simple) {
  Game game;
  EXPECT_THAT(game.GetRepetitionCount(), Eq(0));

  game.Do(MakeMove("e2e3"));
  EXPECT_THAT(game.GetRepetitionCount(), Eq(0));

  game.Do(MakeMove("e7e6"));
  EXPECT_THAT(game.GetRepetitionCount(), Eq(0));

  game.Do(MakeMove("b1c3"));
  EXPECT_THAT(game.GetRepetitionCount(), Eq(0));

  game.Do(MakeMove("b8c6"));
  EXPECT_THAT(game.GetRepetitionCount(), Eq(0));

  game.Do(MakeMove("c3b1"));
  EXPECT_THAT(game.GetRepetitionCount(), Eq(0));

  game.Do(MakeMove("c6b8"));
  EXPECT_THAT(game.GetRepetitionCount(), Eq(1));

  game.Do(MakeMove("b1c3"));
  EXPECT_THAT(game.GetRepetitionCount(), Eq(1));

  game.Do(MakeMove("b8c6"));
  EXPECT_THAT(game.GetRepetitionCount(), Eq(1));

  game.Do(MakeMove("c3b1"));
  EXPECT_THAT(game.GetRepetitionCount(), Eq(1));

  game.Do(MakeMove("c6b8"));
  EXPECT_THAT(game.GetRepetitionCount(), Eq(2));
}

TEST(GetRepetitionCount, FourFoldRepetition) {
  Game game;

  // Initial Game History
  game.Do(MakeMove("g1f3"));
  game.Do(MakeMove("e7e6"));
  game.Do(MakeMove("e2e4#dpp"));
  game.Do(MakeMove("b8c6"));
  game.Do(MakeMove("b1c3"));
  game.Do(MakeMove("d7d6"));
  game.Do(MakeMove("a2a3"));
  game.Do(MakeMove("g8e7"));
  game.Do(MakeMove("b2b4#dpp"));
  game.Do(MakeMove("c8d7"));
  game.Do(MakeMove("c1b2"));
  game.Do(MakeMove("h8g8"));
  game.Do(MakeMove("g2g3"));
  game.Do(MakeMove("a8c8"));
  game.Do(MakeMove("f1g2"));
  game.Do(MakeMove("e6e5"));
  game.Do(MakeMove("d2d4#dpp"));
  game.Do(MakeMove("e5d4#c"));
  game.Do(MakeMove("c3d5"));
  game.Do(MakeMove("d7g4"));
  game.Do(MakeMove("e1g1#oo"));
  game.Do(MakeMove("c6e5"));
  game.Do(MakeMove("f1e1"));
  game.Do(MakeMove("d8d7"));
  game.Do(MakeMove("a3a4"));
  game.Do(MakeMove("c7c5#dpp"));
  game.Do(MakeMove("b4c5#c"));
  game.Do(MakeMove("g4f3#c"));
  game.Do(MakeMove("g2f3#c"));
  game.Do(MakeMove("d6c5#c"));
  game.Do(MakeMove("f3g2"));
  game.Do(MakeMove("c8c6"));
  game.Do(MakeMove("c2c3"));
  game.Do(MakeMove("d4c3#c"));
  game.Do(MakeMove("b2a3"));
  game.Do(MakeMove("e8d8"));
  game.Do(MakeMove("g2h3"));
  game.Do(MakeMove("d7h3#c"));
  game.Do(MakeMove("d5c3#c"));
  game.Do(MakeMove("d8c7"));
  game.Do(MakeMove("a3b2"));
  game.Do(MakeMove("e5g4"));
  game.Do(MakeMove("d1g4#c"));
  game.Do(MakeMove("h3g4#c"));
  game.Do(MakeMove("c3b5"));
  game.Do(MakeMove("c7b8"));
  game.Do(MakeMove("a1d1"));
  game.Do(MakeMove("g4f3"));
  game.Do(MakeMove("b2e5"));
  game.Do(MakeMove("b8a8"));
  game.Do(MakeMove("h2h4#dpp"));
  game.Do(MakeMove("c6c8"));
  game.Do(MakeMove("d1d7"));
  game.Do(MakeMove("e7g6"));
  game.Do(MakeMove("b5c7"));
  game.Do(MakeMove("a8b8"));
  game.Do(MakeMove("e1b1"));
  game.Do(MakeMove("g6e5#c"));
  EXPECT_THAT(game.GetRepetitionCount(), Eq(0));

  // Cycle 1:
  game.Do(MakeMove("c7a6"));
  game.Do(MakeMove("b8a8"));
  game.Do(MakeMove("a6c7"));
  game.Do(MakeMove("a8b8"));
  EXPECT_THAT(game.GetRepetitionCount(), Eq(1));

  // Cycle 2:
  game.Do(MakeMove("c7a6"));
  EXPECT_THAT(game.GetRepetitionCount(), Eq(1));
  game.Do(MakeMove("b8a8"));
  EXPECT_THAT(game.GetRepetitionCount(), Eq(1));
  game.Do(MakeMove("a6c7"));
  EXPECT_THAT(game.GetRepetitionCount(), Eq(1));
  game.Do(MakeMove("a8b8"));
  EXPECT_THAT(game.GetRepetitionCount(), Eq(2));

  // Cycle 3:
  game.Do(MakeMove("c7a6"));
  EXPECT_THAT(game.GetRepetitionCount(), Eq(2));
  game.Do(MakeMove("b8a8"));
  EXPECT_THAT(game.GetRepetitionCount(), Eq(2));
  game.Do(MakeMove("a6c7"));
  EXPECT_THAT(game.GetRepetitionCount(), Eq(2));
  game.Do(MakeMove("a8b8"));
  EXPECT_THAT(game.GetRepetitionCount(), Eq(3));

  // Cycle 4:
  game.Do(MakeMove("c7a6"));
  EXPECT_THAT(game.GetRepetitionCount(), Eq(3));
  game.Do(MakeMove("b8a8"));
  EXPECT_THAT(game.GetRepetitionCount(), Eq(3));
  game.Do(MakeMove("a6c7"));
  EXPECT_THAT(game.GetRepetitionCount(), Eq(3));
  game.Do(MakeMove("a8b8"));
  EXPECT_THAT(game.GetRepetitionCount(), Eq(4));
}

}  // namespace
}  // namespace follychess

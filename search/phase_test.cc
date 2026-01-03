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
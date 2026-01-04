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

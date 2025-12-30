#include "search/evaluation.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "engine/position.h"
#include "engine/testing.h"

namespace follychess {
namespace {

using ::testing::Eq;

TEST(GetMaterialScore, White) {
  EXPECT_THAT(GetMaterialScore<kWhite>(Position::Starting()), Eq(23'900));

  EXPECT_THAT(GetMaterialScore<kWhite>(MakePosition("8: . . . . k . . ."
                                                    "7: . . . . . . . ."
                                                    "6: . . . . . . . ."
                                                    "5: . . . . . . . ."
                                                    "4: . . . . . . . ."
                                                    "3: . . . . . . . ."
                                                    "2: . . . . . . . ."
                                                    "1: . . . . K . . ."
                                                    "   a b c d e f g h"
                                                    //
                                                    "   w - - 0 1")),
              Eq(20'000));

  EXPECT_THAT(GetMaterialScore<kWhite>(MakePosition("8: . . . . k . . ."
                                                    "7: . . . . . . . ."
                                                    "6: . . . . . . . ."
                                                    "5: . . . . . . . ."
                                                    "4: . . . . . . . ."
                                                    "3: . . . . . . . ."
                                                    "2: . . . . . . . ."
                                                    "1: . Q . . K . . ."
                                                    "   a b c d e f g h"
                                                    //
                                                    "   w - - 0 1")),
              Eq(20'900));

  EXPECT_THAT(GetMaterialScore<kWhite>(MakePosition("8: . . . . k . . ."
                                                    "7: . . . . . . . ."
                                                    "6: . . . . . . . ."
                                                    "5: . . . . . . . ."
                                                    "4: . B . . . . B ."
                                                    "3: . . . . . . . ."
                                                    "2: . . . . . . . ."
                                                    "1: . . . . K . . ."
                                                    "   a b c d e f g h"
                                                    //
                                                    "   w - - 0 1")),
              Eq(20'600));

  EXPECT_THAT(GetMaterialScore<kWhite>(MakePosition("8: . . . . k . . ."
                                                    "7: . . . . . . . ."
                                                    "6: . . . . . . . ."
                                                    "5: . . . . . . . ."
                                                    "4: . b . . . . b ."
                                                    "3: . . . . . . . ."
                                                    "2: . . . . . . . ."
                                                    "1: . . . . K . . ."
                                                    "   a b c d e f g h"
                                                    //
                                                    "   w - - 0 1")),
              Eq(20'000));

  EXPECT_THAT(GetMaterialScore<kWhite>(MakePosition("8: . . . . k . . ."
                                                    "7: . . . . . . . ."
                                                    "6: . . . . . . . ."
                                                    "5: . . . . . . . ."
                                                    "4: . N . . . . b ."
                                                    "3: . . . . . . . ."
                                                    "2: . . . . . . . ."
                                                    "1: . . . . K . . ."
                                                    "   a b c d e f g h"
                                                    //
                                                    "   w - - 0 1")),
              Eq(20'300));

  EXPECT_THAT(GetMaterialScore<kWhite>(MakePosition("8: . . . . k . . ."
                                                    "7: . . . . . . . ."
                                                    "6: . . . . . . . ."
                                                    "5: . . . . . . . ."
                                                    "4: . N . . . . b ."
                                                    "3: . . . . . . . ."
                                                    "2: . . . . . . . ."
                                                    "1: R . . . K . . R"
                                                    "   a b c d e f g h"
                                                    //
                                                    "   w - - 0 1")),
              Eq(21'300));
}

TEST(GetPlacementScore, White) {
  EXPECT_THAT(GetPlacementScore<kWhite>(MakePosition("8: . . . . . . . ."
                                                     "7: . . . . . . . ."
                                                     "6: . . . . . . . ."
                                                     "5: . . . P P . . ."
                                                     "4: . . . . . . . ."
                                                     "3: . . . . . . . ."
                                                     "2: . . . . . . . ."
                                                     "1: . . . . . . . ."
                                                     "   a b c d e f g h"
                                                     //
                                                     "   w - - 0 1")),
              Eq(50));

  EXPECT_THAT(GetPlacementScore<kWhite>(MakePosition("8: . . . . . . . ."
                                                     "7: . . . . . . . ."
                                                     "6: . . . . . . . ."
                                                     "5: . . . . . . . ."
                                                     "4: . . . p p . . ."
                                                     "3: . . . . . . . ."
                                                     "2: . . . . . . . ."
                                                     "1: . . . . . . . ."
                                                     "   a b c d e f g h"
                                                     //
                                                     "   w - - 0 1")),
              Eq(0));

  EXPECT_THAT(GetPlacementScore<kWhite>(MakePosition("8: . . . . . . . ."
                                                     "7: . . . . . . . ."
                                                     "6: . . . . . . . ."
                                                     "5: . . . . . . . ."
                                                     "4: . . . . . . . ."
                                                     "3: . . . . . . . ."
                                                     "2: . . . . . . . ."
                                                     "1: . . . . . . . K"
                                                     "   a b c d e f g h"
                                                     //
                                                     "   w - - 0 1")),
              Eq(20));

  EXPECT_THAT(GetPlacementScore<kWhite>(MakePosition("8: Q . . . . . . ."
                                                     "7: . . . . . . . ."
                                                     "6: . . . . . . . ."
                                                     "5: . . . . . . . ."
                                                     "4: . . . . . . . ."
                                                     "3: . . . . . . . ."
                                                     "2: . . . . . . . ."
                                                     "1: . . . . . . . K"
                                                     "   a b c d e f g h"
                                                     //
                                                     "   w - - 0 1")),
              Eq(0));
}

TEST(CountDoubledPawns, White) {
  EXPECT_THAT(CountDoubledPawns<kWhite>(MakePosition("8: . . . . . . . ."
                                                     "7: . . . p . . . ."
                                                     "6: . . . p . . . ."
                                                     "5: . . . . . . . ."
                                                     "4: P . . . P . . ."
                                                     "3: . . . P . . . ."
                                                     "2: . . P . . . . ."
                                                     "1: . . . . . . . ."
                                                     "   a b c d e f g h"
                                                     //
                                                     "   w - - 0 1")),
              Eq(0));

  EXPECT_THAT(CountDoubledPawns<kWhite>(MakePosition("8: . . . . . . . ."
                                                     "7: . . . p . . . ."
                                                     "6: . . . p . . . ."
                                                     "5: . . . . . . . ."
                                                     "4: . . . . . P . ."
                                                     "3: . P . P . P . ."
                                                     "2: . P P . . P . ."
                                                     "1: . . . . . . . ."
                                                     "   a b c d e f g h"
                                                     //
                                                     "   w - - 0 1")),
              Eq(3));

  EXPECT_THAT(CountDoubledPawns<kWhite>(MakePosition("8: . . . . . . . ."
                                                     "7: . . . p . P . ."
                                                     "6: . . . p . . . ."
                                                     "5: . P . . . . . ."
                                                     "4: . . . . . . . ."
                                                     "3: . . . P . . . ."
                                                     "2: . P P . . P . ."
                                                     "1: . . . . . . . ."
                                                     "   a b c d e f g h"
                                                     //
                                                     "   w - - 0 1")),
              Eq(2));
}

TEST(CountDoubledPawns, Black) {
  EXPECT_THAT(CountDoubledPawns<kBlack>(MakePosition("8: . . . . . . . ."
                                                     "7: p p . p . p . ."
                                                     "6: . . . . . . . ."
                                                     "5: . . . . . . . ."
                                                     "4: P . . P . . . ."
                                                     "3: . . . P . . . ."
                                                     "2: . . P P . . . ."
                                                     "1: . . . . . . . ."
                                                     "   a b c d e f g h"
                                                     //
                                                     "   w - - 0 1")),
              Eq(0));

  EXPECT_THAT(CountDoubledPawns<kBlack>(MakePosition("8: . . . . . . . ."
                                                     "7: . . . p . . . ."
                                                     "6: . . . p . . . ."
                                                     "5: . . . . . . . ."
                                                     "4: . . . . . P . ."
                                                     "3: . P . P . P . ."
                                                     "2: . P P . . P . ."
                                                     "1: . . . . . . . ."
                                                     "   a b c d e f g h"
                                                     //
                                                     "   w - - 0 1")),
              Eq(1));

  EXPECT_THAT(CountDoubledPawns<kBlack>(MakePosition("8: . . . . . . . ."
                                                     "7: . . . p . . . p"
                                                     "6: . . . p . . . ."
                                                     "5: . . . . . . . ."
                                                     "4: . . . . . P . ."
                                                     "3: . P . P . P . ."
                                                     "2: . P P . . P . p"
                                                     "1: . . . . . . . ."
                                                     "   a b c d e f g h"
                                                     //
                                                     "   w - - 0 1")),
              Eq(2));
}

TEST(Evaluate, Starting) { EXPECT_THAT(Evaluate(Position::Starting()), Eq(0)); }

}  // namespace
}  // namespace follychess

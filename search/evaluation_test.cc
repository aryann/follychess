#include "search/evaluation.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "engine/position.h"
#include "engine/testing.h"

namespace follychess {
namespace {

using ::testing::Eq;

TEST(Evaluation, GetMaterialScore) {
  EXPECT_THAT(GetMaterialScore(Position::Starting()), Eq(000));

  EXPECT_THAT(GetMaterialScore(MakePosition("8: . . . . k . . ."
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
              Eq(0));

  EXPECT_THAT(GetMaterialScore(MakePosition("8: . . . . k . . ."
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
              Eq(900));

  EXPECT_THAT(GetMaterialScore(MakePosition("8: . . . . k . . ."
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
              Eq(600));

  EXPECT_THAT(GetMaterialScore(MakePosition("8: . . . . k . . ."
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
              Eq(-600));

  EXPECT_THAT(GetMaterialScore(MakePosition("8: . . . . k . . ."
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
              Eq(0));

  EXPECT_THAT(GetMaterialScore(MakePosition("8: . . . . k . . ."
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
              Eq(1000));
}

TEST(Evaluation, GetPlacementScore) {
  EXPECT_THAT(GetPlacementScore(MakePosition("8: . . . . . . . ."
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

  EXPECT_THAT(GetPlacementScore(MakePosition("8: . . . . . . . ."
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
              Eq(-50));

  EXPECT_THAT(GetPlacementScore(MakePosition("8: . . . . . . . ."
                                             "7: . . . . . . . ."
                                             "6: . . . . . . . ."
                                             "5: . . . . . . . ."
                                             "4: . . . . . . . ."
                                             "3: . . . . . . . ."
                                             "2: . . . . . . . ."
                                             "1: . . . . . . . k"
                                             "   a b c d e f g h"
                                             //
                                             "   w - - 0 1")),
              Eq(30));

  EXPECT_THAT(GetPlacementScore(MakePosition("8: q . . . . . . ."
                                             "7: . . . . . . . ."
                                             "6: . . . . . . . ."
                                             "5: . . . . . . . ."
                                             "4: . . . . . . . ."
                                             "3: . . . . . . . ."
                                             "2: . . . . . . . ."
                                             "1: . . . . . . . k"
                                             "   a b c d e f g h"
                                             //
                                             "   w - - 0 1")),
              Eq(50));
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

}  // namespace
}  // namespace follychess

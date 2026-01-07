#include "search/evaluation.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "engine/position.h"
#include "engine/testing.h"
#include "search/phase.h"

namespace follychess {
namespace {

using ::testing::Eq;

TEST(PassedPawnMasks, White) {
  EXPECT_THAT(kPassedPawnMasks[kWhite][E4],
              EqualsBitboard("8: . . . X X X . ."
                             "7: . . . X X X . ."
                             "6: . . . X X X . ."
                             "5: . . . X X X . ."
                             "4: . . . . . . . ."
                             "3: . . . . . . . ."
                             "2: . . . . . . . ."
                             "1: . . . . . . . ."
                             "   a b c d e f g h"));

  EXPECT_THAT(kPassedPawnMasks[kWhite][A4],
              EqualsBitboard("8: X X . . . . . ."
                             "7: X X . . . . . ."
                             "6: X X . . . . . ."
                             "5: X X . . . . . ."
                             "4: . . . . . . . ."
                             "3: . . . . . . . ."
                             "2: . . . . . . . ."
                             "1: . . . . . . . ."
                             "   a b c d e f g h"));

  EXPECT_THAT(kPassedPawnMasks[kWhite][H4],
              EqualsBitboard("8: . . . . . . X X"
                             "7: . . . . . . X X"
                             "6: . . . . . . X X"
                             "5: . . . . . . X X"
                             "4: . . . . . . . ."
                             "3: . . . . . . . ."
                             "2: . . . . . . . ."
                             "1: . . . . . . . ."
                             "   a b c d e f g h"));

  EXPECT_THAT(kPassedPawnMasks[kWhite][E7],
              EqualsBitboard("8: . . . X X X . ."
                             "7: . . . . . . . ."
                             "6: . . . . . . . ."
                             "5: . . . . . . . ."
                             "4: . . . . . . . ."
                             "3: . . . . . . . ."
                             "2: . . . . . . . ."
                             "1: . . . . . . . ."
                             "   a b c d e f g h"));

  EXPECT_THAT(kPassedPawnMasks[kWhite][E8],
              EqualsBitboard("8: . . . . . . . ."
                             "7: . . . . . . . ."
                             "6: . . . . . . . ."
                             "5: . . . . . . . ."
                             "4: . . . . . . . ."
                             "3: . . . . . . . ."
                             "2: . . . . . . . ."
                             "1: . . . . . . . ."
                             "   a b c d e f g h"));
}

TEST(PassedPawnMasks, Black) {
  EXPECT_THAT(kPassedPawnMasks[kBlack][E5],
              EqualsBitboard("8: . . . . . . . ."
                             "7: . . . . . . . ."
                             "6: . . . . . . . ."
                             "5: . . . . . . . ."
                             "4: . . . X X X . ."
                             "3: . . . X X X . ."
                             "2: . . . X X X . ."
                             "1: . . . X X X . ."
                             "   a b c d e f g h"));

  EXPECT_THAT(kPassedPawnMasks[kBlack][A5],
              EqualsBitboard("8: . . . . . . . ."
                             "7: . . . . . . . ."
                             "6: . . . . . . . ."
                             "5: . . . . . . . ."
                             "4: X X . . . . . ."
                             "3: X X . . . . . ."
                             "2: X X . . . . . ."
                             "1: X X . . . . . ."
                             "   a b c d e f g h"));

  EXPECT_THAT(kPassedPawnMasks[kBlack][H5],
              EqualsBitboard("8: . . . . . . . ."
                             "7: . . . . . . . ."
                             "6: . . . . . . . ."
                             "5: . . . . . . . ."
                             "4: . . . . . . X X"
                             "3: . . . . . . X X"
                             "2: . . . . . . X X"
                             "1: . . . . . . X X"
                             "   a b c d e f g h"));

  EXPECT_THAT(kPassedPawnMasks[kBlack][E2],
              EqualsBitboard("8: . . . . . . . ."
                             "7: . . . . . . . ."
                             "6: . . . . . . . ."
                             "5: . . . . . . . ."
                             "4: . . . . . . . ."
                             "3: . . . . . . . ."
                             "2: . . . . . . . ."
                             "1: . . . X X X . ."
                             "   a b c d e f g h"));

  EXPECT_THAT(kPassedPawnMasks[kBlack][E1],
              EqualsBitboard("8: . . . . . . . ."
                             "7: . . . . . . . ."
                             "6: . . . . . . . ."
                             "5: . . . . . . . ."
                             "4: . . . . . . . ."
                             "3: . . . . . . . ."
                             "2: . . . . . . . ."
                             "1: . . . . . . . ."
                             "   a b c d e f g h"));
}
TEST(GetPassedPawnScore, White) {
  {
    const auto [middle, end] =
        GetPassedPawnScore<kWhite>(MakePosition("8: . . . . . . . ."
                                                "7: . . . . . . . ."
                                                "6: . . . . . . . ."
                                                "5: . . . . P . . ."
                                                "4: . . . . . . . ."
                                                "3: . . . . . . . ."
                                                "2: . . . . . . . ."
                                                "1: . . . . . . . ."
                                                "   a b c d e f g h"
                                                //
                                                "   w - - 0 1"));
    EXPECT_THAT(middle, Eq(50));
    EXPECT_THAT(end, Eq(80));
  }

  {
    const auto [middle, end] =
        GetPassedPawnScore<kWhite>(MakePosition("8: . . . . . . . ."
                                                "7: . . . . . . . ."
                                                "6: P . . . . . . ."
                                                "5: . . . . . . . P"
                                                "4: . . . . . . . ."
                                                "3: . . . . . . . ."
                                                "2: . . . . . . . ."
                                                "1: . . . . . . . ."
                                                "   a b c d e f g h"
                                                //
                                                "   w - - 0 1"));
    EXPECT_THAT(middle, Eq(125));
    EXPECT_THAT(end, Eq(200));
  }

  {
    const auto [middle, end] =
        GetPassedPawnScore<kWhite>(MakePosition("8: . . . . . . . ."
                                                "7: . . . . . . . ."
                                                "6: . . . . . . . ."
                                                "5: . . . . p . . ."
                                                "4: . . . . P . . ."
                                                "3: . . . . . . . ."
                                                "2: . . . . . . . ."
                                                "1: . . . . . . . ."
                                                "   a b c d e f g h"
                                                //
                                                "   w - - 0 1"));
    EXPECT_THAT(middle, Eq(0));
    EXPECT_THAT(end, Eq(0));
  }

  {
    const auto [middle, end] =
        GetPassedPawnScore<kWhite>(MakePosition("8: . . . . . . . ."
                                                "7: . . . . . . . ."
                                                "6: . . . . . . . ."
                                                "5: . . . p . . . ."
                                                "4: . . . . P . . ."
                                                "3: . . . . . . . ."
                                                "2: . . . . . . . ."
                                                "1: . . . . . . . ."
                                                "   a b c d e f g h"
                                                //
                                                "   w - - 0 1"));
    EXPECT_THAT(middle, Eq(0));
    EXPECT_THAT(end, Eq(0));
  }

  {
    const auto [middle, end] =
        GetPassedPawnScore<kWhite>(MakePosition("8: . . . . . . . ."
                                                "7: . . . . . . . ."
                                                "6: . . . . . . . ."
                                                "5: . . . . . p . ."
                                                "4: . . . . P . . ."
                                                "3: . . . . . . . ."
                                                "2: . . . . . . . ."
                                                "1: . . . . . . . ."
                                                "   a b c d e f g h"
                                                //
                                                "   w - - 0 1"));
    EXPECT_THAT(middle, Eq(0));
    EXPECT_THAT(end, Eq(0));
  }
}

TEST(GetPassedPawnScore, Black) {
  {
    const auto [middle, end] =
        GetPassedPawnScore<kBlack>(MakePosition("8: . . . . . . . ."
                                                "7: . . . . . . . ."
                                                "6: . . . . . . . ."
                                                "5: . . . . . . . ."
                                                "4: . . . . p . . ."
                                                "3: . . . . . . . ."
                                                "2: . . . . . . . ."
                                                "1: . . . . . . . ."
                                                "   a b c d e f g h"
                                                //
                                                "   b - - 0 1"));
    EXPECT_THAT(middle, Eq(50));
    EXPECT_THAT(end, Eq(80));
  }

  {
    const auto [middle, end] =
        GetPassedPawnScore<kBlack>(MakePosition("8: . . . . . . . ."
                                                "7: . . . . . . . ."
                                                "6: . . . . . . . ."
                                                "5: . . . . . . . ."
                                                "4: . . . . . . . p"
                                                "3: p . . . . . . ."
                                                "2: . . . . . . . ."
                                                "1: . . . . . . . ."
                                                "   a b c d e f g h"
                                                //
                                                "   b - - 0 1"));
    EXPECT_THAT(middle, Eq(125));
    EXPECT_THAT(end, Eq(200));
  }

  {
    const auto [middle, end] =
        GetPassedPawnScore<kBlack>(MakePosition("8: . . . . . . . ."
                                                "7: . . . . . . . ."
                                                "6: . . . . . . . ."
                                                "5: . . . . p . . ."
                                                "4: . . . . P . . ."
                                                "3: . . . . . . . ."
                                                "2: . . . . . . . ."
                                                "1: . . . . . . . ."
                                                "   a b c d e f g h"
                                                //
                                                "   b - - 0 1"));
    EXPECT_THAT(middle, Eq(0));
    EXPECT_THAT(end, Eq(0));
  }

  {
    const auto [middle, end] =
        GetPassedPawnScore<kBlack>(MakePosition("8: . . . . . . . ."
                                                "7: . . . . . . . ."
                                                "6: . . . . . . . ."
                                                "5: . . . . p . . ."
                                                "4: . . . P . . . ."
                                                "3: . . . . . . . ."
                                                "2: . . . . . . . ."
                                                "1: . . . . . . . ."
                                                "   a b c d e f g h"
                                                //
                                                "   b - - 0 1"));
    EXPECT_THAT(middle, Eq(0));
    EXPECT_THAT(end, Eq(0));
  }

  {
    const auto [middle, end] =
        GetPassedPawnScore<kBlack>(MakePosition("8: . . . . . . . ."
                                                "7: . . . . . . . ."
                                                "6: . . . . . . . ."
                                                "5: . . . . p . . ."
                                                "4: . . . . . P . ."
                                                "3: . . . . . . . ."
                                                "2: . . . . . . . ."
                                                "1: . . . . . . . ."
                                                "   a b c d e f g h"
                                                //
                                                "   b - - 0 1"));
    EXPECT_THAT(middle, Eq(0));
    EXPECT_THAT(end, Eq(0));
  }
}

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

TEST(GetMaterialScore, Black) {
  EXPECT_THAT(GetMaterialScore<kBlack>(Position::Starting()), Eq(23'900));

  EXPECT_THAT(GetMaterialScore<kBlack>(MakePosition("8: . . . k . . . ."
                                                    "7: . . . . . . . ."
                                                    "6: . . . . . . . ."
                                                    "5: . . . . . . . ."
                                                    "4: . . . . . . . ."
                                                    "3: . . . . . . . ."
                                                    "2: . . . . . . . ."
                                                    "1: . . . . . . . ."
                                                    "   a b c d e f g h"
                                                    //
                                                    "   b - - 0 1")),
              Eq(20'000));

  EXPECT_THAT(GetMaterialScore<kBlack>(MakePosition("8: . . . k q . . ."
                                                    "7: . . . . . . . ."
                                                    "6: . . . . . . . ."
                                                    "5: . . . . . . . ."
                                                    "4: . . . . . . . ."
                                                    "3: . . . . . . . ."
                                                    "2: . . . . . . . ."
                                                    "1: . . . . . . . ."
                                                    "   a b c d e f g h"
                                                    //
                                                    "   b - - 0 1")),
              Eq(20'900));

  EXPECT_THAT(GetMaterialScore<kBlack>(MakePosition("8: r . . k . . . r"
                                                    "7: . . . . . . . ."
                                                    "6: . . . . p . . ."
                                                    "5: . . . . . . . ."
                                                    "4: . . . . . . . ."
                                                    "3: . . . . . . . ."
                                                    "2: . . . . . . . ."
                                                    "1: . . . . . . . ."
                                                    "   a b c d e f g h"
                                                    //
                                                    "   b - - 0 1")),
              Eq(21'100));
}

TEST(GetPlacementScore, White) {
  {
    const auto [middle, end] =
        GetPlacementScore<kWhite>(MakePosition("8: . . . . . . . ."
                                               "7: . . . . . . . ."
                                               "6: . . . . . . . ."
                                               "5: . . . P P . . ."
                                               "4: . . . . . . . ."
                                               "3: . . . . . . . ."
                                               "2: . . . . . . . ."
                                               "1: . . . . . . . ."
                                               "   a b c d e f g h"
                                               //
                                               "   w - - 0 1"));
    EXPECT_THAT(middle, Eq(50));
    EXPECT_THAT(end, Eq(50));
  }

  {
    const auto [middle, end] =
        GetPlacementScore<kWhite>(MakePosition("8: . . . . . . . ."
                                               "7: . . . . . . . ."
                                               "6: . . . . . . . ."
                                               "5: . . . . . . . ."
                                               "4: . . . p p . . ."
                                               "3: . . . . . . . ."
                                               "2: . . . . . . . ."
                                               "1: . . . . . . . ."
                                               "   a b c d e f g h"
                                               //
                                               "   w - - 0 1"));
    EXPECT_THAT(middle, Eq(0));
    EXPECT_THAT(end, Eq(0));
  }

  {
    const auto [middle, end] =
        GetPlacementScore<kWhite>(MakePosition("8: . . . . . . . ."
                                               "7: . . . . . . . ."
                                               "6: . . . . . . . ."
                                               "5: . . . . . . . ."
                                               "4: . . . . . . . ."
                                               "3: . . . . . . . ."
                                               "2: . . . . . . . ."
                                               "1: . . . . . . . K"
                                               "   a b c d e f g h"
                                               //
                                               "   w - - 0 1"));
    EXPECT_THAT(middle, Eq(20));
    EXPECT_THAT(end, Eq(-50));
  }

  {
    const auto [middle, end] =
        GetPlacementScore<kWhite>(MakePosition("8: Q . . . . . . ."
                                               "7: . . . . . . . ."
                                               "6: . . . . . . . ."
                                               "5: . . . . . . . ."
                                               "4: . . . . . . . ."
                                               "3: . . . . . . . ."
                                               "2: . . . . . . . ."
                                               "1: . . . . . . . K"
                                               "   a b c d e f g h"
                                               //
                                               "   w - - 0 1"));
    EXPECT_THAT(middle, Eq(0));
    EXPECT_THAT(end, Eq(-70));
  }
}

TEST(GetPlacementScore, Black) {
  {
    const auto [middle, end] =
        GetPlacementScore<kBlack>(MakePosition("8: . n . . . . . ."
                                               "7: . . . . . . . ."
                                               "6: . . . . . . . ."
                                               "5: . . . . . . . ."
                                               "4: . . . . . . . ."
                                               "3: . . . . . . . ."
                                               "2: . . . . . . . ."
                                               "1: . . . . . . . K"
                                               "   a b c d e f g h"
                                               //
                                               "   b - - 0 1"));
    EXPECT_THAT(middle, Eq(-40));
    EXPECT_THAT(end, Eq(-40));
  }

  {
    const auto [middle, end] =
        GetPlacementScore<kBlack>(MakePosition("8: . . . . . . . ."
                                               "7: . . . . . . . ."
                                               "6: . . . . . . . ."
                                               "5: . . . . p . . ."
                                               "4: . . . . . . . ."
                                               "3: . . . . . . . ."
                                               "2: . . . . . . . ."
                                               "1: . . . . . . . K"
                                               "   a b c d e f g h"
                                               //
                                               "   b - - 0 1"));
    EXPECT_THAT(middle, Eq(20));
    EXPECT_THAT(end, Eq(20));
  }
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
                                                     "7: . . . . . P . ."
                                                     "6: . . . . . . . ."
                                                     "5: . P . p . . . ."
                                                     "4: . . . p . . . ."
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

TEST(CountDoubledPawns, Gaps) {
  EXPECT_THAT(CountDoubledPawns<kWhite>(MakePosition("8: . . . . . . . ."
                                                     "7: . . . . . . . ."
                                                     "6: P . . . . . . ."
                                                     "5: . . . . . . . ."
                                                     "4: P . . . . . . ."
                                                     "3: . . . . . . . ."
                                                     "2: P . . . . . . ."
                                                     "1: . . . . . . . ."
                                                     "   a b c d e f g h"
                                                     //
                                                     "   w - - 0 1")),
              Eq(2));

  EXPECT_THAT(CountDoubledPawns<kWhite>(MakePosition("8: . . . . . . . ."
                                                     "7: P . . . . . . ."
                                                     "6: . . . . . . . ."
                                                     "5: . . . . . . . ."
                                                     "4: . . . . . . . ."
                                                     "3: . . . . . . . ."
                                                     "2: P . . . . . . ."
                                                     "1: . . . . . . . ."
                                                     "   a b c d e f g h"
                                                     //
                                                     "   w - - 0 1")),
              Eq(1));
}

TEST(CountBlockedPawns, White) {
  EXPECT_THAT(CountBlockedPawns<kWhite>(MakePosition("8: . . . . . . . ."
                                                     "7: . . . . . . . ."
                                                     "6: . . . . . . . ."
                                                     "5: . . . p . . . ."
                                                     "4: . . . . . . . ."
                                                     "3: . . . . . . . ."
                                                     "2: . . . p . . . ."
                                                     "1: . . . . . . . ."
                                                     "   a b c d e f g h"
                                                     //
                                                     "   w - - 0 1")),
              Eq(0));

  EXPECT_THAT(CountBlockedPawns<kWhite>(MakePosition("8: . . . . . . . ."
                                                     "7: . . . . . . . ."
                                                     "6: . . . . . . . ."
                                                     "5: . . . p . . . ."
                                                     "4: . . . P . . . ."
                                                     "3: . . . . . . . ."
                                                     "2: . . . . . . . ."
                                                     "1: . . . . . . . ."
                                                     "   a b c d e f g h"
                                                     //
                                                     "   w - - 0 1")),
              Eq(1));

  EXPECT_THAT(CountBlockedPawns<kWhite>(MakePosition("8: . . . . . . . ."
                                                     "7: . . . . . . . ."
                                                     "6: . . . . . . . ."
                                                     "5: . . . . . . . ."
                                                     "4: . . . P . . . ."
                                                     "3: . . . P . . . ."
                                                     "2: . . . . . . . ."
                                                     "1: . . . . . . . ."
                                                     "   a b c d e f g h"
                                                     //
                                                     "   w - - 0 1")),
              Eq(1));

  EXPECT_THAT(CountBlockedPawns<kWhite>(MakePosition("8: . . . . . . . ."
                                                     "7: . . . . . . . ."
                                                     "6: . . . . . . . ."
                                                     "5: . . . n . . . ."
                                                     "4: . . . P . . . ."
                                                     "3: . . . . . . . ."
                                                     "2: . . . . . . . ."
                                                     "1: . . . . . . . ."
                                                     "   a b c d e f g h"
                                                     //
                                                     "   w - - 0 1")),
              Eq(1));

  EXPECT_THAT(CountBlockedPawns<kWhite>(MakePosition("8: . . . . . . . ."
                                                     "7: . . . . . . . ."
                                                     "6: . . . . . . . ."
                                                     "5: p . . . . . . p"
                                                     "4: P . . . . . . P"
                                                     "3: . . . . . . . ."
                                                     "2: . . . . . . . ."
                                                     "1: . . . . . . . ."
                                                     "   a b c d e f g h"
                                                     //
                                                     "   w - - 0 1")),
              Eq(2));
}

TEST(CountBlockedPawns, Black) {
  EXPECT_THAT(CountBlockedPawns<kBlack>(MakePosition("8: . . . . . . . ."
                                                     "7: . . . . . . . ."
                                                     "6: . . . . . . . ."
                                                     "5: . . . p . . . ."
                                                     "4: . . . . . . . ."
                                                     "3: . . . . . . . ."
                                                     "2: . . . p . . . ."
                                                     "1: . . . . . . . ."
                                                     "   a b c d e f g h"
                                                     //
                                                     "   w - - 0 1")),
              Eq(0));

  EXPECT_THAT(CountBlockedPawns<kBlack>(MakePosition("8: . . . . . . . ."
                                                     "7: . . . . . . . ."
                                                     "6: . . . . . . . ."
                                                     "5: . . . . p . . ."
                                                     "4: . . . . P . . ."
                                                     "3: . . . . . . . ."
                                                     "2: . . . . . . . ."
                                                     "1: . . . . . . . ."
                                                     "   a b c d e f g h"
                                                     //
                                                     "   w - - 0 1")),
              Eq(1));

  EXPECT_THAT(CountBlockedPawns<kBlack>(MakePosition("8: . . . . . . . ."
                                                     "7: . . . . . . . ."
                                                     "6: . . . . . . . ."
                                                     "5: . . . . p . . ."
                                                     "4: . . . . . . . ."
                                                     "3: . . . . P . . ."
                                                     "2: . . . . . . . ."
                                                     "1: . . . . . . . ."
                                                     "   a b c d e f g h"
                                                     //
                                                     "   w - - 0 1")),
              Eq(0));

  EXPECT_THAT(CountBlockedPawns<kBlack>(MakePosition("8: . . . . . . . ."
                                                     "7: . . . . . . . p"
                                                     "6: . . . . . . . R"
                                                     "5: . . . . . . . ."
                                                     "4: . . . . . . . ."
                                                     "3: . . . . . . . ."
                                                     "2: . . . . . . . ."
                                                     "1: . . . . . . . ."
                                                     "   a b c d e f g h"
                                                     //
                                                     "   w - - 0 1")),
              Eq(1));
}

TEST(CountSemiOpenFileRooks, White) {
  EXPECT_THAT(CountSemiOpenFileRooks<kWhite>(MakePosition("8: . . . . . . . ."
                                                          "7: . . p p . . . ."
                                                          "6: . . . . . . . ."
                                                          "5: . . . . . . . ."
                                                          "4: . . . . . . . ."
                                                          "3: . . . . . . . ."
                                                          "2: . P . . . . . ."
                                                          "1: R R R R . . . ."
                                                          "   a b c d e f g h"
                                                          //
                                                          "   w - - 0 1")),
              Eq(3));
}

TEST(CountSemiOpenFileRooks, Black) {
  EXPECT_THAT(CountSemiOpenFileRooks<kBlack>(MakePosition("8: . . . . r r r r"
                                                          "7: . . . . . . p ."
                                                          "6: . . . . . . . ."
                                                          "5: . . . . . . . ."
                                                          "4: . . . . . . . ."
                                                          "3: . . . . . . . ."
                                                          "2: . . . . P P . ."
                                                          "1: . . . . . . . ."
                                                          "   a b c d e f g h"
                                                          //
                                                          "   w - - 0 1")),
              Eq(3));
}

TEST(CountOpenFileRooks, White) {
  EXPECT_THAT(CountOpenFileRooks<kWhite>(MakePosition("8: . . . . . . . ."
                                                      "7: . . p . . . . ."
                                                      "6: . . . . . . . ."
                                                      "5: . . . . . . . ."
                                                      "4: . . . . . . . ."
                                                      "3: . . . . . . . ."
                                                      "2: . P . . . . . ."
                                                      "1: R R R . . . . ."
                                                      "   a b c d e f g h"
                                                      //
                                                      "   w - - 0 1")),
              Eq(1));

  EXPECT_THAT(CountOpenFileRooks<kWhite>(MakePosition("8: . . . . . . . ."
                                                      "7: . . . . . . . ."
                                                      "6: . . . . . . . ."
                                                      "5: . . . . R . . ."
                                                      "4: . . . . . . . ."
                                                      "3: . . . . . . . ."
                                                      "2: . . . . . . . ."
                                                      "1: . . . . R . . ."
                                                      "   a b c d e f g h"
                                                      //
                                                      "   w - - 0 1")),
              Eq(2));
}

TEST(CountOpenFileRooks, Black) {
  EXPECT_THAT(CountOpenFileRooks<kBlack>(MakePosition("8: . . . . . r r r"
                                                      "7: . . . . . . p ."
                                                      "6: . . . . . . . ."
                                                      "5: . . . . . . . ."
                                                      "4: . . . . . . . ."
                                                      "3: . . . . . . . ."
                                                      "2: . . . . . P . ."
                                                      "1: . . . . . . . ."
                                                      "   a b c d e f g h"
                                                      //
                                                      "   w - - 0 1")),
              Eq(1));
}

TEST(GetKingSafetyScore, WhiteCenter) {
  {
    // The king is in the center and has castling rights.
    const auto [middle, end] =
        GetKingSafetyScore<kWhite>(MakePosition("8: . . . . . . . ."
                                                "7: . . . . . . . ."
                                                "6: . . . . . . . ."
                                                "5: . . . . . . . ."
                                                "4: . . . . . . . ."
                                                "3: . . . . . . . ."
                                                "2: . . . . . . . ."
                                                "1: . . . . K . . ."
                                                "   a b c d e f g h"
                                                //
                                                "   w QK - 0 1"));
    EXPECT_THAT(middle, Eq(-25));
    EXPECT_THAT(end, Eq(0));
  }

  {
    // The king is in the center but has no castling rights.
    const auto [middle, end] =
        GetKingSafetyScore<kWhite>(MakePosition("8: . . . . . . . ."
                                                "7: . . . . . . . ."
                                                "6: . . . . . . . ."
                                                "5: . . . . . . . ."
                                                "4: . . . . . . . ."
                                                "3: . . . . . . . ."
                                                "2: . . . . . . . ."
                                                "1: . . . . K . . ."
                                                "   a b c d e f g h"
                                                //
                                                "   w - - 0 1"));
    EXPECT_THAT(middle, Eq(-60));
    EXPECT_THAT(end, Eq(0));
  }
}

TEST(GetKingSafetyScore, WhiteKingside) {
  {
    // The king is castled and has its pawn shield.
    const auto [middle, end] =
        GetKingSafetyScore<kWhite>(MakePosition("8: . . . . . . . ."
                                                "7: . . . . . . . ."
                                                "6: . . . . . . . ."
                                                "5: . . . . . . . ."
                                                "4: . . . . . . . ."
                                                "3: . . . . . . . ."
                                                "2: . . . . . . P P"
                                                "1: . . . . . . K ."
                                                "   a b c d e f g h"
                                                //
                                                "   w - - 0 1"));
    EXPECT_THAT(middle, Eq(40));
    EXPECT_THAT(end, Eq(0));
  }

  {
    // The king is castled but has a partial pawn shield.
    const auto [middle, end] =
        GetKingSafetyScore<kWhite>(MakePosition("8: . . . . . . . ."
                                                "7: . . . . . . . ."
                                                "6: . . . . . . . ."
                                                "5: . . . . . . . ."
                                                "4: . . . . . . . ."
                                                "3: . . . . . . . ."
                                                "2: . . . . . . P ."
                                                "1: . . . . . . K ."
                                                "   a b c d e f g h"
                                                //
                                                "   w - - 0 1"));
    EXPECT_THAT(middle, Eq(20));
    EXPECT_THAT(end, Eq(0));
  }

  {
    // The king is castled but doesn't have a pawn shield.
    const auto [middle, end] =
        GetKingSafetyScore<kWhite>(MakePosition("8: . . . . . . . ."
                                                "7: . . . . . . . ."
                                                "6: . . . . . . . ."
                                                "5: . . . . . . . ."
                                                "4: . . . . . . . ."
                                                "3: . . . . . . . ."
                                                "2: . . . . . . . P"
                                                "1: . . . . . . K ."
                                                "   a b c d e f g h"
                                                //
                                                "   w - - 0 1"));
    EXPECT_THAT(middle, Eq(-20));
    EXPECT_THAT(end, Eq(0));
  }
}

TEST(GetKingSafetyScore, WhiteQueenside) {
  {
    // The king is castled and has its pawn shield.
    const auto [middle, end] =
        GetKingSafetyScore<kWhite>(MakePosition("8: . . . . . . . ."
                                                "7: . . . . . . . ."
                                                "6: . . . . . . . ."
                                                "5: . . . . . . . ."
                                                "4: . . . . . . . ."
                                                "3: . . . . . . . ."
                                                "2: . P P . . . . ."
                                                "1: . . K . . . . ."
                                                "   a b c d e f g h"
                                                //
                                                "   w - - 0 1"));
    EXPECT_THAT(middle, Eq(40));
    EXPECT_THAT(end, Eq(0));
  }

  {
    // The king has moved one position to its normal castling square and has
    // its pawn shield.
    const auto [middle, end] =
        GetKingSafetyScore<kWhite>(MakePosition("8: . . . . . . . ."
                                                "7: . . . . . . . ."
                                                "6: . . . . . . . ."
                                                "5: . . . . . . . ."
                                                "4: . . . . . . . ."
                                                "3: . . . . . . . ."
                                                "2: . P P . . . . ."
                                                "1: . K . . . . . ."
                                                "   a b c d e f g h"
                                                //
                                                "   w - - 0 1"));
    EXPECT_THAT(middle, Eq(40));
    EXPECT_THAT(end, Eq(0));
  }
}

TEST(GetKingSafetyScore, Black) {
  {
    // The king is castled and has its pawn shield.
    const auto [middle, end] =
        GetKingSafetyScore<kBlack>(MakePosition("8: . . . . . . k ."
                                                "7: . . . . . . p p"
                                                "6: . . . . . . . ."
                                                "5: . . . . . . . ."
                                                "4: . . . . . . . ."
                                                "3: . . . . . . . ."
                                                "2: . . . . . . . ."
                                                "1: . . . . . . . ."
                                                "   a b c d e f g h"
                                                //
                                                "   b - - 0 1"));
    EXPECT_THAT(middle, Eq(40));
    EXPECT_THAT(end, Eq(0));
  }

  {
    // The king is in the middle.
    const auto [middle, end] =
        GetKingSafetyScore<kBlack>(MakePosition("8: . . . . . . . ."
                                                "7: . . . . . . . ."
                                                "6: . . . . . . . ."
                                                "5: . . . . k . . ."
                                                "4: . . . . . . . ."
                                                "3: . . . . . . . ."
                                                "2: . . . . . . . ."
                                                "1: . . . . . . . ."
                                                "   a b c d e f g h"
                                                //
                                                "   b - - 0 1"));
    EXPECT_THAT(middle, Eq(-60));
    EXPECT_THAT(end, Eq(0));
  }
}

TEST(Evaluate, Starting) {
  EXPECT_THAT(Evaluate(Position::Starting(), kStartPhaseValue), Eq(0));
}

TEST(Evaluate, MaterialImbalance) {
  EXPECT_THAT(Evaluate(MakePosition("8: r . . . . . . k"
                                    "7: . . . . . . . ."
                                    "6: . . P . . . . ."
                                    "5: . . . . . . . ."
                                    "4: . . . . . . . ."
                                    "3: . . . . . . . ."
                                    "2: . . . . . . . ."
                                    "1: . . . Q . . . K"
                                    "   a b c d e f g h"
                                    //
                                    "   w - - 0 1"),
                       kStartPhaseValue),
              Eq(565));
}

TEST(Evaluate, DoubledPawnPenalty) {
  EXPECT_THAT(Evaluate(MakePosition("8: . . . . . . . k"
                                    "7: . p p . . . . ."
                                    "6: . . . . . . . ."
                                    "5: . . . . . . . ."
                                    "4: . . . . . . . ."
                                    "3: P . . . . . . ."
                                    "2: P . . . . . . ."
                                    "1: . . . . . . . K"
                                    "   a b c d e f g h"
                                    //
                                    "   w - - 0 1"),
                       kStartPhaseValue),
              Eq(-110));
}

TEST(Evaluate, CastledWhiteKing) {
  EXPECT_THAT(Evaluate(MakePosition("8: r . . . k . . ."
                                    "7: . . . . . . . p"
                                    "6: . . . . . . . p"
                                    "5: . . . . . . . ."
                                    "4: . . . . . . . ."
                                    "3: . . . . . . . ."
                                    "2: . . . . . . P P"
                                    "1: . . . . R . K ."
                                    "   a b c d e f g h"
                                    //
                                    "   w - - 0 1"),
                       kStartPhaseValue),
              Eq(240));
}

}  // namespace
}  // namespace follychess

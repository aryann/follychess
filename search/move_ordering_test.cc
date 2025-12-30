#include "search/move_ordering.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "engine/position.h"
#include "engine/testing.h"

namespace follychess {
namespace {

using ::testing::ElementsAreArray;

TEST(MoveOrdering, Captures) {
  Position position = MakePosition(
      "8: . . . . . . . ."
      "7: . . . . . . . ."
      "6: . . p . . . . ."
      "5: . . . Q . . . ."
      "4: . . . . . . . ."
      "3: p . . . . p . ."
      "2: . P . . R . . ."
      "1: . . . . . . . ."
      "   a b c d e f g h"
      //
      "   b KQkq - 0 1");

  std::vector<Move> moves = MakeMoves({
      "a3a2",
      "a3b2#c",
      "f3e2#c",
      "f3f2",
      "c6d5#c",
      "c6c5",
  });

  OrderMoves(position, moves);
  EXPECT_THAT(moves, ElementsAreArray(MakeMoves({
                         "c6d5#c",  // Queen capture
                         "f3e2#c",  // Rook capture
                         "a3b2#c",  // Pawn capture
                         "a3a2",
                         "f3f2",
                         "c6c5",
                     })));
}

TEST(MoveOrdering, Promotions) {
  Position position = MakePosition(
      "8: . . . . . . . ."
      "7: P . . . . . . ."
      "6: . . . . . . . ."
      "5: . . . . . . . ."
      "4: . . . . . . . ."
      "3: . . . . . . . ."
      "2: . . . . . . . ."
      "1: . N . . . . . ."
      "   a b c d e f g h"
      //
      "   w - - 0 1");

  std::vector<Move> moves = MakeMoves({
      "a7a8n",
      "a7a8b",
      "a7a8r",
      "a7a8q",
      "b1c3",
  });

  OrderMoves(position, moves);
  EXPECT_THAT(moves, ElementsAreArray(MakeMoves({
                         "a7a8q",
                         "a7a8r",
                         "a7a8b",
                         "a7a8n",
                         "b1c3",
                     })));
}

TEST(MoveOrdering, Castling) {
  Position position = MakePosition(
      "8: . . . . . . . ."
      "7: . . . . . . . ."
      "6: . . . . . . . ."
      "5: . . . . . . . ."
      "4: . . . . . . . ."
      "3: . . . . . . . ."
      "2: . . . . . . . ."
      "1: . . . . K . . R"
      "   a b c d e f g h"
      //
      "   w K - 0 1");

  std::vector<Move> moves = MakeMoves({
      "h1g1",
      "e1f1",
      "e1g1#oo",
  });

  OrderMoves(position, moves);
  EXPECT_THAT(moves, ElementsAreArray(MakeMoves({
                         "e1g1#oo",  // Kingside castling
                         "h1g1",     // Quiet rook move
                         "e1f1",     // Quiet king move
                     })));
}

TEST(MoveOrdering, PriorityHierarchy) {
  Position position = MakePosition(
      "8: . . . . . . . q"
      "7: . . . . . . P ."
      "6: . . . . p . . ."
      "5: . . . . . . . ."
      "4: . . . N . . . ."
      "3: . . . . . . . ."
      "2: . . . . . . . ."
      "1: R . . . K . . ."
      "   a b c d e f g h"
      //
      "   w Q - 0 1");

  std::vector<Move> moves = MakeMoves({
      "d4c6",
      "e1c1#ooo",
      "g7g8q",
      "d4e6#c",
  });

  OrderMoves(position, moves);
  EXPECT_THAT(moves, ElementsAreArray(MakeMoves({
                         "d4e6#c",    // Capture
                         "g7g8q",     // Quiet promotion
                         "e1c1#ooo",  // Queenside castling
                         "d4c6",      // Quiet
                     })));
}

}  // namespace
}  // namespace follychess

#include "search/search.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "engine/move.h"
#include "engine/move_generator.h"
#include "engine/position.h"
#include "engine/scoped_move.h"
#include "engine/testing.h"

namespace follychess {
namespace {

using ::testing::ElementsAreArray;
using ::testing::Eq;

constexpr int kMaxMovesAllowed = 10;

[[nodiscard]] bool GameOver(Position position) {
  for (Move move : GenerateMoves(position)) {
    ScopedMove scoped_move(move, position);
    if (!position.GetCheckers(~position.SideToMove())) {
      return false;
    }
  }

  return true;
}

std::vector<Move> Play(Game& game) {
  std::vector<Move> moves;

  while (!GameOver(game.GetPosition())) {
    Move move = Search(game, SearchOptions().SetDepth(6));
    game.Do(move);
    moves.push_back(move);

    CHECK_LE(moves.size(), kMaxMovesAllowed)
        << "Reached maximum number of moves allowed.";
  }

  return moves;
}

TEST(Search, SimpleEndGames) {
  {
    Game game(
        MakePosition("8: k . . . . . . ."
                     "7: . . . . . . . ."
                     "6: . r . . . . . ."
                     "5: . . r . . . . ."
                     "4: . . . . . . . ."
                     "3: . . . . . . . ."
                     "2: . q . . . . . ."
                     "1: . . . . . . . K"
                     "   a b c d e f g h"
                     //
                     "   b - - 0 1"));

    std::vector<Move> moves = Play(game);
    EXPECT_THAT(moves, testing::SizeIs(testing::Lt(8)));
  }

  {
    Game game(
        MakePosition("8: . . . K . . . ."
                     "7: . . . . . . . ."
                     "6: . . . . . . . ."
                     "5: . R . . . . . ."
                     "4: . . R . . . . ."
                     "3: . . Q . . . . ."
                     "2: . . . . . . . ."
                     "1: . . . . . . . k"
                     "   a b c d e f g h"
                     //
                     "   w - - 0 1"));

    std::vector<Move> moves = Play(game);
    EXPECT_THAT(moves, testing::SizeIs(testing::Lt(8)));
  }
}

TEST(Search, Repetition) {
  {
    Game game(
        MakePosition("8: . k r . . b r ."
                     "7: p p N R . p p p"
                     "6: . . . . . . . ."
                     "5: . . p . n . . ."
                     "4: P . . . P . . P"
                     "3: . . . . . q P ."
                     "2: . . . . . P . ."
                     "1: . R . . . . K ."
                     "a b c d e f g h"
                     //
                     "w - - 0 30"));

    // Force three repetitions.
    for (int i = 0; i < 3; ++i) {
      game.Do(MakeMove("c7a6"));
      game.Do(MakeMove("b8a8"));
      game.Do(MakeMove("a6c7"));
      game.Do(MakeMove("a8b8"));
    }

    const Move move = Search(game, SearchOptions().SetDepth(6));
    EXPECT_THAT(move, Eq(MakeMove("c7a6")));
  }
}

}  // namespace
}  // namespace follychess
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
using ::testing::HasSubstr;
using ::testing::Optional;

constexpr int kMaxMovesAllowed = 16;

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
    EXPECT_THAT(moves, testing::SizeIs(testing::Lt(16)));
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
    EXPECT_THAT(moves, testing::SizeIs(testing::Lt(16)));
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

// TODO(aryann): Re-enable this or a similar test.
//
// TEST(Search, SpiteCheck) {
//   // Black will lose, but we expect the game to be delayed via a spite check.
//   Game game(
//       MakePosition("8: . . k r . b n r"
//                    "7: p . p . p p p p"
//                    "6: p . P . . q . ."
//                    "5: . . . P . . . ."
//                    "4: . . . P . . . ."
//                    "3: P Q N . . . . ."
//                    "2: . P . . . P P P"
//                    "1: R . B . K . . R"
//                    "a b c d e f g h"
//                    //
//                    "b KQ - 1 13"));
//
//   SearchInfo info;
//   const Move move = Search(
//       game,
//       SearchOptions()
//           .SetInfoObserver([&info](const SearchInfo& curr) { info = curr; })
//           .SetDepth(6));
//   EXPECT_THAT(move, Eq(MakeMove("f6f2#c")));
//   EXPECT_THAT(info.mate_in, Optional(-3));
// }

TEST(Search, WhiteMateInOne) {
  Game game(
      MakePosition("8: r . b q k . n r"
                   "7: p p p p . p p p"
                   "6: . . n . . . . ."
                   "5: . . b . p . . ."
                   "4: . . B . P . . ."
                   "3: . . . . . Q . ."
                   "2: P P P P . P P P"
                   "1: R N B . K . N R"
                   "   a b c d e f g h"
                   //
                   "w KQkq - 4 4"));

  SearchInfo info;
  const Move move = Search(
      game,
      SearchOptions()
          .SetInfoObserver([&info](const SearchInfo& curr) { info = curr; })
          .SetDepth(1));
  EXPECT_THAT(move, Eq(MakeMove("f3f7#c")));
  EXPECT_THAT(info.mate_in, Optional(1));
}

TEST(Search, BlackMateInOne) {
  Game game(
      MakePosition("8: r n b q k b n r"
                   "7: p p p p . p p p"
                   "6: . . . . . . . ."
                   "5: . . . . p . . ."
                   "4: . . . . . . P ."
                   "3: . . . . . P . ."
                   "2: P P P P P . . P"
                   "1: R N B Q K B N R"
                   "   a b c d e f g h"
                   //
                   "   b KQkq - 0 2"));

  SearchInfo info;
  const Move move = Search(
      game,
      SearchOptions()
          .SetInfoObserver([&info](const SearchInfo& curr) { info = curr; })
          .SetDepth(1));
  EXPECT_THAT(move, Eq(MakeMove("d8h4")));
  EXPECT_THAT(info.mate_in, Optional(1));
}

}  // namespace
}  // namespace follychess
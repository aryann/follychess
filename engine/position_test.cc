#include "position.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <expected>

#include "engine/testing.h"
#include "scoped_move.h"

namespace follychess {
namespace {

using ::testing::Eq;
using ::testing::IsFalse;
using ::testing::IsTrue;
using ::testing::Not;

constexpr std::string_view kStartingPosition =
    "8: r n b q k b n r"
    "7: p p p p p p p p"
    "6: . . . . . . . ."
    "5: . . . . . . . ."
    "4: . . . . . . . ."
    "3: . . . . . . . ."
    "2: P P P P P P P P"
    "1: R N B Q K B N R"
    "   a b c d e f g h"
    //
    "   w KQkq - 0 1";

TEST(Position, Starting) {
  EXPECT_THAT(Position::Starting(), EqualsPosition(kStartingPosition));
}

TEST(FEN, Starting) {
  std::expected<Position, std::string> position = Position::FromFen(
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

  ASSERT_THAT(position.has_value(), IsTrue()) << position.error();
  EXPECT_THAT(std::format("{}", position.value()), Eq("8: r n b q k b n r\n"
                                                      "7: p p p p p p p p\n"
                                                      "6: . . . . . . . .\n"
                                                      "5: . . . . . . . .\n"
                                                      "4: . . . . . . . .\n"
                                                      "3: . . . . . . . .\n"
                                                      "2: P P P P P P P P\n"
                                                      "1: R N B Q K B N R\n"
                                                      "   a b c d e f g h\n"
                                                      "\n"
                                                      "   w KQkq - 0 1\n"));
}

TEST(FEN, CastlingRights) {
  {
    std::expected<Position, std::string> position = Position::FromFen(
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1");

    ASSERT_THAT(position.has_value(), IsTrue()) << position.error();
    auto castling_rights = position.value().GetCastlingRights();
    EXPECT_THAT(std::format("{}", castling_rights), Eq("-"));
  }
  {
    std::expected<Position, std::string> position = Position::FromFen(
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    ASSERT_THAT(position.has_value(), IsTrue()) << position.error();
    auto castling_rights = position.value().GetCastlingRights();
    EXPECT_THAT(std::format("{}", castling_rights), Eq("KQkq"));
  }
  {
    std::expected<Position, std::string> position = Position::FromFen(
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w kq - 0 1");

    ASSERT_THAT(position.has_value(), IsTrue()) << position.error();
    auto castling_rights = position.value().GetCastlingRights();
    EXPECT_THAT(std::format("{}", castling_rights), Eq("kq"));
  }
  {
    std::expected<Position, std::string> position = Position::FromFen(
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w Kk - 0 1");

    ASSERT_THAT(position.has_value(), IsTrue()) << position.error();
    auto castling_rights = position.value().GetCastlingRights();
    EXPECT_THAT(std::format("{}", castling_rights), Eq("Kk"));
  }
  {
    std::expected<Position, std::string> position = Position::FromFen(
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w INVALID - 0 1");

    ASSERT_THAT(position.has_value(), IsFalse());
    EXPECT_THAT(position.error(), Eq("Invalid castling rights: INVALID"));
  }
}

TEST(FEN, HalfMoves) {
  {
    std::expected<Position, std::string> position = Position::FromFen(
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 30 1");

    ASSERT_THAT(position.has_value(), IsTrue()) << position.error();
    EXPECT_THAT(position.value().GetHalfMoves(), Eq(30));
  }
  {
    std::expected<Position, std::string> position = Position::FromFen(
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - INVALID 1");

    ASSERT_THAT(position.has_value(), IsFalse());
    EXPECT_THAT(position.error(), Eq("Invalid half moves: INVALID"));
  }
}

TEST(FEN, FullMoves) {
  {
    std::expected<Position, std::string> position = Position::FromFen(
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 50");

    ASSERT_THAT(position.has_value(), IsTrue()) << position.error();
    EXPECT_THAT(position.value().GetFullMoves(), Eq(50));
  }
  {
    std::expected<Position, std::string> position = Position::FromFen(
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 INVALID");

    ASSERT_THAT(position.has_value(), IsFalse());
    EXPECT_THAT(position.error(), Eq("Invalid full moves: INVALID"));
  }
}

TEST(FEN, Kiwipete) {
  std::expected<Position, std::string> position = Position::FromFen(
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

  ASSERT_THAT(position.has_value(), IsTrue()) << position.error();
  EXPECT_THAT(std::format("{}", position.value()), Eq("8: r . . . k . . r\n"
                                                      "7: p . p p q p b .\n"
                                                      "6: b n . . p n p .\n"
                                                      "5: . . . P N . . .\n"
                                                      "4: . p . . P . . .\n"
                                                      "3: . . N . . Q . p\n"
                                                      "2: P P P B B P P P\n"
                                                      "1: R . . . K . . R\n"
                                                      "   a b c d e f g h\n"
                                                      "\n"
                                                      "   w KQkq - 0 1\n"));
}

TEST(FEN, Sparse) {
  std::expected<Position, std::string> position =
      Position::FromFen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - a3 0 1");

  ASSERT_THAT(position.has_value(), IsTrue()) << position.error();
  EXPECT_THAT(std::format("{}", position.value()), Eq("8: . . . . . . . .\n"
                                                      "7: . . p . . . . .\n"
                                                      "6: . . . p . . . .\n"
                                                      "5: K P . . . . . r\n"
                                                      "4: . R . . . p . k\n"
                                                      "3: . . . . . . . .\n"
                                                      "2: . . . . P . P .\n"
                                                      "1: . . . . . . . .\n"
                                                      "   a b c d e f g h\n"
                                                      "\n"
                                                      "   w - a3 0 1\n"));
}

TEST(Position, DoAndUndo) {
  Position position = Position::Starting();

  std::string_view position_one =
      "8: r n b q k b n r"
      "7: p p p p p p p p"
      "6: . . . . . . . ."
      "5: . . . . . . . ."
      "4: . . . . . . . ."
      "3: . . N . . . . ."
      "2: P P P P P P P P"
      "1: R . B Q K B N R"
      "   a b c d e f g h"
      //
      "   b KQkq - 1 1";

  std::string_view position_two =
      "8: r n b q k b n r"
      "7: p p p . p p p p"
      "6: . . . . . . . ."
      "5: . . . p . . . ."
      "4: . . . . . . . ."
      "3: . . N . . . . ."
      "2: P P P P P P P P"
      "1: R . B Q K B N R"
      "   a b c d e f g h"
      //
      "   w KQkq d6 0 2";

  std::string_view position_three =
      "8: r n b q k b n r"
      "7: p p p . p p p p"
      "6: . . . . . . . ."
      "5: . . . N . . . ."
      "4: . . . . . . . ."
      "3: . . . . . . . ."
      "2: P P P P P P P P"
      "1: R . B Q K B N R"
      "   a b c d e f g h"
      //
      "   b KQkq - 0 2";

  UndoInfo move_one = position.Do(Move(B1, C3));
  EXPECT_THAT(position, EqualsPosition(position_one));

  UndoInfo move_two = position.Do(Move(D7, D5, Move::Flags::kDoublePawnPush));
  EXPECT_THAT(position, EqualsPosition(position_two));

  UndoInfo move_three = position.Do(Move(C3, D5));
  EXPECT_THAT(position, EqualsPosition(position_three));

  position.Undo(move_three);
  EXPECT_THAT(position, EqualsPosition(position_two));

  position.Undo(move_two);
  EXPECT_THAT(position, EqualsPosition(position_one));

  position.Undo(move_one);
  EXPECT_THAT(position, EqualsPosition(kStartingPosition));
}

TEST(GetPieces, StartingPosition) {
  Position position = Position::Starting();

  EXPECT_THAT(position.GetPieces(), EqualsBitboard("8: X X X X X X X X"
                                                   "7: X X X X X X X X"
                                                   "6: . . . . . . . ."
                                                   "5: . . . . . . . ."
                                                   "4: . . . . . . . ."
                                                   "3: . . . . . . . ."
                                                   "2: X X X X X X X X"
                                                   "1: X X X X X X X X"
                                                   "   a b c d e f g h"));

  EXPECT_THAT(position.GetPieces(kWhite), EqualsBitboard("8: . . . . . . . ."
                                                         "7: . . . . . . . ."
                                                         "6: . . . . . . . ."
                                                         "5: . . . . . . . ."
                                                         "4: . . . . . . . ."
                                                         "3: . . . . . . . ."
                                                         "2: X X X X X X X X"
                                                         "1: X X X X X X X X"
                                                         "   a b c d e f g h"));
  EXPECT_THAT(position.GetPieces(kWhite, kPawn),
              EqualsBitboard("8: . . . . . . . ."
                             "7: . . . . . . . ."
                             "6: . . . . . . . ."
                             "5: . . . . . . . ."
                             "4: . . . . . . . ."
                             "3: . . . . . . . ."
                             "2: X X X X X X X X"
                             "1: . . . . . . . ."
                             "   a b c d e f g h"));
  EXPECT_THAT(position.GetPieces(kWhite, kKnight),
              EqualsBitboard("8: . . . . . . . ."
                             "7: . . . . . . . ."
                             "6: . . . . . . . ."
                             "5: . . . . . . . ."
                             "4: . . . . . . . ."
                             "3: . . . . . . . ."
                             "2: . . . . . . . ."
                             "1: . X . . . . X ."
                             "   a b c d e f g h"));
  EXPECT_THAT(position.GetPieces(kWhite, kBishop),
              EqualsBitboard("8: . . . . . . . ."
                             "7: . . . . . . . ."
                             "6: . . . . . . . ."
                             "5: . . . . . . . ."
                             "4: . . . . . . . ."
                             "3: . . . . . . . ."
                             "2: . . . . . . . ."
                             "1: . . X . . X . ."
                             "   a b c d e f g h"));
  EXPECT_THAT(position.GetPieces(kWhite, kRook),
              EqualsBitboard("8: . . . . . . . ."
                             "7: . . . . . . . ."
                             "6: . . . . . . . ."
                             "5: . . . . . . . ."
                             "4: . . . . . . . ."
                             "3: . . . . . . . ."
                             "2: . . . . . . . ."
                             "1: X . . . . . . X"
                             "   a b c d e f g h"));
  EXPECT_THAT(position.GetPieces(kWhite, kQueen),
              EqualsBitboard("8: . . . . . . . ."
                             "7: . . . . . . . ."
                             "6: . . . . . . . ."
                             "5: . . . . . . . ."
                             "4: . . . . . . . ."
                             "3: . . . . . . . ."
                             "2: . . . . . . . ."
                             "1: . . . X . . . ."
                             "   a b c d e f g h"));
  EXPECT_THAT(position.GetPieces(kWhite, kKing),
              EqualsBitboard("8: . . . . . . . ."
                             "7: . . . . . . . ."
                             "6: . . . . . . . ."
                             "5: . . . . . . . ."
                             "4: . . . . . . . ."
                             "3: . . . . . . . ."
                             "2: . . . . . . . ."
                             "1: . . . . X . . ."
                             "   a b c d e f g h"));

  EXPECT_THAT(position.GetPieces(kBlack), EqualsBitboard("8: X X X X X X X X"
                                                         "7: X X X X X X X X"
                                                         "6: . . . . . . . ."
                                                         "5: . . . . . . . ."
                                                         "4: . . . . . . . ."
                                                         "3: . . . . . . . ."
                                                         "2: . . . . . . . ."
                                                         "1: . . . . . . . ."
                                                         "   a b c d e f g h"));
  EXPECT_THAT(position.GetPieces(kBlack, kPawn),
              EqualsBitboard("8: . . . . . . . ."
                             "7: X X X X X X X X"
                             "6: . . . . . . . ."
                             "5: . . . . . . . ."
                             "4: . . . . . . . ."
                             "3: . . . . . . . ."
                             "2: . . . . . . . ."
                             "1: . . . . . . . ."
                             "   a b c d e f g h"));
  EXPECT_THAT(position.GetPieces(kBlack, kKnight),
              EqualsBitboard("8: . X . . . . X ."
                             "7: . . . . . . . ."
                             "6: . . . . . . . ."
                             "5: . . . . . . . ."
                             "4: . . . . . . . ."
                             "3: . . . . . . . ."
                             "2: . . . . . . . ."
                             "1: . . . . . . . ."
                             "   a b c d e f g h"));
  EXPECT_THAT(position.GetPieces(kBlack, kBishop),
              EqualsBitboard("8: . . X . . X . ."
                             "7: . . . . . . . ."
                             "6: . . . . . . . ."
                             "5: . . . . . . . ."
                             "4: . . . . . . . ."
                             "3: . . . . . . . ."
                             "2: . . . . . . . ."
                             "1: . . . . . . . ."
                             "   a b c d e f g h"));
  EXPECT_THAT(position.GetPieces(kBlack, kRook),
              EqualsBitboard("8: X . . . . . . X"
                             "7: . . . . . . . ."
                             "6: . . . . . . . ."
                             "5: . . . . . . . ."
                             "4: . . . . . . . ."
                             "3: . . . . . . . ."
                             "2: . . . . . . . ."
                             "1: . . . . . . . ."
                             "   a b c d e f g h"));
  EXPECT_THAT(position.GetPieces(kBlack, kQueen),
              EqualsBitboard("8: . . . X . . . ."
                             "7: . . . . . . . ."
                             "6: . . . . . . . ."
                             "5: . . . . . . . ."
                             "4: . . . . . . . ."
                             "3: . . . . . . . ."
                             "2: . . . . . . . ."
                             "1: . . . . . . . ."
                             "   a b c d e f g h"));
  EXPECT_THAT(position.GetPieces(kBlack, kKing),
              EqualsBitboard("8: . . . . X . . ."
                             "7: . . . . . . . ."
                             "6: . . . . . . . ."
                             "5: . . . . . . . ."
                             "4: . . . . . . . ."
                             "3: . . . . . . . ."
                             "2: . . . . . . . ."
                             "1: . . . . . . . ."
                             "   a b c d e f g h"));
}

TEST(GetAttackers, Pawn) {
  Position position = MakePosition(
      "8: . . . . . . . ."
      "7: . . . . . . . ."
      "6: . . . . . . . ."
      "5: . . p p p p p ."
      "4: . . P . . . P ."
      "3: . . P P P P P ."
      "2: . . . . . . . ."
      "1: . . . . . . . ."
      "   a b c d e f g h"
      //
      "   w KQkq - 0 1");

  EXPECT_THAT(position.GetAttackers(E4, kBlack),
              EqualsBitboard("8: . . . . . . . ."
                             "7: . . . . . . . ."
                             "6: . . . . . . . ."
                             "5: . . . X . X . ."
                             "4: . . . . . . . ."
                             "3: . . . . . . . ."
                             "2: . . . . . . . ."
                             "1: . . . . . . . ."
                             "   a b c d e f g h"));
}

TEST(GetAttackers, Rook) {
  {
    Position position = MakePosition(
        "8: . . . r . . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: . r r . . r . ."
        "   a b c d e f g h"
        //
        "   w KQkq - 0 1");

    EXPECT_THAT(position.GetAttackers(D1, kBlack),
                EqualsBitboard("8: . . . X . . . ."
                               "7: . . . . . . . ."
                               "6: . . . . . . . ."
                               "5: . . . . . . . ."
                               "4: . . . . . . . ."
                               "3: . . . . . . . ."
                               "2: . . . . . . . ."
                               "1: . . X . . X . ."
                               "   a b c d e f g h"));
  }
  {
    Position position = MakePosition(
        "8: . . . r . . . ."
        "7: . R . r . . . ."
        "6: . . . R . . . ."
        "5: . . . . . . . ."
        "4: R R R . . r R r"
        "3: . . . r . . . ."
        "2: . . . R . . r ."
        "1: . . . r . . . ."
        "   a b c d e f g h"
        //
        "   w KQkq - 0 1");

    EXPECT_THAT(position.GetAttackers(D4, kWhite),
                EqualsBitboard("8: . . . . . . . ."
                               "7: . . . . . . . ."
                               "6: . . . X . . . ."
                               "5: . . . . . . . ."
                               "4: . . X . . . . ."
                               "3: . . . . . . . ."
                               "2: . . . . . . . ."
                               "1: . . . . . . . ."
                               "   a b c d e f g h"));
  }
}

TEST(GetAttackers, Bishop) {
  Position position = MakePosition(
      "8: . . . . . . . ."
      "7: . . . . . . . ."
      "6: . b . . . B . ."
      "5: . . . . . . . ."
      "4: . . . . . . . ."
      "3: . . . . . . . ."
      "2: . B . . . b . ."
      "1: B . . . . . . ."
      "   a b c d e f g h"
      //
      "   w KQkq - 0 1");

  EXPECT_THAT(position.GetAttackers(D4, kWhite),
              EqualsBitboard("8: . . . . . . . ."
                             "7: . . . . . . . ."
                             "6: . . . . . X . ."
                             "5: . . . . . . . ."
                             "4: . . . . . . . ."
                             "3: . . . . . . . ."
                             "2: . X . . . . . ."
                             "1: . . . . . . . ."
                             "   a b c d e f g h"));
}

TEST(GetAttackers, Queen) {
  Position position = MakePosition(
      "8: . . . Q . . . Q"
      "7: . . . . . . B ."
      "6: . q . . . q . ."
      "5: . . . r . . . ."
      "4: Q . n . . . . q"
      "3: . . . . . . . ."
      "2: . . R . q . . ."
      "1: Q . . . . . . ."
      "   a b c d e f g h"
      //
      "   w KQkq - 0 1");

  EXPECT_THAT(position.GetAttackers(D4, kBlack),
              EqualsBitboard("8: . . . . . . . ."
                             "7: . . . . . . . ."
                             "6: . X . . . X . ."
                             "5: . . . X . . . ."
                             "4: . . . . . . . X"
                             "3: . . . . . . . ."
                             "2: . . . . . . . ."
                             "1: . . . . . . . ."
                             "   a b c d e f g h"));
}

TEST(GetAttackers, Knight) {
  Position position = MakePosition(
      "8: . . . . . . . ."
      "7: . . . . . . . ."
      "6: . . . n . N . ."
      "5: . . N . . . n ."
      "4: . . . . . . . ."
      "3: . . N . . . N ."
      "2: . . . N . n . ."
      "1: . . . . . . . ."
      "   a b c d e f g h"
      //
      "   w KQkq - 0 1");

  EXPECT_THAT(position.GetAttackers(E4, kWhite),
              EqualsBitboard("8: . . . . . . . ."
                             "7: . . . . . . . ."
                             "6: . . . . . X . ."
                             "5: . . X . . . . ."
                             "4: . . . . . . . ."
                             "3: . . X . . . X ."
                             "2: . . . X . . . ."
                             "1: . . . . . . . ."
                             "   a b c d e f g h"));
}

TEST(GetAttackers, King) {
  Position position = MakePosition(
      "8: . K . . . . . ."
      "7: . . . . . . . ."
      "6: . . . . . . . ."
      "5: . . . K k K . ."
      "4: . . . k . K . ."
      "3: . . . k K k . ."
      "2: . . . . . . . ."
      "1: . k . . . . . ."
      "   a b c d e f g h"
      //
      "   w KQkq - 0 1");

  EXPECT_THAT(position.GetAttackers(E4, kWhite),
              EqualsBitboard("8: . . . . . . . ."
                             "7: . . . . . . . ."
                             "6: . . . . . . . ."
                             "5: . . . X . X . ."
                             "4: . . . . . X . ."
                             "3: . . . . X . . ."
                             "2: . . . . . . . ."
                             "1: . . . . . . . ."
                             "   a b c d e f g h"));
}

TEST(GetCheckers, NoCheck) {
  {
    Position position = MakePosition(
        "8: r n b q k b n r"
        "7: p p p p . p p p"
        "6: . . . . . . . ."
        "5: . . . . p . . ."
        "4: . . . . P . . ."
        "3: . . . . . N . ."
        "2: P P P P . P P P"
        "1: R N B Q K B . R"
        "   a b c d e f g h"
        //
        "   w KQkq - 0 2");

    EXPECT_THAT(position.GetCheckers(kWhite),
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
  {
    Position position = MakePosition(
        "8: r . . . k . . r"
        "7: p p . . q p b p"
        "6: . . n p . n . ."
        "5: . . b . P . p ."
        "4: . . P B . . . ."
        "3: . . . . . N . ."
        "2: P P . . . P P P"
        "1: R . . Q K . B R"
        "   a b c d e f g h"
        //
        "   b - - 0 15");

    EXPECT_THAT(position.GetCheckers(kBlack),
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
}

TEST(GetCheckers, SingleCheck) {
  {
    Position position = MakePosition(
        "8: . . . . q . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: . . . . K . . ."
        "   a b c d e f g h"
        //
        "   w - - 0 1");

    EXPECT_THAT(position.GetCheckers(kWhite),
                EqualsBitboard("8: . . . . X . . ."
                               "7: . . . . . . . ."
                               "6: . . . . . . . ."
                               "5: . . . . . . . ."
                               "4: . . . . . . . ."
                               "3: . . . . . . . ."
                               "2: . . . . . . . ."
                               "1: . . . . . . . ."
                               "   a b c d e f g h"));
  }
  {
    Position position = MakePosition(
        "8: . . . . . . k ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: . . . . . . R ."
        "   a b c d e f g h"
        //
        "   b - - 0 1");

    EXPECT_THAT(position.GetCheckers(kBlack),
                EqualsBitboard("8: . . . . . . . ."
                               "7: . . . . . . . ."
                               "6: . . . . . . . ."
                               "5: . . . . . . . ."
                               "4: . . . . . . . ."
                               "3: . . . . . . . ."
                               "2: . . . . . . . ."
                               "1: . . . . . . X ."
                               "   a b c d e f g h"));
  }
  {
    Position position = MakePosition(
        "8: . . . . . . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . b . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: . . K . . . . ."
        "   a b c d e f g h"
        //
        "   w - - 0 1");

    EXPECT_THAT(position.GetCheckers(kWhite),
                EqualsBitboard("8: . . . . . . . ."
                               "7: . . . . . . . ."
                               "6: . . . . . . . ."
                               "5: . . . . . . . ."
                               "4: . . . . . X . ."
                               "3: . . . . . . . ."
                               "2: . . . . . . . ."
                               "1: . . . . . . . ."
                               "   a b c d e f g h"));
  }
  {
    Position position = MakePosition(
        "8: . . . . k . . ."
        "7: . . . . . . . ."
        "6: . . . . . N . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: . . . . . . . ."
        "   a b c d e f g h"
        //
        "   b - - 0 1");

    EXPECT_THAT(position.GetCheckers(kBlack),
                EqualsBitboard("8: . . . . . . . ."
                               "7: . . . . . . . ."
                               "6: . . . . . X . ."
                               "5: . . . . . . . ."
                               "4: . . . . . . . ."
                               "3: . . . . . . . ."
                               "2: . . . . . . . ."
                               "1: . . . . . . . ."
                               "   a b c d e f g h"));
  }
  {
    Position position = MakePosition(
        "8: . . . . . . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . p . . . ."
        "4: . . . . K . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: . . . . . . . ."
        "   a b c d e f g h"
        //
        "   w - - 0 1");

    EXPECT_THAT(position.GetCheckers(kWhite),
                EqualsBitboard("8: . . . . . . . ."
                               "7: . . . . . . . ."
                               "6: . . . . . . . ."
                               "5: . . . X . . . ."
                               "4: . . . . . . . ."
                               "3: . . . . . . . ."
                               "2: . . . . . . . ."
                               "1: . . . . . . . ."
                               "   a b c d e f g h"));
  }
  {
    Position position = MakePosition(
        "8: . . . . . . . ."
        "7: . . p . . . . ."
        "6: . K . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: . . . . . . . ."
        "   a b c d e f g h"
        //
        "   w - - 0 1");

    EXPECT_THAT(position.GetCheckers(kWhite),
                EqualsBitboard("8: . . . . . . . ."
                               "7: . . X . . . . ."
                               "6: . . . . . . . ."
                               "5: . . . . . . . ."
                               "4: . . . . . . . ."
                               "3: . . . . . . . ."
                               "2: . . . . . . . ."
                               "1: . . . . . . . ."
                               "   a b c d e f g h"));
  }
}

TEST(GetCheckers, DoubleCheck) {
  {
    Position position = MakePosition(
        "8: . . . . . . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . b . . n . ."
        "2: . . . . . . . ."
        "1: . . . . K . . ."
        "   a b c d e f g h"
        //
        "   w - - 0 10");

    EXPECT_THAT(position.GetCheckers(kWhite),
                EqualsBitboard("8: . . . . . . . ."
                               "7: . . . . . . . ."
                               "6: . . . . . . . ."
                               "5: . . . . . . . ."
                               "4: . . . . . . . ."
                               "3: . . X . . X . ."
                               "2: . . . . . . . ."
                               "1: . . . . . . . ."
                               "   a b c d e f g h"));
  }
  {
    Position position = MakePosition(
        "8: . . . . k . . ."
        "7: . . . . . . . ."
        "6: . . . . . N . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: . . . . R . . ."
        "   a b c d e f g h"
        //
        "   b - - 0 12");

    EXPECT_THAT(position.GetCheckers(kBlack),
                EqualsBitboard("8: . . . . . . . ."
                               "7: . . . . . . . ."
                               "6: . . . . . X . ."
                               "5: . . . . . . . ."
                               "4: . . . . . . . ."
                               "3: . . . . . . . ."
                               "2: . . . . . . . ."
                               "1: . . . . X . . ."
                               "   a b c d e f g h"));
  }
  {
    Position position = MakePosition(
        "8: r . b q k . . r"
        "7: p p p . . p p p"
        "6: . . . . . N . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: P P P P . P P P"
        "1: R . B Q R . K ."
        "   a b c d e f g h"
        //
        "   b kq - 0 10");

    EXPECT_THAT(position.GetCheckers(kBlack),
                EqualsBitboard("8: . . . . . . . ."
                               "7: . . . . . . . ."
                               "6: . . . . . X . ."
                               "5: . . . . . . . ."
                               "4: . . . . . . . ."
                               "3: . . . . . . . ."
                               "2: . . . . . . . ."
                               "1: . . . . X . . ."
                               "   a b c d e f g h"));
  }
}

TEST(GetCheckers, Pawns) {
  {
    Position position = MakePosition(
        "8: . . . . . . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . P . P . . ."
        "4: . . . K . . . ."
        "3: . . P . P . . ."
        "2: . . . . . . . ."
        "1: . . . . . . . ."
        "   a b c d e f g h"
        //
        "   w - - 0 1");

    EXPECT_THAT(position.GetCheckers(kWhite),
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
  {
    Position position = MakePosition(
        "8: . . . . . . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . p . p . . ."
        "4: . . . K . . . ."
        "3: . . p . p . . ."
        "2: . . . . . . . ."
        "1: . . . . . . . ."
        "   a b c d e f g h"
        //
        "   w - - 0 1");

    EXPECT_THAT(position.GetCheckers(kWhite),
                EqualsBitboard("8: . . . . . . . ."
                               "7: . . . . . . . ."
                               "6: . . . . . . . ."
                               "5: . . X . X . . ."
                               "4: . . . . . . . ."
                               "3: . . . . . . . ."
                               "2: . . . . . . . ."
                               "1: . . . . . . . ."
                               "   a b c d e f g h"));
  }
}

TEST(EnPassant, White) {
  Position position = Position::Starting();

  EXPECT_THAT(position.GetEnPassantTarget(), Eq(std::nullopt));

  position.Do(MakeMove("d2d3"));
  EXPECT_THAT(position.GetEnPassantTarget(), Eq(std::nullopt));

  position.Do(MakeMove("d7d6"));
  EXPECT_THAT(position.GetEnPassantTarget(), Eq(std::nullopt));

  position.Do(MakeMove("a2a4#dpp"));
  EXPECT_THAT(position.GetEnPassantTarget(), Eq(A3));

  position.Do(MakeMove("a7a6"));
  EXPECT_THAT(position.GetEnPassantTarget(), Eq(std::nullopt));
}

TEST(EnPassant, Black) {
  Position position = Position::Starting();

  EXPECT_THAT(position.GetEnPassantTarget(), Eq(std::nullopt));

  position.Do(MakeMove("d2d3"));
  EXPECT_THAT(position.GetEnPassantTarget(), Eq(std::nullopt));

  position.Do(MakeMove("d7d5#dpp"));
  EXPECT_THAT(position.GetEnPassantTarget(), Eq(D6));

  position.Do(MakeMove("a2a3"));
  EXPECT_THAT(position.GetEnPassantTarget(), Eq(std::nullopt));
}

TEST(EnPassant, Capture) {
  std::string_view initial =
      "8: . . . . . . . ."
      "7: . . . . . . . ."
      "6: . . . . . . . ."
      "5: . . . p P . . ."
      "4: . . . . . . . ."
      "3: . . . . . . . ."
      "2: . . . . . . . ."
      "1: . . . . . . . ."
      "   a b c d e f g h"
      //
      "   w KQkq d6 2 10";

  Position position = MakePosition(initial);

  UndoInfo move = position.Do(MakeMove("e5d6#ep"));
  EXPECT_THAT(position, EqualsPosition("8: . . . . . . . ."
                                       "7: . . . . . . . ."
                                       "6: . . . P . . . ."
                                       "5: . . . . . . . ."
                                       "4: . . . . . . . ."
                                       "3: . . . . . . . ."
                                       "2: . . . . . . . ."
                                       "1: . . . . . . . ."
                                       "   a b c d e f g h"
                                       //
                                       "   b KQkq - 0 10"));

  position.Undo(move);
  EXPECT_THAT(position, EqualsPosition(initial));
}

TEST(EnPassant, OtherPiecesCanMoveToEnPassantTarget) {
  static constexpr absl::string_view kInitialPosition =
      "8: . . . . . . . ."
      "7: . . p . . . . ."
      "6: . . . p . . . ."
      "5: K P . . . . . r"
      "4: . R . . . p P k"
      "3: . . . . . . . ."
      "2: . . . . P . . ."
      "1: . . . . . . . ."
      "   a b c d e f g h"
      //
      "   b - g3 1 1";

  Position position = MakePosition(kInitialPosition);

  {
    ScopedMove move(Move(H4, G3), position);
    EXPECT_THAT(position, EqualsPosition("8: . . . . . . . ."
                                         "7: . . p . . . . ."
                                         "6: . . . p . . . ."
                                         "5: K P . . . . . r"
                                         "4: . R . . . p P ."
                                         "3: . . . . . . k ."
                                         "2: . . . . P . . ."
                                         "1: . . . . . . . ."
                                         "   a b c d e f g h"
                                         //
                                         "   w - - 2 2"));
  }

  EXPECT_THAT(position, EqualsPosition(kInitialPosition));
}

TEST(QuietPromotion, White) {
  Position position = MakePosition(
      "8: . . . . . . . ."
      "7: . . . P . . . ."
      "6: . . . . . . . ."
      "5: . . . . . . . ."
      "4: . . . . . . . ."
      "3: . . . . . . . ."
      "2: . . . . . . . ."
      "1: . . . . . . . ."
      "   a b c d e f g h"
      //
      "   w KQkq - 0 1");

  {
    ScopedMove scoped_move(MakeMove("d7d8q"), position);
    EXPECT_THAT(position, EqualsPosition("8: . . . Q . . . ."
                                         "7: . . . . . . . ."
                                         "6: . . . . . . . ."
                                         "5: . . . . . . . ."
                                         "4: . . . . . . . ."
                                         "3: . . . . . . . ."
                                         "2: . . . . . . . ."
                                         "1: . . . . . . . ."
                                         "   a b c d e f g h"
                                         //
                                         "   b KQkq - 0 1"));
  }
  EXPECT_THAT(position, EqualsPosition("8: . . . . . . . ."
                                       "7: . . . P . . . ."
                                       "6: . . . . . . . ."
                                       "5: . . . . . . . ."
                                       "4: . . . . . . . ."
                                       "3: . . . . . . . ."
                                       "2: . . . . . . . ."
                                       "1: . . . . . . . ."
                                       "   a b c d e f g h"
                                       //
                                       "   w KQkq - 0 1"));

  {
    ScopedMove scoped_move(MakeMove("d7d8n"), position);
    EXPECT_THAT(position.GetPiece(D8), Eq(kKnight));
  }

  {
    ScopedMove scoped_move(MakeMove("d7d8b"), position);
    EXPECT_THAT(position.GetPiece(D8), Eq(kBishop));
  }

  {
    ScopedMove scoped_move(MakeMove("d7d8r"), position);
    EXPECT_THAT(position.GetPiece(D8), Eq(kRook));
  }
}

TEST(QuietPromotion, Black) {
  Position position = MakePosition(
      "8: . . . . . . . ."
      "7: . . . . . . . ."
      "6: . . . . . . . ."
      "5: . . . . . . . ."
      "4: . . . . . . . ."
      "3: . . . . . . . ."
      "2: . . . . . . . p"
      "1: . . . . . . . ."
      "   a b c d e f g h"
      //
      "   b KQkq - 0 1");

  {
    ScopedMove scoped_move(MakeMove("h2h1q"), position);
    EXPECT_THAT(position, EqualsPosition("8: . . . . . . . ."
                                         "7: . . . . . . . ."
                                         "6: . . . . . . . ."
                                         "5: . . . . . . . ."
                                         "4: . . . . . . . ."
                                         "3: . . . . . . . ."
                                         "2: . . . . . . . ."
                                         "1: . . . . . . . q"
                                         "   a b c d e f g h"
                                         //
                                         "   w Qkq - 0 2"));
  }
  EXPECT_THAT(position, EqualsPosition("8: . . . . . . . ."
                                       "7: . . . . . . . ."
                                       "6: . . . . . . . ."
                                       "5: . . . . . . . ."
                                       "4: . . . . . . . ."
                                       "3: . . . . . . . ."
                                       "2: . . . . . . . p"
                                       "1: . . . . . . . ."
                                       "   a b c d e f g h"
                                       //
                                       "   b KQkq - 0 1"));

  {
    ScopedMove scoped_move(MakeMove("h2h1n"), position);
    EXPECT_THAT(position.GetPiece(H1), Eq(kKnight));
  }

  {
    ScopedMove scoped_move(MakeMove("h2h1b"), position);
    EXPECT_THAT(position.GetPiece(H1), Eq(kBishop));
  }

  {
    ScopedMove scoped_move(MakeMove("h2h1r"), position);
    EXPECT_THAT(position.GetPiece(H1), Eq(kRook));
  }
}

TEST(Castling, WhiteKing) {
  {
    Position position = MakePosition(
        "8: . . . . . . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: R . . . K . . R"
        "   a b c d e f g h"
        //
        "   w KQkq - 0 1");

    UndoInfo undo_info = position.Do(MakeMove("e1g1#oo"));
    EXPECT_THAT(position, EqualsPosition("8: . . . . . . . ."
                                         "7: . . . . . . . ."
                                         "6: . . . . . . . ."
                                         "5: . . . . . . . ."
                                         "4: . . . . . . . ."
                                         "3: . . . . . . . ."
                                         "2: . . . . . . . ."
                                         "1: R . . . . R K ."
                                         "   a b c d e f g h"
                                         //
                                         "   b kq - 1 1"));

    position.Undo(undo_info);
    EXPECT_THAT(position, EqualsPosition("8: . . . . . . . ."
                                         "7: . . . . . . . ."
                                         "6: . . . . . . . ."
                                         "5: . . . . . . . ."
                                         "4: . . . . . . . ."
                                         "3: . . . . . . . ."
                                         "2: . . . . . . . ."
                                         "1: R . . . K . . R"
                                         "   a b c d e f g h"
                                         //
                                         "   w KQkq - 0 1"));
  }
  {
    Position position = MakePosition(
        "8: . . . . . . . ."
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: R . . . K . . R"
        "   a b c d e f g h"
        //
        "   w KQkq - 0 1");

    UndoInfo undo_info = position.Do(MakeMove("e1c1#ooo"));
    EXPECT_THAT(position, EqualsPosition("8: . . . . . . . ."
                                         "7: . . . . . . . ."
                                         "6: . . . . . . . ."
                                         "5: . . . . . . . ."
                                         "4: . . . . . . . ."
                                         "3: . . . . . . . ."
                                         "2: . . . . . . . ."
                                         "1: . . K R . . . R"
                                         "   a b c d e f g h"
                                         //
                                         "   b kq - 1 1"));

    position.Undo(undo_info);
    EXPECT_THAT(position, EqualsPosition("8: . . . . . . . ."
                                         "7: . . . . . . . ."
                                         "6: . . . . . . . ."
                                         "5: . . . . . . . ."
                                         "4: . . . . . . . ."
                                         "3: . . . . . . . ."
                                         "2: . . . . . . . ."
                                         "1: R . . . K . . R"
                                         "   a b c d e f g h"
                                         //
                                         "   w KQkq - 0 1"));
  }
}

TEST(Castling, BlackKing) {
  {
    Position position = MakePosition(
        "8: r . . . k . . r"
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: . . . . . . . ."
        "   a b c d e f g h"
        //
        "   b KQkq - 0 1");

    UndoInfo undo_info = position.Do(MakeMove("e8g8#oo"));
    EXPECT_THAT(position, EqualsPosition("8: r . . . . r k ."
                                         "7: . . . . . . . ."
                                         "6: . . . . . . . ."
                                         "5: . . . . . . . ."
                                         "4: . . . . . . . ."
                                         "3: . . . . . . . ."
                                         "2: . . . . . . . ."
                                         "1: . . . . . . . ."
                                         "   a b c d e f g h"
                                         //
                                         "   w KQ - 1 2"));

    position.Undo(undo_info);
    EXPECT_THAT(position, EqualsPosition("8: r . . . k . . r"
                                         "7: . . . . . . . ."
                                         "6: . . . . . . . ."
                                         "5: . . . . . . . ."
                                         "4: . . . . . . . ."
                                         "3: . . . . . . . ."
                                         "2: . . . . . . . ."
                                         "1: . . . . . . . ."
                                         "   a b c d e f g h"
                                         //
                                         "   b KQkq - 0 1"));
  }
  {
    Position position = MakePosition(
        "8: r . . . k . . r"
        "7: . . . . . . . ."
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: . . . . . . . ."
        "1: . . . . . . . ."
        "   a b c d e f g h"
        //
        "   b KQkq - 0 1");

    UndoInfo undo_info = position.Do(MakeMove("e8c8#ooo"));
    EXPECT_THAT(position, EqualsPosition("8: . . k r . . . r"
                                         "7: . . . . . . . ."
                                         "6: . . . . . . . ."
                                         "5: . . . . . . . ."
                                         "4: . . . . . . . ."
                                         "3: . . . . . . . ."
                                         "2: . . . . . . . ."
                                         "1: . . . . . . . ."
                                         "   a b c d e f g h"
                                         //
                                         "   w KQ - 1 2"

                                         ));

    position.Undo(undo_info);
    EXPECT_THAT(position, EqualsPosition("8: r . . . k . . r"
                                         "7: . . . . . . . ."
                                         "6: . . . . . . . ."
                                         "5: . . . . . . . ."
                                         "4: . . . . . . . ."
                                         "3: . . . . . . . ."
                                         "2: . . . . . . . ."
                                         "1: . . . . . . . ."
                                         "   a b c d e f g h"
                                         //
                                         "   b KQkq - 0 1"));
  }
}

TEST(Position, Key) {
  Position position = Position::Starting();
  std::uint64_t v0 = position.GetKey();

  {
    ScopedMove first(Move(E2, E4), position);
    std::uint64_t v1 = position.GetKey();
    EXPECT_THAT(v1, Not(Eq(v0)));

    {
      ScopedMove second(Move(E7, E5), position);
      std::uint64_t v2 = position.GetKey();
      EXPECT_THAT(v2, Not(Eq(v1)));

      {
        ScopedMove third(Move(B1, C3), position);
        EXPECT_THAT(position.GetKey(), Not(Eq(v2)));
      }
      EXPECT_THAT(position.GetKey(), Eq(v2));
    }

    EXPECT_THAT(position.GetKey(), Eq(v1));
  }

  EXPECT_THAT(position.GetKey(), Eq(v0));
}

TEST(HalfMoveClock, ResetsOnPawnMovesAndCaptures) {
  // Quiet move:
  {
    Position position = MakePosition(
        "8: r n b q k b n r"
        "7: p p p p p p p p"
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: P P P P P P P P"
        "1: R N B Q K B N R"
        "   a b c d e f g h"
        //
        "   w KQkq - 50 1");

    ScopedMove move(MakeMove("g1f3"), position);
    EXPECT_THAT(position.GetHalfMoves(), Eq(51));
  }

  // Pawn push:
  {
    Position position = MakePosition(
        "8: r n b q k b n r"
        "7: p p p p p p p p"
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: P P P P P P P P"
        "1: R N B Q K B N R"
        "   a b c d e f g h"
        //
        "   w KQkq - 50 1");

    ScopedMove move(MakeMove("e2e3"), position);
    EXPECT_THAT(position.GetHalfMoves(), Eq(0));
  }

  // Double pawn push:
  {
    Position position = MakePosition(
        "8: r n b q k b n r"
        "7: p p p p p p p p"
        "6: . . . . . . . ."
        "5: . . . . . . . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: P P P P P P P P"
        "1: R N B Q K B N R"
        "   a b c d e f g h"
        //
        "   w KQkq - 50 1");

    ScopedMove move(MakeMove("e2e4#dpp"), position);
    EXPECT_THAT(position.GetHalfMoves(), Eq(0));
  }

  // Capture:
  {
    Position position = MakePosition(
        "8: r . . . k . . r"
        "7: p p p . . p p p"
        "6: . . . . . . . ."
        "5: . . . p q . . ."
        "4: . . . . P . . ."
        "3: . . . . . . . ."
        "2: P P P P . P P P"
        "1: R N B Q K B N R"
        "   a b c d e f g h"
        //
        "   w KQkq - 50 1");

    ScopedMove move(MakeMove("e4d5#c"), position);
    EXPECT_THAT(position.GetHalfMoves(), Eq(0));
  }

  // En-Passant capture:
  {
    Position position = MakePosition(
        "8: r . . . k . . r"
        "7: p p p . . . p p"
        "6: . . . . . . . ."
        "5: . . . p P p . ."
        "4: . . . . . . . ."
        "3: . . . . . . . ."
        "2: P P P P . P P P"
        "1: R N B Q K B N R"
        "   a b c d e f g h"
        //
        "   w KQkq f6 50 1");

    ScopedMove move(MakeMove("e5f6#ep"), position);
    EXPECT_THAT(position.GetHalfMoves(), Eq(0));
  }
}

TEST(NullMove, DoAndUndo) {
  std::string_view start =
      "8: r n b q k b n r"
      "7: p p p . p p p p"
      "6: . . . . . . . ."
      "5: . . . N . . . ."
      "4: . . . . . . . ."
      "3: . . . . . . . ."
      "2: P P P P P P P P"
      "1: R . B Q K B N R"
      "   a b c d e f g h"
      //
      "   b KQkq - 0 2";
  Position position = MakePosition(start);

  UndoInfo undo_info = position.Do(Move::NullMove());
  EXPECT_THAT(position, EqualsPosition("8: r n b q k b n r"
                                       "7: p p p . p p p p"
                                       "6: . . . . . . . ."
                                       "5: . . . N . . . ."
                                       "4: . . . . . . . ."
                                       "3: . . . . . . . ."
                                       "2: P P P P P P P P"
                                       "1: R . B Q K B N R"
                                       "   a b c d e f g h"
                                       //
                                       "   w KQkq - 0 2"));

  position.Undo(undo_info);
  EXPECT_THAT(position, EqualsPosition(start));
}

}  // namespace
}  // namespace follychess

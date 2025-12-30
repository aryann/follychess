#include "move.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace follychess {
namespace {

using ::testing::Eq;
using ::testing::IsEmpty;
using ::testing::IsFalse;
using ::testing::IsTrue;
using ::testing::Ne;
using ::testing::Not;

TEST(Move, NonPromotion) {
  Move move(A1, B2);

  EXPECT_THAT(move.GetFrom(), Eq(A1));
  EXPECT_THAT(move.GetTo(), Eq(B2));
  EXPECT_THAT(move.IsPromotion(), IsFalse());
}

TEST(Move, DoublePawnPush) {
  {
    Move move(D2, D4, Move::Flags::kDoublePawnPush);
    ASSERT_THAT(move.IsDoublePawnPush(), IsTrue());
    EXPECT_THAT(move.GetEnPassantTarget(), Eq(D3));
  }

  {
    Move move(A7, A5, Move::Flags::kDoublePawnPush);
    ASSERT_THAT(move.IsDoublePawnPush(), IsTrue());
    EXPECT_THAT(move.GetEnPassantTarget(), Eq(A6));
  }
}

TEST(Move, EnPassantCapture) {
  {
    Move move(F4, E3, Move::Flags::kEnPassantCapture);
    ASSERT_THAT(move.IsEnPassantCapture(), IsTrue());
    EXPECT_THAT(move.GetEnPassantVictim(), Eq(E4));
  }

  {
    Move move(B5, A6, Move::Flags::kEnPassantCapture);
    ASSERT_THAT(move.IsEnPassantCapture(), IsTrue());
    EXPECT_THAT(move.GetEnPassantVictim(), Eq(A5));
  }
}

TEST(Move, Promotion) {
  {
    Move move(A7, A8, Move::Flags::kKnightPromotion);

    EXPECT_THAT(move.GetFrom(), Eq(A7));
    EXPECT_THAT(move.GetTo(), Eq(A8));
    EXPECT_THAT(move.IsPromotion(), IsTrue());
    EXPECT_THAT(move.GetPromotedPiece(), Eq(kKnight));
  }
  {
    Move move(A7, A8, Move::Flags::kBishopPromotion);

    EXPECT_THAT(move.GetFrom(), Eq(A7));
    EXPECT_THAT(move.GetTo(), Eq(A8));
    EXPECT_THAT(move.IsPromotion(), IsTrue());
    EXPECT_THAT(move.GetPromotedPiece(), Eq(kBishop));
  }
  {
    Move move(A7, A8, Move::Flags::kRookPromotion);

    EXPECT_THAT(move.GetFrom(), Eq(A7));
    EXPECT_THAT(move.GetTo(), Eq(A8));
    EXPECT_THAT(move.IsPromotion(), IsTrue());
    EXPECT_THAT(move.GetPromotedPiece(), Eq(kRook));
  }
  {
    Move move(A7, A8, Move::Flags::kQueenPromotion);

    EXPECT_THAT(move.GetFrom(), Eq(A7));
    EXPECT_THAT(move.GetTo(), Eq(A8));
    EXPECT_THAT(move.IsPromotion(), IsTrue());
    EXPECT_THAT(move.GetPromotedPiece(), Eq(kQueen));
  }
}

TEST(Move, Equality) {
  EXPECT_THAT(Move(A1, A2), Eq(Move(A1, A2)));

  EXPECT_THAT(Move(A7, A8, Move::Flags::kKnightPromotion),
              Eq(Move(A7, A8, Move::Flags::kKnightPromotion)));

  EXPECT_THAT(Move(A7, A8, Move::Flags::kKnightPromotion),
              Not(Eq(Move(A7, A8, Move::Flags::kBishopPromotion))));

  EXPECT_THAT(Move(A1, A2), Not(Eq(Move(H1, H2))));

  EXPECT_THAT(Move(A1, A2), Not(Ne(Move(A1, A2))));
  EXPECT_THAT(Move(A1, A2), Ne(Move(H1, H2)));
}

TEST(FromUCI, Valid) {
  {
    std::expected<Move, std::string> move = Move::FromUCI("e2e4");
    ASSERT_THAT(move.error_or(""), IsEmpty());
    EXPECT_THAT(move->GetFrom(), Eq(E2));
    EXPECT_THAT(move->GetTo(), Eq(E4));
    EXPECT_THAT(move->IsCapture(), IsFalse());
    EXPECT_THAT(move->IsDoublePawnPush(), IsFalse());
    EXPECT_THAT(move->IsEnPassantCapture(), IsFalse());
    EXPECT_THAT(move->IsKingSideCastling(), IsFalse());
    EXPECT_THAT(move->IsQueenSideCastling(), IsFalse());
    EXPECT_THAT(move->IsCastling(), IsFalse());
    EXPECT_THAT(move->IsPromotion(), IsFalse());
  }
  {
    std::expected<Move, std::string> move = Move::FromUCI("e7e8n");
    ASSERT_THAT(move.error_or(""), IsEmpty());
    EXPECT_THAT(move->GetFrom(), Eq(E7));
    EXPECT_THAT(move->GetTo(), Eq(E8));
    EXPECT_THAT(move->IsCapture(), IsFalse());
    EXPECT_THAT(move->IsDoublePawnPush(), IsFalse());
    EXPECT_THAT(move->IsEnPassantCapture(), IsFalse());
    EXPECT_THAT(move->IsKingSideCastling(), IsFalse());
    EXPECT_THAT(move->IsQueenSideCastling(), IsFalse());
    EXPECT_THAT(move->IsCastling(), IsFalse());
    ASSERT_THAT(move->IsPromotion(), IsTrue());
    EXPECT_THAT(move->GetPromotedPiece(), Eq(kKnight));
  }
  {
    std::expected<Move, std::string> move = Move::FromUCI("e7e8b");
    ASSERT_THAT(move.error_or(""), IsEmpty());
    EXPECT_THAT(move->IsCapture(), IsFalse());
    EXPECT_THAT(move->IsDoublePawnPush(), IsFalse());
    EXPECT_THAT(move->IsEnPassantCapture(), IsFalse());
    EXPECT_THAT(move->IsKingSideCastling(), IsFalse());
    EXPECT_THAT(move->IsQueenSideCastling(), IsFalse());
    EXPECT_THAT(move->IsCastling(), IsFalse());
    ASSERT_THAT(move->IsPromotion(), IsTrue());
    EXPECT_THAT(move->GetPromotedPiece(), Eq(kBishop));
  }
  {
    std::expected<Move, std::string> move = Move::FromUCI("e7e8r");
    ASSERT_THAT(move.error_or(""), IsEmpty());
    EXPECT_THAT(move->IsCapture(), IsFalse());
    EXPECT_THAT(move->IsDoublePawnPush(), IsFalse());
    EXPECT_THAT(move->IsEnPassantCapture(), IsFalse());
    EXPECT_THAT(move->IsKingSideCastling(), IsFalse());
    EXPECT_THAT(move->IsQueenSideCastling(), IsFalse());
    EXPECT_THAT(move->IsCastling(), IsFalse());
    ASSERT_THAT(move->IsPromotion(), IsTrue());
    EXPECT_THAT(move->GetPromotedPiece(), Eq(kRook));
  }
  {
    std::expected<Move, std::string> move = Move::FromUCI("e7e8q");
    ASSERT_THAT(move.error_or(""), IsEmpty());
    EXPECT_THAT(move->IsCapture(), IsFalse());
    EXPECT_THAT(move->IsDoublePawnPush(), IsFalse());
    EXPECT_THAT(move->IsEnPassantCapture(), IsFalse());
    EXPECT_THAT(move->IsKingSideCastling(), IsFalse());
    EXPECT_THAT(move->IsQueenSideCastling(), IsFalse());
    EXPECT_THAT(move->IsCastling(), IsFalse());
    ASSERT_THAT(move->IsPromotion(), IsTrue());
    EXPECT_THAT(move->GetPromotedPiece(), Eq(kQueen));
  }
  {
    std::expected<Move, std::string> move = Move::FromUCI("e7f8q#c");
    ASSERT_THAT(move.error_or(""), IsEmpty());
    EXPECT_THAT(move->IsCapture(), IsTrue());
    EXPECT_THAT(move->IsDoublePawnPush(), IsFalse());
    EXPECT_THAT(move->IsEnPassantCapture(), IsFalse());
    EXPECT_THAT(move->IsKingSideCastling(), IsFalse());
    EXPECT_THAT(move->IsQueenSideCastling(), IsFalse());
    EXPECT_THAT(move->IsCastling(), IsFalse());
    ASSERT_THAT(move->IsPromotion(), IsTrue());
    EXPECT_THAT(move->GetPromotedPiece(), Eq(kQueen));
  }
  {
    std::expected<Move, std::string> move = Move::FromUCI("e2e3#c");
    ASSERT_THAT(move.error_or(""), IsEmpty());
    EXPECT_THAT(move->GetFrom(), Eq(E2));
    EXPECT_THAT(move->GetTo(), Eq(E3));
    EXPECT_THAT(move->IsCapture(), IsTrue());
    EXPECT_THAT(move->IsDoublePawnPush(), IsFalse());
    EXPECT_THAT(move->IsEnPassantCapture(), IsFalse());
    EXPECT_THAT(move->IsKingSideCastling(), IsFalse());
    EXPECT_THAT(move->IsQueenSideCastling(), IsFalse());
    EXPECT_THAT(move->IsCastling(), IsFalse());
    EXPECT_THAT(move->IsPromotion(), IsFalse());
  }
  {
    std::expected<Move, std::string> move = Move::FromUCI("d2d4#dpp");
    ASSERT_THAT(move.error_or(""), IsEmpty());
    ASSERT_THAT(move->IsDoublePawnPush(), IsTrue());
    EXPECT_THAT(move->GetEnPassantTarget(), Eq(D3));
    EXPECT_THAT(move->IsKingSideCastling(), IsFalse());
    EXPECT_THAT(move->IsQueenSideCastling(), IsFalse());
    EXPECT_THAT(move->IsCastling(), IsFalse());
    EXPECT_THAT(move->IsPromotion(), IsFalse());
  }
  {
    std::expected<Move, std::string> move = Move::FromUCI("b5a6#ep");
    ASSERT_THAT(move.error_or(""), IsEmpty());
    EXPECT_THAT(move->IsCapture(), IsTrue());
    EXPECT_THAT(move->IsDoublePawnPush(), IsFalse());
    ASSERT_THAT(move->IsEnPassantCapture(), IsTrue());
    EXPECT_THAT(move->GetEnPassantVictim(), Eq(A5));
    EXPECT_THAT(move->IsKingSideCastling(), IsFalse());
    EXPECT_THAT(move->IsQueenSideCastling(), IsFalse());
    EXPECT_THAT(move->IsCastling(), IsFalse());
    EXPECT_THAT(move->IsPromotion(), IsFalse());
  }
  {
    std::expected<Move, std::string> move = Move::FromUCI("e1c1#oo");
    ASSERT_THAT(move.error_or(""), IsEmpty());
    EXPECT_THAT(move->IsCapture(), IsFalse());
    EXPECT_THAT(move->IsDoublePawnPush(), IsFalse());
    EXPECT_THAT(move->IsEnPassantCapture(), IsFalse());
    EXPECT_THAT(move->IsKingSideCastling(), IsTrue());
    EXPECT_THAT(move->IsQueenSideCastling(), IsFalse());
    EXPECT_THAT(move->IsCastling(), IsTrue());
    EXPECT_THAT(move->IsPromotion(), IsFalse());
  }
  {
    std::expected<Move, std::string> move = Move::FromUCI("e1c1#ooo");
    ASSERT_THAT(move.error_or(""), IsEmpty());
    EXPECT_THAT(move->IsCapture(), IsFalse());
    EXPECT_THAT(move->IsDoublePawnPush(), IsFalse());
    EXPECT_THAT(move->IsEnPassantCapture(), IsFalse());
    EXPECT_THAT(move->IsKingSideCastling(), IsFalse());
    EXPECT_THAT(move->IsQueenSideCastling(), IsTrue());
    EXPECT_THAT(move->IsCastling(), IsTrue());
    EXPECT_THAT(move->IsPromotion(), IsFalse());
  }
}

TEST(FromUCI, Invalid) {
  EXPECT_THAT(Move::FromUCI("").error_or(""), Eq("Invalid UCI move: "));
  EXPECT_THAT(Move::FromUCI("e1").error_or(""), Eq("Invalid UCI move: e1"));
  EXPECT_THAT(Move::FromUCI("e1e").error_or(""), Eq("Invalid UCI move: e1e"));
  EXPECT_THAT(Move::FromUCI("e1e2invalid").error_or(""),
              Eq("Invalid UCI move: e1e2invalid"));
  EXPECT_THAT(Move::FromUCI("e7e8k").error_or(""),
              Eq("Invalid UCI move: e7e8k"));
  EXPECT_THAT(Move::FromUCI("e7e8p").error_or(""),
              Eq("Invalid UCI move: e7e8p"));
  EXPECT_THAT(Move::FromUCI("e7e8#ccc").error_or(""),
              Eq("Invalid UCI move: e7e8#ccc"));
}

TEST(Move, String) {
  auto ToString = [](const Move &move) {
    std::stringstream buf;
    buf << move;
    return buf.str();
  };

  EXPECT_THAT(ToString(Move(E7, E5)), Eq("e7e5"));
  EXPECT_THAT(ToString(Move(E2, F3, Move::Flags::kCapture)), Eq("e2f3#c"));
  EXPECT_THAT(ToString(Move(D2, D4, Move::Flags::kDoublePawnPush)),
              Eq("d2d4#dpp"));
  EXPECT_THAT(ToString(Move(B5, A6, Move::Flags::kEnPassantCapture)),
              Eq("b5a6#ep"));
  EXPECT_THAT(ToString(Move(G2, G1, Move::Flags::kKnightPromotion)),
              Eq("g2g1n"));
  EXPECT_THAT(ToString(Move(G2, G1, Move::Flags::kBishopPromotion)),
              Eq("g2g1b"));
  EXPECT_THAT(ToString(Move(G2, G1, Move::Flags::kRookPromotion)), Eq("g2g1r"));
  EXPECT_THAT(ToString(Move(G2, G1, Move::Flags::kQueenPromotion)),
              Eq("g2g1q"));

  EXPECT_THAT(ToString(Move(E1, G1, Move::Flags::kKingCastle)), Eq("e1g1#oo"));
  EXPECT_THAT(ToString(Move(E1, C1, Move::Flags::kQueenCastle)),
              Eq("e1c1#ooo"));

  EXPECT_THAT(std::format("{}", Move(E7, E5)), Eq("e7e5"));
  EXPECT_THAT(std::format("{}", Move(E2, F3, Move::Flags::kCapture)),
              Eq("e2f3"));
  EXPECT_THAT(std::format("{}", Move(D2, D4, Move::Flags::kDoublePawnPush)),
              Eq("d2d4"));
  EXPECT_THAT(std::format("{}", Move(B5, A6, Move::Flags::kEnPassantCapture)),
              Eq("b5a6"));
  EXPECT_THAT(std::format("{}", Move(G2, G1, Move::Flags::kKnightPromotion)),
              Eq("g2g1n"));
  EXPECT_THAT(std::format("{}", Move(G2, G1, Move::Flags::kBishopPromotion)),
              Eq("g2g1b"));
  EXPECT_THAT(std::format("{}", Move(G2, G1, Move::Flags::kRookPromotion)),
              Eq("g2g1r"));
  EXPECT_THAT(std::format("{}", Move(G2, G1, Move::Flags::kQueenPromotion)),
              Eq("g2g1q"));
  EXPECT_THAT(std::format("{}", Move(E1, G1, Move::Flags::kKingCastle)),
              Eq("e1g1"));
  EXPECT_THAT(std::format("{}", Move(E1, C1, Move::Flags::kQueenCastle)),
              Eq("e1c1"));

  EXPECT_THAT(std::format("{:f}", Move(E7, E5)), Eq("e7e5"));
  EXPECT_THAT(std::format("{:f}", Move(E2, F3, Move::Flags::kCapture)),
              Eq("e2f3#c"));
  EXPECT_THAT(std::format("{:f}", Move(D2, D4, Move::Flags::kDoublePawnPush)),
              Eq("d2d4#dpp"));
  EXPECT_THAT(std::format("{:f}", Move(B5, A6, Move::Flags::kEnPassantCapture)),
              Eq("b5a6#ep"));
  EXPECT_THAT(std::format("{:f}", Move(G2, G1, Move::Flags::kKnightPromotion)),
              Eq("g2g1n"));
  EXPECT_THAT(std::format("{:f}", Move(G2, G1, Move::Flags::kBishopPromotion)),
              Eq("g2g1b"));
  EXPECT_THAT(std::format("{:f}", Move(G2, G1, Move::Flags::kRookPromotion)),
              Eq("g2g1r"));
  EXPECT_THAT(std::format("{:f}", Move(G2, G1, Move::Flags::kQueenPromotion)),
              Eq("g2g1q"));

  EXPECT_THAT(std::format("{:f}", Move(E1, G1, Move::Flags::kKingCastle)),
              Eq("e1g1#oo"));
  EXPECT_THAT(std::format("{:f}", Move(E1, C1, Move::Flags::kQueenCastle)),
              Eq("e1c1#ooo"));
}

}  // namespace
}  // namespace follychess

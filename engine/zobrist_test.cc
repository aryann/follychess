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

#include "engine/zobrist.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "engine/castling.h"

namespace follychess {
namespace {

using ::testing::Eq;
using ::testing::Not;

TEST(ZobristKey, Empty) {
  EXPECT_THAT(ZobristKey().GetKey(), Eq(0ULL));
  EXPECT_THAT(ZobristKey(), Eq(ZobristKey()));
}

TEST(ZobristKey, PieceMoves) {
  ZobristKey key;
  std::uint64_t v0 = key.GetKey();
  EXPECT_THAT(v0, Eq(0ULL));

  // Simulate e2e4 in the starting position:
  key.Update(E2, kPawn, kWhite);
  key.Update(E4, kPawn, kWhite);

  std::uint64_t v1 = key.GetKey();
  EXPECT_THAT(v1, Not(Eq(v0)));

  // Undo part of e2e4:
  key.Update(E2, kPawn, kWhite);
  EXPECT_THAT(key.GetKey(), Not(Eq(v1)));

  // Fully undo e2e4:
  key.Update(E4, kPawn, kWhite);
  EXPECT_THAT(key.GetKey(), Eq(v0));

  // Make e2e4 again:
  key.Update(E2, kPawn, kWhite);
  key.Update(E4, kPawn, kWhite);
  EXPECT_THAT(key.GetKey(), Eq(v1));
}

TEST(ZobristKey, SideToMove) {
  ZobristKey key;
  key.Update(E7, kPawn, kBlack);
  key.Update(E5, kPawn, kBlack);

  std::uint64_t v0 = key.GetKey();
  key.UpdateSideToMove();
  std::uint64_t v1 = key.GetKey();
  key.UpdateSideToMove();
  std::uint64_t v2 = key.GetKey();
  key.UpdateSideToMove();
  std::uint64_t v3 = key.GetKey();
  key.UpdateSideToMove();
  std::uint64_t v4 = key.GetKey();

  EXPECT_THAT(v0, Not(Eq(v1)));
  EXPECT_THAT(v0, Eq(v2));
  EXPECT_THAT(v0, Not(Eq(v3)));
  EXPECT_THAT(v0, Eq(v4));
}

TEST(ZobristKey, EnPassant) {
  ZobristKey key;
  key.Update(E7, kPawn, kBlack);
  key.Update(E5, kPawn, kBlack);

  std::uint64_t v0 = key.GetKey();
  key.ToggleEnPassantTarget(G3);
  std::uint64_t v1 = key.GetKey();
  key.ToggleEnPassantTarget(G3);
  std::uint64_t v2 = key.GetKey();

  EXPECT_THAT(v0, Not(Eq(v1)));
  EXPECT_THAT(v0, Eq(v2));
}

TEST(ZobristKey, Castling) {
  ZobristKey key;
  key.Update(E7, kPawn, kBlack);
  key.Update(E5, kPawn, kBlack);

  std::uint64_t v0 = key.GetKey();
  key.ToggleCastlingRights(CastlingRights(kAllCastlingRights));
  std::uint64_t v1 = key.GetKey();
  key.ToggleCastlingRights(CastlingRights(kAllCastlingRights));
  std::uint64_t v2 = key.GetKey();
  key.ToggleCastlingRights(CastlingRights(kWhiteQueen));
  std::uint64_t v3 = key.GetKey();
  key.ToggleCastlingRights(CastlingRights(kWhiteQueen));
  std::uint64_t v4 = key.GetKey();

  EXPECT_THAT(v0, Not(Eq(v1)));
  EXPECT_THAT(v0, Eq(v2));
  EXPECT_THAT(v2, Not(Eq(v3)));
  EXPECT_THAT(v2, Eq(v4));
}

}  // namespace
}  // namespace follychess

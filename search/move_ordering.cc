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

#include "search/move_ordering.h"

#include <functional>
#include <vector>

#include "absl/strings/str_join.h"
#include "engine/move.h"
#include "engine/position.h"
#include "search/killer_moves.h"

namespace follychess {

namespace {

// Returns the Most Valuable Victim - Least Valuable Aggressor (MVV-LVA) score
// table. The first index is the victim, the second is the attacker.
consteval auto GenerateMvvLvaTable() {
  std::array<std::array<int, kNumPieces>, kNumPieces> table = {};

  // Victim: Pawn
  table[kPawn][kPawn] = 9;
  table[kPawn][kKnight] = 8;
  table[kPawn][kBishop] = 7;
  table[kPawn][kRook] = 6;
  table[kPawn][kQueen] = 5;
  table[kPawn][kKing] = 4;

  // Victim: Knight
  table[kKnight][kPawn] = 19;
  table[kKnight][kKnight] = 18;
  table[kKnight][kBishop] = 17;
  table[kKnight][kRook] = 16;
  table[kKnight][kQueen] = 15;
  table[kKnight][kKing] = 14;

  // Victim: Bishop
  table[kBishop][kPawn] = 29;
  table[kBishop][kKnight] = 28;
  table[kBishop][kBishop] = 27;
  table[kBishop][kRook] = 26;
  table[kBishop][kQueen] = 25;
  table[kBishop][kKing] = 24;

  // Victim: Rook
  table[kRook][kPawn] = 39;
  table[kRook][kKnight] = 38;
  table[kRook][kBishop] = 37;
  table[kRook][kRook] = 36;
  table[kRook][kQueen] = 35;
  table[kRook][kKing] = 34;

  // Victim: Queen
  table[kQueen][kPawn] = 49;
  table[kQueen][kKnight] = 48;
  table[kQueen][kBishop] = 47;
  table[kQueen][kRook] = 46;
  table[kQueen][kQueen] = 45;
  table[kQueen][kKing] = 44;

  // Victim: King
  table[kKing][kPawn] = 59;
  table[kKing][kKnight] = 58;
  table[kKing][kBishop] = 57;
  table[kKing][kRook] = 56;
  table[kKing][kQueen] = 55;
  table[kKing][kKing] = 54;

  return table;
}

constexpr auto kMvvLvaTable = GenerateMvvLvaTable();

constexpr int kPriorityMoveScale = 100'000'000;
constexpr int kCaptureScale = 10'000'000;
constexpr int kPromotionScale = 1'000'000;
constexpr int kCastlingScale = 100'000;
constexpr int kKillerMoveScale = 10'000;
constexpr int kHistoryHeuristicScale = 1'000;

}  // namespace

std::string join(const std::vector<Move>& moves) {
  std::string result;
  for (Move move : moves) {
    result.append(std::format("{}, ", move));
  }
  return result;
}

void OrderMoves(const Position& position, Move priority_move,
                const KillerMoves::Entry& killer_moves,
                const HistoryHeuristic& history_heuristic,
                std::vector<Move>& moves) {
  auto sort_key = [&](const Move move) {
    if (priority_move == move) {
      return kPriorityMoveScale;
    }

    if (move.IsCapture()) {
      const Piece attacker = position.GetPiece(move.GetFrom());
      const Piece victim =
          move.IsEnPassantCapture() ? kPawn : position.GetPiece(move.GetTo());
      DCHECK_NE(attacker, kEmptyPiece);
      DCHECK_NE(victim, kEmptyPiece);

      return kCaptureScale + kMvvLvaTable[victim][attacker];
    }

    if (move.IsPromotion()) {
      DCHECK_GT(kQueen, kRook);
      DCHECK_GT(kRook, kBishop);
      DCHECK_GT(kBishop, kKnight);

      return kPromotionScale + move.GetPromotedPiece();
    }

    if (move.IsCastling()) {
      return kCastlingScale;
    }

    if (move == killer_moves.first) {
      return kKillerMoveScale + 1;
    }
    if (move == killer_moves.second) {
      return kKillerMoveScale;
    }

    const int history_score = history_heuristic.Get(position, move);
    DCHECK_LT(history_score, 1'000);
    return kHistoryHeuristicScale + history_score;
  };

  std::ranges::sort(moves, std::greater(), sort_key);
}

}  // namespace follychess

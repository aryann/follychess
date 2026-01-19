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

#include "engine/move.h"
#include "engine/position.h"
#include "search/killer_moves.h"

namespace follychess {

void OrderMoves(const Position& position, Move priority_move,
                const KillerMoves::Entry& killer_moves,
                const HistoryHeuristic& history_heuristic,
                std::vector<Move>& moves) {
  auto sort_key = [&](Move move) {
    if (priority_move == move) {
      return 0;
    }

    if (move.IsCapture()) {
      const Piece attacker = position.GetPiece(move.GetFrom());
      const Piece victim = position.GetPiece(move.GetTo());

      const int victim_score = kKing - victim;
      const int attacker_score = attacker;

      return victim_score * static_cast<int>(kNumPieces) + attacker_score;
    }

    if (move.IsPromotion()) {
      DCHECK_GT(kQueen, kRook);
      DCHECK_GT(kRook, kBishop);
      DCHECK_GT(kBishop, kKnight);

      constexpr int kPromotionScale = 1'000;
      return kPromotionScale + static_cast<int>(kNumPieces) -
             move.GetPromotedPiece();
    }

    if (move.IsCastling()) {
      return 10'000;
    }

    if (move == killer_moves.first) {
      return 100'000;
    }
    if (move == killer_moves.second) {
      return 100'001;
    }

    return 1'000'000;
  };

  std::ranges::sort(moves, std::less(), sort_key);
}

}  // namespace follychess

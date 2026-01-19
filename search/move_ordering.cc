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
namespace {

[[nodiscard]] int MoveKey(const Position& position, Move priority_move,
                          const KillerMoves::Entry& killer_moves, Move move) {
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
    switch (move.GetPromotedPiece()) {
      case kQueen:
        return 1'000;
      case kRook:
        return 1'001;
      case kBishop:
        return 1'002;
      default:
        return 1'003;
    }
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
}

}  // namespace

void OrderMoves(const Position& position, Move priority_move,
                const KillerMoves::Entry& killer_moves,
                std::vector<Move>& moves) {
  std::ranges::sort(
      moves, std::less(),
      std::bind_front(MoveKey, position, priority_move, killer_moves));
}

}  // namespace follychess

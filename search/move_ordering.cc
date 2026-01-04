#include "search/move_ordering.h"

#include <functional>
#include <vector>

#include "engine/move.h"
#include "engine/position.h"

namespace follychess {
namespace {

[[nodiscard]] int MoveKey(const Position& position, Move priority_move,
                          Move move) {
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

  return 100'000;
}

}  // namespace

void OrderMoves(const Position& position, Move priority_move,
                std::vector<Move>& moves) {
  std::ranges::sort(moves, std::less(),
                    std::bind_front(MoveKey, position, priority_move));
}

}  // namespace follychess

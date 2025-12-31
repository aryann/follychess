#include "engine/move_generator.h"

#include <vector>

#include "absl/log/check.h"
#include "engine/attacks.h"
#include "engine/types.h"
#include "line.h"

namespace follychess {
namespace {

void AddPawnMoves(Bitboard destinations, int offset, Move::Flags flag,
                  std::vector<Move> &moves) {
  while (destinations) {
    Square to = destinations.PopLeastSignificantBit();
    const auto from = static_cast<Square>(to - offset);
    moves.emplace_back(from, to, flag);
  }
}

void AddPawnPromotions(Bitboard promotions, int offset, Move::Flags flag,
                       std::vector<Move> &moves) {
  using enum Move::Flags;

  while (promotions) {
    const Square to = promotions.PopLeastSignificantBit();
    const auto from = static_cast<Square>(to - offset);

    moves.emplace_back(from, to,
                       static_cast<Move::Flags>(kKnightPromotion | flag));
    moves.emplace_back(from, to,
                       static_cast<Move::Flags>(kBishopPromotion | flag));
    moves.emplace_back(from, to,
                       static_cast<Move::Flags>(kRookPromotion | flag));
    moves.emplace_back(from, to,
                       static_cast<Move::Flags>(kQueenPromotion | flag));
  }
}

template <Side Side, MoveType MoveType>
void GeneratePawnMoves(const Position &position, std::vector<Move> &moves) {
  static constexpr Direction forward = Side == kWhite ? kNorth : kSouth;
  static constexpr Bitboard promotion_rank =
      Side == kWhite ? rank::k8 : rank::k1;
  Bitboard pawns = position.GetPieces(position.SideToMove(), kPawn);

  if constexpr (MoveType == kQuiet || MoveType == kEvasion) {
    Bitboard empty = ~position.GetPieces();

    // Single pawn pushes:
    Bitboard single_moves = pawns.Shift<forward>() & empty;
    AddPawnMoves(single_moves & ~promotion_rank, forward, Move::Flags::kNone,
                 moves);
    AddPawnPromotions(single_moves & promotion_rank, forward,
                      Move::Flags::kNone, moves);

    // Double pawn pushes:
    Bitboard second_rank = Side == kWhite ? rank::k3 : rank::k6;
    Bitboard double_moves =
        (single_moves & second_rank).Shift<forward>() & empty;
    AddPawnMoves(double_moves, forward * 2, Move::Flags::kDoublePawnPush,
                 moves);
  }

  if constexpr (MoveType == kCapture || MoveType == kEvasion) {
    constexpr Direction left = Side == kWhite ? kNorthWest : kSouthEast;
    constexpr Direction right = Side == kWhite ? kNorthEast : kSouthWest;

    Bitboard enemies = position.GetPieces(~Side);

    Bitboard left_captures = pawns.Shift<left>() & enemies;
    Bitboard right_captures = pawns.Shift<right>() & enemies;

    AddPawnMoves(left_captures & ~promotion_rank, left, Move::Flags::kCapture,
                 moves);
    AddPawnMoves(right_captures & ~promotion_rank, right, Move::Flags::kCapture,
                 moves);

    std::optional<Square> en_passant_target = position.GetEnPassantTarget();
    if (en_passant_target) {
      auto target = Bitboard(*en_passant_target);
      AddPawnMoves(pawns.Shift<left>() & target, left,
                   Move::Flags::kEnPassantCapture, moves);
      AddPawnMoves(pawns.Shift<right>() & target, right,
                   Move::Flags::kEnPassantCapture, moves);
    }

    AddPawnPromotions(left_captures & promotion_rank, left,
                      Move::Flags::kCapture, moves);
    AddPawnPromotions(right_captures & promotion_rank, right,
                      Move::Flags::kCapture, moves);
  }
}

template <Side Side, Piece Piece>
void GenerateMoves(const Position &position, Bitboard targets,
                   std::vector<Move> &moves) {
  Bitboard pieces = position.GetPieces(Side, Piece);
  while (pieces) {
    Square from = pieces.PopLeastSignificantBit();
    Bitboard attacks =
        GenerateAttacks<Piece>(from, position.GetPieces()) & targets;

    while (attacks) {
      Square to = attacks.PopLeastSignificantBit();

      Move::Flags flags = Move::Flags::kNone;
      if (position.GetPiece(to) != kEmptyPiece) {
        flags = Move::Flags::kCapture;
      }
      moves.emplace_back(from, to, flags);
    }
  }
}

[[nodiscard]] bool IsImpeded(const Position &position, Bitboard path) {
  return static_cast<bool>(position.GetPieces() & path);
}

template <Side Side>
[[nodiscard]] bool IsAttacked(const Position &position, Bitboard path) {
  while (path) {
    Square square = path.PopLeastSignificantBit();
    if (position.GetAttackers(square, ~Side)) {
      return true;
    }
  }
  return false;
}

template <Side Side>
void GenerateCastlingMoves(const Position &position, std::vector<Move> &moves) {
  static_assert(Side == kWhite || Side == kBlack);

  if (position.GetCastlingRights().HasKingSide<Side>()) {
    Bitboard rook_path = GetKingSideCastlingPath<Side>();
    if (!IsImpeded(position, rook_path) &&
        !IsAttacked<Side>(position, rook_path)) {
      static constexpr Move kCastlingMoves[] = {
          Move(E1, G1, Move::Flags::kKingCastle),
          Move(E8, G8, Move::Flags::kKingCastle),
      };
      moves.push_back(kCastlingMoves[Side]);
    }
  }

  if (position.GetCastlingRights().HasQueenSide<Side>()) {
    Bitboard rook_path = GetQueenSideCastlingPath<Side>();

    Bitboard king_path = rook_path;
    king_path.PopLeastSignificantBit();

    if (!IsImpeded(position, rook_path) &&
        !IsAttacked<Side>(position, king_path)) {
      static constexpr Move kCastlingMoves[] = {
          Move(E1, C1, Move::Flags::kQueenCastle),
          Move(E8, C8, Move::Flags::kQueenCastle),
      };
      moves.push_back(kCastlingMoves[Side]);
    }
  }
}

template <Side Side, MoveType MoveType>
Bitboard GetTargets(const Position &position) {
  if constexpr (MoveType == kQuiet) {
    return ~position.GetPieces();
  }

  if constexpr (MoveType == kCapture) {
    return position.GetPieces(~Side);
  }

  if constexpr (MoveType == kEvasion) {
    Bitboard checkers = position.GetCheckers(Side);
    Square from = checkers.LeastSignificantBit();
    Square to = position.GetKing(Side);

    DCHECK(checkers.GetCount() == 1);

    // If the former Bitboard is zero, then the checker is a knight.
    DCHECK(GetLine(from, to) || position.GetPiece(from) == kKnight);

    return GetLine(from, to) | checkers;
  }

  return {};
}

template <Side Side, MoveType MoveType>
Bitboard GetKingTargets(const Position &position) {
  if constexpr (MoveType == kQuiet) {
    return ~position.GetPieces();
  }

  if constexpr (MoveType == kCapture) {
    return position.GetPieces(~Side);
  }

  if constexpr (MoveType == kEvasion) {
    // If the move type is an evasion, then the king should be allowed to move
    // to any square that is not occupied by its own side.
    return ~position.GetPieces(Side);
  }

  return {};
}

template <Side Side, MoveType MoveType>
void GenerateMoves(const Position &position, std::vector<Move> &moves) {
  // Generate moves for all non-king pieces. This logic is shared for two
  // main scenarios:
  //
  //   1. Normal Moves (MoveType == kQuiet || MoveType == kCapture):
  //      The `targets` bitboard is set to all empty squares (for quiet)
  //      or all enemy pieces (for captures), and we generate all moves.
  //
  //   2. Evasion Moves (position.GetCheckers().GetCount() == 1):
  //      This block only runs if there is *exactly one* checker. In a
  //      double check, only the king can move, so this is skipped.
  //
  //      When MoveType is kEvasion, `GetTargets()` returns a bitboard of
  //      all squares that can resolve the check:
  //
  //        * The square of the checking piece (for capture). This square
  //          may be occupied by a knight.
  //        * For sliding checkers (Q, R, B), the squares *between* the
  //          checker and the king (for blocking).
  //
  //    The piece generators then find all moves to these target squares.
  //
  if (MoveType == kQuiet || MoveType == kCapture ||
      position.GetCheckers(Side).GetCount() == 1) {
    Bitboard targets = GetTargets<Side, MoveType>(position);

    GeneratePawnMoves<Side, MoveType>(position, moves);
    GenerateMoves<Side, kKnight>(position, targets, moves);
    GenerateMoves<Side, kBishop>(position, targets, moves);
    GenerateMoves<Side, kRook>(position, targets, moves);
    GenerateMoves<Side, kQueen>(position, targets, moves);
  }

  GenerateMoves<Side, kKing>(position, GetKingTargets<Side, MoveType>(position),
                             moves);

  if constexpr (MoveType == kQuiet) {
    GenerateCastlingMoves<Side>(position, moves);
  }
}

}  // namespace

template <MoveType MoveType>
void GenerateMoves(const Position &position, std::vector<Move> &moves) {
  if (position.SideToMove() == kWhite) {
    GenerateMoves<kWhite, MoveType>(position, moves);
  } else {
    GenerateMoves<kBlack, MoveType>(position, moves);
  }
}

template <MoveType MoveType>
std::vector<Move> GenerateMoves(const Position &position) {
  std::vector<Move> moves;
  GenerateMoves<MoveType>(position, moves);
  return moves;
}

// Explicitly instantiate the templates for `GenerateMoves()`.
// This ensures the function is compiled and available to the linker, as the
// template's definition is in this .cc file rather than a header.
template std::vector<Move> GenerateMoves<kQuiet>(const Position &position);
template std::vector<Move> GenerateMoves<kCapture>(const Position &position);
template std::vector<Move> GenerateMoves<kEvasion>(const Position &position);

std::vector<Move> GenerateMoves(const Position &position) {
  std::vector<Move> moves;
  if (position.GetCheckers(position.SideToMove())) {
    GenerateMoves<kEvasion>(position, moves);
  } else {
    GenerateMoves<kQuiet>(position, moves);
    GenerateMoves<kCapture>(position, moves);
  }
  return moves;
}

}  // namespace follychess

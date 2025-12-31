#include "position.h"

#include "absl/log/check.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "attacks.h"
#include "bitboard.h"
#include "engine/castling.h"

namespace follychess {

namespace {

// Returns a bitboard with the rook's from and to squares for castling if and
// only if the move is a castling move. Otherwise, returns an empty bitboard.
[[nodiscard]] constexpr Bitboard GetCastlingRookMask(const Move &move,
                                                     Side side) {
  if (move.IsKingSideCastling()) {
    if (side == kWhite) {
      return Bitboard(H1) | Bitboard(F1);
    } else {
      return Bitboard(H8) | Bitboard(F8);
    }
  }

  if (move.IsQueenSideCastling()) {
    if (side == kWhite) {
      return Bitboard(A1) | Bitboard(D1);
    } else {
      return Bitboard(A8) | Bitboard(D8);
    }
  }

  return kEmptyBoard;
}

}  // namespace

Piece Position::GetPiece(Square square) const {
  if (pieces_[kPawn] & square) {
    return kPawn;
  }
  if (pieces_[kKnight] & square) {
    return kKnight;
  }
  if (pieces_[kBishop] & square) {
    return kBishop;
  }
  if (pieces_[kRook] & square) {
    return kRook;
  }
  if (pieces_[kQueen] & square) {
    return kQueen;
  }
  if (pieces_[kKing] & square) {
    return kKing;
  }

  return kEmptyPiece;
}

Side Position::GetSide(Square square) const {
  if (sides_[kWhite] & square) {
    return kWhite;
  }
  if (sides_[kBlack] & square) {
    return kBlack;
  }
  return kEmptySide;
}

Bitboard Position::GetPieces() const { return sides_[kWhite] | sides_[kBlack]; }

Bitboard Position::GetPieces(Side side) const { return sides_[side]; }

Bitboard Position::GetPieces(Piece type) const { return pieces_[type]; }

Bitboard Position::GetPieces(Side side, Piece type) const {
  return sides_[side] & pieces_[type];
}

Bitboard Position::GetAttackers(Square to, Side attacker_side) const {
  Bitboard occupied = GetPieces();
  Bitboard attackers;

  Side victim_side = ~attacker_side;

  attackers |=
      GetPawnAttacks(to, victim_side) & GetPieces(attacker_side, kPawn);
  attackers |= GenerateAttacks<kKnight>(to, occupied) &
               GetPieces(attacker_side, kKnight);
  attackers |=
      GenerateAttacks<kKing>(to, occupied) & GetPieces(attacker_side, kKing);

  // Generates attacks from rooks, bishops, and queens:
  attackers |=
      GenerateAttacks<kRook>(to, occupied) &
      (GetPieces(attacker_side, kRook) | GetPieces(attacker_side, kQueen));
  attackers |=
      GenerateAttacks<kBishop>(to, occupied) &
      (GetPieces(attacker_side, kBishop) | GetPieces(attacker_side, kQueen));

  return attackers;
}

Square Position::GetKing(Side side) const {
  Bitboard king = GetPieces(side, kKing);
  DCHECK(king.GetCount() == 1)
      << "Board must have exactly one king of each color.\n\n"
      << std::format("{}", *this);

  return king.LeastSignificantBit();
}

Bitboard Position::GetCheckers(Side king_side) const {
  Side attacker_side = ~king_side;
  return GetAttackers(GetKing(king_side), attacker_side);
}

namespace {

std::expected<void, std::string> FillPiece(
    char curr, Square square, std::array<Bitboard, kNumPieces> &pieces) {
  if (curr == 'p' || curr == 'P') {
    pieces[kPawn].Set(square);
  } else if (curr == 'n' || curr == 'N') {
    pieces[kKnight].Set(square);
  } else if (curr == 'b' || curr == 'B') {
    pieces[kBishop].Set(square);
  } else if (curr == 'r' || curr == 'R') {
    pieces[kRook].Set(square);
  } else if (curr == 'q' || curr == 'Q') {
    pieces[kQueen].Set(square);
  } else if (curr == 'k' || curr == 'K') {
    pieces[kKing].Set(square);
  } else {
    return std::unexpected(
        std::format("FEN string contained invalid square character: {}", curr));
  }

  return {};
}

std::expected<void, std::string> ParseBoard(
    std::string_view board, std::array<Bitboard, kNumPieces> &pieces,
    std::array<Bitboard, kNumSides> &sides) {
  int square_index = 0;
  for (char curr : board) {
    if (square_index > 63) {
      return std::unexpected("FEN string did not contain exactly 64 squares.");
    }

    if (curr == '/') {
      continue;
    }

    if (std::isdigit(curr)) {
      const int empty_squares = curr - '0';
      square_index += empty_squares;
      continue;
    }

    const auto square = static_cast<Square>(square_index);

    const Side side = std::isupper(curr) ? kWhite : kBlack;
    sides[side].Set(square);

    if (auto result = FillPiece(curr, square, pieces); !result.has_value()) {
      return result;
    }

    ++square_index;
  }

  return {};
}

std::expected<CastlingRights, std::string> SetCastlingRights(
    std::string_view input, CastlingRights &rights) {
  if (input == "-") {
    return {};
  }

  if (input.find_first_not_of("KQkq") != std::string::npos) {
    return std::unexpected(std::format("Invalid castling rights: {}", input));
  }

  if (input.contains("K")) {
    rights.Set(kWhiteKing);
  }
  if (input.contains("Q")) {
    rights.Set(kWhiteQueen);
  }
  if (input.contains("k")) {
    rights.Set(kBlackKing);
  }
  if (input.contains("q")) {
    rights.Set(kBlackQueen);
  }

  return {};
}

bool IsNumeric(std::string_view input) {
  return std::ranges::all_of(input, isdigit);
}

}  // namespace

Position Position::Starting() {
  constexpr absl::string_view kFen =
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

  std::expected<Position, std::string> result = FromFen(kFen);
  CHECK(result.has_value())
      << "Failed for parse starting position FEN: " << result.error();
  return result.value();
}

std::expected<Position, std::string> Position::FromFen(
    const std::vector<std::string_view> &fen_parts) {
  if (fen_parts.size() != 6) {
    return std::unexpected(std::format(
        "FEN string must have 6 parts; received: {}", fen_parts.size()));
  }

  std::string_view board = fen_parts[0];
  std::string_view side_to_move = fen_parts[1];
  std::string_view castling_rights = fen_parts[2];
  std::string_view en_passant_target = fen_parts[3];
  std::string_view half_moves = fen_parts[4];
  std::string_view full_moves = fen_parts[5];

  Position position;
  if (auto result = ParseBoard(board, position.pieces_, position.sides_);
      !result.has_value()) {
    return std::unexpected(result.error());
  }

  if (side_to_move == "w") {
    position.side_to_move_ = kWhite;
  } else if (side_to_move == "b") {
    position.side_to_move_ = kBlack;
  } else {
    return std::unexpected(
        std::format("Invalid side to move value: {}", side_to_move));
  }

  if (auto result =
          SetCastlingRights(castling_rights, position.castling_rights_);
      !result.has_value()) {
    return std::unexpected(result.error());
  }

  if (en_passant_target != "-") {
    position.en_passant_target_ = ParseSquare(en_passant_target);
    if (!position.en_passant_target_) {
      return std::unexpected(
          std::format("Invalid en-passant target: {}", en_passant_target));
    }
  }

  if (!IsNumeric(half_moves)) {
    return std::unexpected(std::format("Invalid half moves: {}", half_moves));
  }
  position.half_moves_ = std::stoi(std::string(half_moves));

  if (!IsNumeric(full_moves)) {
    return std::unexpected(std::format("Invalid full moves: {}", full_moves));
  }
  position.full_moves_ = std::stoi(std::string(full_moves));

  position.InitKey();
  return position;
}

std::expected<Position, std::string> Position::FromFen(std::string_view fen) {
  return Position::FromFen(absl::StrSplit(fen, absl::ByAsciiWhitespace()));
}

UndoInfo Position::Do(const Move &move) {
  // TODO(aryann): Reset the half move clock if there was a pawn move.
  Piece victim = GetPiece(move.GetTo());
  const UndoInfo undo_info = {
      .move = move,
      .en_passant_target = en_passant_target_,
      .captured_piece = victim,
      .half_moves = half_moves_,
      .castling_rights = castling_rights_,
  };

  if (victim == kEmptyPiece) {
    ++half_moves_;
  } else {
    DCHECK(GetSide(move.GetTo()) == ~side_to_move_);

    pieces_[victim].Clear(move.GetTo());
    sides_[~side_to_move_].Clear(move.GetTo());
    half_moves_ = 0;
    zobrist_key_.Update(move.GetTo(), victim, ~side_to_move_);
  }

  Piece piece = GetPiece(move.GetFrom());
  DCHECK(piece != kEmptyPiece);
  if (piece == kPawn) {
    half_moves_ = 0;
  }

  zobrist_key_.Update(move.GetFrom(), piece, side_to_move_);
  zobrist_key_.Update(move.GetTo(), piece, side_to_move_);

  if (move.IsEnPassantCapture()) {
    Square en_passant_victim = move.GetEnPassantVictim();
    pieces_[kPawn].Clear(en_passant_victim);
    sides_[~side_to_move_].Clear(en_passant_victim);
    zobrist_key_.Update(en_passant_victim, kPawn, ~side_to_move_);
    half_moves_ = 0;
  }

  Bitboard from_to = Bitboard(move.GetFrom()) | Bitboard(move.GetTo());
  pieces_[piece] ^= from_to;

  Side side = GetSide(move.GetFrom());
  DCHECK(side != kEmptySide);
  sides_[side] ^= from_to;

  if (move.IsPromotion()) {
    pieces_[kPawn].Clear(move.GetTo());
    pieces_[move.GetPromotedPiece()].Set(move.GetTo());
    zobrist_key_.Update(move.GetTo(), kPawn, side_to_move_);
    zobrist_key_.Update(move.GetTo(), move.GetPromotedPiece(), side_to_move_);
  }

  // Non-empty if and only if the move is a castling move.
  //
  // TODO(aryann): The Do() and Undo() castling logic can be shared.
  Bitboard rook_mask = GetCastlingRookMask(move, side);
  DCHECK(!rook_mask || move.IsCastling());
  pieces_[kRook] ^= rook_mask;
  sides_[side] ^= rook_mask;
  while (rook_mask) {
    Square square = rook_mask.PopLeastSignificantBit();
    zobrist_key_.Update(square, kRook, side_to_move_);
  }

  zobrist_key_.ToggleCastlingRights(castling_rights_);
  castling_rights_.InvalidateOnMove(move.GetFrom());
  castling_rights_.InvalidateOnMove(move.GetTo());
  zobrist_key_.ToggleCastlingRights(castling_rights_);

  if (side_to_move_ == kBlack) {
    ++full_moves_;
  }
  side_to_move_ = ~side_to_move_;

  zobrist_key_.ToggleEnPassantTarget(en_passant_target_);
  if (move.IsDoublePawnPush()) {
    en_passant_target_ = move.GetEnPassantTarget();
    zobrist_key_.ToggleEnPassantTarget(move.GetEnPassantTarget());
  } else {
    en_passant_target_ = std::nullopt;
  }

  zobrist_key_.UpdateSideToMove();
  return undo_info;
}

void Position::Undo(const UndoInfo &undo_info) {
  const Move &move = undo_info.move;

  zobrist_key_.ToggleEnPassantTarget(en_passant_target_);
  en_passant_target_ = undo_info.en_passant_target;
  zobrist_key_.ToggleEnPassantTarget(en_passant_target_);

  zobrist_key_.ToggleCastlingRights(castling_rights_);
  castling_rights_ = undo_info.castling_rights;
  zobrist_key_.ToggleCastlingRights(castling_rights_);

  side_to_move_ = ~side_to_move_;

  if (move.IsPromotion()) {
    pieces_[move.GetPromotedPiece()].Clear(move.GetTo());
    pieces_[kPawn].Set(move.GetTo());
    zobrist_key_.Update(move.GetTo(), kPawn, side_to_move_);
    zobrist_key_.Update(move.GetTo(), move.GetPromotedPiece(), side_to_move_);
  }

  Bitboard from_to = Bitboard(move.GetFrom()) | Bitboard(move.GetTo());

  Piece piece = GetPiece(move.GetTo());
  DCHECK(piece != kEmptyPiece);
  zobrist_key_.Update(move.GetFrom(), piece, side_to_move_);
  zobrist_key_.Update(move.GetTo(), piece, side_to_move_);

  pieces_[piece] ^= from_to;

  Side side = GetSide(move.GetTo());
  DCHECK(side != kEmptySide);
  sides_[side] ^= from_to;

  if (move.IsEnPassantCapture()) {
    Square en_passant_victim = move.GetEnPassantVictim();
    pieces_[kPawn].Set(move.GetEnPassantVictim());
    sides_[~side].Set(move.GetEnPassantVictim());
    zobrist_key_.Update(en_passant_victim, kPawn, ~side_to_move_);
  }

  if (undo_info.captured_piece != kEmptyPiece) {
    // Restores a non-passant captured piece.
    pieces_[undo_info.captured_piece].Set(move.GetTo());
    sides_[~side].Set(move.GetTo());
    zobrist_key_.Update(move.GetTo(), undo_info.captured_piece, ~side);
  }

  // Non-empty if and only if the move is a castling move.
  Bitboard rook_mask = GetCastlingRookMask(move, side);
  DCHECK(!rook_mask || move.IsCastling());
  pieces_[kRook] ^= rook_mask;
  sides_[side] ^= rook_mask;
  while (rook_mask) {
    Square square = rook_mask.PopLeastSignificantBit();
    zobrist_key_.Update(square, kRook, side_to_move_);
  }

  if (side == kBlack) {
    --full_moves_;
  }
  half_moves_ = undo_info.half_moves;
  zobrist_key_.UpdateSideToMove();
}

void Position::InitKey() {
  for (int i = 0; i < kNumSquares; ++i) {
    const auto square = static_cast<Square>(i);
    const Piece piece = GetPiece(square);
    if (piece == kEmptyPiece) {
      continue;
    }

    zobrist_key_.Update(square, piece, GetSide(square));
  }

  if (side_to_move_ == kBlack) {
    zobrist_key_.UpdateSideToMove();
  }

  zobrist_key_.ToggleEnPassantTarget(en_passant_target_);
  zobrist_key_.ToggleCastlingRights(castling_rights_);
}

}  // namespace follychess

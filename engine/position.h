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

#ifndef FOLLYCHESS_POSITION_H_
#define FOLLYCHESS_POSITION_H_

#include <expected>
#include <format>
#include <string_view>

#include "engine/bitboard.h"
#include "engine/castling.h"
#include "engine/move.h"
#include "engine/types.h"
#include "engine/zobrist.h"

namespace follychess {

class Position {
 public:
  static Position Starting();

  // Parses a Forsyth–Edwards Notation (FEN) string to create a `Position`.
  //
  // Returns an error string with a descriptive message if and only if the FEN
  // value is invalid.
  static std::expected<Position, std::string> FromFen(std::string_view fen);

  static std::expected<Position, std::string> FromFen(
      const std::vector<std::string_view> &fen_parts);

  // Returns the piece at the given square.
  [[nodiscard]] Piece GetPiece(Square square) const;

  // Returns the side at the given square.
  [[nodiscard]] Side GetSide(Square square) const;

  [[nodiscard]] Bitboard GetPieces() const;

  [[nodiscard]] Bitboard GetPieces(Side side) const;

  [[nodiscard]] Bitboard GetPieces(Piece type) const;

  [[nodiscard]] Bitboard GetPieces(Side side, Piece type) const;

  [[nodiscard]] Side SideToMove() const {
    DCHECK(side_to_move_ == kWhite || side_to_move_ == kBlack);
    return side_to_move_;
  }

  // Returns all pieces that attack the given square.
  [[nodiscard]] Bitboard GetAttackers(Square to, Side by) const;

  // Returns the king for the side to move.
  [[nodiscard]] Square GetKing(Side side) const;

  // Returns the king checkers for the side to move.
  [[nodiscard]] Bitboard GetCheckers(Side of) const;

  [[nodiscard]] const CastlingRights &GetCastlingRights() const {
    return castling_rights_;
  }

  [[nodiscard]] std::optional<Square> GetEnPassantTarget() const {
    return en_passant_target_;
  }

  [[nodiscard]] int GetHalfMoves() const { return half_moves_; }

  [[nodiscard]] int GetFullMoves() const { return full_moves_; }

  bool operator==(const Position &other) const = default;

  UndoInfo Do(const Move &move);

  void Undo(const UndoInfo &undo_info);

  [[nodiscard]] std::uint64_t GetKey() const { return zobrist_key_.GetKey(); }

 private:
  Position()
      : side_to_move_(kWhite),
        en_passant_target_(std::nullopt),
        half_moves_(0),
        full_moves_(1) {}

  void InitKey();

  std::array<Bitboard, kNumPieces> pieces_;
  std::array<Bitboard, kNumSides> sides_;

  Side side_to_move_;
  CastlingRights castling_rights_;

  std::optional<Square> en_passant_target_;

  std::uint8_t half_moves_;
  int full_moves_;

  ZobristKey zobrist_key_;
};

}  // namespace follychess

template <>
struct std::formatter<follychess::Position> : std::formatter<std::string> {
  template <class ParseContext>
  constexpr auto parse(ParseContext &context) {
    auto it = context.begin();
    if (it == context.end()) {
      return it;
    }

    if (*it == 'k') {
      show_key = true;
      return ++it;
    }

    if (it != context.end() && *it != '}') {
      throw std::format_error(
          "Invalid format specifier for follychess::Position: Expected {} or "
          "{:k}.");
    }
    return it;
  }

  auto format(const follychess::Position &position,
              std::format_context &context) const {
    auto GetSquare = [&](follychess::Square square) {
      static char kPieceChars[] = {'P', 'N', 'B', 'R', 'Q', 'K', '.', '.'};
      char result = kPieceChars[static_cast<int>(position.GetPiece(square))];

      if (position.GetSide(square) == follychess::kBlack) {
        result = std::tolower(result);
      }
      return result;
    };

    auto out = context.out();

    for (int row = 0; row < 8; ++row) {
      out = std::format_to(out, "{}:", 8 - row);
      for (int col = 0; col < 8; ++col) {
        auto square = static_cast<follychess::Square>(row * 8 + col);
        out = std::format_to(out, " {}", GetSquare(square));
      }

      out = std::format_to(out, "\n");
    }

    out = std::format_to(out, "  ");
    for (int col = 0; col < 8; ++col) {
      out = std::format_to(out, " {:c}", 'a' + col);
    }
    out = std::format_to(out, "\n\n");

    char side = position.SideToMove() == follychess::kWhite ? 'w' : 'b';
    std::string en_passant_target = "-";
    if (position.GetEnPassantTarget()) {
      en_passant_target = ToString(position.GetEnPassantTarget().value());
    }
    out = std::format_to(out, "   {} {} {} {} {}\n", side,
                         position.GetCastlingRights(), en_passant_target,
                         position.GetHalfMoves(), position.GetFullMoves());

    if (show_key) {
      out = std::format_to(out, "   {:x}", position.GetKey());
    }

    return out;
  }

 private:
  bool show_key = false;
};

#endif  // FOLLYCHESS_POSITION_H_

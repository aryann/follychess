#ifndef FOLLYCHESS_MOVE_H_
#define FOLLYCHESS_MOVE_H_

#include <expected>
#include <format>
#include <string>

#include "absl/log/check.h"
#include "engine/castling.h"
#include "engine/types.h"

namespace follychess {

class Move {
 public:
  // This scheme is forward-compatible with the scheme documented at
  // https://www.chessprogramming.org/Encoding_Moves.
  enum Flags : std::uint8_t {
    kNone = /*                  */ 0b0000,
    kDoublePawnPush = /*        */ 0b0001,
    kKingCastle = /*            */ 0b0010,
    kQueenCastle = /*           */ 0b0011,
    kCapture = /*               */ 0b0100,
    kEnPassantCapture = /*      */ 0b0101,
    kKnightPromotion = /*       */ 0b1000,
    kBishopPromotion = /*       */ 0b1001,
    kRookPromotion = /*         */ 0b1010,
    kQueenPromotion = /*        */ 0b1011,
    kKnightPromotionCapture = /**/ 0b1100,
    kBishopPromotionCapture = /**/ 0b1101,
    kRookPromotionCapture = /*  */ 0b1110,
    kQueenPromotionCapture = /* */ 0b1111,
  };

  explicit constexpr Move(Square from, Square to, Flags flags = kNone)
      : data_(from + (to << 6) + (flags << 12)) {}

  static std::expected<Move, std::string> FromUCI(std::string_view input);

  constexpr Move() : data_(0) {}

  constexpr static Move NullMove() { return Move(); }

  [[nodiscard]] constexpr Square GetFrom() const {
    DCHECK(!IsNullMove());
    return static_cast<Square>(data_ & 0b111111);
  }

  [[nodiscard]] constexpr Square GetTo() const {
    DCHECK(!IsNullMove());
    return static_cast<Square>((data_ >> 6) & 0b111111);
  }

  [[nodiscard]] constexpr bool IsNullMove() const { return data_ == 0; }

  [[nodiscard]] constexpr bool IsCapture() const {
    return GetFlags() & kCapture;
  }

  [[nodiscard]] constexpr bool IsDoublePawnPush() const {
    return GetFlags() == kDoublePawnPush;
  }

  [[nodiscard]] constexpr Square GetEnPassantTarget() const {
    DCHECK(IsDoublePawnPush());

    int diff = GetFrom() < GetTo() ? 8 : -8;
    return static_cast<Square>(GetFrom() + diff);
  }

  [[nodiscard]] constexpr bool IsEnPassantCapture() const {
    return GetFlags() == kEnPassantCapture;
  }

  [[nodiscard]] constexpr Square GetEnPassantVictim() const {
    DCHECK(IsEnPassantCapture());
    return MakeSquare(GetRank(GetFrom()), GetFile(GetTo()));
  }

  [[nodiscard]] constexpr bool IsKingSideCastling() const {
    return GetFlags() == kKingCastle;
  }

  [[nodiscard]] constexpr bool IsQueenSideCastling() const {
    return GetFlags() == kQueenCastle;
  }

  [[nodiscard]] constexpr bool IsCastling() const {
    return IsKingSideCastling() || IsQueenSideCastling();
  }

  [[nodiscard]] constexpr bool IsPromotion() const {
    return GetFlags() & 0b1000;
  }

  [[nodiscard]] constexpr Piece GetPromotedPiece() const {
    DCHECK(IsPromotion());
    return static_cast<Piece>((GetFlags() & 0b11) + kKnight);
  }

  constexpr auto operator<=>(const Move &other) const = default;

  template <typename Out>
  Out FormatTo(Out out, bool full) const {
    if (IsNullMove()) {
      out = std::format_to(out, "0000");
      return out;
    }

    out = std::format_to(out, "{}{}", GetFrom(), GetTo());

    if (IsPromotion()) {
      static char kPieceChars[] = {'n', 'b', 'r', 'q'};
      out =
          std::format_to(out, "{}", kPieceChars[GetPromotedPiece() - kKnight]);
    }

    if (!full) {
      return out;
    }

    if (IsCapture()) {
      if (IsEnPassantCapture()) {
        out = std::format_to(out, "#ep");
      } else {
        out = std::format_to(out, "#c");
      }
    }
    if (IsDoublePawnPush()) {
      out = std::format_to(out, "#dpp");
    }
    if (IsKingSideCastling()) {
      out = std::format_to(out, "#oo");
    }
    if (IsQueenSideCastling()) {
      out = std::format_to(out, "#ooo");
    }

    return out;
  }

 private:
  [[nodiscard]] constexpr std::uint8_t GetFlags() const { return data_ >> 12; }

  // Stores the move state:
  //
  //   * Bits [0,   6): The `from` Square.
  //   * Bits [6,  12): The `to` Square.
  //   * Bits [12, 16): Flags.
  //
  // TODO(aryann): Find a more efficient encoding.
  std::uint16_t data_;
};

std::ostream &operator<<(std::ostream &os, const Move &move);

struct UndoInfo {
  Move move;
  std::optional<Square> en_passant_target;
  Piece captured_piece;
  std::uint8_t half_moves;
  CastlingRights castling_rights;
};

static_assert(sizeof(UndoInfo) == 8,
              "UndoInfo size is not 8 bytes! "
              "Check field ordering for padding or new members.");

static_assert(alignof(UndoInfo) == 2,
              "UndoInfo alignment is not 2 bytes! "
              "Check for new members with larger alignment.");

}  // namespace follychess

template <>
struct std::formatter<follychess::Move> : std::formatter<std::string> {
 public:
  template <class ParseContext>
  constexpr auto parse(ParseContext &context) {
    auto it = context.begin();
    if (it == context.end()) {
      return it;
    }

    if (*it == 'f') {
      full = true;
      return ++it;
    }

    if (it != context.end() && *it != '}') {
      throw std::format_error(
          "Invalid format specifier for follychess::Move: Expected {} or "
          "{:f}.");
    }
    return it;
  }

  auto format(const follychess::Move &move,
              std::format_context &context) const {
    return move.FormatTo(context.out(), full);
  }

 private:
  bool full = false;
};

#endif  // FOLLYCHESS_MOVE_H_

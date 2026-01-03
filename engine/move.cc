#include "move.h"

#include <ostream>

#include "absl/strings/str_split.h"

namespace follychess {
namespace {

std::optional<Move::Flags> ParsePromotionPiece(char c) {
  switch (c) {
    case 'n':
      return Move::Flags::kKnightPromotion;
    case 'b':
      return Move::Flags::kBishopPromotion;
    case 'r':
      return Move::Flags::kRookPromotion;
    case 'q':
      return Move::Flags::kQueenPromotion;
    default:
      return std::nullopt;
  }
}

std::optional<Move::Flags> ParseFlags(std::string_view input) {
  if (input == "c") {
    return Move::Flags::kCapture;
  }
  if (input == "dpp") {
    return Move::Flags::kDoublePawnPush;
  }
  if (input == "ep") {
    return Move::Flags::kEnPassantCapture;
  }
  if (input == "oo") {
    return Move::Flags::kKingCastle;
  }
  if (input == "ooo") {
    return Move::Flags::kQueenCastle;
  }
  return std::nullopt;
}

}  // namespace

std::expected<Move, std::string> Move::FromUCI(std::string_view input) {
  auto error = std::unexpected(std::format("Invalid UCI move: {}", input));
  if (input.empty()) {
    return error;
  }

  if (input == "0000") {
    return NullMove();
  }

  auto parts = std::views::split(input, '#') |
               std::ranges::to<std::vector<std::string>>();

  if (parts.size() > 2) {
    return error;
  }
  std::string_view head = parts[0];

  if (head.size() < 4) {
    return error;
  }
  if (head.size() > 5) {
    return error;
  }

  std::optional<Square> from = ParseSquare(head.substr(0, 2));
  std::optional<Square> to = ParseSquare(head.substr(2, 2));
  if (!from || !to) {
    return error;
  }

  Flags flags = kNone;
  if (parts.size() == 2) {
    std::optional<Flags> parsed = ParseFlags(parts.back());
    if (!parsed) {
      return error;
    }
    flags = *parsed;
  }

  if (head.size() == 5) {
    std::optional<Flags> promotion_flag = ParsePromotionPiece(head[4]);
    if (!promotion_flag) {
      return error;
    }
    flags = static_cast<Flags>(flags | *promotion_flag);
  }

  return Move(*from, *to, flags);
}

std::ostream &operator<<(std::ostream &os, const Move &move) {
  static bool kFull = true;
  move.FormatTo(std::ostream_iterator<char>(os), kFull);
  return os;
}

}  // namespace follychess

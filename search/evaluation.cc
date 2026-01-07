#include "search/evaluation.h"

#include "engine/move_generator.h"
#include "engine/position.h"
#include "search/phase.h"

namespace follychess {
namespace {

consteval auto MakePlacementScores(std::array<std::int8_t, kNumSquares> all) {
  std::array<Score, kNumSquares> result = {};
  for (int i = 0; i < kNumSquares; ++i) {
    result[i] = {.middle = all[i], .end = all[i]};
  }
  return result;
}

consteval auto MakePlacementScores(std::array<std::int8_t, kNumSquares> middle,
                                   std::array<std::int8_t, kNumSquares> end) {
  std::array<Score, kNumSquares> result = {};
  for (int i = 0; i < kNumSquares; ++i) {
    result[i] = {.middle = middle[i], .end = end[i]};
  }
  return result;
}

// Piece placement value source:
// https://www.chessprogramming.org/Simplified_Evaluation_Function.
//
// All placement values are from white's perspective.
consteval auto MakePlacementScores() {
  std::array<std::array<Score, kNumSquares>, kNumPieces> scores = {};

  scores[kPawn] = MakePlacementScores({
      0,  0,  0,   0,   0,   0,   0,  0,   //
      50, 50, 50,  50,  50,  50,  50, 50,  //
      10, 10, 20,  30,  30,  20,  10, 10,  //
      5,  5,  10,  25,  25,  10,  5,  5,   //
      0,  0,  0,   20,  20,  0,   0,  0,   //
      5,  -5, -10, 0,   0,   -10, -5, 5,   //
      5,  10, 10,  -20, -20, 10,  10, 5,   //
      0,  0,  0,   0,   0,   0,   0,  0    //
  });

  scores[kKnight] = MakePlacementScores({
      -50, -40, -30, -30, -30, -30, -40, -50,  //
      -40, -20, 0,   0,   0,   0,   -20, -40,  //
      -30, 0,   10,  15,  15,  10,  0,   -30,  //
      -30, 5,   15,  20,  20,  15,  5,   -30,  //
      -30, 0,   15,  20,  20,  15,  0,   -30,  //
      -30, 5,   10,  15,  15,  10,  5,   -30,  //
      -40, -20, 0,   5,   5,   0,   -20, -40,  //
      -50, -40, -30, -30, -30, -30, -40, -50,  //
  });

  scores[kBishop] = MakePlacementScores({
      -20, -10, -10, -10, -10, -10, -10, -20,  //
      -10, 0,   0,   0,   0,   0,   0,   -10,  //
      -10, 0,   5,   10,  10,  5,   0,   -10,  //
      -10, 5,   5,   10,  10,  5,   5,   -10,  //
      -10, 0,   10,  10,  10,  10,  0,   -10,  //
      -10, 10,  10,  10,  10,  10,  10,  -10,  //
      -10, 5,   0,   0,   0,   0,   5,   -10,  //
      -20, -10, -10, -10, -10, -10, -10, -20,  //
  });

  scores[kRook] = MakePlacementScores({
      0,  0,  0,  0,  0,  0,  0,  0,   //
      5,  10, 10, 10, 10, 10, 10, 5,   //
      -5, 0,  0,  0,  0,  0,  0,  -5,  //
      -5, 0,  0,  0,  0,  0,  0,  -5,  //
      -5, 0,  0,  0,  0,  0,  0,  -5,  //
      -5, 0,  0,  0,  0,  0,  0,  -5,  //
      -5, 0,  0,  0,  0,  0,  0,  -5,  //
      0,  0,  0,  5,  5,  0,  0,  0,   //
  });

  scores[kQueen] = MakePlacementScores({
      -20, -10, -10, -5, -5, -10, -10, -20,  //
      -10, 0,   0,   0,  0,  0,   0,   -10,  //
      -10, 0,   5,   5,  5,  5,   0,   -10,  //
      -5,  0,   5,   5,  5,  5,   0,   -5,   //
      0,   0,   5,   5,  5,  5,   0,   -5,   //
      -10, 5,   5,   5,  5,  5,   0,   -10,  //
      -10, 0,   5,   0,  0,  0,   0,   -10,  //
      -20, -10, -10, -5, -5, -10, -10, -20,  //
  });

  scores[kKing] = MakePlacementScores(
      // Middle game:
      {
          -30, -40, -40, -50, -50, -40, -40, -30,  //
          -30, -40, -40, -50, -50, -40, -40, -30,  //
          -30, -40, -40, -50, -50, -40, -40, -30,  //
          -30, -40, -40, -50, -50, -40, -40, -30,  //
          -20, -30, -30, -40, -40, -30, -30, -20,  //
          -10, -20, -20, -20, -20, -20, -20, -10,  //
          20,  20,  0,   0,   0,   0,   20,  20,   //
          20,  30,  10,  0,   0,   10,  30,  20,   //
      },
      // End game:
      {
          -50, -40, -30, -20, -20, -30, -40, -50,  //
          -30, -20, -10, 0,   0,   -10, -20, -30,  //
          -30, -10, 20,  30,  30,  20,  -10, -30,  //
          -30, -10, 30,  40,  40,  30,  -10, -30,  //
          -30, -10, 30,  40,  40,  30,  -10, -30,  //
          -30, -10, 20,  30,  30,  20,  -10, -30,  //
          -30, -30, 0,   0,   0,   0,   -30, -30,  //
          -50, -30, -30, -30, -30, -30, -30, -50,  //
      });
  ;

  return scores;
}

constexpr auto kPlacementScores = MakePlacementScores();

template <Side Side, Piece Piece>
[[nodiscard]] constexpr Score GetPlacementScore(const Position& position) {
  Bitboard pieces = position.GetPieces(Side, Piece);

  Score score;
  while (pieces) {
    Square square = pieces.PopLeastSignificantBit();
    if constexpr (Side == kBlack) {
      square = Reflect(square);
    }

    score.middle += kPlacementScores[Piece][square].middle;
    score.end += kPlacementScores[Piece][square].end;
  }
  return score;
}

}  // namespace

template <Side Side>
[[nodiscard]] Score GetPlacementScore(const Position& position) {
  return GetPlacementScore<Side, kPawn>(position) +
         GetPlacementScore<Side, kKnight>(position) +
         GetPlacementScore<Side, kBishop>(position) +
         GetPlacementScore<Side, kRook>(position) +
         GetPlacementScore<Side, kQueen>(position) +
         GetPlacementScore<Side, kKing>(position);
}

template Score GetPlacementScore<kWhite>(const Position& position);
template Score GetPlacementScore<kBlack>(const Position& position);

template <Side Side>
[[nodiscard]] int GetMaterialScore(const Position& position) {
  return 20'000 * position.GetPieces(Side, kKing).GetCount() +
         900 * position.GetPieces(Side, kQueen).GetCount() +
         500 * position.GetPieces(Side, kRook).GetCount() +
         300 * position.GetPieces(Side, kBishop).GetCount() +
         300 * position.GetPieces(Side, kKnight).GetCount() +
         100 * position.GetPieces(Side, kPawn).GetCount();
}

template int GetMaterialScore<kWhite>(const Position& position);
template int GetMaterialScore<kBlack>(const Position& position);

template <Side Side>
[[nodiscard]] int CountDoubledPawns(const Position& position) {
  const Bitboard pawns = position.GetPieces(Side, kPawn);
  int count = 0;
  for (Bitboard file : file::kFileMasks) {
    const int current = (pawns & file).GetCount();

    // If a file has N pawns, we count N - 1 doubled pawns.
    if (current > 1) {
      count += current - 1;
    }
  }

  return count;
}

template int CountDoubledPawns<kWhite>(const Position& position);
template int CountDoubledPawns<kBlack>(const Position& position);

template <Side Side>
[[nodiscard]] int CountBlockedPawns(const Position& position) {
  const Bitboard pawns = position.GetPieces(Side, kPawn);
  constexpr Direction forward = Side == kWhite ? kNorth : kSouth;
  return (pawns.Shift<forward>() & position.GetPieces()).GetCount();
}

template int CountBlockedPawns<kWhite>(const Position& position);
template int CountBlockedPawns<kBlack>(const Position& position);

template <Side Side>
[[nodiscard]] int GetPassedPawnScore(const Position& position) {
  int score = 0;
  Bitboard pawns = position.GetPieces(Side, kPawn);

  while (pawns) {
    const Square square = pawns.PopLeastSignificantBit();
    const Bitboard blockers =
        kPassedPawnMasks[Side][square] & position.GetPieces(~Side, kPawn);
    const int rank = GetRank(square);

    if (!blockers) {
      if constexpr (Side == kWhite) {
        static constexpr std::array kWhiteScores = {
            0, 150, 100, 75, 50, 30, 0, 0,
        };
        score += kWhiteScores[rank];

      } else {
        static constexpr std::array kBlackScores = {
            0, 0, 30, 50, 75, 100, 150, 0,
        };
        score += kBlackScores[rank];
      }
    }
  }

  return score;
}

template int GetPassedPawnScore<kWhite>(const Position& position);
template int GetPassedPawnScore<kBlack>(const Position& position);

namespace {

template <Side Side>
[[nodiscard]] int CountOpenFileRooks(const Position& position,
                                     Bitboard blockers) {
  int count = 0;
  Bitboard rooks = position.GetPieces(Side, kRook);
  while (rooks) {
    const Square square = rooks.PopLeastSignificantBit();
    const Bitboard file = file::kFileMasks[GetFile(square)];
    if (!(file & blockers)) {
      ++count;
    }
  }
  return count;
}

}  // namespace

template <Side Side>
[[nodiscard]] int CountSemiOpenFileRooks(const Position& position) {
  Bitboard blockers = position.GetPieces(Side, kPawn);
  return CountOpenFileRooks<Side>(position, blockers);
}

template int CountSemiOpenFileRooks<kWhite>(const Position& position);
template int CountSemiOpenFileRooks<kBlack>(const Position& position);

template <Side Side>
[[nodiscard]] int CountOpenFileRooks(const Position& position) {
  Bitboard blockers = position.GetPieces(kPawn);
  return CountOpenFileRooks<Side>(position, blockers);
}

template int CountOpenFileRooks<kWhite>(const Position& position);
template int CountOpenFileRooks<kBlack>(const Position& position);

template <Side Side>
[[nodiscard]] Score GetKingSafetyScore(const Position& position) {
  constexpr Bitboard kCenter = Side == kWhite ? Bitboard(E1) | Bitboard(D1)
                                              : Bitboard(E8) | Bitboard(D8);

  const Bitboard king = position.GetPieces(Side, kKing);
  if (king & kCenter) {
    const bool can_castle = position.GetCastlingRights().HasKingSide<Side>() ||
                            position.GetCastlingRights().HasQueenSide<Side>();
    return can_castle ? Score{.middle = -25, .end = 0}
                      : Score{.middle = -60, .end = 0};
  }

  const Bitboard pawns = position.GetPieces(Side, kPawn);

  constexpr Bitboard kKingSideZone = Side == kWhite
                                         ? Bitboard(G1) | Bitboard(H1)
                                         : Bitboard(G8) | Bitboard(H8);
  if (king & kKingSideZone) {
    constexpr Square kGPawn = Side == kWhite ? G2 : G7;
    constexpr Square kHPawn = Side == kWhite ? H2 : H7;

    if (pawns & kGPawn && pawns & kHPawn) {
      return {.middle = 40, .end = 0};
    }
    if (pawns & kGPawn) {
      return {.middle = 20, .end = 0};
    }
    return {.middle = -20, .end = 0};
  }

  constexpr Bitboard kQueenSideCastleZone = Side == kWhite
                                                ? Bitboard(B1) | Bitboard(C1)
                                                : Bitboard(C8) | Bitboard(B8);
  if (king & kQueenSideCastleZone) {
    constexpr Square kBPawn = Side == kWhite ? B2 : B7;
    constexpr Square kCPawn = Side == kWhite ? C2 : C7;

    if (pawns & kBPawn && pawns & kCPawn) {
      return {.middle = 40, .end = 0};
    }
    if (pawns & kCPawn) {
      return {.middle = 20, .end = 0};
    }
    return {.middle = -20, .end = 0};
  }

  // The king is not castled.
  DCHECK(!position.GetCastlingRights().HasKingSide<Side>());
  DCHECK(!position.GetCastlingRights().HasQueenSide<Side>());
  return {.middle = -60, .end = 0};
}

template Score GetKingSafetyScore<kWhite>(const Position& position);
template Score GetKingSafetyScore<kBlack>(const Position& position);

namespace {

[[nodiscard]] int Interpolate(Score score, int phase) {
  const int middle = score.middle * (kEndPhaseValue - phase);
  const int end = score.end * phase;
  return (middle + end) / kEndPhaseValue;
}

template <Side Side>
[[nodiscard]] int Evaluate(const Position& position, int phase) {
  const Score tapered_score =              //
      GetPlacementScore<Side>(position) +  //
      GetKingSafetyScore<Side>(position);

  return Interpolate(tapered_score, phase) +            //
         GetMaterialScore<Side>(position) +             //
         -50 * CountDoubledPawns<Side>(position) +      //
         -50 * CountBlockedPawns<Side>(position) +      //
         10 * CountSemiOpenFileRooks<Side>(position) +  //
         15 * CountOpenFileRooks<Side>(position) +      //
         GetPassedPawnScore<Side>(position);
}

}  // namespace

[[nodiscard]] int Evaluate(const Position& position, int phase) {
  return Evaluate<kWhite>(position, phase) - Evaluate<kBlack>(position, phase);
}

}  // namespace follychess

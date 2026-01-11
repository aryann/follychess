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

#include "search/evaluation.h"

#include "engine/attacks.h"
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
[[nodiscard]] Score GetPassedPawnScore(const Position& position) {
  Score score = {.middle = 0, .end = 0};
  Bitboard pawns = position.GetPieces(Side, kPawn);

  constexpr std::array kMiddleGameBonuses = {0, 0, 10, 30, 50, 75, 100, 150};
  constexpr std::array kEndGameBonuses = {0, 0, 20, 40, 80, 120, 160, 250};

  while (pawns) {
    const Square square = pawns.PopLeastSignificantBit();
    const Bitboard blockers =
        kPassedPawnMasks[Side][square] & position.GetPieces(~Side, kPawn);

    if (!blockers) {
      int rank = GetRank(square);
      if constexpr (Side == kWhite) {
        rank = 7 - rank;
      }

      score.middle += kMiddleGameBonuses[rank];
      score.end += kEndGameBonuses[rank];
    }
  }

  return score;
}

template Score GetPassedPawnScore<kWhite>(const Position& position);
template Score GetPassedPawnScore<kBlack>(const Position& position);

namespace {

template <Side Side, Piece Piece>
int CountMoves(const Position& position) {
  int mobility = 0;

  Bitboard pieces = position.GetPieces(Side, Piece);
  const Bitboard blockers = position.GetPieces();
  while (pieces) {
    const Square square = pieces.PopLeastSignificantBit();
    const Bitboard attacks =
        GenerateAttacks<Piece>(square, blockers) & ~position.GetPieces(Side);
    mobility += attacks.GetCount();
  }

  return mobility;
}

}  // namespace

template <Side Side>
Score GetBishopMobilityScore(const Position& position) {
  const int mobility = CountMoves<Side, kBishop>(position) * 5;
  return {.middle = mobility, .end = mobility};
}

template Score GetBishopMobilityScore<kWhite>(const Position& position);
template Score GetBishopMobilityScore<kBlack>(const Position& position);

template <Side Side>
Score GetQueenMobilityScore(const Position& position) {
  const int mobility = CountMoves<Side, kQueen>(position);
  return {.middle = mobility * 1, .end = mobility * 2};
}

template Score GetQueenMobilityScore<kWhite>(const Position& position);
template Score GetQueenMobilityScore<kBlack>(const Position& position);

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
  const Score tapered_score =                   //
      GetPlacementScore<Side>(position) +       //
      GetKingSafetyScore<Side>(position) +      //
      GetPassedPawnScore<Side>(position) +      //
      GetBishopMobilityScore<Side>(position) +  //
      GetQueenMobilityScore<Side>(position);

  return Interpolate(tapered_score, phase) +            //
         GetMaterialScore<Side>(position) +             //
         -50 * CountDoubledPawns<Side>(position) +      //
         -50 * CountBlockedPawns<Side>(position) +      //
         10 * CountSemiOpenFileRooks<Side>(position) +  //
         15 * CountOpenFileRooks<Side>(position);
}

}  // namespace

[[nodiscard]] int Evaluate(const Position& position, int phase) {
  return Evaluate<kWhite>(position, phase) - Evaluate<kBlack>(position, phase);
}

}  // namespace follychess

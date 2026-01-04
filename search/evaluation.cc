#include "search/evaluation.h"

#include "engine/move_generator.h"
#include "engine/position.h"

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

namespace {

[[nodiscard]] int Interpolate(Score score, int phase) {
  const int middle = score.middle * (kEndPhaseValue - phase);
  const int end = score.end * phase;
  return (middle + end) / kEndPhaseValue;
}

template <Side Side>
[[nodiscard]] int Evaluate(const Position& position, int phase) {
  Score placement_score = GetPlacementScore<Side>(position);

  return Interpolate(placement_score, phase) +      //
         GetMaterialScore<Side>(position) +         //
         -50 * CountDoubledPawns<Side>(position) +  //
         -50 * CountBlockedPawns<Side>(position);
}

}  // namespace

[[nodiscard]] int Evaluate(const Position& position, int phase) {
  return Evaluate<kWhite>(position, phase) - Evaluate<kBlack>(position, phase);
}

}  // namespace follychess

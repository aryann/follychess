#ifndef FOLLYCHESS_SEARCH_EVALUATION_H_
#define FOLLYCHESS_SEARCH_EVALUATION_H_

#include "engine/position.h"

namespace follychess {

struct Score {
  int middle = 0;
  int end = 0;

  constexpr Score operator+(const Score other) const {
    return {.middle = middle + other.middle, .end = end + other.end};
  }
};

template <Side Side>
consteval auto MakePassedPawnMasks() {
  std::array<Bitboard, kNumSquares> masks = {};
  constexpr Direction backward = Side == kWhite ? kSouth : kNorth;

  for (int square = A8; square < kNumSquares; ++square) {
    const Square target = static_cast<Square>(square);
    Bitboard file = file::kFileMasks[GetFile(target)];

    Bitboard mask = file;
    mask |= file.Shift<kEast>();
    mask |= file.Shift<kWest>();

    Bitboard rank = rank::kRankMasks[GetRank(target)];
    while (rank) {
      mask &= ~rank;
      rank = rank.Shift<backward>();
    }

    masks[square] = mask;
  }

  return masks;
}

consteval auto MakePassedPawnMasks() {
  return std::array{
      MakePassedPawnMasks<kWhite>(),
      MakePassedPawnMasks<kBlack>(),
  };
}

constexpr auto kPassedPawnMasks = MakePassedPawnMasks();

template <Side Side>
[[nodiscard]] Score GetPlacementScore(const Position& position);

template <Side Side>
[[nodiscard]] int GetMaterialScore(const Position& position);

template <Side Side>
[[nodiscard]] int CountDoubledPawns(const Position& position);

template <Side Side>
[[nodiscard]] int CountBlockedPawns(const Position& position);

template <Side Side>
[[nodiscard]] int GetPassedPawnScore(const Position& position);

template <Side Side>
[[nodiscard]] int CountSemiOpenFileRooks(const Position& position);

template <Side Side>
[[nodiscard]] int CountOpenFileRooks(const Position& position);

template <Side Side>
[[nodiscard]] Score GetKingSafetyScore(const Position& position);

[[nodiscard]] int Evaluate(const Position& position, int phase);

}  // namespace follychess

#endif  // FOLLYCHESS_SEARCH_EVALUATION_H_

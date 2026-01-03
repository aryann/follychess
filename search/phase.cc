#include "search/phase.h"

#include "engine/position.h"

namespace follychess {
namespace {

constexpr int kKnightMaterialScore = 1;
constexpr int kBishopMaterialScore = 1;
constexpr int kRookMaterialScore = 2;
constexpr int kQueenMaterialScore = 4;

constexpr int kStartMaterialScore = 4 * kKnightMaterialScore +  //
                                    4 * kBishopMaterialScore +  //
                                    4 * kRookMaterialScore +    //
                                    2 * kQueenMaterialScore;

}  // namespace

int CalculatePhase(const Position& position) {
  int phase = kStartMaterialScore;
  phase -= position.GetPieces(kKnight).GetCount() * kKnightMaterialScore;
  phase -= position.GetPieces(kBishop).GetCount() * kBishopMaterialScore;
  phase -= position.GetPieces(kRook).GetCount() * kRookMaterialScore;
  phase -= position.GetPieces(kQueen).GetCount() * kQueenMaterialScore;

  phase = phase * kEndPhaseValue + kStartMaterialScore / 2;
  phase /= kStartMaterialScore;
  return phase;
}

}  // namespace follychess

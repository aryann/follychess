#include "search/phase.h"

#include "engine/position.h"

namespace follychess {
namespace {

constexpr int kKnightPhase = 1;
constexpr int kBishopPhase = 1;
constexpr int kRookPhase = 2;
constexpr int kQueenPhase = 4;

constexpr int kStartPhase = 4 * kKnightPhase +  //
                            4 * kBishopPhase +  //
                            4 * kRookPhase +    //
                            2 * kQueenPhase;

}  // namespace

int CalculatePhase(const Position& position) {
  int phase = kStartPhase;
  phase -= position.GetPieces(kKnight).GetCount() * kKnightPhase;
  phase -= position.GetPieces(kBishop).GetCount() * kBishopPhase;
  phase -= position.GetPieces(kRook).GetCount() * kRookPhase;
  phase -= position.GetPieces(kQueen).GetCount() * kQueenPhase;

  phase = (phase * 256 + kStartPhase / 2) / kStartPhase;
  return phase;
}

}  // namespace follychess

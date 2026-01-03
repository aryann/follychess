#include "benchmark/benchmark.h"
#include "engine/attacks.h"
#include "engine/move_generator.h"
#include "engine/position.h"
#include "engine/scoped_move.h"
#include "engine/types.h"

namespace follychess {
namespace {

void MakeMoves(std::size_t depth, Position& position) {
  if (depth == 0) {
    return;
  }

  for (Move move : GenerateMoves(position)) {
    ScopedMove scoped_move(move, position);
    if (position.GetCheckers(~position.SideToMove())) {
      continue;
    }
    MakeMoves(depth - 1, position);
  }
}

template <class... Args>
void BM_MakeMoves(benchmark::State& state, Args&&... args) {
  auto args_tuple = std::make_tuple(std::move(args)...);

  std::size_t depth = state.range(0);
  auto position = Position::FromFen(std::get<0>(args_tuple));
  CHECK_EQ(position.error_or(""), "");

  for (auto _ : state) {
    MakeMoves(depth, position.value());
  }
}

BENCHMARK_CAPTURE(  //
    BM_MakeMoves, Starting,
    R"(rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1)")
    ->DenseRange(/* start = */ 1, /* limit = */ 6, /* step = */ 1);

BENCHMARK_CAPTURE(
    BM_MakeMoves, Position2,
    R"(r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1)")
    ->DenseRange(/* start = */ 1, /* limit = */ 5, /* step = */ 1);

BENCHMARK_CAPTURE(
    BM_MakeMoves, Position5,
    R"(rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8)")
    ->DenseRange(/* start = */ 1, /* limit = */ 5, /* step = */ 1);

}  // namespace
}  // namespace follychess

BENCHMARK_MAIN();

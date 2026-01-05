#include "benchmark/benchmark.h"
#include "engine/position.h"
#include "search/search.h"

namespace follychess {
namespace {

template <class... Args>
void BM_Search(benchmark::State& state, Args&&... args) {
  auto args_tuple = std::make_tuple(std::move(args)...);

  int depth = state.range(0);
  auto position = Position::FromFen(std::get<0>(args_tuple));
  CHECK_EQ(position.error_or(""), "");
  Game game(position.value());

  for (auto _ : state) {
    Search(game, SearchOptions().SetDepth(depth));
  }
}

BENCHMARK_CAPTURE(  //
    BM_Search, Starting,
    R"(rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1)")
    ->DenseRange(/* start = */ 1, /* limit = */ 7, /* step = */ 1);

BENCHMARK_CAPTURE(BM_Search, Position3,
                  R"(8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1)")
    ->DenseRange(/* start = */ 1, /* limit = */ 8, /* step = */ 1);

BENCHMARK_CAPTURE(BM_Search, HighTransposition,
                  R"(8/8/7r/K7/1R6/7k/8/N7 w - - 0 1)")
    ->DenseRange(/* start = */ 1, /* limit = */ 8, /* step = */ 1);

}  // namespace
}  // namespace follychess

BENCHMARK_MAIN();

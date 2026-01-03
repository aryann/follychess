#include "benchmark/benchmark.h"
#include "engine/attacks.h"
#include "engine/move_generator.h"
#include "engine/position.h"
#include "engine/scoped_move.h"
#include "engine/types.h"

namespace follychess {
namespace {

template <class... Args>
void BM_GenerateLegalMovesWithCopy(benchmark::State& state, Args&&... args) {
  auto args_tuple = std::make_tuple(std::move(args)...);

  auto position_or_error = Position::FromFen(std::get<0>(args_tuple));
  CHECK_EQ(position_or_error.error_or(""), "");
  Position position = position_or_error.value();

  for (auto _ : state) {
    std::vector<Move> moves;
    for (Move move : GenerateMoves(position)) {
      Position new_position = position;
      new_position.Do(move);
      if (new_position.GetCheckers(~new_position.SideToMove())) {
        continue;
      }
      moves.push_back(move);
    }
    benchmark::DoNotOptimize(moves);
  }
}

template <class... Args>
void BM_GenerateLegalMovesWithScopedMove(benchmark::State& state,
                                         Args&&... args) {
  auto args_tuple = std::make_tuple(std::move(args)...);

  auto position_or_error = Position::FromFen(std::get<0>(args_tuple));
  CHECK_EQ(position_or_error.error_or(""), "");
  Position position = position_or_error.value();

  for (auto _ : state) {
    std::vector<Move> moves;
    for (Move move : GenerateMoves(position)) {
      ScopedMove scoped_move(move, position);
      if (position.GetCheckers(~position.SideToMove())) {
        continue;
      }
      moves.push_back(move);
    }
    benchmark::DoNotOptimize(moves);
  }
}

template <class... Args>
void BM_GeneratePseudoLegalMoves(benchmark::State& state, Args&&... args) {
  auto args_tuple = std::make_tuple(std::move(args)...);

  auto position = Position::FromFen(std::get<0>(args_tuple));
  CHECK_EQ(position.error_or(""), "");

  for (auto _ : state) {
    benchmark::DoNotOptimize(GenerateMoves(position.value()));
  }
}

constexpr auto kStarting =
    R"(rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1)";

constexpr auto kPosition2 =
    R"(r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1)";

constexpr auto kPosition3 =  //
    R"(8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1)";

constexpr auto kPosition5 =
    R"(rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8)";

BENCHMARK_CAPTURE(BM_GenerateLegalMovesWithCopy, kStarting, kStarting);
BENCHMARK_CAPTURE(BM_GenerateLegalMovesWithCopy, Position2, kPosition2);
BENCHMARK_CAPTURE(BM_GenerateLegalMovesWithCopy, Position3, kPosition3);
BENCHMARK_CAPTURE(BM_GenerateLegalMovesWithCopy, kPosition5, kPosition5);

BENCHMARK_CAPTURE(BM_GenerateLegalMovesWithScopedMove, Starting, kStarting);
BENCHMARK_CAPTURE(BM_GenerateLegalMovesWithScopedMove, Position2, kPosition2);
BENCHMARK_CAPTURE(BM_GenerateLegalMovesWithScopedMove, Position3, kPosition3);
BENCHMARK_CAPTURE(BM_GenerateLegalMovesWithScopedMove, kPosition5, kPosition5);

BENCHMARK_CAPTURE(BM_GeneratePseudoLegalMoves, kStarting, kStarting);
BENCHMARK_CAPTURE(BM_GeneratePseudoLegalMoves, kPosition2, kPosition2);
BENCHMARK_CAPTURE(BM_GeneratePseudoLegalMoves, kPosition3, kPosition3);
BENCHMARK_CAPTURE(BM_GeneratePseudoLegalMoves, kPosition5, kPosition5);

}  // namespace
}  // namespace follychess

BENCHMARK_MAIN();

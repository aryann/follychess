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

#include "absl/container/flat_hash_map.h"
#include "benchmark/benchmark.h"
#include "benchmarks/attacks.h"
#include "engine/attacks.h"
#include "engine/types.h"

namespace follychess {
namespace {

template <Piece Piece>
void BM_GenerateAttacksOnTheFly(benchmark::State& state) {
  std::mt19937 engine(std::random_device{}());
  std::uniform_int_distribution<std::uint64_t> dist(0);
  int square = 0;

  for (auto _ : state) {
    Bitboard occupied(dist(engine));
    benchmark::DoNotOptimize(
        GenerateAttacksOnTheFly<Piece>(static_cast<Square>(square), occupied));

    square = (square + 1) % kNumSquares;
  }
}

template <template <typename...> class Map, Piece Piece>
void BM_LookupAttacksFrom(benchmark::State& state) {
  static_assert(Piece == kBishop || Piece == kRook || Piece == kQueen);

  std::mt19937 engine(std::random_device{}());
  std::uniform_int_distribution<std::uint64_t> dist(0);
  int square = 0;

  for (auto _ : state) {
    Bitboard occupied(dist(engine));
    benchmark::DoNotOptimize(
        GetAttacksFromMap<Map, Piece>(static_cast<Square>(square), occupied));
    benchmark::ClobberMemory();

    square = (square + 1) % kNumSquares;
  }
}

template <Piece Piece>
void BM_LookupAttacksFromMagicTables(benchmark::State& state) {
  static_assert(Piece == kBishop || Piece == kRook || Piece == kQueen);

  std::mt19937 engine(std::random_device{}());
  std::uniform_int_distribution<std::uint64_t> dist(0);
  int square = 0;

  for (auto _ : state) {
    Bitboard occupied(dist(engine));
    benchmark::DoNotOptimize(
        GenerateAttacks<Piece>(static_cast<Square>(square), occupied));

    square = (square + 1) % kNumSquares;
  }
}

// Naively generate attacks on the fly:
BENCHMARK(BM_GenerateAttacksOnTheFly<kBishop>);
BENCHMARK(BM_GenerateAttacksOnTheFly<kRook>);
BENCHMARK(BM_GenerateAttacksOnTheFly<kQueen>);

// Use absl::flat_hash_map to lookup precomputed attacks:
BENCHMARK(BM_LookupAttacksFrom<absl::flat_hash_map, kBishop>);
BENCHMARK(BM_LookupAttacksFrom<absl::flat_hash_map, kRook>);
BENCHMARK(BM_LookupAttacksFrom<absl::flat_hash_map, kQueen>);

// Use std::map to lookup precomputed attacks:
BENCHMARK(BM_LookupAttacksFrom<std::map, kBishop>);
BENCHMARK(BM_LookupAttacksFrom<std::map, kRook>);
BENCHMARK(BM_LookupAttacksFrom<std::map, kQueen>);

// Use std::unordered_map to lookup precomputed attacks:
BENCHMARK(BM_LookupAttacksFrom<std::unordered_map, kBishop>);
BENCHMARK(BM_LookupAttacksFrom<std::unordered_map, kRook>);
BENCHMARK(BM_LookupAttacksFrom<std::unordered_map, kQueen>);

// Use magic bitboard to lookup precomputed attacks:
BENCHMARK(BM_LookupAttacksFromMagicTables<kBishop>);
BENCHMARK(BM_LookupAttacksFromMagicTables<kRook>);
BENCHMARK(BM_LookupAttacksFromMagicTables<kQueen>);

}  // namespace
}  // namespace follychess

BENCHMARK_MAIN();

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

std::vector<Bitboard> GetRandomOccupancies() {
  constexpr std::size_t kSampleSize = 10'000'000;

  std::mt19937 engine(std::random_device{}());
  std::uniform_int_distribution<std::uint64_t> dist(
      0, std::numeric_limits<std::uint64_t>::max());

  std::vector<Bitboard> result;
  result.reserve(kSampleSize);
  for (int i = 0; i < kSampleSize; ++i) {
    result.emplace_back(dist(engine));
  }
  return result;
}

template <Piece Piece>
void BM_GenerateAttacksOnTheFly(benchmark::State& state) {
  int square = 0;
  std::vector<Bitboard> occupancies = GetRandomOccupancies();
  int occupancy_index = 0;

  for (auto _ : state) {
    Bitboard occupied = occupancies[occupancy_index % occupancies.size()];
    benchmark::DoNotOptimize(GenerateAttacksOnTheFly<Piece>(
        static_cast<Square>(square % kNumSquares), occupied));

    ++square;
    ++occupancy_index;
  }
}

template <template <typename...> class Map, Piece Piece>
void BM_LookupAttacksFrom(benchmark::State& state) {
  int square = 0;
  std::vector<Bitboard> occupancies = GetRandomOccupancies();
  int occupancy_index = 0;

  for (auto _ : state) {
    Bitboard occupied = occupancies[occupancy_index % occupancies.size()];
    benchmark::DoNotOptimize(GetAttacksFromMap<Map, Piece>(
        static_cast<Square>(square % kNumSquares), occupied));

    ++square;
    ++occupancy_index;
  }
}

template <Piece Piece>
void BM_LookupAttacksFromMagicTables(benchmark::State& state) {
  int square = 0;
  std::vector<Bitboard> occupancies = GetRandomOccupancies();
  int occupancy_index = 0;

  for (auto _ : state) {
    Bitboard occupied = occupancies[occupancy_index % occupancies.size()];
    benchmark::DoNotOptimize(GenerateAttacks<Piece>(
        static_cast<Square>(square % kNumSquares), occupied));

    ++square;
    ++occupancy_index;
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

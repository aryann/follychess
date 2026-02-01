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
#include "engine/attacks.h"
#include "engine/types.h"

namespace follychess {
namespace {

[[nodiscard]] Bitboard GenerateBishopAttacksOnTheFly(Square square,
                                                     Bitboard occupied) {
  occupied &= kSlidingAttackTables.bishop_magic_squares[square].mask;
  return GenerateSlidingAttacks<kNorthEast, kSouthEast, kSouthWest, kNorthWest>(
      square, occupied);
}

[[nodiscard]] Bitboard GenerateRookAttacksOnTheFly(Square square,
                                                   Bitboard occupied) {
  occupied &= kSlidingAttackTables.rook_magic_squares[square].mask;
  return GenerateSlidingAttacks<kNorth, kEast, kSouth, kWest>(square, occupied);
}

template <Piece Piece>
[[nodiscard]] Bitboard GenerateAttacksOnTheFly(Square square,
                                               Bitboard occupied) {
  static_assert(Piece == kBishop || Piece == kRook || Piece == kQueen);

  if constexpr (Piece == kBishop) {
    return GenerateBishopAttacksOnTheFly(square, occupied);
  } else if constexpr (Piece == kRook) {
    return GenerateRookAttacksOnTheFly(square, occupied);
  } else {
    return GenerateAttacksOnTheFly<kBishop>(square, occupied) |
           GenerateAttacksOnTheFly<kRook>(square, occupied);
  }
}

template <Piece Piece>
void BM_GenerateAttacksOnTheFly(benchmark::State& state) {
  std::mt19937 engine(std::random_device{}());
  std::uniform_int_distribution<std::uint64_t> dist(0);

  for (auto _ : state) {
    const auto square = static_cast<Square>(dist(engine) % kNumSquares);
    Bitboard occupied(dist(engine));
    benchmark::DoNotOptimize(GenerateAttacksOnTheFly<Piece>(square, occupied));
  }
}

template <Direction... Directions>
[[nodiscard]] std::vector<Bitboard> GenerateOccupancies(Square square) {
  Bitboard mask = (MakeRay<Directions>(square) | ...);
  std::vector<Bitboard> occupancies = MakePowerSet(mask);
  return occupancies;
}

template <Piece Piece>
[[nodiscard]] std::vector<Bitboard> GenerateOccupancies(Square square) {
  if constexpr (Piece == kBishop) {
    return GenerateOccupancies<kNorthEast, kSouthEast, kSouthWest, kNorthWest>(
        square);
  } else if constexpr (Piece == kRook) {
    return GenerateOccupancies<kNorth, kEast, kSouth, kWest>(square);
  } else {
    return GenerateOccupancies<kNorth, kNorthEast, kEast, kSouthEast, kSouth,
                               kSouthWest, kWest, kNorthWest>(square);
  }
}

template <template <typename...> class Map, Piece Piece>
[[nodiscard]] auto GenerateAttacksMap() {
  std::array<Map<Bitboard, Bitboard>, kNumSquares> result;
  for (int square = kFirstSquare; square < kNumSquares; ++square) {
    Square from = static_cast<Square>(square);
    for (Bitboard occupied : GenerateOccupancies<Piece>(from)) {
      result[from][occupied] = GenerateAttacksOnTheFly<Piece>(from, occupied);
    }
  }
  return result;
}

template <template <typename...> class Map, Piece Piece>
[[nodiscard]] Bitboard GetAttacksFromMap(Square square, Bitboard occupied) {
  static_assert(Piece == kBishop || Piece == kRook || Piece == kQueen);

  if constexpr (Piece == kBishop) {
    static std::array<Map<Bitboard, Bitboard>, kNumSquares> kBishopAttacks =
        GenerateAttacksMap<Map, kBishop>();
    return kBishopAttacks[square][occupied];

  } else if constexpr (Piece == kRook) {
    static std::array<Map<Bitboard, Bitboard>, kNumSquares> kRookAttacks =
        GenerateAttacksMap<Map, kRook>();
    return kRookAttacks[square][occupied];

  } else {
    static std::array<Map<Bitboard, Bitboard>, kNumSquares> kQueenAttacks =
        GenerateAttacksMap<Map, kQueen>();
    return kQueenAttacks[square][occupied];
  }
}

template <template <typename...> class Map, Piece Piece>
void BM_LookupAttacksFrom(benchmark::State& state) {
  static_assert(Piece == kBishop || Piece == kRook || Piece == kQueen);

  std::mt19937 engine(std::random_device{}());
  std::uniform_int_distribution<std::uint64_t> dist(0);

  for (auto _ : state) {
    const auto square = static_cast<Square>(dist(engine) % kNumSquares);
    Bitboard occupied(dist(engine));
    benchmark::DoNotOptimize(GetAttacksFromMap<Map, Piece>(square, occupied));
    benchmark::ClobberMemory();
  }
}

template <Piece Piece>
void BM_LookupAttacksFromMagicTables(benchmark::State& state) {
  static_assert(Piece == kBishop || Piece == kRook || Piece == kQueen);

  std::mt19937 engine(std::random_device{}());
  std::uniform_int_distribution<std::uint64_t> dist(0);

  for (auto _ : state) {
    const auto square = static_cast<Square>(dist(engine) % kNumSquares);
    Bitboard occupied(dist(engine));
    benchmark::DoNotOptimize(GenerateAttacks<Piece>(square, occupied));
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

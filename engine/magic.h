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

#ifndef FOLLYCHESS_ENGINE_MAGIC_H_
#define FOLLYCHESS_ENGINE_MAGIC_H_

#include <array>
#include <random>

#include "absl/log/log.h"
#include "bitboard.h"

namespace follychess {

// Returns a bitboard representing a ray from the `from` square in the given
// `Direction`. The ray extends to the edge of the board, but the `from`
// square is not included.
template <Direction Direction>
constexpr Bitboard MakeRay(Square from) {
  Bitboard result;
  Bitboard curr(from);
  Bitboard edge;
  while (curr) {
    curr = curr.Shift<Direction>();
    if (curr) {
      edge = curr;
    }
    result |= curr;
  }
  return result & ~edge;
}

template <Direction Direction>
constexpr Bitboard GenerateRayAttacks(Square from, Bitboard occupied) {
  Bitboard attacks;
  Bitboard curr(from);
  while (curr) {
    curr = curr.Shift<Direction>();
    attacks |= curr;
    if (curr & occupied) {
      break;
    }
  }
  return attacks;
}

template <Direction... Directions>
constexpr Bitboard GenerateSlidingAttacks(Square from, Bitboard occupied) {
  return (GenerateRayAttacks<Directions>(from, occupied) | ...);
}

constexpr std::vector<Bitboard> MakePowerSet(Bitboard mask) {
  // https://www.chessprogramming.org/Traversing_Subsets_of_a_Set
  const std::size_t cardinality = 1ULL << mask.GetCount();
  std::vector<Bitboard> subsets(cardinality);

  Bitboard sub_mask;
  for (int i = 0; i < cardinality; ++i) {
    subsets[i] = sub_mask;
    sub_mask = (sub_mask - mask) & mask;
  }

  return subsets;
}

// Holds the magic bitboard data for a single square and piece type
// (bishop or rook).
struct MagicEntry {
  // The mask of relevant squares for this piece and square.
  Bitboard mask;

  std::uint64_t magic;
  std::uint8_t shift;

  // Pointer to the start of this square's attack table.
  std::size_t attack_table_index;
};

struct SlidingAttackTables {
  // The following diagram shows the number of relevancy bits (i.e., squares
  // on the relevant attack rays, excluding edges) for a bishop *on* each
  // square:
  //
  //   8: 6 5 5 5 5 5 5 6
  //   7: 5 5 5 5 5 5 5 5
  //   6: 5 5 7 7 7 7 5 5
  //   5: 5 5 7 9 9 7 5 5
  //   4: 5 5 7 9 9 7 5 5
  //   3: 5 5 7 7 7 7 5 5
  //   2: 5 5 5 5 5 5 5 5
  //   1: 6 5 5 5 5 5 5 6
  //      a b c d e f g h
  //
  // The worst-case is 9 bits (in the center). For simplicity, we allocate
  // a table of size 2^9 for every square.
  static constexpr int kBishopTableSizePerSquare = (1 << 9);

  // The number of relevancy bits for a rook also varies:
  //
  //   * 12 bits for corners (a1, h1, a8, h8)
  //   * 11 bits for other edge squares
  //   * 10 bits for all other squares
  //
  // The worst-case is 12 bits. For simplicity, we allocate a table of size 2^12
  // for every square.
  static constexpr int kRookTableSizePerSquare = (1 << 12);

  // TODO(aryann): Consider tightly packing all attacks. Only 107,648 slots are
  // required. The current scheme sets aside 294,912 slots, which is 2.7 times
  // more space than is necessary.
  static constexpr int kAttackTableSize =
      (kBishopTableSizePerSquare + kRookTableSizePerSquare) * kNumSquares;
  std::array<Bitboard, kAttackTableSize> attacks;

  std::array<MagicEntry, kNumSquares> bishop_magic_squares;
  std::array<MagicEntry, kNumSquares> rook_magic_squares;
};

template <Direction... Directions>
constexpr void FindMagicForSquare(Square from, std::size_t attack_table_index,
                                  Bitboard *attack_table,
                                  MagicEntry &magic_struct) {
  Bitboard mask = (MakeRay<Directions>(from) | ...);
  std::vector<Bitboard> occupancies = MakePowerSet(mask);
  std::uint8_t shift = 64 - mask.GetCount();

  std::vector<Bitboard> attacks;
  attacks.reserve(occupancies.size());
  for (Bitboard occupied : occupancies) {
    attacks.push_back(GenerateSlidingAttacks<Directions...>(from, occupied));
  }

  static std::mt19937 kEngine(std::random_device{}());
  std::uniform_int_distribution<std::uint64_t> dist(0);

  int attempt = 1;
  while (true) {
    // Generate a "sparse" magic number candidate. ANDing three random numbers
    // reduces the bit density to ~1/8.
    //
    // This heuristic is known to produce "good" magic numbers (those
    // that minimize collisions) much faster than fully random numbers.
    std::uint64_t magic = dist(kEngine) & dist(kEngine) & dist(kEngine);

    std::vector<Bitboard> placements(attacks.size(), Bitboard(0));
    bool found = true;

    for (int i = 0; i < occupancies.size(); ++i) {
      std::size_t index = (magic * occupancies[i].Data()) >> shift;
      if (placements[index]) {
        found = false;
        ++attempt;
        break;
      }
      placements[index] = attacks[i];
    }

    if (found) {
      LOG(INFO) << "Found magic for " << ToString(from) << " after " << attempt
                << " attempts: " << magic;
      for (int i = 0; i < placements.size(); ++i) {
        attack_table[attack_table_index + i] = placements[i];
      }

      magic_struct = {
          .mask = mask,
          .magic = magic,
          .shift = shift,
          .attack_table_index = attack_table_index,
      };
      break;
    }
  }
}

constexpr SlidingAttackTables GenerateSlidingAttackTables() {
  SlidingAttackTables sliding_attacks;
  std::size_t rook_attack_table_index =
      SlidingAttackTables::kBishopTableSizePerSquare * kNumSquares;

  for (int square = kFirstSquare; square < kNumSquares; ++square) {
    const auto from = static_cast<Square>(square);

    // Generate the MagicEntry for a bishop on this square:
    FindMagicForSquare<kNorthEast, kSouthEast, kSouthWest, kNorthWest>(
        from, (1 << 9) * from, sliding_attacks.attacks.begin(),
        sliding_attacks.bishop_magic_squares[square]);

    // Generate the MagicEntry for a rook on this square:
    FindMagicForSquare<kNorth, kEast, kSouth, kWest>(
        from, rook_attack_table_index + (1 << 12) * from,
        sliding_attacks.attacks.begin(),
        sliding_attacks.rook_magic_squares[square]);
  }
  return sliding_attacks;
}

}  // namespace follychess

#endif  // FOLLYCHESS_ENGINE_MAGIC_H_

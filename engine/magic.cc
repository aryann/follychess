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

#include "magic.h"

#include "absl/log/log.h"

namespace follychess {

template <Direction... Directions>
void FindMagicForSquare(Square from, std::size_t attack_table_index,
                        Bitboard *attack_table, MagicEntry &magic_struct) {
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
      LOG(INFO) << std::format("  Found magic for {} after {:6} attempts: {}",
                               ToString(from), attempt, magic);
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

SlidingAttackTables GenerateSlidingAttacks() {
  SlidingAttackTables sliding_attacks;

  LOG(INFO) << "Finding magic numbers for bishops:";
  for (int square = kFirstSquare; square < kNumSquares; ++square) {
    const auto from = static_cast<Square>(square);
    FindMagicForSquare<kNorthEast, kSouthEast, kSouthWest, kNorthWest>(
        from, (1 << 9) * from, sliding_attacks.attacks.begin(),
        sliding_attacks.bishop_magic_squares[square]);
  }

  std::size_t rook_attack_table_index =
      SlidingAttackTables::kBishopTableSizePerSquare * kNumSquares;

  LOG(INFO) << "Finding magic numbers for rooks:";
  for (int square = kFirstSquare; square < kNumSquares; ++square) {
    const auto from = static_cast<Square>(square);
    FindMagicForSquare<kNorth, kEast, kSouth, kWest>(
        from, rook_attack_table_index + (1 << 12) * from,
        sliding_attacks.attacks.begin(),
        sliding_attacks.rook_magic_squares[square]);
  }
  return sliding_attacks;
}

}  // namespace follychess
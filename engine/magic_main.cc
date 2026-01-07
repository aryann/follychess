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

#include <fstream>
#include <iostream>
#include <print>

#include "magic.h"

namespace {
using ::follychess::kNumSquares;
using ::follychess::MagicEntry;
using ::follychess::SlidingAttackTables;

void AddInclude(std::string_view file, std::ofstream& output) {
  std::println(output, R"(#include "{}")", file);
}

void AddMagicEntry(const MagicEntry& entry, std::ofstream& output) {
  std::println(output, "    MagicEntry{{");
  std::println(output, "      .mask = Bitboard({}ULL),", entry.mask.Data());
  std::println(output, "      .magic = {}ULL,", entry.magic);
  std::println(output, "      .shift = {}U,", entry.shift);
  std::println(output, "      .attack_table_index = {},",
               entry.attack_table_index);
  std::println(output, "    }},");
}

void AddTable(std::ofstream& output) {
  SlidingAttackTables table = follychess::GenerateSlidingAttackTables();

  std::println(output,
               "constexpr SlidingAttackTables kSlidingAttackTables = {{");
  std::println(output, "  .attacks = {{");
  for (int i = 0; i < SlidingAttackTables::kAttackTableSize; ++i) {
    std::println(output, "    Bitboard({}ULL),", table.attacks[i].Data());
  }
  std::println(output, "   }},");

  std::println(output, "  .bishop_magic_squares = {{");
  for (int i = 0; i < kNumSquares; ++i) {
    AddMagicEntry(table.bishop_magic_squares[i], output);
  }
  std::println(output, "  }},");

  std::println(output, "  .rook_magic_squares = {{");
  for (int i = 0; i < kNumSquares; ++i) {
    AddMagicEntry(table.rook_magic_squares[i], output);
  }
  std::println(output, "  }},");
  std::println(output, "}};");
}

}  // namespace

int main(int argc, char** argv) {
  if (argc != 2) {
    std::println(std::cerr,
                 "Expected output file name as a single positional argument.");
    return 1;
  }

  std::string output_filename = argv[1];
  std::ofstream output(output_filename);

  if (!output.is_open()) {
    std::println(std::cerr, "Could not open output file: {}", output_filename);
    return 1;
  }

  std::println(output, "#ifndef CHESS_ENGINE_ENGINE_MAGIC_GENERATED_H_");
  std::println(output, "#define CHESS_ENGINE_ENGINE_MAGIC_GENERATED_H_");
  std::println(output);
  AddInclude("engine/bitboard.h", output);
  AddInclude("engine/magic.h", output);
  AddInclude("engine/types.h", output);
  std::println(output);
  std::println(output, "namespace follychess {{");
  std::println(output);

  AddTable(output);

  std::println(output);
  std::println(output, "}}  // namespace follychess");
  std::println(output);
  std::println(output, "#endif  // CHESS_ENGINE_ENGINE_MAGIC_GENERATED_H_");

  output.close();
  return 0;
}
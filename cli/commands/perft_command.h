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

#ifndef FOLLYCHESS_CLI_COMMANDS_PERFT_COMMAND_H_
#define FOLLYCHESS_CLI_COMMANDS_PERFT_COMMAND_H_

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "cli/command.h"
#include "engine/game.h"
#include "engine/perft.h"

namespace follychess {

class PerftCommand : public Command {
 public:
  explicit PerftCommand(CommandState& state) : state_(state) {}

  ~PerftCommand() override = default;

  std::expected<void, std::string> Run(
      std::vector<std::string_view> args) override {
    int depth = 1;
    if (!args.empty()) {
      depth = std::stoi(std::string(args[0]));
    }

    std::vector<std::size_t> depth_counts;
    std::map<Move, std::size_t> final_move_counts;
    RunPerft(depth, state_.game.GetPosition(), depth_counts, final_move_counts);

    for (auto [move, count] : final_move_counts) {
      state_.printer.Println(std::cout, "{}: {}", move, count);
    }

    state_.printer.Println(std::cout);
    state_.printer.Println(std::cout, "Nodes searched: {}",
                           depth_counts.back());
    return {};
  }

 private:
  CommandState& state_;
};

}  // namespace follychess

#endif  // FOLLYCHESS_CLI_COMMANDS_PERFT_COMMAND_H_

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

#include <iostream>
#include <string_view>

#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "cli.h"

int main(int argc, char **argv) {
  std::println(R"(
        ,,
      ,/  \
     / 'O  |
    (   < _|
     \___/
     /   \
    /_____\
   /_______\
   (________)

   FollyChess
)");

  using ::follychess::Command;
  using ::follychess::CommandDispatcher;
  using ::follychess::CommandState;
  using ::follychess::MakeCommandDispatcher;

  CommandState state;
  CommandDispatcher dispatcher = MakeCommandDispatcher(state);

  int positional_arg = 1;

  std::string command;
  while (true) {
    if (positional_arg < argc) {
      command = argv[positional_arg];
      state.printer.Println(std::cout, ">>> {}", command);
      ++positional_arg;
    } else {
      std::getline(std::cin, command);
    }
    state.printer.PrintStdIn(command);

    if (command == "quit") {
      return EXIT_SUCCESS;
    }

    std::vector<std::string_view> parts = absl::StrSplit(
        command, absl::ByAsciiWhitespace(), absl::SkipWhitespace());

    auto error = dispatcher.Run(parts);
    if (!error.has_value()) {
      state.printer.Println(std::cerr, "{}", error.error());
    }
  }
}

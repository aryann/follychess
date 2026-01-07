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

#ifndef FOLLYCHESS_CLI_COMMANDS_UCI_COMMAND_H_
#define FOLLYCHESS_CLI_COMMANDS_UCI_COMMAND_H_

#include <iostream>
#include <print>

#include "cli/command.h"
#include "cli/options.h"
#include "search/search.h"

namespace follychess {

class Uci : public Command {
 public:
  explicit Uci(CommandState& state) : state_(state) {}

  std::expected<void, std::string> Run(
      std::vector<std::string_view> args) override {
    state_.printer.Println(std::cout, "id name FollyChess");
    state_.printer.Println(std::cout, "id author Aryan Naraghi");
    state_.printer.Println(std::cout);

    for (const Option* option : GetOptions()) {
      state_.printer.Println(std::cout, "option name {} {}", option->GetName(),
                             option->GetType());
    }
    state_.printer.Println(std::cout, "uciok");
    return {};
  }

 private:
  CommandState& state_;
};

class SetOption : public Command {
 public:
  explicit SetOption(CommandState& state) : state_(state) {}

  std::expected<void, std::string> Run(
      std::vector<std::string_view> args) override {
    if (args.size() != 4) {
      return std::unexpected(
          std::format("Invalid setoption command: {}", args));
    }

    if (args[0] != "name" || args[2] != "value") {
      return std::unexpected(
          std::format("Invalid setoption command: {}", args));
    }

    std::string_view name = args[1];
    std::string_view value = args[3];

    for (Option* option : GetOptions()) {
      if (option->GetName() == name) {
        return option->Set(value, state_);
      }
    }

    return std::unexpected(std::format("Invalid option: {}", name));
  }

 private:
  CommandState& state_;
};

class Go : public Command {
 public:
  explicit Go(CommandState& state) : state_(state) {}

  std::expected<void, std::string> Run(
      std::vector<std::string_view> args) override {
    constexpr int kDefaultSearchDepth = 6;
    int depth = kDefaultSearchDepth;

    for (int i = 0; i < args.size() - 1; i += 2) {
      if (args[i] == "depth") {
        depth = std::stoi(std::string(args[i + 1]));
      }
    }

    Move move = Search(state_.game,
                       SearchOptions()       //
                           .SetDepth(depth)  //
                           .SetLogger([&](std::string_view value) {
                             state_.printer.Println(std::cout, "{}", value);
                           }));

    state_.printer.Println(std::cout, "bestmove {}", move);
    return {};
  }

 private:
  CommandState& state_;
};

}  // namespace follychess

#endif  // FOLLYCHESS_CLI_COMMANDS_UCI_COMMAND_H_

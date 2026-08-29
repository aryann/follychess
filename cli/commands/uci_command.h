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

#include <algorithm>
#include <charconv>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <optional>

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

class UciNewGame : public Command {
 public:
  explicit UciNewGame(CommandState& state) : state_(state) {}

  std::expected<void, std::string> Run(
      std::vector<std::string_view> args) override {
    state_.game = Game();
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

    SearchOptions options;
    std::optional<int> depth;
    for (std::size_t i = 0; i < args.size(); ++i) {
      const std::string_view key = args[i];
      if (key != "depth" && key != "movetime" && key != "nodes") {
        // The UCI protocol instructs engines to skip tokens they do not
        // understand. GUIs routinely send options FollyChess does not support
        // yet, such as wtime and btime.
        continue;
      }

      if (i + 1 == args.size()) {
        return std::unexpected(std::format("Missing value for go {}", key));
      }

      const std::expected<std::int64_t, std::string> value =
          ParseValue(key, args[++i]);
      if (!value) {
        return std::unexpected(value.error());
      }

      if (key == "depth") {
        // Depths beyond kMaxSearchDepth would overrun the search's per-ply
        // tables.
        depth =
            static_cast<int>(std::min<std::int64_t>(*value, kMaxSearchDepth));
      } else if (key == "movetime") {
        options.SetMoveTime(std::chrono::milliseconds(*value));
      } else {
        options.SetNodeLimit(*value);
      }
    }

    if (depth) {
      options.SetDepth(*depth);
    } else if (options.move_time || options.node_limit) {
      // The search is bounded by time or nodes, so let iterative deepening
      // run as far as the limit allows.
      options.SetDepth(kMaxSearchDepth);
    } else {
      options.SetDepth(kDefaultSearchDepth);
    }

    Move move = Search(state_.game,
                       options  //
                           .SetInfoObserver([&](const SearchInfo& info) {
                             state_.printer.Println(std::cout, "{}", info);
                           }));

    state_.printer.Println(std::cout, "bestmove {}", move);
    return {};
  }

 private:
  [[nodiscard]] static std::expected<std::int64_t, std::string> ParseValue(
      std::string_view key, std::string_view value) {
    std::int64_t result = 0;
    const auto [ptr, ec] =
        std::from_chars(value.data(), value.data() + value.size(), result);
    if (ec != std::errc() || ptr != value.data() + value.size() ||
        result <= 0) {
      return std::unexpected(
          std::format("Invalid value for go {}: {}", key, value));
    }
    return result;
  }

  CommandState& state_;
};

}  // namespace follychess

#endif  // FOLLYCHESS_CLI_COMMANDS_UCI_COMMAND_H_

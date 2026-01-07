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

#ifndef FOLLYCHESS_CLI_COMMAND_H_
#define FOLLYCHESS_CLI_COMMAND_H_

#include <expected>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "engine/game.h"

namespace follychess {

class Printer {
 public:
  template <typename... Args>
  void Println(std::ostream &stream, std::format_string<Args...> fmt = "",
               Args &&...args) const {
    const std::string value = std::format(fmt, std::forward<Args>(args)...);
    stream << value << std::endl;
    if (log_file_) {
      *log_file_ << value << std::endl;
    }
  }

  void PrintStdIn(std::string_view in) const;

  std::expected<void, std::string> SetLogFile(std::string filename);

 private:
  std::unique_ptr<std::ofstream> log_file_;
};

struct CommandState {
  Game game;
  Printer printer;
};

class Command {
 public:
  virtual ~Command() = default;

  [[nodiscard]] virtual std::expected<void, std::string> Run(
      std::vector<std::string_view> args) = 0;
};

class CommandDispatcher {
 public:
  CommandDispatcher() = default;

  CommandDispatcher &Add(std::string arg, std::unique_ptr<Command> command);

  CommandDispatcher &Add(std::string arg, CommandDispatcher dispatcher);

  [[nodiscard]] std::expected<void, std::string> Run(
      std::vector<std::string_view> args);

 private:
  std::map<std::string,
           std::variant<std::unique_ptr<Command>, CommandDispatcher>,
           std::less<>>
      command_map_;
};

}  // namespace follychess

#endif  // FOLLYCHESS_CLI_COMMAND_H_

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

#include "command.h"

#include <format>
#include <string>
#include <string_view>
#include <vector>

namespace follychess {
namespace {

template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};

}  // namespace

void Printer::PrintStdIn(std::string_view in) const {
  if (log_file_) {
    *log_file_ << in << std::endl;
  }
}

std::expected<void, std::string> Printer::SetLogFile(std::string filename) {
  auto log_file = std::make_unique<std::ofstream>(filename);
  if (!log_file->is_open()) {
    return std::unexpected(std::format("Could not open file: {}", filename));
  }
  log_file_ = std::move(log_file);
  return {};
}

CommandDispatcher &CommandDispatcher::Add(std::string arg,
                                          std::unique_ptr<Command> command) {
  command_map_[std::move(arg)] = std::move(command);
  return *this;
}

CommandDispatcher &CommandDispatcher::Add(std::string arg,
                                          CommandDispatcher dispatcher) {
  command_map_[std::move(arg)] = std::move(dispatcher);
  return *this;
}

[[nodiscard]] std::expected<void, std::string> CommandDispatcher::Run(
    std::vector<std::string_view> args) {
  if (args.empty() || args[0].empty()) {
    return {};
  }

  auto it = command_map_.find(args[0]);
  if (it == command_map_.end()) {
    return std::unexpected(std::format("Invalid command: {}", args));
  }

  std::vector<std::string_view> remaining_args(args.begin() + 1, args.end());

  return std::visit(overloaded{
                        [&](std::unique_ptr<Command> &command) {
                          return command->Run(remaining_args);
                        },
                        [&](CommandDispatcher &dispatcher) {
                          return dispatcher.Run(remaining_args);
                        },
                    },
                    it->second);
}

}  // namespace follychess

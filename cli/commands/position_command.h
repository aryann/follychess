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

#ifndef FOLLYCHESS_CLI_COMMANDS_POSITION_COMMAND_H_
#define FOLLYCHESS_CLI_COMMANDS_POSITION_COMMAND_H_

#include <string>
#include <string_view>
#include <vector>

#include "cli/command.h"
#include "engine/game.h"
#include "engine/position.h"

namespace follychess {

class StartPos : public Command {
 public:
  explicit StartPos(Game& game) : game_(game) {}

  ~StartPos() override = default;

  std::expected<void, std::string> Run(
      std::vector<std::string_view> args) override;

 private:
  Game& game_;
};

class FenPos : public Command {
 public:
  explicit FenPos(Game& game) : game_(game) {}

  ~FenPos() override = default;

  std::expected<void, std::string> Run(
      std::vector<std::string_view> args) override;

 private:
  Game& game_;
};

}  // namespace follychess

#endif  // FOLLYCHESS_CLI_COMMANDS_POSITION_COMMAND_H_

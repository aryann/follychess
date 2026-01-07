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

#ifndef FOLLYCHESS_CLI_OPTIONS_H_
#define FOLLYCHESS_CLI_OPTIONS_H_

#include <string_view>
#include <vector>

#include "command.h"

namespace follychess {

class Option {
 public:
  virtual ~Option() = default;

  [[nodiscard]] virtual std::string_view GetName() const = 0;

  [[nodiscard]] virtual std::string_view GetType() const = 0;

  virtual std::expected<void, std::string> Set(std::string_view log_directory,
                                               CommandState& state) = 0;
};

std::vector<Option*> GetOptions();

}  // namespace follychess

#endif  // FOLLYCHESS_CLI_OPTIONS_H_

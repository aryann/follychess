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

#include "options.h"

#include <chrono>

namespace follychess {
namespace {

namespace fs = std::filesystem;

class LogDirectory : public Option {
 public:
  [[nodiscard]] std::string_view GetName() const override {
    return "LogDirectory";
  }

  [[nodiscard]] std::string_view GetType() const override {
    return "type string default <empty>";
  }

  std::expected<void, std::string> Set(std::string_view log_directory,
                                       CommandState& state) override {
    auto now = std::chrono::system_clock::now();
    const std::string iso_time_utc = std::format("{:%Y-%m-%dT%H:%M:%SZ}", now);
    const fs::path path = fs::path(log_directory) / fs::path(iso_time_utc);

    return state.printer.SetLogFile(path);
  }
};

}  // namespace

std::vector<Option*> GetOptions() {
  static LogDirectory kLogDirectory;

  return {
      &kLogDirectory,
  };
}

}  // namespace follychess

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

#ifndef FOLLYCHESS_SEARCH_SEARCH_H_
#define FOLLYCHESS_SEARCH_SEARCH_H_

#include "engine/game.h"
#include "engine/move.h"
#include "engine/position.h"

namespace follychess {

struct SearchOptions {
  SearchOptions& SetDepth(int value) {
    depth = value;
    return *this;
  }

  int depth = 5;

  SearchOptions& SetLogger(const std::function<void(std::string_view)>& value) {
    logger = value;
    return *this;
  }

  std::function<void(std::string_view)> logger = [](std::string_view) {};
};

Move Search(const Game& game, const SearchOptions& options = SearchOptions());

}  // namespace follychess

#endif  // FOLLYCHESS_SEARCH_SEARCH_H_

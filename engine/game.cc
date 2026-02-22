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

#include "engine/game.h"

namespace follychess {

[[nodiscard]] int Game::GetRepetitionCount() const {
  int repetitions = 1;
  const ZobristKey current_key = GetPosition().GetKey();

  const int start = std::ssize(history_) - 3;
  const int limit = std::ssize(history_) - 1 - GetPosition().GetHalfMoves();

  for (int i = start; i >= limit; --i) {
    if (history_[i].key == current_key) {
      ++repetitions;
    }
  }

  return repetitions;
}

}  // namespace follychess

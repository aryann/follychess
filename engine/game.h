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

#ifndef FOLLYCHESS_ENGINE_GAME_H_
#define FOLLYCHESS_ENGINE_GAME_H_

#include "engine/position.h"

namespace follychess {

class Game {
 public:
  explicit Game(const Position& position) {
    history_.push_back({
        .key = position.GetKey(),
        .position = position,
    });
  }

  Game() : Game(Position::Starting()) {}

  void Do(Move move) {
    DCHECK(!history_.empty());
    Position position = history_.back().position;
    position.Do(move);

    history_.push_back({
        .key = position.GetKey(),
        .position = position,
    });
  }

  void Undo() {
    DCHECK(!history_.empty());
    history_.pop_back();
  }

  [[nodiscard]] int GetRepetitionCount() const;

  [[nodiscard]] const Position& GetPosition() const {
    DCHECK(!history_.empty());
    return history_.back().position;
  }

 private:
  struct State {
    ZobristKey key;
    Position position;
  };

  std::vector<State> history_;
};

}  // namespace follychess

#endif  // FOLLYCHESS_ENGINE_GAME_H_

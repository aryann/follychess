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
  explicit Game(const Position& position) : position_(position) {}

  Game() : position_(Position::Starting()) {}

  void Do(Move move) {
    history_.push_back({
        .key = position_.GetKey(),
        .undo_info = position_.Do(move),
    });
  }

  void Undo() {
    DCHECK(!history_.empty());
    position_.Undo(history_.back().undo_info);
    history_.pop_back();
  }

  [[nodiscard]] int GetRepetitionCount() const;

  [[nodiscard]] const Position& GetPosition() const { return position_; }

 private:
  struct State {
    std::uint64_t key{0};
    UndoInfo undo_info;
  };

  Position position_;
  std::vector<State> history_;
};

}  // namespace follychess

#endif  // FOLLYCHESS_ENGINE_GAME_H_

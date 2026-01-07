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

#ifndef FOLLYCHESS_SCOPED_MOVE_H_
#define FOLLYCHESS_SCOPED_MOVE_H_

#include "engine/game.h"
#include "engine/move.h"
#include "engine/position.h"

namespace follychess {

// A RAII class to make a move on a Position.
//
// When this object is constructed, it calls `position.Do(move)`. When it
// goes out of scope, its destructor automatically calls `position.Undo(move)`
// to restore the previous state.
class ScopedMove {
 public:
  ScopedMove(const Move &move, Position &position)
      : position_(position), undo_info_(position.Do(move)) {}

  ~ScopedMove() { position_.Undo(undo_info_); }

  ScopedMove(const ScopedMove &) = delete;

  ScopedMove &operator=(const ScopedMove &) = delete;

  ScopedMove(ScopedMove &&) = delete;

  ScopedMove &operator=(ScopedMove &&) = delete;

 private:
  Position &position_;
  UndoInfo undo_info_;
};

class ScopedMove2 {
 public:
  ScopedMove2(const Move &move, Game &game) : game_(game) { game.Do(move); }

  ~ScopedMove2() { game_.Undo(); }

  ScopedMove2(const ScopedMove2 &) = delete;

  ScopedMove2 &operator=(const ScopedMove2 &) = delete;

  ScopedMove2(ScopedMove2 &&) = delete;

  ScopedMove2 &operator=(ScopedMove2 &&) = delete;

 private:
  Game &game_;
};

}  // namespace follychess

#endif  // FOLLYCHESS_SCOPED_MOVE_H_

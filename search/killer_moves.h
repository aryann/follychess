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

#ifndef FOLLYCHESS_SEARCH_KILLER_MOVES_H_
#define FOLLYCHESS_SEARCH_KILLER_MOVES_H_

#include "engine/move.h"

namespace follychess {

class KillerMoves {
 public:
  struct Entry {
    Move first = Move::NullMove();
    Move second = Move::NullMove();
  };

  void Set(const int ply, const Move move) {
    if (move.IsCapture()) {
      // Killer moves must be quiet.
      return;
    }

    Entry& entry = killer_moves_[ply];
    if (move == entry.first) {
      // Don't duplicate the move if it's already present.
      return;
    }

    entry.second = entry.first;
    entry.first = move;
  }

  const Entry& operator[](const int ply) const {
    DCHECK_GE(ply, 0);
    DCHECK_LT(ply, kMaxDepth);
    return killer_moves_[ply];
  }

 private:
  static constexpr int kMaxDepth = 128;
  std::array<Entry, kMaxDepth> killer_moves_;
};

}  // namespace follychess

#endif  // FOLLYCHESS_SEARCH_KILLER_MOVES_H_

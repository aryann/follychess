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

#ifndef FOLLYCHESS_PERFT_H_
#define FOLLYCHESS_PERFT_H_

#include <map>

#include "move.h"
#include "position.h"

namespace follychess {

void RunPerft(std::size_t depth, const Position &position,
              std::vector<std::size_t> &final_depth_counts,
              std::map<Move, std::size_t> &final_move_counts);

}  // namespace follychess

#endif  // FOLLYCHESS_PERFT_H_

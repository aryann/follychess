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

#include "cli.h"

#include "absl/strings/str_join.h"
#include "command.h"
#include "commands/display.h"
#include "commands/isready_command.h"
#include "commands/perft_command.h"
#include "commands/position_command.h"
#include "commands/uci_command.h"

namespace follychess {

CommandDispatcher MakeCommandDispatcher(CommandState& state) {
  Game& game = state.game;
  CommandDispatcher dispatcher;

  CommandDispatcher position_commands;
  position_commands.Add("fen", std::make_unique<FenPos>(game));
  position_commands.Add("startpos", std::make_unique<StartPos>(game));

  dispatcher.Add("position", std::move(position_commands));

  dispatcher.Add("perft", std::make_unique<PerftCommand>(state));

  dispatcher.Add("d", std::make_unique<Display>(state));
  dispatcher.Add("isready", std::make_unique<IsReady>(state));
  dispatcher.Add("uci", std::make_unique<Uci>(state));
  dispatcher.Add("ucinewgame", std::make_unique<UciNewGame>(state));
  dispatcher.Add("setoption", std::make_unique<SetOption>(state));
  dispatcher.Add("go", std::make_unique<Go>(state));

  return dispatcher;
}

}  // namespace follychess
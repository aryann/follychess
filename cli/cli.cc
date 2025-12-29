#include "cli.h"

#include "absl/strings/str_join.h"
#include "command.h"
#include "commands/display.h"
#include "commands/isready_command.h"
#include "commands/perft_command.h"
#include "commands/position_command.h"
#include "commands/uci_command.h"
#include "engine/move_generator.h"
#include "engine/position.h"

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
  dispatcher.Add("setoption", std::make_unique<SetOption>(state));
  dispatcher.Add("go", std::make_unique<Go>(state));
  dispatcher.Add("quit", std::make_unique<Quit>());

  return dispatcher;
}

}  // namespace follychess
#ifndef FOLLYCHESS_CLI_COMMANDS_DISPLAY_H_
#define FOLLYCHESS_CLI_COMMANDS_DISPLAY_H_

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "cli/command.h"
#include "engine/game.h"

namespace follychess {
class Position;

class Display : public Command {
 public:
  explicit Display(CommandState& state) : state_(state) {}

  std::expected<void, std::string> Run(
      std::vector<std::string_view> args) override {
    state_.printer.Println(std::cout, "{}", state_.game.GetPosition());
    return {};
  }

 private:
  CommandState& state_;
};

}  // namespace follychess

#endif  // FOLLYCHESS_CLI_COMMANDS_DISPLAY_H_

#ifndef FOLLYCHESS_CLI_COMMANDS_ISREADY_COMMAND_H_
#define FOLLYCHESS_CLI_COMMANDS_ISREADY_COMMAND_H_

#include <iostream>
#include <print>

#include "cli/command.h"

namespace follychess {

class IsReady : public Command {
 public:
  explicit IsReady(CommandState& state) : state_(state) {}

  std::expected<void, std::string> Run(
      std::vector<std::string_view> args) override {
    state_.printer.Println(std::cout, "readyok");
    return {};
  }

 private:
  CommandState& state_;
};

}  // namespace follychess

#endif  // FOLLYCHESS_CLI_COMMANDS_ISREADY_COMMAND_H_

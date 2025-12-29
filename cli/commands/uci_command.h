#ifndef FOLLYCHESS_CLI_COMMANDS_UCI_COMMAND_H_
#define FOLLYCHESS_CLI_COMMANDS_UCI_COMMAND_H_

#include <iostream>
#include <print>

#include "cli/command.h"
#include "cli/options.h"
#include "search/search.h"

namespace follychess {

class Uci : public Command {
 public:
  explicit Uci(CommandState& state) : state_(state) {}

  std::expected<void, std::string> Run(
      std::vector<std::string_view> args) override {
    state_.printer.Println(std::cout, "id name chessengine");
    state_.printer.Println(std::cout, "id author Aryan Naraghi");
    state_.printer.Println(std::cout);

    for (const Option* option : GetOptions()) {
      state_.printer.Println(std::cout, "option name {} {}", option->GetName(),
                             option->GetType());
    }
    state_.printer.Println(std::cout, "uciok");
    return {};
  }

 private:
  CommandState& state_;
};

class SetOption : public Command {
 public:
  explicit SetOption(CommandState& state) : state_(state) {}

  std::expected<void, std::string> Run(
      std::vector<std::string_view> args) override {
    if (args.size() != 4) {
      return std::unexpected(
          std::format("Invalid setoption command: {}", args));
    }

    if (args[0] != "name" || args[2] != "value") {
      return std::unexpected(
          std::format("Invalid setoption command: {}", args));
    }

    std::string_view name = args[1];
    std::string_view value = args[3];

    for (Option* option : GetOptions()) {
      if (option->GetName() == name) {
        return option->Set(value, state_);
      }
    }

    return std::unexpected(std::format("Invalid option: {}", name));
  }

 private:
  CommandState& state_;
};

class Go : public Command {
 public:
  explicit Go(CommandState& state) : state_(state) {}

  std::expected<void, std::string> Run(
      std::vector<std::string_view> args) override {
    constexpr static int kDefaultSearchDepth = 6;
    int depth = kDefaultSearchDepth;

    if (!args.empty()) {
      if (args.front() == "depth") {
        if (args.size() != 2) {
          return std::unexpected(std::format("Invalid go command: {}", args));
        }

        depth = std::stoi(std::string(args.back()));
      }
    }

    Move move = Search(state_.game,
                       SearchOptions()       //
                           .SetDepth(depth)  //
                           .SetLogEveryN(1 << 10)
                           .SetLogger([&](std::string_view value) {
                             state_.printer.Println(std::cout, "{}", value);
                           }));

    state_.printer.Println(std::cout, "bestmove {}", move);
    return {};
  }

 private:
  CommandState& state_;
};

}  // namespace follychess

#endif  // FOLLYCHESS_CLI_COMMANDS_UCI_COMMAND_H_

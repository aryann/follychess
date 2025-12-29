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
  std::expected<void, std::string> Run(
      std::vector<std::string_view> args) override {
    std::println(std::cout, "id name chessengine");
    std::println(std::cout, "id author Aryan Naraghi");
    std::println(std::cout);

    for (const Option* option : GetOptions()) {
      std::println(std::cout, "option name {} {}", option->GetName(),
                   option->GetType());
    }
    std::println(std::cout, "uciok");
    return {};
  }
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

class Quit : public Command {
  std::expected<void, std::string> Run(
      std::vector<std::string_view> args) override {
    std::exit(0);
  }
};

class Go : public Command {
 public:
  explicit Go(Game& game) : game_(game) {}

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

    Move move =
        Search(game_, SearchOptions().SetDepth(depth).SetLogEveryN(1 << 10));
    std::println(std::cout, "bestmove {}", move);
    return {};
  }

 private:
  Game& game_;
};

}  // namespace follychess

#endif  // FOLLYCHESS_CLI_COMMANDS_UCI_COMMAND_H_

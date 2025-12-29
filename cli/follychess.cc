#include <iostream>
#include <string_view>

#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "cli.h"

[[noreturn]] int main(int argc, char **argv) {
  std::println(R"(
        ,,
      ,/  \
     / 'O  |
    (   < _|
     \___/
     /   \
    /_____\
   /_______\
   (________)

   FollyChess
)");

  using ::follychess::Command;
  using ::follychess::CommandDispatcher;
  using ::follychess::CommandState;
  using ::follychess::MakeCommandDispatcher;

  CommandState state;
  CommandDispatcher dispatcher = MakeCommandDispatcher(state);

  std::string command;
  while (true) {
    std::getline(std::cin, command);
    state.printer.PrintStdIn(command);

    std::vector<std::string_view> parts = absl::StrSplit(
        command, absl::ByAsciiWhitespace(), absl::SkipWhitespace());

    auto error = dispatcher.Run(parts);
    if (!error.has_value()) {
      state.printer.Println(std::cerr, "{}", error.error());
    }
  }
}

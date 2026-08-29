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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace follychess {
namespace {

using ::testing::Eq;
using ::testing::HasSubstr;
using ::testing::IsEmpty;
using ::testing::StartsWith;

std::size_t CountLeadingSpaces(std::string_view input) {
  std::size_t count = 0;
  for (char c : input) {
    if (!std::isspace(c)) {
      break;
    }
    ++count;
  }
  return count;
}

std::string Dedent(std::string_view input) {
  auto lines = std::views::split(input, '\n') |
               std::ranges::to<std::vector<std::string>>();

  // Remove the first line since it doesn't contain any part of the expected
  // output.
  lines.erase(lines.begin());

  std::size_t leading_spaces = std::numeric_limits<std::size_t>::max();
  for (std::string_view line : lines) {
    if (line.empty()) {
      continue;
    }
    leading_spaces = std::min(leading_spaces, CountLeadingSpaces(line));
  }

  if (leading_spaces == std::numeric_limits<std::size_t>::max()) {
    return std::string(input);
  }

  std::string output;
  for (std::string_view line : lines) {
    if (line.empty()) {
      output.append(line);
    } else {
      output.append(line.substr(leading_spaces, line.size()));
    }
    output.append(1, '\n');
  }
  return output;
}

class CliTest : public ::testing::Test {
 protected:
  CliTest()
      : command_dispatcher_(MakeCommandDispatcher(state_)),
        old_stdout_buffer_((std::cout.rdbuf())),
        old_stderr_buffer_((std::cerr.rdbuf())) {
    std::cout.rdbuf(stream_.rdbuf());
    std::cerr.rdbuf(error_stream_.rdbuf());
  }

  ~CliTest() override {
    std::cout.rdbuf(old_stdout_buffer_);
    std::cerr.rdbuf(old_stderr_buffer_);
  }

  std::string GetOutput() {
    std::string val = stream_.str();
    stream_.str("");
    return val;
  }

  std::string GetErrorOutput() {
    std::string val = error_stream_.str();
    error_stream_.str("");
    return val;
  }

  std::expected<void, std::string> Run(
      const std::vector<std::string_view>& command) {
    return command_dispatcher_.Run(command);
  }

  CommandState state_;
  CommandDispatcher command_dispatcher_;

  std::stringstream stream_;
  std::stringstream error_stream_;
  std::streambuf* old_stdout_buffer_;
  std::streambuf* old_stderr_buffer_;
};

TEST_F(CliTest, Uci) {
  ASSERT_THAT(Run({"uci"}).error_or(""), IsEmpty());

  EXPECT_THAT(GetOutput(), Eq(Dedent(R"(
    id name FollyChess
    id author Aryan Naraghi

    option name LogDirectory type string default <empty>
    uciok)")));
}

TEST_F(CliTest, UciNewGame) {
  ASSERT_THAT(Run({"position", "startpos", "moves", "d2d4"}).error_or(""),
              IsEmpty());
  ASSERT_THAT(Run({"d"}).error_or(""), IsEmpty());
  EXPECT_THAT(GetOutput(), HasSubstr("b KQkq d3 0 1"));

  ASSERT_THAT(Run({"ucinewgame"}).error_or(""), IsEmpty());
  ASSERT_THAT(Run({"d"}).error_or(""), IsEmpty());
  EXPECT_THAT(GetOutput(), HasSubstr("w KQkq - 0 1"));
}

TEST_F(CliTest, Display) {
  ASSERT_THAT(Run({"d"}).error_or(""), IsEmpty());

  EXPECT_THAT(GetOutput(), Eq(Dedent(R"(
    8: r n b q k b n r
    7: p p p p p p p p
    6: . . . . . . . .
    5: . . . . . . . .
    4: . . . . . . . .
    3: . . . . . . . .
    2: P P P P P P P P
    1: R N B Q K B N R
       a b c d e f g h

       w KQkq - 0 1
    )")));
}

TEST_F(CliTest, IsReady) {
  ASSERT_THAT(Run({"isready"}).error_or(""), IsEmpty());

  EXPECT_THAT(GetOutput(), Eq("readyok\n"));
}

TEST_F(CliTest, Go) {
  ASSERT_THAT(Run({"go", "depth", "5"}).error_or(""), IsEmpty());
  EXPECT_THAT(GetOutput(),
              AllOf(                               //
                  HasSubstr("info depth 1"),       //
                  HasSubstr("info depth 2"),       //
                  HasSubstr("info depth 3"),       //
                  HasSubstr("info depth 4"),       //
                  HasSubstr("info depth 5"),       //
                  Not(HasSubstr("info depth 6")),  //
                  HasSubstr("bestmove d2d4")));

  ASSERT_THAT(
      Run({"go", "wtime", "1000", "btime", "1000", "depth", "2"}).error_or(""),
      IsEmpty());
  EXPECT_THAT(GetOutput(), AllOf(                               //
                               HasSubstr("info depth 1"),       //
                               HasSubstr("info depth 2"),       //
                               Not(HasSubstr("info depth 3")),  //
                               HasSubstr("bestmove e2e4")));
}

TEST_F(CliTest, GoNodes) {
  ASSERT_THAT(Run({"go", "nodes", "10000"}).error_or(""), IsEmpty());

  EXPECT_THAT(GetOutput(), AllOf(                          //
                               HasSubstr("info depth 1"),  //
                               HasSubstr("bestmove")));
}

TEST_F(CliTest, GoMovetime) {
  ASSERT_THAT(Run({"go", "movetime", "100"}).error_or(""), IsEmpty());

  EXPECT_THAT(GetOutput(), AllOf(                          //
                               HasSubstr("info depth 1"),  //
                               HasSubstr("bestmove")));
}

TEST_F(CliTest, GoNoArguments) {
  ASSERT_THAT(Run({"go"}).error_or(""), IsEmpty());

  EXPECT_THAT(GetOutput(), AllOf(                          //
                               HasSubstr("info depth 6"),  //
                               HasSubstr("bestmove")));
}

TEST_F(CliTest, GoRejectsMalformedValues) {
  EXPECT_THAT(Run({"go", "depth", "abc"}).error_or(""),
              Eq("Invalid value for go depth: abc"));
  EXPECT_THAT(Run({"go", "depth", "5x"}).error_or(""),
              Eq("Invalid value for go depth: 5x"));
  EXPECT_THAT(Run({"go", "movetime", "-100"}).error_or(""),
              Eq("Invalid value for go movetime: -100"));
  EXPECT_THAT(Run({"go", "nodes", "0"}).error_or(""),
              Eq("Invalid value for go nodes: 0"));
  EXPECT_THAT(Run({"go", "nodes", "99999999999999999999"}).error_or(""),
              Eq("Invalid value for go nodes: 99999999999999999999"));
  EXPECT_THAT(Run({"go", "depth"}).error_or(""),
              Eq("Missing value for go depth"));

  // No search ran, so nothing was written to standard output.
  EXPECT_THAT(GetOutput(), IsEmpty());
}

TEST_F(CliTest, GoClampsExcessiveDepth) {
  // The depth is clamped to kMaxSearchDepth; the node limit keeps the test
  // fast.
  ASSERT_THAT(Run({"go", "depth", "1000", "nodes", "2000"}).error_or(""),
              IsEmpty());

  EXPECT_THAT(GetOutput(), HasSubstr("bestmove"));
}

TEST_F(CliTest, StopIsANoOp) {
  ASSERT_THAT(Run({"stop"}).error_or(""), IsEmpty());

  EXPECT_THAT(GetOutput(), IsEmpty());
}

TEST_F(CliTest, Bench) {
  ASSERT_THAT(Run({"bench", "2"}).error_or(""), IsEmpty());

  // The regular search output goes to stdout.
  EXPECT_THAT(GetOutput(), AllOf(                          //
                               HasSubstr("info depth 1"),  //
                               HasSubstr("info depth 2"),  //
                               HasSubstr("bestmove")));

  // The position headers and the summary block go to stderr.
  EXPECT_THAT(GetErrorOutput(),
              AllOf(                                          //
                  HasSubstr("Position: 1/46 ("),              //
                  HasSubstr("Position: 46/46 ("),             //
                  HasSubstr("==========================="),   //
                  HasSubstr("Total time (ms) : "),            //
                  HasSubstr("Nodes searched  : "),            //
                  HasSubstr("Nodes/second    : ")));
}

TEST_F(CliTest, BenchRejectsMalformedDepth) {
  EXPECT_THAT(Run({"bench", "abc"}).error_or(""),
              Eq("Invalid value for bench depth: abc"));
  EXPECT_THAT(Run({"bench", "0"}).error_or(""),
              Eq("Invalid value for bench depth: 0"));

  EXPECT_THAT(GetOutput(), IsEmpty());
}

TEST_F(CliTest, GoIgnoresUnknownTokens) {
  ASSERT_THAT(Run({"go", "ponder", "movestogo", "40", "depth", "2"}).error_or(""),
              IsEmpty());

  EXPECT_THAT(GetOutput(), AllOf(                               //
                               HasSubstr("info depth 2"),       //
                               Not(HasSubstr("info depth 3")),  //
                               HasSubstr("bestmove")));
}

}  // namespace
}  // namespace follychess
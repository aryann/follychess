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
        old_stdout_buffer_((std::cout.rdbuf())) {
    std::cout.rdbuf(stream_.rdbuf());
  }

  ~CliTest() override { std::cout.rdbuf(old_stdout_buffer_); }

  std::string GetOutput() const { return stream_.str(); }

  std::expected<void, std::string> Run(
      const std::vector<std::string_view>& command) {
    return command_dispatcher_.Run(command);
  }

  CommandState state_;
  CommandDispatcher command_dispatcher_;

  std::stringstream stream_;
  std::streambuf* old_stdout_buffer_;
};

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

  EXPECT_THAT(GetOutput(), HasSubstr("bestmove d2d4"));
}

}  // namespace
}  // namespace follychess
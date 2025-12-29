#include "options.h"

namespace follychess {
namespace {

class LogDirectory : public Option {
 public:
  [[nodiscard]] std::string_view GetName() const override {
    return "LogDirectory";
  }

  [[nodiscard]] std::string_view GetType() const override {
    return "type string default <empty>";
  }

  void Set(std::string_view value, CommandState& game_state) override {}
};

}  // namespace

std::vector<const Option*> GetOptions() {
  static LogDirectory kLogDirectory;

  return {
      &kLogDirectory,
  };
}

}  // namespace follychess
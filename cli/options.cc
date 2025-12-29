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

  std::expected<void, std::string> Set(std::string_view value,
                                       CommandState& state) override {
    return state.printer.SetLogFile(std::string(value));
  }
};

}  // namespace

std::vector<Option*> GetOptions() {
  static LogDirectory kLogDirectory;

  return {
      &kLogDirectory,
  };
}

}  // namespace follychess
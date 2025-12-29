#include "options.h"

#include <chrono>

namespace follychess {
namespace {

namespace fs = std::filesystem;

class LogDirectory : public Option {
 public:
  [[nodiscard]] std::string_view GetName() const override {
    return "LogDirectory";
  }

  [[nodiscard]] std::string_view GetType() const override {
    return "type string default <empty>";
  }

  std::expected<void, std::string> Set(std::string_view log_directory,
                                       CommandState& state) override {
    auto now = std::chrono::system_clock::now();
    const std::string iso_time_utc = std::format("{:%Y-%m-%dT%H:%M:%SZ}", now);
    const fs::path path = fs::path(log_directory) / fs::path(iso_time_utc);

    return state.printer.SetLogFile(path);
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

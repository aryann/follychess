#ifndef FOLLYCHESS_CLI_OPTIONS_H_
#define FOLLYCHESS_CLI_OPTIONS_H_

#include <string_view>
#include <vector>

#include "command.h"

namespace follychess {

class Option {
 public:
  virtual ~Option() = default;

  [[nodiscard]] virtual std::string_view GetName() const = 0;

  [[nodiscard]] virtual std::string_view GetType() const = 0;

  virtual std::expected<void, std::string> Set(std::string_view value,
                                               CommandState& state) = 0;
};

std::vector<Option*> GetOptions();

}  // namespace follychess

#endif  // FOLLYCHESS_CLI_OPTIONS_H_

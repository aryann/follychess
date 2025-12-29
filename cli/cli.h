#ifndef FOLLYCHESS_CLI_CLI_H_
#define FOLLYCHESS_CLI_CLI_H_

#include "command.h"

namespace follychess {

CommandDispatcher MakeCommandDispatcher(CommandState& state);

}  // namespace follychess

#endif  // FOLLYCHESS_CLI_CLI_H_

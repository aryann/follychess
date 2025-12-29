#ifndef FOLLYCHESS_SEARCH_SEARCH_H_
#define FOLLYCHESS_SEARCH_SEARCH_H_

#include "engine/game.h"
#include "engine/move.h"
#include "engine/position.h"

namespace follychess {

struct SearchOptions {
  SearchOptions& SetDepth(int depth) {
    this->depth = depth;
    return *this;
  }

  int depth = 5;

  SearchOptions& SetLogEveryN(std::int64_t log_every_n) {
    this->log_every_n = log_every_n;
    return *this;
  }

  std::int64_t log_every_n = std::numeric_limits<std::int64_t>::max();

  SearchOptions& SetLogger(std::function<void(std::string_view)> logger) {
    this->logger = logger;
    return *this;
  }

  std::function<void(std::string_view)> logger = [](std::string_view) {};
};

Move Search(const Game& game, const SearchOptions& options = SearchOptions());

}  // namespace follychess

#endif  // FOLLYCHESS_SEARCH_SEARCH_H_

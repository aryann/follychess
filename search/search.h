#ifndef FOLLYCHESS_SEARCH_SEARCH_H_
#define FOLLYCHESS_SEARCH_SEARCH_H_

#include "engine/game.h"
#include "engine/move.h"
#include "engine/position.h"

namespace follychess {

struct SearchOptions {
  SearchOptions& SetDepth(int value) {
    depth = value;
    return *this;
  }

  int depth = 5;

  SearchOptions& SetLogger(const std::function<void(std::string_view)>& value) {
    logger = value;
    return *this;
  }

  std::function<void(std::string_view)> logger = [](std::string_view) {};
};

Move Search(const Game& game, const SearchOptions& options = SearchOptions());

}  // namespace follychess

#endif  // FOLLYCHESS_SEARCH_SEARCH_H_

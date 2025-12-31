#include "engine/game.h"

namespace follychess {

[[nodiscard]] int Game::GetRepetitionCount() const {
  int repetitions = 0;
  const std::uint64_t current_key = position_.GetKey();

  const int start = std::ssize(history_) - 2;
  const int limit = std::ssize(history_) - 1 - position_.GetHalfMoves();

  for (int i = start; i >= limit; --i) {
    if (history_[i].key == current_key) {
      ++repetitions;
    }
  }

  return repetitions;
}

}  // namespace follychess

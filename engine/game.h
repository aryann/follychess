#ifndef FOLLYCHESS_ENGINE_GAME_H_
#define FOLLYCHESS_ENGINE_GAME_H_

#include "engine/position.h"

namespace follychess {

class Game {
 public:
  explicit Game(const Position& position) : position_(position) {}

  Game() : position_(Position::Starting()) {}

  void Do(Move move) {
    history_.push_back({
        .key = position_.GetKey(),
        .undo_info = position_.Do(move),
    });
  }

  void Undo() {
    DCHECK(!history_.empty());
    position_.Undo(history_.back().undo_info);
    history_.pop_back();
  }

  [[nodiscard]] int GetRepetitionCount() const;

  [[nodiscard]] const Position& GetPosition() const { return position_; }

 private:
  struct State {
    std::uint64_t key{0};
    UndoInfo undo_info;
  };

  Position position_;
  std::vector<State> history_;
};

}  // namespace follychess

#endif  // FOLLYCHESS_ENGINE_GAME_H_

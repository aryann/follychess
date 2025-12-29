#include "search/search.h"

#include <iostream>
#include <vector>

#include "engine/move.h"
#include "engine/move_generator.h"
#include "engine/position.h"
#include "engine/scoped_move.h"
#include "engine/types.h"
#include "search/evaluation.h"
#include "search/move_ordering.h"
#include "transposition.h"

namespace follychess {
namespace {

class AlphaBetaSearcher {
 public:
  AlphaBetaSearcher(const Game& game, const SearchOptions& options)
      : game_{game},
        position_{game_.GetPosition()},
        requested_search_depth_{options.depth},
        log_every_n_{options.log_every_n},
        logger_{options.logger},
        nodes_{0},
        transpositions_{position_} {}

  [[nodiscard]] Move GetBestMove() {
    if (best_move_) {
      return *best_move_;
    }

    start_time_ = std::chrono::system_clock::now();

    constexpr static int kAlpha = -100'000;
    constexpr static int kBeta = 100'000;
    Search(kAlpha, kBeta, 0);
    DCHECK(best_move_.has_value());

    return *best_move_;
  }

 private:
  // NOLINTNEXTLINE(misc-no-recursion)
  int Search(int alpha, const int beta, const int depth) {
    using enum TranspositionTable::BoundType;

    ++nodes_;
    MaybeLog(depth);

    if (game_.GetRepetitionCount() >= 3) {
      // TODO(aryann): Figure out what when draws can be offered due to the
      // three repetition rule.
      return 0;
    }

    if (std::optional<int> score = transpositions_.Probe(alpha, beta, depth)) {
      return *score;
    }

    if (depth == requested_search_depth_) {
      int score = QuiescentSearch(alpha, beta, 1);
      transpositions_.Record(score, depth, Exact);
      return score;
    }

    bool has_legal_moves = false;
    std::vector<Move> moves = GenerateMoves(position_);
    OrderMoves(position_, moves);

    TranspositionTable::BoundType transposition_type = UpperBound;
    for (Move move : moves) {
      ScopedMove2 scoped_move(move, game_);
      if (!IsLastMoveLegal()) {
        continue;
      }
      has_legal_moves = true;

      const int score = -Search(-beta, -alpha, depth + 1);

      if (score >= beta) {
        transpositions_.Record(score, depth, LowerBound);

        return beta;
      }

      if (score > alpha) {
        alpha = score;
        transposition_type = Exact;
        if (depth == 0) {
          // Store this move as the best move if and only if this is a root
          // node.
          best_move_ = move;
        }
      }
    }

    if (has_legal_moves) {
      transpositions_.Record(alpha, depth, transposition_type);
      return alpha;
    }

    if (CurrentSideInCheck()) {
      constexpr int kBaseCheckMateScore = -20'000;

      // Favor checkmates closer to the root of the tree.
      return kBaseCheckMateScore + depth;
    }

    constexpr int kStalemateScore = 0;
    return kStalemateScore;
  }

  // NOLINTNEXTLINE(misc-no-recursion)
  [[nodiscard]] int QuiescentSearch(int alpha, const int beta,
                                    const int depth) {
    ++nodes_;
    MaybeLog(requested_search_depth_, depth);

    int score = GetScore();
    if (score >= beta) {
      return beta;
    }
    alpha = std::max(alpha, score);

    std::vector<Move> moves = GenerateMoves<kCapture>(position_);
    OrderMoves(position_, moves);
    for (Move move : moves) {
      ScopedMove2 scoped_move(move, game_);
      const bool is_legal = !position_.GetCheckers(~position_.SideToMove());
      if (!is_legal) {
        continue;
      }

      score = -QuiescentSearch(-beta, -alpha, depth + 1);

      if (score >= beta) {
        return beta;
      }
      alpha = std::max(alpha, score);
    }

    return alpha;
  }

  [[nodiscard]] int GetScore() const {
    const int score = Evaluate(position_);
    return position_.SideToMove() == kWhite ? score : -score;
  }

  [[nodiscard]] constexpr bool IsLastMoveLegal() const {
    return !position_.GetCheckers(~position_.SideToMove());
  }

  [[nodiscard]] constexpr bool CurrentSideInCheck() const {
    return position_.GetCheckers(position_.SideToMove());
  }

  constexpr void MaybeLog(const int depth,
                          const int additional_depth = 0) const {
    if (nodes_ % log_every_n_ != 0) {
      return;
    }

    const auto now = std::chrono::system_clock::now();
    const std::chrono::duration<double> elapsed = now - start_time_;
    const double elapsed_seconds = elapsed.count();
    auto nodes_per_second = static_cast<std::int64_t>(nodes_ / elapsed_seconds);

    const int selective_depth = depth + additional_depth;

    logger_(std::format("info depth {} seldepth {} nodes {} nps {} tbhits {}",
                        depth, selective_depth, nodes_, nodes_per_second,
                        transpositions_.GetHits()));
  }

  Game game_;
  const Position& position_;

  const int requested_search_depth_;
  const std::int64_t log_every_n_;
  const std::function<void(std::string_view)> logger_;

  std::optional<Move> best_move_;

  std::chrono::system_clock::time_point start_time_;
  std::int64_t nodes_;

  TranspositionTable transpositions_;
};

}  // namespace

Move Search(const Game& game, const SearchOptions& options) {
  AlphaBetaSearcher searcher(game, options);
  return searcher.GetBestMove();
}

}  // namespace follychess

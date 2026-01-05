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
#include "search/phase.h"
#include "search/principal_variation.h"
#include "search/transposition.h"

namespace follychess {
namespace {

class AlphaBetaSearcher {
 public:
  AlphaBetaSearcher(const Game& game, const SearchOptions& options)
      : game_{game},
        position_{game_.GetPosition()},
        max_depth_{options.depth},
        logger_{options.logger},
        nodes_{0},
        transpositions_{position_} {}

  [[nodiscard]] Move GetBestMove() {
    start_time_ = std::chrono::system_clock::now();

    for (int depth = 1; depth <= max_depth_; ++depth) {
      constexpr static int kAlpha = -100'000;
      constexpr static int kBeta = 100'000;
      Search(kAlpha, kBeta, 0, depth);
      Log(depth);
    }

    DCHECK_NE(pv_table_.GetBestMove(), Move::NullMove());
    return pv_table_.GetBestMove();
  }

 private:
  // NOLINTNEXTLINE(misc-no-recursion)
  int Search(int alpha, const int beta, const int depth, const int max_depth) {
    using enum TranspositionTable::BoundType;

    pv_table_.RecordMove(depth, Move::NullMove());
    ++nodes_;

    const int remaining_depth = max_depth_ - depth;
    if (std::optional<int> score =
            transpositions_.Probe(alpha, beta, remaining_depth)) {
      return *score;
    }

    if (depth == max_depth) {
      int score = QuiescentSearch(alpha, beta, depth);
      transpositions_.Record(score, depth, Exact);
      return score;
    }

    std::vector<Move> moves = GenerateLegalMoves(position_);
    OrderMoves(position_, pv_table_.GetBestMove(), moves);

    TranspositionTable::BoundType transposition_type = UpperBound;
    for (Move move : moves) {
      ScopedMove2 scoped_move(move, game_);
      const int score = -Search(-beta, -alpha, depth + 1, max_depth);

      if (score >= beta) {
        transpositions_.Record(score, depth, LowerBound);
        return beta;
      }

      if (score > alpha) {
        alpha = score;
        transposition_type = Exact;
        pv_table_.RecordMove(depth, move);
      }
    }

    if (game_.GetRepetitionCount() >= 3) {
      // A draw can be claimed due to the threefold repetition rule.
      return 0;
    }

    if (!moves.empty()) {
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
    pv_table_.RecordMove(depth, Move::NullMove());

    int score = GetScore();
    if (score >= beta) {
      return beta;
    }
    alpha = std::max(alpha, score);

    std::vector<Move> moves = GenerateLegalMoves<kCapture>(position_);
    OrderMoves(position_, pv_table_.GetBestMove(), moves);
    for (Move move : moves) {
      ScopedMove2 scoped_move(move, game_);
      score = -QuiescentSearch(-beta, -alpha, depth + 1);

      if (score >= beta) {
        return beta;
      }

      if (score > alpha) {
        alpha = score;
        pv_table_.RecordMove(depth, move);
      }
    }

    return alpha;
  }

  [[nodiscard]] int GetScore() const {
    const int score = Evaluate(position_, CalculatePhase(position_));
    return position_.SideToMove() == kWhite ? score : -score;
  }

  [[nodiscard]] constexpr bool CurrentSideInCheck() const {
    return position_.GetCheckers(position_.SideToMove());
  }

  constexpr void Log(const int depth, const int additional_depth = 0) const {
    const auto now = std::chrono::system_clock::now();
    const std::chrono::duration<double> elapsed = now - start_time_;
    const double elapsed_seconds = elapsed.count();
    auto nodes_per_second = static_cast<std::int64_t>(nodes_ / elapsed_seconds);

    const int selective_depth = depth + additional_depth;

    logger_(
        std::format("info depth {} seldepth {} nodes {} nps {} tbhits {} pv {}",
                    depth, selective_depth, nodes_, nodes_per_second,
                    transpositions_.GetHits(), pv_table_));
  }

  Game game_;
  const Position& position_;

  const int max_depth_;
  const std::function<void(std::string_view)> logger_;

  PrincipalVariationTable pv_table_;

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

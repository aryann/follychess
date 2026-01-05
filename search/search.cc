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

struct SearchContext {
  Game game;

  const std::function<void(std::string_view)> logger;
  std::chrono::system_clock::time_point start_time;
  int max_depth;

  PrincipalVariationTable pv_table;
  TranspositionTable transpositions;
};

class AlphaBetaSearcher {
 public:
  explicit AlphaBetaSearcher(SearchContext& context)
      : context_(context), position_(context.game.GetPosition()), nodes_(0) {}

  [[nodiscard]] Move Search(int depth) {
    constexpr static int kAlpha = -100'000;
    constexpr static int kBeta = 100'000;
    Search(kAlpha, kBeta, 0, depth);
    Log(depth);

    DCHECK_NE(context_.pv_table.GetBestMove(), Move::NullMove());
    return context_.pv_table.GetBestMove();
  }

 private:
  // NOLINTNEXTLINE(misc-no-recursion)
  int Search(int alpha, const int beta, const int depth, const int max_depth) {
    using enum TranspositionTable::BoundType;

    context_.pv_table.RecordMove(depth, Move::NullMove());
    ++nodes_;

    const int remaining_depth = context_.max_depth - depth;
    if (std::optional<int> score = context_.transpositions.Probe(
            position_, alpha, beta, remaining_depth)) {
      return *score;
    }

    if (depth == max_depth) {
      int score = QuiescentSearch(alpha, beta, depth);
      context_.transpositions.Record(position_, score, depth, Exact);
      return score;
    }

    std::vector<Move> moves = GenerateLegalMoves(position_);
    OrderMoves(position_, context_.pv_table.GetBestMove(), moves);

    TranspositionTable::BoundType transposition_type = UpperBound;
    for (Move move : moves) {
      ScopedMove2 scoped_move(move, context_.game);
      const int score = -Search(-beta, -alpha, depth + 1, max_depth);

      if (score >= beta) {
        context_.transpositions.Record(position_, score, depth, LowerBound);
        return beta;
      }

      if (score > alpha) {
        alpha = score;
        transposition_type = Exact;
        context_.pv_table.RecordMove(depth, move);
      }
    }

    if (context_.game.GetRepetitionCount() >= 3) {
      // A draw can be claimed due to the threefold repetition rule.
      return 0;
    }

    if (!moves.empty()) {
      context_.transpositions.Record(position_, alpha, depth,
                                     transposition_type);
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
    context_.pv_table.RecordMove(depth, Move::NullMove());

    int score = GetScore();
    if (score >= beta) {
      return beta;
    }
    alpha = std::max(alpha, score);

    std::vector<Move> moves = GenerateLegalMoves<kCapture>(position_);
    OrderMoves(position_, context_.pv_table.GetBestMove(), moves);
    for (Move move : moves) {
      ScopedMove2 scoped_move(move, context_.game);
      score = -QuiescentSearch(-beta, -alpha, depth + 1);

      if (score >= beta) {
        return beta;
      }

      if (score > alpha) {
        alpha = score;
        context_.pv_table.RecordMove(depth, move);
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
    const std::chrono::duration<double> elapsed = now - context_.start_time;
    const double elapsed_seconds = elapsed.count();
    auto nodes_per_second = static_cast<std::int64_t>(nodes_ / elapsed_seconds);

    const int selective_depth = depth + additional_depth;

    context_.logger(
        std::format("info depth {} seldepth {} nodes {} nps {} tbhits {} pv {}",
                    depth, selective_depth, nodes_, nodes_per_second,
                    context_.transpositions.GetHits(), context_.pv_table));
  }

  SearchContext& context_;
  const Position& position_;
  std::int64_t nodes_;
};

}  // namespace

Move Search(const Game& game, const SearchOptions& options) {
  SearchContext context = {
      .game = game,
      .logger = options.logger,
      .start_time = std::chrono::system_clock::now(),
      .max_depth = options.depth,
      .pv_table = PrincipalVariationTable(),
      .transpositions = TranspositionTable(),
  };

  AlphaBetaSearcher searcher(context);
  Move best_move = Move::NullMove();
  for (int depth = 1; depth <= context.max_depth; ++depth) {
    best_move = searcher.Search(depth);
  }

  return best_move;
}

}  // namespace follychess

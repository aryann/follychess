// FollyChess is a UCI-compatible chess engine written in C++23.
//
// Copyright (C) 2025-2026 Aryan Naraghi <aryan.naraghi@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

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

  PrincipalVariationTable pv_table;
  TranspositionTable transpositions;
};

class AlphaBetaSearcher {
 public:
  explicit AlphaBetaSearcher(SearchContext& context)
      : context_(context), game_(context.game), nodes_(0) {}

  [[nodiscard]] Move Search(int depth) {
    constexpr static int kAlpha = -100'000;
    constexpr static int kBeta = 100'000;
    const int score = Search(kAlpha, kBeta, 0, depth);
    Log(score, depth);

    Move best_move = context_.pv_table.GetBestMove();
    if (best_move == Move::NullMove()) {
      // If the Principal Variation table is empty, then it is likely due to a
      // root transposition table cutoff. In this case, we check the
      // transposition table for the best move.
      context_.transpositions.Probe(game_.GetPosition(),
                                    {
                                        .alpha = kAlpha,
                                        .beta = kBeta,
                                        .depth = depth,
                                        .remaining_depth = 0,
                                    },
                                    &best_move);
    }

    DCHECK_NE(best_move, Move::NullMove());
    return best_move;
  }

 private:
  // NOLINTNEXTLINE(misc-no-recursion)
  [[nodiscard]] int Search(int alpha, const int beta, const int depth,
                           const int max_depth) {
    using enum TranspositionTable::BoundType;

    context_.pv_table.RecordMove(depth, Move::NullMove());
    ++nodes_;

    const int remaining_depth = max_depth - depth;
    Move best_move;
    if (std::optional<int> score = context_.transpositions.Probe(
            game_.GetPosition(),
            {
                .alpha = alpha,
                .beta = beta,
                .depth = depth,
                .remaining_depth = remaining_depth,
            },
            &best_move)) {
      return *score;
    }

    if (depth >= max_depth && !CurrentSideInCheck()) {
      const int score = QuiescentSearch(alpha, beta, depth);
      context_.transpositions.Record(game_.GetPosition(), score,
                                     {
                                         .depth = depth,
                                         .remaining_depth = remaining_depth,
                                     },
                                     Exact, Move::NullMove());
      return score;
    }

    if (NullPrune(depth, max_depth)) {
      ScopedMove2 scoped_move(Move::NullMove(), context_.game);
      constexpr int kDepthReduction = 2;
      const int score =
          -Search(-beta, -beta + 1, depth + 1 + kDepthReduction, max_depth);
      if (score >= beta) {
        return beta;
      }
    }

    std::vector<Move> moves = GenerateLegalMoves(game_.GetPosition());
    OrderMoves(game_.GetPosition(), best_move, moves);

    TranspositionTable::BoundType transposition_type = UpperBound;
    for (Move move : moves) {
      ScopedMove2 scoped_move(move, context_.game);
      const int score = -Search(-beta, -alpha, depth + 1, max_depth);

      if (score >= beta) {
        context_.transpositions.Record(game_.GetPosition(), score,
                                       {
                                           .depth = depth,
                                           .remaining_depth = remaining_depth,
                                       },
                                       LowerBound, move);
        return beta;
      }

      if (score > alpha) {
        alpha = score;
        transposition_type = Exact;
        context_.pv_table.RecordMove(depth, move);
        best_move = move;
      }
    }

    if (context_.game.GetRepetitionCount() >= 3) {
      // A draw can be claimed due to the threefold repetition rule.
      return 0;
    }

    if (!moves.empty()) {
      context_.transpositions.Record(game_.GetPosition(), alpha,
                                     {
                                         .depth = depth,
                                         .remaining_depth = remaining_depth,
                                     },
                                     transposition_type, best_move);
      return alpha;
    }

    if (CurrentSideInCheck()) {
      // Favor checkmates closer to the root of the tree.
      return -kBaseCheckMateScore + depth;
    }

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

    Move best_move;
    context_.transpositions.Probe(game_.GetPosition(),
                                  {
                                      .alpha = alpha,
                                      .beta = beta,
                                      .depth = depth,
                                      .remaining_depth = 0,
                                  },
                                  &best_move);

    std::vector<Move> moves = GenerateLegalMoves<kCapture>(game_.GetPosition());
    OrderMoves(game_.GetPosition(), best_move, moves);

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
    const int score =
        Evaluate(game_.GetPosition(), CalculatePhase(game_.GetPosition()));
    return game_.GetPosition().SideToMove() == kWhite ? score : -score;
  }

  [[nodiscard]] bool CurrentSideInCheck() const {
    return game_.GetPosition().GetCheckers(game_.GetPosition().SideToMove());
  }

  [[nodiscard]] bool NullPrune(const int depth, const int max_depth) const {
    const int remaining_depth = max_depth - depth;
    const Position& position = game_.GetPosition();
    const bool king_and_pawn_endgame =
        (position.GetPieces(kKing) | position.GetPieces(kPawn)) ==
        position.GetPieces();

    return
        // Don't prune the root.
        depth > 0
        // Don't prune too close to the leaf.
        && remaining_depth >= 2
        // Don't prune endgames that can result in Zugzwang.
        && !king_and_pawn_endgame
        // Don't prune if we're in check since it doesn't make sense.
        && !CurrentSideInCheck();
  }

  static std::string LogScore(const int score) {
    if (std::abs(score) > kCheckMateThreshold) {
      int plies = (kBaseCheckMateScore - std::abs(score));
      int moves = (plies + 1) / 2;
      if (score < 0) {
        moves *= -1;
      }
      return std::format("mate {}", moves);
    }

    return std::format("cp {}", score);
  }

  void Log(const int score, const int depth,
           const int additional_depth = 0) const {
    const auto now = std::chrono::system_clock::now();
    const std::chrono::duration<double> elapsed = now - context_.start_time;
    const double elapsed_seconds = elapsed.count();
    auto nodes_per_second = static_cast<std::int64_t>(nodes_ / elapsed_seconds);

    const int selective_depth = depth + additional_depth;

    context_.logger(std::format(
        "info depth {} seldepth {} score {} nodes {} nps {} tbhits {} pv {}",
        depth, selective_depth, LogScore(score), nodes_, nodes_per_second,
        context_.transpositions.GetHits(), context_.pv_table));
  }

  SearchContext& context_;
  Game& game_;
  std::int64_t nodes_;
};

}  // namespace

Move Search(const Game& game, const SearchOptions& options) {
  SearchContext context = {
      .game = game,
      .logger = options.logger,
      .start_time = std::chrono::system_clock::now(),
      .pv_table = PrincipalVariationTable(),
      .transpositions = TranspositionTable(),
  };

  AlphaBetaSearcher searcher(context);
  Move best_move = Move::NullMove();
  for (int depth = 1; depth <= options.depth; ++depth) {
    best_move = searcher.Search(depth);
  }

  return best_move;
}

}  // namespace follychess

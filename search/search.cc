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
#include "search/history_heuristic.h"
#include "search/killer_moves.h"
#include "search/move_ordering.h"
#include "search/phase.h"
#include "search/principal_variation.h"
#include "search/transposition.h"

namespace follychess {
namespace {

std::optional<int> GetMateIn(const int score) {
  if (std::abs(score) < kCheckMateThreshold) {
    return std::nullopt;
  }

  const int plies = (kBaseCheckMateScore - std::abs(score));
  int moves = (plies + 1) / 2;
  if (score < 0) {
    moves *= -1;
  }
  return moves;
}

struct SearchContext {
  Game game;

  std::function<void(const SearchInfo&)> info_observer;
  std::chrono::system_clock::time_point start_time;

  KillerMoves killer_moves;
  PrincipalVariationTable pv_table;
  TranspositionTable transpositions;
  HistoryHeuristic history_heuristic;
};

class AlphaBetaSearcher {
 public:
  explicit AlphaBetaSearcher(SearchContext& context)
      : context_(context), game_(context.game), nodes_(0) {}

  [[nodiscard]] Move Search(const int depth) {
    constexpr int kAlpha = -100'000;
    constexpr int kBeta = 100'000;
    constexpr int kStartPly = 0;
    const int score = Search(kAlpha, kBeta, depth, kStartPly);
    context_.info_observer(MakeSearchInfo(score, depth));

    Move best_move = context_.pv_table.GetBestMove();
    if (best_move == Move::NullMove()) {
      // If the Principal Variation table is empty, then it is likely due to a
      // root transposition table cutoff. In this case, we check the
      // transposition table for the best move.
      context_.transpositions.Probe(game_.GetPosition(),
                                    {
                                        .alpha = kAlpha,
                                        .beta = kBeta,
                                        .ply = kStartPly,
                                        .depth = depth,
                                    },
                                    &best_move);
    }

    DCHECK_NE(best_move, Move::NullMove());
    return best_move;
  }

 private:
  // The main search routine.
  //
  // `depth` signifies how much work is left. Once `depth` becomes zero,
  // quiescent search is entered. `ply` denotes how far we are from the root.
  //
  // NOLINTNEXTLINE(misc-no-recursion)
  [[nodiscard]] int Search(int alpha, const int beta, const int depth,
                           const int ply) {
    using enum TranspositionTable::BoundType;

    context_.pv_table.RecordMove(ply, Move::NullMove());
    ++nodes_;

    Move best_move;
    if (std::optional<int> score =
            context_.transpositions.Probe(game_.GetPosition(),
                                          {
                                              .alpha = alpha,
                                              .beta = beta,
                                              .ply = ply,
                                              .depth = depth,
                                          },
                                          &best_move)) {
      return *score;
    }

    if (depth <= 0 && !CurrentSideInCheck()) {
      const int score = QuiescentSearch(alpha, beta, ply);
      context_.transpositions.Record(game_.GetPosition(), score,
                                     {
                                         .ply = ply,
                                         .depth = depth,
                                     },
                                     Exact, Move::NullMove());
      return score;
    }

    if (NullPrune(depth, ply)) {
      ScopedMove2 scoped_move(Move::NullMove(), context_.game);
      constexpr int kDepthReduction = 2;
      const int next_depth = std::max(0, depth - 1 - kDepthReduction);
      const int score = -Search(-beta, -beta + 1, next_depth, ply + 1);
      if (score >= beta) {
        return beta;
      }
    }

    std::vector<Move> moves = GenerateLegalMoves(game_.GetPosition());
    OrderMoves(game_.GetPosition(), best_move, context_.killer_moves[ply],
               context_.history_heuristic, moves);

    TranspositionTable::BoundType transposition_type = UpperBound;
    for (Move move : moves) {
      ScopedMove2 scoped_move(move, context_.game);
      const int score = -Search(-beta, -alpha, depth - 1, ply + 1);

      if (score >= beta) {
        // Beta cutoff.
        context_.transpositions.Record(game_.GetPosition(), score,
                                       {
                                           .ply = ply,
                                           .depth = depth,
                                       },
                                       LowerBound, move);
        context_.killer_moves.Set(ply, move);
        context_.history_heuristic.Set(game_.GetPosition(), move, depth);
        return beta;
      }

      if (score > alpha) {
        alpha = score;
        transposition_type = Exact;
        context_.pv_table.RecordMove(ply, move);
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
                                         .ply = ply,
                                         .depth = depth,
                                     },
                                     transposition_type, best_move);
      return alpha;
    }

    if (CurrentSideInCheck()) {
      // Favor checkmates closer to the root of the tree.
      return -kBaseCheckMateScore + ply;
    }

    return kStalemateScore;
  }

  // NOLINTNEXTLINE(misc-no-recursion)
  [[nodiscard]] int QuiescentSearch(int alpha, const int beta, const int ply) {
    ++nodes_;
    context_.pv_table.RecordMove(ply, Move::NullMove());

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
                                      .ply = ply,
                                      .depth = 0,
                                  },
                                  &best_move);

    std::vector<Move> moves = GenerateLegalMoves<kCapture>(game_.GetPosition());
    OrderMoves(game_.GetPosition(), best_move, context_.killer_moves[ply],
               context_.history_heuristic, moves);

    for (Move move : moves) {
      ScopedMove2 scoped_move(move, context_.game);
      score = -QuiescentSearch(-beta, -alpha, ply + 1);

      if (score >= beta) {
        return beta;
      }

      if (score > alpha) {
        alpha = score;
        context_.pv_table.RecordMove(ply, move);
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
    Bitboard checkers =
        game_.GetPosition().GetCheckers(game_.GetPosition().SideToMove());
    return checkers != kEmptyBoard;
  }

  [[nodiscard]] bool NullPrune(const int depth, const int ply) const {
    const Position& position = game_.GetPosition();
    const bool king_and_pawn_endgame =
        (position.GetPieces(kKing) | position.GetPieces(kPawn)) ==
        position.GetPieces();

    return
        // Don't prune the root.
        ply > 0
        // Don't prune too close to the leaf.
        && depth >= 2
        // Don't prune endgames that can result in Zugzwang.
        && !king_and_pawn_endgame
        // Don't prune if we're in check since it doesn't make sense.
        && !CurrentSideInCheck();
  }

  [[nodiscard]] SearchInfo MakeSearchInfo(const int score,
                                          const int depth) const {
    const auto now = std::chrono::system_clock::now();
    const std::chrono::duration<double> elapsed = now - context_.start_time;
    const double elapsed_seconds = elapsed.count();

    return {
        .depth = depth,
        .score = score,
        .mate_in = GetMateIn(score),
        .nodes = nodes_,
        .node_per_second = static_cast<std::int64_t>(nodes_ / elapsed_seconds),
        .tbhits = context_.transpositions.GetHits(),
        .principal_variation = std::format("{}", context_.pv_table),
    };
  }

  SearchContext& context_;
  Game& game_;
  std::int64_t nodes_;
};

}  // namespace

Move Search(const Game& game, const SearchOptions& options) {
  SearchContext context = {
      .game = game,
      .info_observer = options.info_observer,
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

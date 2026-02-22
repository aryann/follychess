// FollyChess is a UCI-compatible chess engine written in C++23.
//
// Copyright (C) 2025-2026 Aryan Naraghi <aryan.naraghi@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "search/transposition.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "engine/testing.h"

namespace follychess {
namespace {

using ::testing::Eq;
using ::testing::Optional;
using enum TranspositionTable::BoundType;

TEST(TranspositionTable, TableSize) {
  EXPECT_THAT(TranspositionTable(1).size(), Eq(1 << 14));

  EXPECT_THAT(TranspositionTable(94).size(), Eq(1 << 20));
  EXPECT_THAT(TranspositionTable(95).size(), Eq(1 << 20));

  EXPECT_THAT(TranspositionTable(96).size(), Eq(1 << 21));
  EXPECT_THAT(TranspositionTable(97).size(), Eq(1 << 21));
  EXPECT_THAT(TranspositionTable(125).size(), Eq(1 << 21));
  EXPECT_THAT(TranspositionTable(126).size(), Eq(1 << 21));
  EXPECT_THAT(TranspositionTable(127).size(), Eq(1 << 21));
  EXPECT_THAT(TranspositionTable(128).size(), Eq(1 << 21));
  EXPECT_THAT(TranspositionTable(130).size(), Eq(1 << 21));

  EXPECT_THAT(TranspositionTable(256).size(), Eq(1 << 22));
}

TEST(TranspositionTable, EmptyTable) {
  TranspositionTable table;
  Move best_move;

  std::optional<int> score = table.Probe(
      ZobristKey(123), {.alpha = -100, .beta = 100, .ply = 1, .depth = 5},
      &best_move);

  EXPECT_THAT(score, Eq(std::nullopt));
  EXPECT_THAT(best_move, Eq(Move::NullMove()));
  EXPECT_THAT(table.GetHits(), Eq(0));
}

TEST(TranspositionTable, ExactScoreHit) {
  TranspositionTable table;
  Move best_move;

  table.Record(ZobristKey(123), 50, {.ply = 1, .depth = 5}, Exact,
               MakeMove("e2e4"));

  std::optional<int> score = table.Probe(
      ZobristKey(123), {.alpha = -100, .beta = 100, .ply = 1, .depth = 5},
      &best_move);

  EXPECT_THAT(score, Optional(50));
  EXPECT_THAT(best_move, Eq(MakeMove("e2e4")));
  EXPECT_THAT(table.GetHits(), Eq(1));
}

TEST(TranspositionTable, LowerBoundBetaCutoff) {
  TranspositionTable table;
  Move best_move;

  table.Record(ZobristKey(123), 50, {.ply = 1, .depth = 5}, LowerBound,
               MakeMove("d2d4"));

  std::optional<int> score_cutoff = table.Probe(
      ZobristKey(123), {.alpha = -100, .beta = 40, .ply = 1, .depth = 5},
      &best_move);
  EXPECT_THAT(score_cutoff, Optional(40));
  EXPECT_THAT(table.GetHits(), Eq(1));

  std::optional<int> score_no_cutoff = table.Probe(
      ZobristKey(123), {.alpha = -100, .beta = 60, .ply = 1, .depth = 5},
      &best_move);
  EXPECT_THAT(score_no_cutoff, Eq(std::nullopt));
}

TEST(TranspositionTable, UpperBoundAlphaCutoff) {
  TranspositionTable table;
  Move best_move;

  table.Record(ZobristKey(123), -50, {.ply = 1, .depth = 5}, UpperBound,
               MakeMove("g1f3"));

  std::optional<int> score_cutoff = table.Probe(
      ZobristKey(123), {.alpha = -40, .beta = 100, .ply = 1, .depth = 5},
      &best_move);
  EXPECT_THAT(score_cutoff, Optional(-40));

  std::optional<int> score_no_cutoff = table.Probe(
      ZobristKey(123), {.alpha = -60, .beta = 100, .ply = 1, .depth = 5},
      &best_move);
  EXPECT_THAT(score_no_cutoff, Eq(std::nullopt));
}

TEST(TranspositionTable, InsufficientDepthReturnsMoveOnly) {
  TranspositionTable table;
  Move best_move;

  table.Record(ZobristKey(123), 100, {.ply = 1, .depth = 2}, Exact,
               MakeMove("b1c3"));

  std::optional<int> score = table.Probe(
      ZobristKey(123), {.alpha = -100, .beta = 100, .ply = 1, .depth = 4},
      &best_move);

  EXPECT_THAT(score, Eq(std::nullopt));
  EXPECT_THAT(best_move, Eq(MakeMove("b1c3")));
}

TEST(TranspositionTable, MateScoreNormalization) {
  TranspositionTable table;
  Move best_move;
  constexpr int kMakeStore = 20'000;
  constexpr int kNegativeMateScore = -20'000;

  table.Record(ZobristKey(10), kMakeStore, {.ply = 2, .depth = 5}, Exact,
               MakeMove("h7h8"));

  table.Record(ZobristKey(20), kNegativeMateScore, {.ply = 3, .depth = 5},
               Exact, MakeMove("a1a8"));

  std::optional<int> mate_probe = table.Probe(
      ZobristKey(10), {.alpha = -30000, .beta = 30000, .ply = 4, .depth = 5},
      &best_move);
  EXPECT_THAT(mate_probe, Optional(19998));

  std::optional<int> neg_mate_probe = table.Probe(
      ZobristKey(20), {.alpha = -30000, .beta = 30000, .ply = 5, .depth = 5},
      &best_move);
  EXPECT_THAT(neg_mate_probe, Optional(-19998));
}

}  // namespace
}  // namespace follychess
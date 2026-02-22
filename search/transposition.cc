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

#include "search/transposition.h"

#include <optional>

#include "engine/move.h"
#include "engine/zobrist.h"
#include "search/evaluation.h"

namespace follychess {

int TranspositionTable::NormalizeScore(const int score, const int ply) {
  if (score > kCheckMateThreshold) {
    return score + ply;
  }
  if (score < -kCheckMateThreshold) {
    return score - ply;
  }
  return score;
}

int TranspositionTable::DenormalizeScore(const int score, const int ply) {
  if (score > kCheckMateThreshold) {
    return score - ply;
  }
  if (score < -kCheckMateThreshold) {
    return score + ply;
  }
  return score;
}

const TranspositionTable::Entry* TranspositionTable::GetEntry(
    const ZobristKey key) const {
  const Bucket& bucket = GetBucket(key);

  // Always check deep_entry first. If it's a hit, it's guaranteed to be
  // >= the depth of always_entry.
  if (bucket.deep_entry.key == key) {
    return &bucket.deep_entry;
  }

  if (bucket.always_entry.key == key) {
    return &bucket.always_entry;
  }

  return nullptr;
}

std::optional<int> TranspositionTable::Probe(ZobristKey key,
                                             ProbeParams probe_params,
                                             Move* best_move) {
  ++probes_;
  const Entry* entry = GetEntry(key);
  if (entry == nullptr) {
    return std::nullopt;
  }

  const int score = DenormalizeScore(entry->score, probe_params.ply);
  *best_move = entry->best_move;

  if (entry->remaining_depth < probe_params.depth) {
    return std::nullopt;
  }

  switch (entry->type) {
    case BoundType::Exact:
      ++hits_;
      return score;

    case BoundType::UpperBound:
      if (score <= probe_params.alpha) {
        ++hits_;
        return probe_params.alpha;
      }
      break;

    case BoundType::LowerBound:
      if (score >= probe_params.beta) {
        ++hits_;
        return probe_params.beta;
      }
      break;
  }

  return std::nullopt;
}

void TranspositionTable::Record(ZobristKey key, int score,
                                RecordParams record_params, BoundType type,
                                Move best_move) {
  const Entry new_entry = {
      .key = key,
      .best_move = best_move,
      .remaining_depth = record_params.depth,
      .score = NormalizeScore(score, record_params.ply),
      .type = type,
  };

  Bucket& bucket = GetBucket(key);
  bucket.always_entry = new_entry;
  if (!bucket.deep_entry.key ||
      new_entry.remaining_depth >= bucket.deep_entry.remaining_depth) {
    bucket.deep_entry = new_entry;
  }
}

}  // namespace follychess

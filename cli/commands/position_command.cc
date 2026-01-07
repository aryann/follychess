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

#include "position_command.h"

#include <format>

#include "engine/move.h"
#include "engine/move_generator.h"

namespace follychess {
namespace {

std::optional<Move> FindMove(std::string_view uci_move,
                             const std::vector<Move> &moves) {
  for (const Move &move : moves) {
    if (std::format("{}", move) == uci_move) {
      return move;
    }
  }
  return std::nullopt;
}

std::expected<void, std::string> ApplyMoves(
    const std::vector<std::string_view> &uci_moves, Game &game) {
  if (uci_moves.empty()) {
    return {};
  }
  if (uci_moves.front() != "moves") {
    return std::unexpected(
        std::format("Invalid remainder for position command: {}", uci_moves));
  }

  for (int i = 1; i < uci_moves.size(); ++i) {
    std::vector<Move> moves = GenerateMoves(game.GetPosition());

    std::optional<Move> move = FindMove(uci_moves[i], moves);
    if (!move) {
      return std::unexpected(std::format("Illegal move: {}", uci_moves[i]));
    }

    game.Do(*move);
    if (game.GetPosition().GetCheckers(~game.GetPosition().SideToMove())) {
      return std::unexpected(std::format(
          "Illegal move (cannot place own king in check): {}", *move));
    }
  }

  return {};
}

}  // namespace

std::expected<void, std::string> StartPos::Run(
    std::vector<std::string_view> args) {
  game_ = Game(Position::Starting());
  return ApplyMoves(args, game_);
}

std::expected<void, std::string> FenPos::Run(
    std::vector<std::string_view> args) {
  auto result = Position::FromFen({
      args.begin(),
      args.begin() + std::min(static_cast<std::size_t>(6), args.size()),
  });
  if (!result.has_value()) {
    return std::unexpected(result.error());
  }

  game_ = Game(result.value());
  return ApplyMoves({args.begin() + 6, args.end()}, game_);
}

}  // namespace follychess

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

#include "perft.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <ranges>

#include "absl/strings/str_join.h"
#include "engine/move_generator.h"
#include "move.h"
#include "position.h"

namespace follychess {
namespace {

using ::testing::ElementsAreArray;
using ::testing::IsEmpty;

struct PerftTestCase {
  std::string name;
  std::string_view fen;
  std::vector<std::size_t> expected_node_counts;
};

const std::vector<PerftTestCase> kTestCases = {
    {
        // https://www.chessprogramming.org/Perft_Results#Initial_Position
        .name = "StartingPosition",
        .fen = R"(rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1)",
        .expected_node_counts =
            {
                1,        //
                20,       //
                400,      //
                8'902,    //
                197'281,  //
#ifdef NDEBUG
                4'865'609,      //
                119'060'324,    //
                3'195'901'860,  //
#endif
            },
    },

    {
        // https://www.chessprogramming.org/Perft_Results#Position_2
        .name = "Position2",
        .fen =
            R"(r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1)",
        .expected_node_counts =
            {
                1,       //
                48,      //
                2'039,   //
                97'862,  //
#ifdef NDEBUG
                4'085'603,      //
                193'690'690,    //
                8'031'647'685,  //
#endif
            },
    },

    {
        // https://www.chessprogramming.org/Perft_Results#Position_3
        .name = "Position3",
        .fen = R"(8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1)",
        .expected_node_counts =
            {
                1,        //
                14,       //
                191,      //
                2'812,    //
                43'238,   //
                674'624,  //
#ifdef NDEBUG
                11'030'083,     //
                178'633'661,    //
                3'009'794'393,  //
#endif
            },
    },

    {
        // https://www.chessprogramming.org/Perft_Results#Position_4
        .name = "Position4",
        .fen =
            R"(r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1)",
        .expected_node_counts =
            {
                1,        //
                6,        //
                264,      //
                9'467,    //
                422'333,  //
#ifdef NDEBUG
                15'833'292,  //
#endif
            },
    },

    {
        // https://www.chessprogramming.org/Perft_Results#Position_5
        .name = "Position5",
        .fen = R"(rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8)",
        // This test is incomplete.
        .expected_node_counts =
            {
                1,       //
                44,      //
                1'486,   //
                62'379,  //
#ifdef NDEBUG
                2'103'487,   //
                89'941'194,  //
#endif
            },
    },

    {
        // https://www.chessprogramming.org/Perft_Results#Position_6
        .name = "Position6",
        .fen =
            R"(r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10)",
        .expected_node_counts =
            {
                1,       //
                46,      //
                2'079,   //
                89'890,  //
#ifdef NDEBUG
                3'894'594,      //
                164'075'551,    //
                6'923'051'137,  //
#endif
            },
    }};

void PrintTo(const PerftTestCase &test_case, std::ostream *os) {
  *os << "{ "
      << R"(.name = ")" << test_case.name << R"(")"
      << R"(, .fen = ")" << test_case.fen << R"(")"
      << ", .expected_node_counts = {"
      << absl::StrJoin(test_case.expected_node_counts, ", ") << "} "
      << "}";
}

class PerftTest : public testing::TestWithParam<PerftTestCase> {};

std::string GetTestName(const testing::TestParamInfo<PerftTestCase> &info) {
  return info.param.name;
}

TEST_P(PerftTest, Run) {
  const auto &[_, fen, expected_node_count] = GetParam();
  const std::size_t depth = expected_node_count.size() - 1;

  std::expected<Position, std::string> position = Position::FromFen(fen);
  ASSERT_THAT(position.error_or(""), IsEmpty());

  std::vector<std::size_t> depth_counts;
  std::map<Move, std::size_t> final_move_counts;
  RunPerft(depth, position.value(), depth_counts, final_move_counts);

  EXPECT_THAT(depth_counts, ElementsAreArray(expected_node_count));
}

INSTANTIATE_TEST_SUITE_P(Perft, PerftTest, testing::ValuesIn(kTestCases),
                         GetTestName);

}  // namespace
}  // namespace follychess

//
// Created by leon on 2019-07-31.
//

#include <vector>
#include <algorithm>
#include <chess/board.h>
#include <gtest/gtest.h>
#include <chess/move_gen.h>
#include <chess/fen.h>
#include "../test_common.h"

#define DEBUG(x)

using namespace chess::core;

bool recursive_cmp(const board & b, int depth) {
    if (depth == 0) return true;

    auto expected = move_gen(b).generate(); //b.get_legal_moves(b.side_to_play);
    auto actual = move_gen(b).generate();

    for (move m : expected) {
        if (std::find(begin(actual), end(actual), m) == end(actual)) {
            std::cout << "Move " << to_long_move(m) << " expected and not found" << std::endl;
            std::cout << fen::to_string(b) << std::endl;
            actual = move_gen(b).generate();
            return false;
        }
    }

    for (move m : actual) {
        if (std::find(begin(expected), end(expected), m) == end(expected)) {
            std::cout << "Move " << to_long_move(m) << " found and not expected" << std::endl;
            std::cout << fen::to_string(b) << std::endl;
            return false;
        }
    }

    for (move m : expected) {
        board bnew = b;
        bnew.make_move(m);
        if (!recursive_cmp(bnew, depth - 1)) {
            std::cout << "Previous move " << to_long_move(m) << std::endl;
            return false;
        }
    }
    return true;
}

template <bool log>
int perft(const board& b, int depth) {
    int n = 0;
    //auto moves = b.get_legal_moves(b.side_to_play);
    auto moves = move_gen(b).generate();
    if (depth == 1) return moves.size();
    if (depth == 0) return 1;

    for (move m : moves) {
        board bnew = b;
        bnew.make_move(m);
        auto p = perft<false>(bnew, depth - 1);
        n += p;
        if (log) std::cout << to_long_move(m) << ": " << p << std::endl;
        //if (log) bnew.print();
    }
    return n;
}

//TEST(board_test, recursive_cmp_1) {
//    board b; b.set_initial_position();
//    b.make_move(SQ_C2, SQ_C3);
////    b.make_move(SQ_C7, SQ_C6);
////    b.make_move(SQ_B2, SQ_B4);
////    b.make_move(SQ_D8, SQ_A5);
//
//    recursive_cmp(b, 5);
//}

TEST(board_test, perft_test_1) {
    std::cout << "\n\n";
    board b; b.set_initial_position();

    ASSERT_EQ(perft<true>(b, 1), 20);
    ASSERT_EQ(perft<true>(b, 2), 400);
    ASSERT_EQ(perft<true>(b, 3), 8902);
    ASSERT_EQ(perft<true>(b, 4), 197281);
    EXPECT_EQ(perft<true>(b, 5), 4865609);
    ASSERT_EQ(perft<true>(b, 6), 119'060'324);
}

TEST(board_test, perft_test_1_c2c3) {
    board b; b.set_initial_position();
    b.make_move(SQ_C2, SQ_C3);
    ASSERT_EQ(perft<true>(b, 4), 222861);
}

TEST(board_test, perft_test_1_c2c3_e7e6) {
    board b; b.set_initial_position();
    b.make_move(SQ_C2, SQ_C3);
    b.make_move(SQ_E7, SQ_E6);
    ASSERT_EQ(perft<true>(b, 3), 15014);
}

TEST(board_test, perft_test_1_c2c3_e7e6_d2d3) {
    board b; b.set_initial_position();
    b.make_move(SQ_C2, SQ_C3);
    b.make_move(SQ_E7, SQ_E6);
    b.make_move(SQ_D2, SQ_D3);
    b.print();
    ASSERT_EQ(perft<true>(b, 2), 832);
}

TEST(board_test, perft_test_1_c2c3_e7e6_d2d3_f8b4) {
    board b; b.set_initial_position();
    b.make_move(SQ_C2, SQ_C3);
    b.make_move(SQ_E7, SQ_E6);
    b.make_move(SQ_D2, SQ_D3);
    b.make_move(SQ_F8, SQ_B4);
    b.print();
    ASSERT_EQ(perft<true>(b, 1), 27);
}

TEST(board_test, perft_test_1_c2c3_a7a5_d1a4) {
    board b; b.set_initial_position();
    b.make_move(SQ_C2, SQ_C3);
    b.make_move(SQ_A7, SQ_A5);
    b.make_move(SQ_D1, SQ_A4);
    ASSERT_EQ(perft<true>(b, 1), 18);
}

TEST(board_test, perft_test_2) {
    board b = fen::board_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    //b.make_move("e2", "b5");
    ASSERT_EQ(perft<true>(b, 1), 48);
    ASSERT_EQ(perft<true>(b, 2), 2039);
    ASSERT_EQ(perft<true>(b, 3), 97862);
    ASSERT_EQ(perft<true>(b, 4), 4085603);
    ASSERT_EQ(perft<true>(b, 5), 193690690);
}

TEST(board_test, perft_test_3) {
    board b = fen::board_from_fen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ");

    ASSERT_EQ(perft<true>(b, 1), 14);
    ASSERT_EQ(perft<true>(b, 2), 191);
    ASSERT_EQ(perft<true>(b, 3), 2812);
    ASSERT_EQ(perft<true>(b, 4), 43238);
    ASSERT_EQ(perft<true>(b, 5), 674624);
}

TEST(board_test, perft_test_4) {
    board b = fen::board_from_fen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");

    EXPECT_EQ(perft<true>(b, 1), 6);
    EXPECT_EQ(perft<true>(b, 2), 264);
    EXPECT_EQ(perft<true>(b, 3), 9467);
    EXPECT_EQ(perft<true>(b, 4), 422333);
    EXPECT_EQ(perft<true>(b, 5), 15833292);
}

TEST(board_test, perft_test_5) {
    board b = fen::board_from_fen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    b.print();
    ASSERT_EQ(perft<true>(b, 1), 44);
    ASSERT_EQ(perft<true>(b, 2), 1'486);
    ASSERT_EQ(perft<true>(b, 3), 62'379);
    ASSERT_EQ(perft<true>(b, 4), 2'103'487);
    ASSERT_EQ(perft<true>(b, 5), 89'941'194);
}

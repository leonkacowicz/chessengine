//
// Created by leon on 2019-07-31.
//


#include <board.h>
#include <gtest/gtest.h>
#include <move_gen.h>

#define DEBUG(x)

template <bool log>
int perft(const board& b, int depth) {
    int n = 0;
    //auto moves = b.get_legal_moves(b.side_to_play);
    auto moves = move_gen(b).generate();
    //if (depth == 1) return moves.size();
    if (depth == 0) return 1;

    for (move m : moves) {
        board bnew = b;
        bnew.make_move(m);
        auto p = perft<false>(bnew, depth - 1);
        n += p;
        if (log) std::cout << m.to_long_move() << ": " << p << std::endl;
        //if (log) bnew.print();
    }
    return n;
}

TEST(board_test, perft_test_1) {
    std::cout << "\n\n";
    board b; b.set_initial_position();

//    ASSERT_EQ(perft<true>(b, 1), 20);
//    ASSERT_EQ(perft<true>(b, 2), 400);
//    ASSERT_EQ(perft<true>(b, 3), 8902);
//    ASSERT_EQ(perft<true>(b, 4), 197281);
    EXPECT_EQ(perft<true>(b, 5), 4865609);
//    ASSERT_EQ(perft<true>(b, 6), 119'060'324);
}

TEST(board_test, perft_test_1_c2c3) {
    board b; b.set_initial_position();
    b.make_move({"c2", "c3"});
    ASSERT_EQ(perft<true>(b, 4), 222861);
}

TEST(board_test, perft_test_1_c2c3_e7e6) {
    board b; b.set_initial_position();
    b.make_move({"c2", "c3"});
    b.make_move({"e7", "e6"});
    ASSERT_EQ(perft<true>(b, 3), 10614);
}

TEST(board_test, perft_test_1_c2c3_e7e6_d2d3) {
    board b; b.set_initial_position();
    b.make_move({"c2", "c3"});
    b.make_move({"e7", "e6"});
    b.make_move({"d2", "d3"});
    b.print();
    ASSERT_EQ(perft<true>(b, 2), 832);
}

TEST(board_test, perft_test_1_c2c3_e7e6_d2d3_f8b4) {
    board b; b.set_initial_position();
    b.make_move({"c2", "c3"});
    b.make_move({"e7", "e6"});
    b.make_move({"d2", "d3"});
    b.make_move({"f8", "b4"});
    b.print();
    ASSERT_EQ(perft<true>(b, 1), 27);
}

TEST(board_test, perft_test_1_c2c3_a7a5_d1a4) {
    board b; b.set_initial_position();
    b.make_move({"c2", "c3"});
    b.make_move({"a7", "a5"});
    b.make_move({"d1", "a4"});
    ASSERT_EQ(perft<true>(b, 1), 18);
}

TEST(board_test, perft_test_2) {
    board b("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");

    ASSERT_EQ(perft<true>(b, 1), 48);
    ASSERT_EQ(perft<true>(b, 2), 2039);
    ASSERT_EQ(perft<true>(b, 3), 97862);
    ASSERT_EQ(perft<true>(b, 4), 4085603);
    ASSERT_EQ(perft<true>(b, 5), 193690690);
}

TEST(board_test, perft_test_3) {
    board b("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ");

    ASSERT_EQ(perft<true>(b, 1), 14);
    ASSERT_EQ(perft<true>(b, 2), 191);
    ASSERT_EQ(perft<true>(b, 3), 2812);
    ASSERT_EQ(perft<true>(b, 4), 43238);
    ASSERT_EQ(perft<true>(b, 5), 674624);
}

TEST(board_test, perft_test_4) {
    board b("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");

    ASSERT_EQ(perft<true>(b, 1), 6);
    ASSERT_EQ(perft<true>(b, 2), 264);
    ASSERT_EQ(perft<true>(b, 3), 9467);
    ASSERT_EQ(perft<true>(b, 4), 422333);
    //ASSERT_EQ(perft<true>(b, 5), 15833292);
}

TEST(board_test, perft_test_5) {
    board b("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");


    ASSERT_EQ(perft<true>(b, 1), 44);
    ASSERT_EQ(perft<true>(b, 2), 1'486);
    ASSERT_EQ(perft<true>(b, 3), 62'379);
    ASSERT_EQ(perft<true>(b, 4), 2'103'487);
    ASSERT_EQ(perft<true>(b, 5), 89'941'194);
}

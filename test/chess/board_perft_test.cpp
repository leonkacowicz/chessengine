//
// Created by leon on 2019-07-31.
//


#include <board.h>
#include <gtest/gtest.h>
#pragma ide diagnostic ignored "cert-err58-cpp"

template <int depth>
int perft(const board& b) {
    int n = 0;
    for (move m : b.get_legal_moves(b.side_to_play)) {
        board bnew = b;
        bnew.make_move(m);
        n += perft<depth - 1>(bnew);
    }
    return n;
}

template <>
int perft<0>(const board& b) {
    return 1;
}

TEST(board_test, perft_test_1) {
    board b; b.set_initial_position();

    ASSERT_EQ(perft<1>(b), 20);
    ASSERT_EQ(perft<2>(b), 400);
    ASSERT_EQ(perft<3>(b), 8902);
    ASSERT_EQ(perft<4>(b), 197281);
    //ASSERT_EQ(perft<5>(b), 4865609);
}

TEST(board_test, perft_test_2) {
    board b("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");

    ASSERT_EQ(perft<1>(b), 48);
    ASSERT_EQ(perft<2>(b), 2039);
    ASSERT_EQ(perft<3>(b), 97862);
    ASSERT_EQ(perft<4>(b), 4085603);
    //ASSERT_EQ(perft<5>(b), 193690690);
}

TEST(board_test, perft_test_3) {
    board b("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ");

    ASSERT_EQ(perft<1>(b), 14);
    ASSERT_EQ(perft<2>(b), 191);
    ASSERT_EQ(perft<3>(b), 2812);
    ASSERT_EQ(perft<4>(b), 43238);
    ASSERT_EQ(perft<5>(b), 674624);
}

TEST(board_test, perft_test_4) {
    board b("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");

    ASSERT_EQ(perft<1>(b), 6);
    ASSERT_EQ(perft<2>(b), 264);
    ASSERT_EQ(perft<3>(b), 9467);
    ASSERT_EQ(perft<4>(b), 422333);
    //ASSERT_EQ(perft<5>(b), 15833292);
}

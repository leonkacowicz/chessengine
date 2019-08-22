//
// Created by leon on 2019-07-29.
//

#include <gtest/gtest.h>
#include <square.h>
#include <engine.h>
#include <zobrist.h>
#include <fen.h>

TEST(engine_test, engine_call) {
    board b;
    b.set_initial_position();
    b.make_move(SQ_E2, SQ_E4);
    engine e;
    for (int i = 1; i < 2; i++) {
        std::cout << i << ".\n\n";
        auto m = e.search_iterate(b);
        if (move_type(m) == NULL_MOVE) break;
        b.make_move(m);
        b.print();
    }
}

TEST(engine_test, engine_should_find_mate_in_one) {
    board b("6k1/5ppp/8/8/8/8/5PPP/3R2K1 w - -");
    engine e;
    
    move m = e.search_iterate(b);

    ASSERT_EQ(m, get_move(SQ_D1, SQ_D8));
}

TEST(engine_test, engine_should_find_mate_in_one_b) {
    board b("r1qr1b2/1R3pkp/3p2pN/ppnPp1Q1/bn2P3/4P2P/PBBP2P1/5RK1 w - e6");
    engine e;

    move m = e.search_iterate(b);

    ASSERT_EQ(m, get_move(SQ_D5, SQ_E6));
}
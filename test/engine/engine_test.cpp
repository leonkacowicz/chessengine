//
// Created by leon on 2019-07-29.
//

#include <gtest/gtest.h>
#include <square.h>
#include <engine.h>
#include <zobrist.h>
#include <fen.h>
#include <mcts.h>

TEST(engine_test, engine_call) {
    board b;
    b.set_initial_position();
    engine e;
    for (int i = 1; i < 100; i++) {
        std::cout << i << ".\n\n";
        auto m = e.get_move(b);
        if (move_type(m) == NULL_MOVE) break;
        b.make_move(m);
        b.print();
    }
}


TEST(engine_test, engine_call_mcts) {
//    board b("6k1/4Rppp/8/8/8/8/5PPP/6K1 w - - 0 1");
    board b("6k1/5r1p/p2N4/nppP2q1/2P5/1P2N3/PQ5P/7K w - - 0 30");
    //b.set_initial_position();
    for (int i = 1; i < 2; i++) {
        std::cout << i << ".\n\n";
        auto m = mcts(b).get_best_move(6);
        if (move_type(m) == NULL_MOVE) break;
        b.make_move(m);
        b.print();
    }
}

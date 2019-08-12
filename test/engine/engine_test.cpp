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
    engine e;
    for (int i = 1; i < 2; i++) {
        std::cout << i << ".\n\n";
        auto m = e.search_iterate(b);
        if (move_type(m) == NULL_MOVE) break;
        b.make_move(m);
        b.print();
    }
}

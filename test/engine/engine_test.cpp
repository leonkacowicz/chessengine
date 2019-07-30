//
// Created by leon on 2019-07-29.
//

#include <gtest/gtest.h>
#include <square.h>
#include <engine.h>
#include <zobrist.h>

TEST(engine_test, engine_call) {
    board b;
    b.set_initial_position();
    engine e(b);
    e.get_move();
}

TEST(random_test, random_t) {
    zobrist::init();

    auto empty = board();
    uint64_t i = zobrist::hash(empty);
    printf("%lx\n", i);
    auto b = empty;
    b.set_initial_position();
    printf("%lx\n", zobrist::hash(b));
    printf("%lx\n", zobrist::hash(empty));
    printf("%lx\n", zobrist::hash(b));
}
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
    engine e;
    for (int i = 1; i < 50; i++) {
        std::cout << i << ".\n\n";
        auto m = e.get_move(b);
        if (m.special == NULL_MOVE) break;
        b.make_move(m);
        b.print();
    }
}

TEST(random_test, random_t) {
    zobrist::init();

    auto empty = board();
    uint64_t i = zobrist::hash(empty);
    printf("%lx\n", i);
    auto b = empty;
    b.set_initial_position();
    printf("%lx\n", zobrist::hash(b));
    printf("%s\n", b.fen().c_str());
    b.make_move(b.get_legal_moves(WHITE)[3]);
    printf("%lx\n", zobrist::hash(b));
    printf("%s\n", b.fen().c_str());
    b.make_move(b.get_legal_moves(BLACK)[3]);
    printf("%lx\n", zobrist::hash(b));
    printf("%s\n", b.fen().c_str());
    b.make_move(b.get_legal_moves(WHITE)[3]);
    printf("%lx\n", zobrist::hash(b));
    printf("%s\n", b.fen().c_str());
    b.make_move(b.get_legal_moves(BLACK)[3]);
    printf("%lx\n", zobrist::hash(b));
    printf("%s\n", b.fen().c_str());
    b.make_move(b.get_legal_moves(WHITE)[3]);
    printf("%lx\n", zobrist::hash(b));
    printf("%s\n", b.fen().c_str());
    b.make_move(b.get_legal_moves(BLACK)[3]);
    printf("%lx\n", zobrist::hash(b));
    printf("%s\n", b.fen().c_str());
}
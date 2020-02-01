//
// Created by leon on 2019-11-19.
//

#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include <chess/board.h>
#include <chess/square.h>
#include <chess/fen.h>
#include <chess/move_gen.h>
#include <chess/game.h>
#include "../test_common.h"


using namespace chess::core;

TEST(game_test, undo_move) {
    board b;
    b.set_initial_position();
    game g(b);
    auto moves = move_gen(b).generate();
    for (move m : moves) {
        g.do_move(m);
        board b2 = b;
        b2.make_move(m);
        ASSERT_EQ(g.states.back().b, b2);
        g.undo_last_move();
        ASSERT_EQ(g.states.back().b, b);
    }
}

TEST(game_test, three_fold_repetition) {

    board b;
    b.set_initial_position();
    game g(b);

    g.do_move(get_move(SQ_B1, SQ_A3));
    ASSERT_FALSE(g.is_draw_by_3foldrep());
    g.do_move(get_move(SQ_B8, SQ_A6));
    ASSERT_FALSE(g.is_draw_by_3foldrep());
    g.do_move(get_move(SQ_A3, SQ_B1));
    ASSERT_FALSE(g.is_draw_by_3foldrep());
    g.do_move(get_move(SQ_A6, SQ_B8));
    ASSERT_FALSE(g.is_draw_by_3foldrep());
    g.do_move(get_move(SQ_B1, SQ_A3));
    ASSERT_FALSE(g.is_draw_by_3foldrep());
    g.do_move(get_move(SQ_B8, SQ_A6));
    ASSERT_FALSE(g.is_draw_by_3foldrep());
    g.do_move(get_move(SQ_A3, SQ_B1));
    ASSERT_FALSE(g.is_draw_by_3foldrep());
    g.do_move(get_move(SQ_A6, SQ_B8));
    ASSERT_TRUE(g.is_draw_by_3foldrep());

}
//
// Created by leon on 2019-08-10.
//

#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include <board.h>
#include <square.h>
#include <fen.h>
#include <move_gen.h>
#include "../test_common.h"

TEST(move_gen_test, king_move_into_check_by_pawn_1) {
    board b("Nn4q1/7r/3B1k1b/p3pP2/N4P2/2PKRn1P/b3B3/3Q3R b - - 1 52");
    auto moves = move_gen(b).generate();
    ASSERT_NOT_CONTAINS(moves, get_move(SQ_F6, SQ_G5));
}
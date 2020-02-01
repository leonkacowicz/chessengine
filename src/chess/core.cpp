//
// Created by leon on 2020-01-08.
//

#include <chess/core.h>
#include <chess/bitboard.h>
#include <chess/magic_bitboard.h>
#include <chess/zobrist.h>

void chess::core::init() {
    init_bitboards();
    init_magic_bitboards();
    zobrist::init();
}

//
// Created by leon on 2020-01-08.
//

#include "core.h"
#include <bitboard.h>
#include <magic_bitboard.h>
#include <zobrist.h>

void chess::core::init() {
    init_bitboards();
    init_magic_bitboards();
    zobrist::init();
}

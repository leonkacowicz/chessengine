//
// Created by leon on 2019-11-16.
//

#ifndef CHESSENGINE_CORE_H
#define CHESSENGINE_CORE_H

#include <bitboard.h>
#include <magic_bitboard.h>
#include <zobrist.h>

namespace chess {
    namespace core {
        void init() {
            init_bitboards();
            init_magic_bitboards();
            zobrist::init();
        }
    }
}

#endif //CHESSENGINE_CORE_H

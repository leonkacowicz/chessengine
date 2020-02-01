//
// Created by leon on 2019-07-30.
//

#ifndef CHESSENGINE_ZOBRIST_H
#define CHESSENGINE_ZOBRIST_H

#include <chess/board.h>

namespace chess::core::zobrist {
    extern uint64_t table[64][6][2];
    extern uint64_t side;
    extern uint64_t castling_rights[4];
    extern uint64_t en_passant[16];

    void init();

    uint64_t hash(const board& b);
    uint64_t hash_update(const board& b, uint64_t hash, move m);
}


#endif //CHESSENGINE_ZOBRIST_H

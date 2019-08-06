//
// Created by leon on 2019-08-05.
//

#ifndef CHESSENGINE_MAGIC_BITBOARD_H
#define CHESSENGINE_MAGIC_BITBOARD_H

#include "bitboard.h"

namespace chess {
    namespace core {


        struct magic {
            bitboard* attack_table; // pointer to beginning of attack table originating from a given square
            bitboard attack_mask;
            uint64_t magic_number;
            uint shift;
        };

        extern bitboard rook_table[0x20000];
        extern magic rook_wizardry[64];

        inline bitboard attacks_from_rook(square origin, bitboard occupancy) {
            magic m = rook_wizardry[origin];
            return m.attack_table[((m.attack_mask & occupancy) * m.magic_number) >> m.shift];
        }

        void init_magic_bitboards();
    }
}

class magic_bitboard {

};


#endif //CHESSENGINE_MAGIC_BITBOARD_H

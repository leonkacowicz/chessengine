//
// Created by leon on 2019-07-30.
//

#ifndef CHESSENGINE_ZOBRIST_H
#define CHESSENGINE_ZOBRIST_H

#include <board.h>
#include <random>

class zobrist {


public:
    static uint64_t table[64][6][2];
    static uint64_t side;

    static void init() {
        std::default_random_engine gen;
        std::uniform_int_distribution<uint64_t> dis(0, (uint64_t)-1);
        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 6; j++) {
                table[i][j][0] = dis(gen);
                table[i][j][1] = dis(gen);
            }
        }
        side = dis(gen);
    }

    static uint64_t hash(const board& b) {
        uint64_t h = 0;
        bitboard sq(1);
        for (int i = 0; i < 64; i++, sq <<= 1) {
            piece p = b.piece_at(sq);
            if (p == NO_PIECE) continue;
            color c = b.color_at(sq);
            h ^= table[i][p][c];
        }
        if (b.side_to_play == BLACK) return h ^ side;
        return h;
    }
};


#endif //CHESSENGINE_ZOBRIST_H

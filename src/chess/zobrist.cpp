//
// Created by leon on 2019-07-30.
//

#include "zobrist.h"

using namespace chess::core::zobrist;

uint64_t zobrist::table[64][6][2];
uint64_t zobrist::side = 0;
uint64_t zobrist::castling_rights[4];
uint64_t zobrist::en_passant[16];

void zobrist::init() {
    std::default_random_engine gen;
    std::uniform_int_distribution<uint64_t> dis(0, (uint64_t)-1);
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 6; j++) {
            table[i][j][0] = dis(gen);
            table[i][j][1] = dis(gen);
        }
    }
    side = dis(gen);
    for (int i = 0; i < 4; i++) castling_rights[i] = dis(gen);
    for (int i = 0; i < 16; i++) en_passant[i] = dis(gen);
}

uint64_t zobrist::hash(const board& b) {
    uint64_t h = 0;
    h ^= b.can_castle_king_side[WHITE] ? castling_rights[0] : 0;
    h ^= b.can_castle_king_side[BLACK] ? castling_rights[1] : 0;
    h ^= b.can_castle_queen_side[WHITE] ? castling_rights[2] : 0;
    h ^= b.can_castle_queen_side[BLACK] ? castling_rights[3] : 0;

    if (b.en_passant >= SQ_A3 && b.en_passant <= SQ_H3) h ^= en_passant[b.en_passant - SQ_A3];
    else if (b.en_passant >= SQ_A6 && b.en_passant <= SQ_H6) h ^= en_passant[b.en_passant - SQ_A6 + 8];

    bitboard sq(1);
    for (int i = 0; i < 64; i++, sq <<= 1u) {
        piece p = b.piece_at(sq);
        if (p == NO_PIECE) continue;
        color c = b.color_at(sq);
        h ^= table[i][p][c];
    }
    if (b.side_to_play == BLACK) return h ^ side;
    return h;
}
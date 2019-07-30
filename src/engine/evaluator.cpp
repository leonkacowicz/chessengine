//
// Created by leon on 7/28/19.
//

#include <random>
#include "evaluator.h"

int evaluator::evaluate(const board &b) {

    int accum[2] = {0, 0};
//    if (b.under_check(b.side_to_play) && b.get_legal_moves(b.side_to_play).empty()) {
//        accum[b.side_to_play] -= 100'00;
//    }
    for (bitboard i(1); !i.isEmpty(); i <<= 1uL) {
        color c = b.color_at(i);
        if (b.piece_of_type[PAWN][i]) accum[c] += 1'00 + 20 * (i.get_square().get_rank()) * (c == WHITE) - 20 * (7 - i.get_square().get_rank()) * (c == BLACK) + 20 * (file_d | file_e)[i];
        if (b.piece_of_type[KNIGHT][i]) accum[c] += 2'95 + 50 * ((file_c | file_d | file_e | file_f) & (rank_3 | rank_4 | rank_5 | rank_6))[i] + 50 * ((file_d | file_e) & (rank_4 | rank_5))[i];
        if (b.piece_of_type[BISHOP][i]) accum[c] += 3'55 + ((rank_1[i] && c == WHITE) || (rank_8[i] && c == BLACK) ? 0 : 100);
        if (b.piece_of_type[ROOK][i]) accum[c] += 5'00;
        if (b.piece_of_type[QUEEN][i]) accum[c] += 9'00;
    }
    rnd = (rnd + 33) % 100;
    return accum[WHITE] - accum[BLACK] + rnd - 50;
}

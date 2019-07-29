//
// Created by leon on 7/28/19.
//

#include "evaluator.h"

int evaluator::evaluate(const board &b) {

    int accum[2] = {0, 0};
    if (b.under_check(b.side_to_play) && b.get_legal_moves(b.side_to_play).empty()) {
        accum[b.side_to_play] -= 100'00;
    }
    for (bitboard i(1); !i.isEmpty(); i <<= 1uL) {
        color c = b.color_at(i);
        if (b.piece_at(i) == PAWN) accum[c] += 1'00 + 20 * (i.get_square().get_rank()) * (c == WHITE) - 20 * (7 - i.get_square().get_rank()) * (c == BLACK);
        if (b.piece_at(i) == KNIGHT) accum[c] += 2'95 + 30 * (file_c | file_d | file_e | file_f)[i] + 20 * (file_d | file_e)[i];
        if (b.piece_at(i) == BISHOP) accum[c] += 3'55;
        if (b.piece_at(i) == ROOK) accum[c] += 5'00;
        if (b.piece_at(i) == QUEEN) accum[c] += 9'00;
    }
    return accum[WHITE] - accum[BLACK];
}

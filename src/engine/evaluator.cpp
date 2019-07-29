//
// Created by leon on 7/28/19.
//

#include "evaluator.h"

int evaluator::evaluate(const board &b) {

    int accum[2] = {0, 0};
    if (b.under_check(b.side_to_play) && b.get_legal_moves(b.side_to_play).empty()) {
        accum[b.side_to_play] -= 100000;
    }
    for (bitboard i(1); !i.isEmpty(); i <<= 1uL) {
        color c = b.color_at(i);
        if (b.piece_at(i) == PAWN) accum[c] += 100;
        if (b.piece_at(i) == KNIGHT) accum[c] += 295;
        if (b.piece_at(i) == BISHOP) accum[c] += 305;
        if (b.piece_at(i) == ROOK) accum[c] += 500;
        if (b.piece_at(i) == QUEEN) accum[c] += 900;
    }
    return accum[b.side_to_play] - accum[opposite(b.side_to_play)];
}

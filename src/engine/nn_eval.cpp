//
// Created by leon on 2019-08-17.
//

#include <fstream>
#include <cstring>
#include "nn_eval.h"

using namespace chess::core;

#ifndef DEBUG
#define DEBUG(x)
#endif

nn_eval::nn_eval(std::istream& fin): net(fin) {
}

int nn_eval::eval(const board& b) {
    fill_input_vector(b);
    return std::min(std::max(int(10000 * ((net(input_vector))[0] - .5)), -10000), 10000);
}

void nn_eval::fill_input_vector(const board& b) {

    constexpr int WHITE_OFFSET = 0;
    constexpr int BLACK_OFFSET = 6;
    constexpr int BITS_PER_SQUARE = 12;

    input_vector = Eigen::VectorXd::Zero(INPUT_SIZE);
    int i = 0;
    bitboard bb = 1;
    for (square sq = SQ_A1; sq <= SQ_H8; ++sq, i += BITS_PER_SQUARE, bb <<= 1uL) {
        if (!((b.piece_of_color[WHITE] | b.piece_of_color[BLACK]) & bb)) continue;
        int offset = WHITE_OFFSET;
        color c = WHITE;
        if (b.piece_of_color[BLACK] & bb) {
            c = BLACK;
            offset = BLACK_OFFSET;
        }
        if (b.king_pos[c] == sq) input_vector[i + KING + offset] = 1;
        else {
            for (piece p = PAWN; p <= QUEEN; p = piece(p + 1)) {
                if (b.piece_of_type[p] & bb) {
                    input_vector[i + p + offset] = 1;
                    break;
                }
            }
        }
    }
    if (b.en_passant != SQ_NONE)
        input_vector[en_passant_offset + get_file(b.en_passant)] = 1;

    input_vector[castling_rights_offset + 0] = b.can_castle_king_side[WHITE];
    input_vector[castling_rights_offset + 1] = b.can_castle_king_side[BLACK];
    input_vector[castling_rights_offset + 2] = b.can_castle_queen_side[WHITE];
    input_vector[castling_rights_offset + 3] = b.can_castle_queen_side[BLACK];
    input_vector[turn_offset] = b.side_to_play;
    input_vector[in_check_offset] = b.under_check(b.side_to_play);
}

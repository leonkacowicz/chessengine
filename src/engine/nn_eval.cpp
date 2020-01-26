//
// Created by leon on 2019-08-17.
//

#include <fstream>
#include "nn_eval.h"

using namespace chess::core;

#ifndef DEBUG
#define DEBUG(x)
#endif

nn_eval::nn_eval(chess::neural::mlp  net): net(std::move(net)) {
}

int nn_eval::eval(const board& b) {
    if (b.side_to_play == color::WHITE) {
        fill_input_vector(b);
        return std::min(std::max(int(10000 * net(input_vector)[0]), -10000), 10000);
    } else {
        fill_input_vector(b.flip_colors());
        return std::min(std::max(int(-10000 * net(input_vector)[0]), -10000), 10000);
    }
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
        color c = (color)((b.piece_of_color[BLACK] & bb) != 0);
        offset += c * (BLACK_OFFSET - WHITE_OFFSET);
        input_vector[i + PAWN + offset] = (b.piece_of_type[PAWN] & bb) != 0;
        input_vector[i + KNIGHT + offset] = (b.piece_of_type[KNIGHT] & bb) != 0;
        input_vector[i + BISHOP + offset] = (b.piece_of_type[BISHOP] & bb) != 0;
        input_vector[i + ROOK + offset] = (b.piece_of_type[ROOK] & bb) != 0;
        input_vector[i + QUEEN + offset] = (b.piece_of_type[QUEEN] & bb) != 0;
        input_vector[i + KING + offset] = b.king_pos[c] == sq;
    }
    if (b.en_passant != SQ_NONE)
        input_vector[en_passant_offset + get_file(b.en_passant)] = 1;

    input_vector[castling_rights_offset + 0] = b.can_castle_king_side[WHITE];
    input_vector[castling_rights_offset + 1] = b.can_castle_king_side[BLACK];
    input_vector[castling_rights_offset + 2] = b.can_castle_queen_side[WHITE];
    input_vector[castling_rights_offset + 3] = b.can_castle_queen_side[BLACK];
//    input_vector[turn_offset] = b.side_to_play;
//    input_vector[in_check_offset] = b.under_check(b.side_to_play);
//    for (int i = 0; i < b.half_move_counter; i++)
//        input_vector[half_move_counter_offset + i] = 1;
}

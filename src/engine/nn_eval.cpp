//
// Created by leon on 2019-08-17.
//

#include <fstream>
#include <chess/engine/nn_eval.h>

using namespace chess::core;

#ifndef DEBUG
#define DEBUG(x)
#endif

nn_eval::nn_eval(chess::neural::mlp net): net(std::move(net)) {
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
    input_vector = Eigen::VectorXd::Zero(INPUT_SIZE);
    int i = 0;

    for (color c: {color::WHITE, color::BLACK}) {
        bitboard bb = b.piece_of_type[PAWN] & b.piece_of_color[c];
        while (bb) {
            square sq = pop_lsb(&bb);
            input_vector[sq - SQ_A2 + IV_OFFSETS[c][piece::PAWN]] = 1;
        }
        for (piece p = piece::KNIGHT; p <= piece::QUEEN; p = piece(int(p) + 1)) {
            bb = b.piece_of_type[p] & b.piece_of_color[c];
            while (bb) {
                square sq = pop_lsb(&bb);
                input_vector[sq + IV_OFFSETS[c][p]] = 1;
            }
        }
        input_vector[b.king_pos[c] + IV_OFFSETS[c][KING]] = 1;
    }

    if (b.en_passant != SQ_NONE)
        input_vector[IV_EN_PASSANT_OFFSET + get_file(b.en_passant)] = 1;

    input_vector[IV_CASTLING_RIGHTS_OFFSET + 0] = b.can_castle_king_side[WHITE];
    input_vector[IV_CASTLING_RIGHTS_OFFSET + 1] = b.can_castle_king_side[BLACK];
    input_vector[IV_CASTLING_RIGHTS_OFFSET + 2] = b.can_castle_queen_side[WHITE];
    input_vector[IV_CASTLING_RIGHTS_OFFSET + 3] = b.can_castle_queen_side[BLACK];
}

board nn_eval::board_from_iv(const Eigen::Vector<double, INPUT_SIZE>& iv) const {
    board b;
    for (color c: {color::WHITE, color::BLACK}) {
        for (int i = IV_OFFSETS[c][piece::PAWN]; i < IV_OFFSETS[c][piece::KNIGHT]; i++) {
            auto sq = square(SQ_A2 + i - IV_OFFSETS[c][piece::PAWN]);
            auto bb = get_bb(sq);
            if (iv[i] > 0.5) {
                b.piece_of_type[piece::PAWN] |= bb;
                b.piece_of_color[c] |= bb;
            }
        }
        for (piece p = piece::KNIGHT; p <= piece::QUEEN; p = piece(int(p) + 1)) {
            for (int i = IV_OFFSETS[c][p]; i < IV_OFFSETS[c][piece(int(p) + 1)]; i++) {
                auto sq = square(i - IV_OFFSETS[c][p]);
                auto bb = get_bb(sq);
                if (iv[i] > 0.5) {
                    b.piece_of_type[p] |= bb;
                    b.piece_of_color[c] |= bb;
                }
            }
        }
        for (int i = IV_OFFSETS[c][piece::KING]; i < IV_OFFSETS[c][piece::KING] + IV_PIECE_NUM_SQUARES; i++) {
            auto sq = square(i - IV_OFFSETS[c][piece::KING]);
            auto bb = get_bb(sq);
            if (iv[i] > 0.5) {
                b.king_pos[c] = sq;
                b.piece_of_color[c] |= bb;
                break;
            }
        }
    }

    for (int i = 0; i < 8; i++)
        if (input_vector[IV_EN_PASSANT_OFFSET + i] > .5)
            b.en_passant = get_square(i, 2);

    b.can_castle_king_side[WHITE] = iv[IV_CASTLING_RIGHTS_OFFSET + 0] > .5;
    b.can_castle_king_side[BLACK] = iv[IV_CASTLING_RIGHTS_OFFSET + 1] > .5;
    b.can_castle_queen_side[WHITE] = iv[IV_CASTLING_RIGHTS_OFFSET + 2] > .5;
    b.can_castle_queen_side[BLACK] = iv[IV_CASTLING_RIGHTS_OFFSET + 3] > .5;
    
    return b;
}
//
// Created by leon on 2019-08-17.
//

#include <fstream>
#include <cstring>
#include "nn_eval.h"

#ifndef DEBUG
#define DEBUG(x)
#endif

nn_eval::nn_eval(std::istream& fin) {

    int num_layers;
    fin >> num_layers;
    while (num_layers--) {
        int rows, cols;
        fin >> rows >> cols;
        matrices.emplace_back(rows, cols);
        auto& M = matrices.back();
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                fin >> M(i, j);
    }
}

int nn_eval::eval(const board& b) {

    fill_input_vector(b);

    Eigen::VectorXd v(INPUT_SIZE + 1);

    v << 1, input_vector;

    for (int i = 0; i < matrices.size() - 1; i++) {
        Eigen::VectorXd w(matrices[i].rows() + 1);
        DEBUG(std::cout << "Multiplying:\n" << matrices[i] << std::endl);
        w << 1, matrices[i] * v;
        DEBUG(std::cout << "by:\n" << v.transpose() << std::endl);
        DEBUG(std::cout << "yielding:\n" << w.transpose() << std::endl);
        v = (w.array() > 0).matrix().cast<double>();
        DEBUG(std::cout << "with activations:\n" << v.transpose() << std::endl);
    }
    DEBUG(std::cout << "M: \n" << matrices.back() << std::endl);
    DEBUG(std::cout << "v: \n" << v.transpose() << std::endl);

    return std::min(std::max(int(10000 * (matrices.back() * v)[0]), -10000), 10000);
}

void nn_eval::fill_input_vector(const board& b) {
    //std::memset(input_vector, 0, sizeof(input_vector));
    input_vector = Eigen::VectorXd::Zero(INPUT_SIZE);
    int i = 0;
    for (square sq = SQ_A1; sq <= SQ_H8; ++sq, i += 12) {
        bitboard bb = get_bb(sq);
        if (b.piece_of_color[WHITE] & bb) {
            if (b.king_pos[WHITE] == sq) input_vector[i + KING] = 1;
            else
                for (piece p = PAWN; p <= QUEEN; p = piece(p + 1))
                    if (b.piece_of_type[p] & bb) input_vector[i + p] = 1;

        } else if (b.piece_of_color[BLACK] & bb) {
            if (b.king_pos[BLACK] == sq) input_vector[i + KING + 6] = 1;
            else
                for (piece p = PAWN; p <= QUEEN; p = piece(p + 1))
                    if (b.piece_of_type[p] & bb) input_vector[i + p + 6] = 1;
        }
    }
    if (b.en_passant != SQ_NONE)
        input_vector[en_passant_offset + get_file(b.en_passant)] = 1;

    input_vector[castling_rights_offset + 0] = b.can_castle_king_side[WHITE];
    input_vector[castling_rights_offset + 1] = b.can_castle_king_side[BLACK];
    input_vector[castling_rights_offset + 2] = b.can_castle_queen_side[WHITE];
    input_vector[castling_rights_offset + 3] = b.can_castle_queen_side[BLACK];
}

//
// Created by leon on 2019-08-17.
//

#ifndef CHESSENGINE_NN_EVAL_H
#define CHESSENGINE_NN_EVAL_H


#include <string>
#include <vector>
#include <chess/board.h>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <chess/neuralnet/mlp.h>
#include <chess/engine/evaluator.h>


class nn_eval : public evaluator {

    using board = chess::core::board;
public:
    static constexpr int IV_PAWN_NUM_SQUARES = 64 - 8 * 2;
    static constexpr int IV_PIECE_NUM_SQUARES = 64;
    static constexpr int IV_WHITE_PAWN_OFFSET = 0;
    static constexpr int IV_WHITE_KNIGHT_OFFSET = IV_WHITE_PAWN_OFFSET + IV_PAWN_NUM_SQUARES;
    static constexpr int IV_WHITE_BISHOP_OFFSET = IV_WHITE_KNIGHT_OFFSET + IV_PIECE_NUM_SQUARES;
    static constexpr int IV_WHITE_ROOK_OFFSET = IV_WHITE_BISHOP_OFFSET + IV_PIECE_NUM_SQUARES;
    static constexpr int IV_WHITE_QUEEN_OFFSET = IV_WHITE_ROOK_OFFSET + IV_PIECE_NUM_SQUARES;
    static constexpr int IV_WHITE_KING_OFFSET = IV_WHITE_QUEEN_OFFSET + IV_PIECE_NUM_SQUARES;

    static constexpr int IV_BLACK_PAWN_OFFSET = IV_WHITE_KING_OFFSET + IV_PIECE_NUM_SQUARES;
    static constexpr int IV_BLACK_KNIGHT_OFFSET = IV_BLACK_PAWN_OFFSET + IV_PAWN_NUM_SQUARES;
    static constexpr int IV_BLACK_BISHOP_OFFSET = IV_BLACK_KNIGHT_OFFSET + IV_PIECE_NUM_SQUARES;
    static constexpr int IV_BLACK_ROOK_OFFSET = IV_BLACK_BISHOP_OFFSET + IV_PIECE_NUM_SQUARES;
    static constexpr int IV_BLACK_QUEEN_OFFSET = IV_BLACK_ROOK_OFFSET + IV_PIECE_NUM_SQUARES;
    static constexpr int IV_BLACK_KING_OFFSET = IV_BLACK_QUEEN_OFFSET + IV_PIECE_NUM_SQUARES;
    static constexpr int IV_EN_PASSANT_OFFSET = IV_BLACK_KING_OFFSET + IV_PIECE_NUM_SQUARES;
    static constexpr int IV_CASTLING_RIGHTS_OFFSET = IV_EN_PASSANT_OFFSET + 8;
    static constexpr int INPUT_SIZE = IV_CASTLING_RIGHTS_OFFSET + 4;

    static constexpr int IV_OFFSETS[2][6] = {
            {IV_WHITE_PAWN_OFFSET, IV_WHITE_KNIGHT_OFFSET, IV_WHITE_BISHOP_OFFSET, IV_WHITE_ROOK_OFFSET, IV_WHITE_QUEEN_OFFSET, IV_WHITE_KING_OFFSET},
            {IV_BLACK_PAWN_OFFSET, IV_BLACK_KNIGHT_OFFSET, IV_BLACK_BISHOP_OFFSET, IV_BLACK_ROOK_OFFSET, IV_BLACK_QUEEN_OFFSET, IV_BLACK_KING_OFFSET}
    };
    Eigen::Vector<double, INPUT_SIZE> input_vector;
    chess::neural::mlp net;


    nn_eval(chess::neural::mlp net);

    int eval(const board& b) override;

    void fill_input_vector(const board& b);

    board board_from_iv(const Eigen::Vector<double, INPUT_SIZE>& iv) const;
};


#endif //CHESSENGINE_NN_EVAL_H

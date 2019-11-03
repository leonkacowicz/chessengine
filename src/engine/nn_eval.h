//
// Created by leon on 2019-08-17.
//

#ifndef CHESSENGINE_NN_EVAL_H
#define CHESSENGINE_NN_EVAL_H


#include <string>
#include <vector>
#include <board.h>
#include <Eigen/Dense>
#include <Eigen/Core>
#include "evaluator.h"

constexpr int INPUT_SIZE = 832;
constexpr int en_passant_offset = 64 * 12;
constexpr int castling_rights_offset = en_passant_offset + 8;

class nn_eval : public evaluator {

    Eigen::Vector<double, 832> input_vector;
    std::vector<Eigen::MatrixXd> matrices;

public:

    nn_eval(std::istream& fin);

    int eval(const board& b) override;

    void fill_input_vector(const board& b);
};


#endif //CHESSENGINE_NN_EVAL_H

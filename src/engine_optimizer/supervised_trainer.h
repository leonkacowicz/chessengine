//
// Created by leon on 2020-01-22.
//

#ifndef CHESSENGINE_SUPERVISED_TRAINER_H
#define CHESSENGINE_SUPERVISED_TRAINER_H

#include <vector>
#include <Eigen/Core>
#include <mlp.h>
#include "particle_swarm_optimizer.h"

namespace chess::optimizer {

class supervised_trainer {

public:
    std::vector<Eigen::VectorXd> training_set;
    std::vector<Eigen::VectorXd> training_set_outputs;
    std::vector<int> layers;

    supervised_trainer(std::vector<int>  layers);
    double mse(const Eigen::VectorXd& candidate);
    void train();
};
}

#endif //CHESSENGINE_SUPERVISED_TRAINER_H

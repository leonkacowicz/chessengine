//
// Created by leon on 2020-01-22.
//

#ifndef CHESSENGINE_SUPERVISED_TRAINER_H
#define CHESSENGINE_SUPERVISED_TRAINER_H

#include <vector>
#include <Eigen/Core>
#include <chess/neuralnet/mlp.h>
#include "particle_swarm_optimizer.h"

namespace chess::optimizer {

class supervised_trainer {

public:
    std::vector<Eigen::VectorXd> training_set;
    std::vector<Eigen::VectorXd> training_set_outputs;
    std::vector<int> layers;

    supervised_trainer(std::vector<int> layers) : layers(std::move(layers)) {

    }
    double mse(const Eigen::VectorXd& candidate) const {

        chess::neural::mlp net(layers, candidate);
        double acum = 0;
        int count = 0;
        for (int i = 0; i < training_set.size(); i++) {
            Eigen::VectorXd y_hat = net(training_set[i]);
            for (int j = 0; j < training_set_outputs[i].size(); j++) {
                count++;
                double diff = training_set_outputs[i][j] - y_hat[j];
                acum += diff * diff;
            }
        }
        acum /= double(count);
        return acum;
    }
    void train(int iterations = 100) {
        particle_swarm_optimizer<double> pso;
        pso.particle_count = 20;
        pso.inertia = 0.9;
        pso.local_best_param = .2;
        pso.global_best_param = .2;
        pso.random_vec_param = 0.2;
        pso.learn_speed = 2;
        pso.vector_size = chess::neural::mlp::num_vector_dimensions(layers);
        pso.steps = 1;
        std::atomic<int> num_cmp = 0;
        for (int t = 0; t < iterations; t++) {
            pso.run([&] (const Eigen::VectorXd& x) {
                    return std::sqrt(mse(x));
                },
                [&] (double x1, double x2) { num_cmp++; return x2 < x1; }
            );
            int x = num_cmp;
            std::printf("val: %.12f ; num_cmp = %d; num_improved = %d\n", pso.particles_best_values[pso.current_best_particle], x, pso.num_improved);
            std::cout.flush();
        }
    }
};
}

#endif //CHESSENGINE_SUPERVISED_TRAINER_H

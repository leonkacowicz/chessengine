//
// Created by leon on 2020-01-22.
//

#include "supervised_trainer.h"

#include <utility>

using namespace chess::optimizer;

supervised_trainer::supervised_trainer(std::vector<int> layers) : layers(std::move(layers)) {

}

double supervised_trainer::mse(const Eigen::VectorXd& candidate) {

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

void supervised_trainer::train() {
    particle_swarm_optimizer pso;
    pso.vector_size = chess::neural::mlp::num_vector_dimensions(layers);
    auto comp = [&](const Eigen::VectorXd& p1, const Eigen::VectorXd& p2) {
        return mse(p2) < mse(p1);
    };
    pso.steps = 1;
    for (int t = 0; t < 5000; t++) {
        pso.run(comp);
        std::printf("%.6f\n", mse(pso.particles_best_pos[pso.current_best_particle]));
    }
}

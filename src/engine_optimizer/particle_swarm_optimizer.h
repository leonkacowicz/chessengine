//
// Created by leon on 2020-01-20.
//

#ifndef CHESSENGINE_PARTICLE_SWARM_OPTIMIZER_H
#define CHESSENGINE_PARTICLE_SWARM_OPTIMIZER_H

#include <vector>
#include <future>
#include <functional>
#include <Eigen/Core>
#include <random>
#include "opt_util.h"

namespace chess::optimizer {

template<typename T>
class particle_swarm_optimizer {

public:
    using particle = Eigen::VectorXd;
    using velocity = Eigen::VectorXd;

    int particle_count = 20;
    int steps = 20;
    int vector_size;
    double inertia = 0.95;
    double global_best_param = .1;
    double local_best_param = .1;
    double random_vec_param = 0.01;
    double learn_speed = 1;
    int current_best_particle = 0;
    int num_improved = 0;
    std::vector<particle> particles_current_pos;
    std::vector<particle> particles_best_pos;
    std::vector<velocity> particles_vel;
    std::vector<T> particles_best_values;

    void run(auto eval, auto comp) {

        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution unif(0., 1.);

        while (particles_current_pos.size() < particle_count)
            particles_current_pos.push_back(Eigen::VectorXd::Random(vector_size) * random_vec_param);

        while (particles_best_pos.size() < particle_count) {
            particles_best_pos.push_back(particles_current_pos[particles_best_pos.size()]);
            particles_best_values.push_back(eval(particles_best_pos.back()));
        }

        while (particles_vel.size() < particle_count)
            particles_vel.push_back(Eigen::VectorXd::Random(vector_size) * 0);


        for (int t = 0; t < steps; t++) {

            std::vector<std::future<bool>> local_comps;
            for (int p = 0; p < particle_count; p++) {
                for (int d = 0; d < vector_size; d++) {
                    double rp = unif(mt);
                    double rg = unif(mt);
                    double rr = unif(mt);
                    particles_vel[p][d] = inertia * particles_vel[p][d] +
                                          local_best_param * rp * (particles_best_pos[p][d] - particles_current_pos[p][d]) +
                                          global_best_param * rg * (particles_best_pos[current_best_particle][d] - particles_current_pos[p][d]) +
                                          random_vec_param * (rr - .5) +
                                          0.0;
                }
                particles_current_pos[p] += learn_speed * particles_vel[p];
                local_comps.push_back(std::async([&] (int i) {
                    T val = eval(particles_current_pos[i]);
                    if (comp(particles_best_values[i], val)) {
                        particles_best_values[i] = val;
                        return true;
                    }
                    return false;
                }, p));
            }

            std::vector<int> global_candidates;
            for (int p = 0; p < particle_count; p++) {
                if (local_comps[p].get()) {
                    particles_best_pos[p] = particles_current_pos[p];
                    global_candidates.push_back(p);
                } else if (p == current_best_particle || t == 0) {
                    //particles_best_pos[p] = particles_current_pos[p];
                    global_candidates.push_back(p);
                }
            }
            num_improved = global_candidates.size();
            int max_index = util::parallel_max(global_candidates, 0, global_candidates.size(), [&](int p1, int p2) {
                return comp(particles_best_values[p1], particles_best_values[p2]);
            });
            current_best_particle = global_candidates[max_index];
        }
    }


};

}

#endif //CHESSENGINE_PARTICLE_SWARM_OPTIMIZER_H

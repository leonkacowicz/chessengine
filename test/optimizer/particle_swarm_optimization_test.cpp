//
// Created by leon on 2020-01-21.
//

#include <gtest/gtest.h>
#include <algorithm.h>
#include <mlp.h>
#include <particle_swarm_optimizer.h>
#include "../test_common.h"
#include <opt_util.h>
using namespace chess::optimizer;
using namespace chess::neural;


TEST(particle_swarm_optimization_test, std_parallel_max_test) {

    using particle = particle_swarm_optimizer::particle;
    std::cout << sizeof(particle_swarm_optimizer::comparator) << std::endl;
    int n = 1000;
    std::vector<particle> particles;
    for (int i = 0; i < n; i++) {
        Eigen::VectorXd x(1);
        x << i;
        particles.emplace_back(x);
    }

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution unif(50, 80);

    particle_swarm_optimizer::comparator c = [&] (const particle& p1, const particle& p2) -> int {
        //std::cout << "comparing " << p1[0] << " with " << p2[0] << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(unif(mt)));
        return p1[0] < p2[0];
    };


    chess::optimizer::util::parallel_max(particles, 0, n, c);
}
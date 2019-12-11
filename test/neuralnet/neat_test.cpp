//
// Created by leon on 2019-12-10.
//

#include <gtest/gtest.h>
#include <neat.h>

using namespace chess::neural::neat;
TEST(neat, test1) {

    neat_genepool gp(2, 1);
    genome g1;
    gp.mutate_add_connection(g1, 0, 2, 1.5);

    nn_graph nn(g1, 2, 1);
    auto eval = nn.evaluate({2, 3});
    ASSERT_DOUBLE_EQ(eval[0], 1.0 / (1.0 + std::exp(-3)));
    std::cout << eval[0] << std::endl;

    gp.mutate_add_connection(g1, 1, 2, -1);
    nn_graph nn2(g1, 2, 1);
    auto eval2 = nn2.evaluate({2, 3});
    ASSERT_DOUBLE_EQ(eval2[0], 0.5);

    gp.mutate_add_node(g1, 0);

    auto eval3 = nn_graph(g1, 2, 1).evaluate({2, 3});
    std::cout << eval3[0] << std::endl;
}

double eval_xor(nn_graph nn) {
    double e = 0;
    auto squared = [] (auto x) { return x * x; };
    e += squared(nn.evaluate({0, 0})[0]);
    e += squared(nn.evaluate({1, 1})[0]);
    e += squared(1 - nn.evaluate({0, 1})[0]);
    e += squared(1 - nn.evaluate({1, 0})[0]);
    return std::sqrt(e / 4);
}

TEST(neat, detcrowd) {


    std::vector<std::pair<double, genome>> population;
    neat_genepool pool(2, 1);
    for (int i = 0; i < 10; i++) {
        genome g;
        pool.mutate_add_random_connection(g);
        double e = eval_xor(nn_graph(g, 2, 1));
        population.emplace_back(e, g);
    }
    std::cout << population[0].first << std::endl;
}
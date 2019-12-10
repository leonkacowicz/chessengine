//
// Created by leon on 2019-12-10.
//

#include <gtest/gtest.h>
#include <neat.h>

TEST(neat, test1) {

    using namespace chess::neural::neat;
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

}
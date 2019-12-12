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

std::vector<int> randomperm(int n) {
    std::vector<int> perm(n, 0);
    for (int i = 0; i < n; i++) perm[i] = i;
    std::shuffle(perm.begin(), perm.end(), std::random_device());
    return perm;
}

TEST(neat, detcrowd) {

    int pop_size = 10;
    std::vector<std::pair<double, genome>> population;
    neat_genepool pool(2, 1);
    for (int i = 0; i < pop_size; i++) {
        genome g;
        pool.mutate_add_random_connection(g);
        double e = eval_xor(nn_graph(g, 2, 1));
        population.emplace_back(e, g);
    }

    for (int generation = 1; generation < 100; generation++) {
        auto permutation = randomperm(pop_size);
        std::vector<std::pair<double, genome>> new_population(pop_size, std::make_pair(0, genome()));
        for (int i = 0; i < pop_size - 1; i += 2) {
            auto child1 = pool.crossover(population[i].second, population[i + 1].second);
            if (std::rand() % 10 == 0)
                pool.mutate_add_random_connection(child1);
            if (std::rand() % 20 == 0)
                pool.mutate_add_node_at_random(child1);
            double eval1 = eval_xor(nn_graph(child1, 2, 1));
            if (eval1 < population[i].first) {
                new_population[permutation[i]] = std::make_pair(eval1, child1);
            } else {
                new_population[permutation[i]] = population[i];
            }

            auto child2 = pool.crossover(population[i + 1].second, population[i].second);
            if (std::rand() % 10 == 0)
                pool.mutate_add_random_connection(child2);
            if (std::rand() % 20 == 0)
                pool.mutate_add_node_at_random(child2);
            double eval2 = eval_xor(nn_graph(child1, 2, 1));
            if (eval2 < population[i + 1].first) {
                auto pair = std::make_pair(eval2, child2);
                new_population[permutation[i + 1]] = pair;
            } else {
                new_population[permutation[i + 1]] = population[i + 1];
            }
        }
        population = new_population;

        for (int i = 0; i < pop_size; i++) std::cout << ", " << population[i].first;
        std::cout << std::endl;
    }

    std::cout << population[0].first << std::endl;
}
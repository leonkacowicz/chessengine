//
// Created by leon on 2019-12-10.
//

#include <gtest/gtest.h>
#include <neat.h>

using namespace chess::neural::neat;
TEST(neat, test1) {

    neat_genepool gp(2, 1);
    genome g1;
    gp.mutate_add_connection(g1, 1, 3, 1.5);

    nn_graph nn(g1, 2, 1);
    auto eval = nn.evaluate({2, 3});
    ASSERT_DOUBLE_EQ(eval[0], 1.0 / (1.0 + std::exp(-3)));
    std::cout << eval[0] << std::endl;

    gp.mutate_add_connection(g1, 2, 3, -1);
    nn_graph nn2(g1, 2, 1);
    auto eval2 = nn2.evaluate({2, 3});
    ASSERT_DOUBLE_EQ(eval2[0], 0.5);

    gp.mutate_add_node(g1, 0);

    auto eval3 = nn_graph(g1, 2, 1).evaluate({2, 3});
    std::cout << eval3[0] << std::endl;
}

TEST(neat, cross_over) {
    std::cout << std::endl;

    neat_genepool gp(2, 1);

    genome p1;

    gp.mutate_add_connection(p1, 0, 3, 0.1);
    gp.mutate_add_connection(p1, 1, 3, 0.1);
    gp.mutate_add_connection(p1, 2, 3, 0.1);
    gp.mutate_add_node(p1, 1);
    genome p2 = p1;
    gp.mutate_add_node(p2, 4);
    gp.mutate_add_connection(p1, 0, 4, 0.1);

    gp.mutate_add_connection(p2, 2, 4, 0.2);
    gp.mutate_add_connection(p2, 0, 5, 0.2);

    auto c1 = gp.crossover(p2, p1);

    for (int i = 0; i < gp.connection_genes.size(); i++) {
        std::cout << i << ": ";
        if (p1.connections.find(i) != p1.connections.end())
            std::cout << p1.connections[i].from+1 << (p1.connections[i].enabled ? " -> " : " xx ") << p1.connections[i].to+1 << " " << p1.connections[i].weight << "; ";
        else
            std::cout << "            ";
        if (p2.connections.find(i) != p2.connections.end())
            std::cout << p2.connections[i].from+1 << (p2.connections[i].enabled ? " -> " : " xx ") << p2.connections[i].to+1 << " " << p2.connections[i].weight << "; ";
        else
            std::cout << "            ";

        if (c1.connections.find(i) != c1.connections.end())
            std::cout << c1.connections[i].from+1 << (c1.connections[i].enabled ? " -> " : " xx ") << c1.connections[i].to+1 << " " << c1.connections[i].weight << "; ";
        else
            std::cout << "            ";

        std::cout << std::endl;
    }
}


double eval_xor(nn_graph&& nn) {
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

    int pop_size = 250;
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution unif;
    std::uniform_int_distribution rnd;

    std::vector<std::pair<double, genome>> population;
    neat_genepool pool(2, 1);
    for (int i = 0; i < pop_size; i++) {
        genome g;
        pool.mutate_add_random_connection(g);
        double e = eval_xor(nn_graph(g, 2, 1));
        population.emplace_back(e, g);
    }

    int improved_after_cross = 0;
    int improved_after_no_cross = 0;
    int total_evals = 0;
    for (int generation = 1; generation < 1000; generation++) {
        auto permutation = randomperm(pop_size);
        std::vector<std::pair<double, genome>> new_population(pop_size, std::make_pair(0, genome()));
        for (int i = 0; i < pop_size - 1; i += 2) {
            for (int j = 0; j < 2; j++) {
                genome child = pool.crossover(population[i + j].second, population[i + 1 - j].second);;
                pool.random_mutation(child);
                double eval = eval_xor(nn_graph(child, 2, 1));
                total_evals++;
                if (eval < population[i + j].first) {
                    new_population[permutation[i + j]] = std::make_pair(eval, child);
                    improved_after_cross++;
                } else {
                    child = population[i + j].second;
                    pool.random_mutation(child);
                    eval = eval_xor(nn_graph(child, 2, 1));
                    total_evals++;
                    if (eval < population[i + j].first) {
                        new_population[permutation[i + j]] = std::make_pair(eval, child);
                        improved_after_no_cross++;
                    } else
                        new_population[permutation[i + j]] = std::make_pair(population[i + j].first, population[i + j].second);
                }
            }
        }
        population = new_population;
        double min = 1;
        int min_idx = -1;
        double avg = 0;
        for (int i = 0; i < pop_size; i++) {
            if (population[i].first < min) {
                min = population[i].first;
                min_idx = i;
            }
            avg += population[i].first;
        }
        avg /= pop_size;
        std::cout << generation << ": ";
        std::cout << min << "; " << avg;
        std::cout << std::endl;

        auto connections = population[min_idx].second.connections;
        for (int i = 0; i < pool.connection_genes.size(); i++) {
            auto iter = connections.find(i);
            if (iter != connections.end() && iter->second.enabled)
                std::cout << iter->second.weight << "     " << iter->second.from << " -> " << iter->second.to << std::endl;
        }
    }
    std::cout << "improved after mutation: " << improved_after_cross
        << "; improved after no crossover: " << improved_after_no_cross
        << "; total evaluations: " << total_evals
        << std::endl
        << std::endl;
}
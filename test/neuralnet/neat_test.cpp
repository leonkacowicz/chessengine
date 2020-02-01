//
// Created by leon on 2019-12-10.
//

#include <gtest/gtest.h>
#include <chess/neuralnet/neat.h>
#include "../test_common.h"

using namespace chess::neural::neat;
TEST(neat, test1) {

    neat_genepool gp(2, 1);
    genome g1;
    gp.mutate_add_connection(g1, 1, 3, 1.5);

    nn_graph nn(g1, 2, 1);
    auto eval = nn.evaluate({2, 3});
    ASSERT_DOUBLE_EQ(eval[0], 2.0 / (1.0 + std::exp(-3)) - 1.0);
    std::cout << eval[0] << std::endl;

    gp.mutate_add_connection(g1, 2, 3, -1);
    nn_graph nn2(g1, 2, 1);
    auto eval2 = nn2.evaluate({2, 3});
    ASSERT_DOUBLE_EQ(eval2[0], 0.);

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

double eval_xor3(const nn_graph& nn) {
    double e = 0;
    auto squared = [] (auto x) { return x * x; };
    e += squared(-1.0 - nn.evaluate({0, 0, 0})[0]);
    e += squared(1.0 - nn.evaluate({0, 0, 1})[0]);
    e += squared(1.0 - nn.evaluate({0, 1, 0})[0]);
    e += squared(-1.0 - nn.evaluate({0, 1, 1})[0]);
    e += squared(1.0 - nn.evaluate({1, 0, 0})[0]);
    e += squared(-1.0 - nn.evaluate({1, 0, 1})[0]);
    e += squared(-1.0 - nn.evaluate({1, 1, 0})[0]);
    e += squared(1.0 - nn.evaluate({1, 1, 1})[0]);
    return std::sqrt(e);
}

TEST(neat, detcrowd) {

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution unif;
    const int inputs = 3;
    int pop_size = 10;

    std::vector<std::pair<double, genome>> population;
    neat_genepool pool(inputs, 1);
    for (int i = 0; i < pop_size; i++) {
        genome g;
        pool.mutate_add_random_connection(g);
        double e = eval_xor3(nn_graph(g, inputs, 1));
        population.emplace_back(e, g);
    }

    int improved_after_cross = 0;
    int improved_after_no_cross = 0;
    int total_evals = 0;
    for (int generation = 1; generation < 1500; generation++) {
        auto permutation = randomperm(pop_size);
        std::vector<std::pair<double, genome>> new_population(pop_size, std::make_pair(0, genome()));
        for (int i = 0; i < pop_size - 1; i += 2) {
            for (int j = 0; j < 2; j++) {
                genome child = pool.crossover(population[i + j].second, population[i + 1 - j].second);
                pool.random_mutation(child);
                double eval = eval_xor3(nn_graph(child, inputs, 1));
                total_evals++;
                if (eval < population[i + j].first) {
                    new_population[permutation[i + j]] = std::make_pair(eval, child);
                    improved_after_cross++;
                } else {
                    child = population[i + j].second;
                    pool.random_mutation(child);
                    eval = eval_xor3(nn_graph(child, inputs, 1));
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
        double min = 10000;
        int min_idx = -1;
        double avg = 0;
        for (int i = 0; i < pop_size; i++) {
            if (population[i].first < min) {
                min = population[i].first;
                min_idx = i;
            }
            avg += population[i].first;
            std::cout << "connections: " << population[i].second.active_connetions_count() << std::endl;
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
    std::cout << "improved after crossover: " << improved_after_cross
        << "; improved after no crossover: " << improved_after_no_cross
        << "; total evaluations: " << total_evals
        << std::endl
        << std::endl;
}

TEST(neat, bin_search_insert) {

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution unif(0., 1.);
    std::vector<double> v;

    int comparations = 0;
    std::function<int(const double&, const double&)> comp = [&](const double& a, const double& b) { comparations++; return b - a < 0 ? 1 : b - a > 0 ? -1 : 0; };

    for (int i = 0; i < 100; i++) {
        binary_search_insert(v, unif(mt), comp);
        for (int j = 0; j < v.size() - 1; j++)
            ASSERT_LE(v[j], v[j + 1]);
        std::cout << "comparations: " << comparations << std::endl;
    }

}

TEST(neat, speciation) {
    int comparations = 0;
    const int inputs = 3;
    genome_comparator comp = [&](const genome& g1, const genome& g2) {
        comparations++;
        auto e1 = eval_xor3(nn_graph(g1, inputs, 1));
        auto e2 = eval_xor3(nn_graph(g2, inputs, 1));
        if (e1 == e2) return 0;
        return e1 < e2 ? -1 : 1;
    };
    neat_algorithm ga(inputs, 1, 150, comp);
    for (int gen = 0; gen < 50; gen++) {
        std::cout << "generation " << gen << std::endl;
        std::cout << "comparisons: " << comparations << std::endl;
        for (int s = 0; s < ga.all_species.size(); s++) {
            int connections = 0;
            for (const auto& kv : ga.all_species[s].population.front().connections)
                if (kv.second.enabled) connections++;
            auto eval = eval_xor3(nn_graph(ga.all_species[s].population.front(), inputs, 1));
            std::cout << "species " << ga.all_species[s].id << ": " << ga.all_species[s].population.size() << " - "
                      << ga.all_species[s].num_stale_generations << " - <" << connections << "> - " << eval << std::endl;
        }
        std::cout << std::endl;
        ga.add_generation();
    }
}

TEST(neat, mutation_only) {

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution unif;
    const int inputs = 3;
    double p = .9;
    neat_genepool pool(inputs, 1);
    genome g;
    for (int i = 0; i < 1; i++) pool.random_mutation(g);
    const nn_graph n(g, inputs, 1);
    double current = eval_xor3(n);
    for (int i = 0; i < 5000; i++) {
        genome child = g;
        pool.random_mutation(child);
        const nn_graph child_n(child, inputs, 1);
        double e = eval_xor3(child_n);
        p *= .99;
        if (e < current) {
            //p *= 0.5;
            current = e;
            g = child;
        } else if (unif(mt) < p) {
            std::cout << "randomly changing" << std::endl;
            current = e;
            g = child;
        }
        std::printf("generation %d: p = %.4f; e = %.6f; connections = %d\n", i, p, current, g.active_connetions_count());
        if (e < 0.001) break;
    }
    auto connections = g.connections;
    std::unordered_set<int> nodes;
    for (int i = 0; i < pool.connection_genes.size(); i++) {
        auto iter = connections.find(i);
        if (iter != connections.end() && iter->second.enabled) {
            nodes.insert(iter->second.from);
            nodes.insert(iter->second.to);
            std::cout << iter->second.weight << "     " << iter->second.from << " -> " << iter->second.to << std::endl;
        }
    }
    std::printf("nodes: %ld\n", nodes.size());
}
//
// Created by leon on 12/9/19.
//

#include <cassert>
#include <chess/neuralnet/neat.h>

using namespace chess::neural::neat;

nn_graph::nn_graph(const genome& genome_, int inputs, int outputs) : inputs(inputs), outputs(outputs), g(0, std::vector<connection>(0)) {
    int size = inputs + outputs;
    for (const auto& p : genome_.connections) {
        size = std::max(size, p.second.from);
        size = std::max(size, p.second.to);
    }
    g.resize(size + 1);

    for (const auto& p : genome_.connections) {
        g[p.second.to].push_back(p.second);
    }
}

std::vector<double> nn_graph::evaluate(const std::vector<double>& input) const {
    std::vector<double> all_values(g.size(), 0);
    std::vector<bool> evaluated(g.size(), false);

    all_values[0] = 1;
    evaluated[0] = true;
    for (int i = 1; i <= input.size(); i++) {
        all_values[i] = input[i - 1];
        evaluated[i] = true;
    }

    std::vector<double> output(outputs, 0);
    for (int i = 0; i < outputs; i++) {
        int output_index = 1 + inputs + i;
        evaluate(output_index, all_values, evaluated);
        output[i] = all_values[output_index];
    }
    return output;
}

void nn_graph::evaluate(int node, std::vector<double>& all_values, std::vector<bool>& evaluated) const {
    for (const auto& con : g[node]) if (con.enabled) {
        if (!evaluated[con.from]) evaluate(con.from, all_values, evaluated);
        all_values[node] += con.weight * all_values[con.from];
    }
    all_values[node] = 2.0 / (1.0 + std::exp(-all_values[node])) - 1;
    evaluated[node] = true;
}

bool nn_graph::has_path(int from, int to) const {
    std::vector<bool> visited(g.size(), false);
    dfs(to, visited);
    return visited[from];
}

void nn_graph::dfs(int start, std::vector<bool>& visited) const {
    for (const auto& c : g[start]) {
        if (!visited[c.from]) {
            visited[c.from] = true;
            dfs(c.from, visited);
        }
    }
}

neat_genepool::neat_genepool(int inputs, int outputs) : inputs(inputs), outputs(outputs) {
    node_genes.push_back({INPUT}); // bias node
    for (int i = 0; i < inputs; i++) {
        node_genes.push_back({INPUT});
    }
    for (int i = 0; i < outputs; i++) {
        node_genes.push_back({OUTPUT});
    }
}

int neat_genepool::mutate_add_random_connection(genome& original) {
    std::vector<int> except;
    while (true) {
        int from = select_random_from_node(original, except);
        if (from == -1) return false;
        int to = select_random_to_node(original, from);
        if (to == -1) except.push_back(from);
        else return mutate_add_connection(original, from, to, std::uniform_real_distribution(-2.0, 2.0)(mt));
    }
}

int neat_genepool::mutate_add_connection(genome& original, int from, int to, double weight) {
    int innovation_number = -1;
    if (connections_map.find(std::make_pair(from, to)) != connections_map.end()) {
        innovation_number = connections_map.at(std::make_pair(from, to));
    }
    if (innovation_number < 0) {
        innovation_number = connection_genes.size();
        connection_genes.push_back({from, to, -1});
        connections_map[std::make_pair(from, to)] = innovation_number;
    }

    if (original.contains(innovation_number)) {
        return -1;
    } else {
        original.connections[innovation_number] = {innovation_number, from, to, weight, true};
        assert(original.connections.size() <= connection_genes.size());
        return innovation_number;
    }
}

void neat_genepool::mutate_add_node_at_random(genome& original) {
    std::uniform_int_distribution dis(0);
    int size = original.connections.size();
    if (size == 0) return;
    int connection = dis(mt) % size;
    for (auto it = original.connections.begin(); it != original.connections.end(); it++) {
        if (connection-- == 0) {
            mutate_add_node(original, it->first);
            return;
        }
    }
}

void neat_genepool::mutate_add_node(genome& original, int connection_to_break) {
    auto& conn = original.connections[connection_to_break];
    int node;
    int new_connection_base_index = connection_genes[conn.connection_gene_id].new_connection_base_index;
    conn.enabled = false;
    if (new_connection_base_index > -1) {
        node = connection_genes[new_connection_base_index].to;
        original.connections[new_connection_base_index] = {new_connection_base_index, conn.from, node, 1, true};
        original.connections[new_connection_base_index + 1] = {new_connection_base_index + 1, node, conn.to, conn.weight, true};
    } else {
        node = node_genes.size();
        node_genes.push_back({HIDDEN});
        connection_genes[conn.connection_gene_id].new_connection_base_index = mutate_add_connection(original, connection_genes[conn.connection_gene_id].from, node, 1);
        mutate_add_connection(original, node, connection_genes[conn.connection_gene_id].to, conn.weight);
    }
}

genome neat_genepool::crossover(const genome& g1, const genome& g2) {

    std::uniform_int_distribution dis(0, 1);
    genome child{};

    for (auto& kv : g1.connections) {
        int i = kv.first;
        auto conn1 = kv.second;
        if (g2.contains(i)) {
            auto conn2 = g2.connections.at(i);
            if (dis(mt)) child.connections[i] = conn1;
            else child.connections[i] = conn2;
        } else {
            child.connections[i] = conn1;
        }
    }
    return child;
}

connection& neat_genepool::select_random_connection(genome& original) {
    std::uniform_int_distribution dis(0);
    assert(!original.connections.empty());
    assert(original.connections.size() > 0);
    assert(original.connections.size() <= connection_genes.size());
    assert(original.connections.begin() != original.connections.end());
    int n = dis(mt) % original.connections.size();
    assert(n >= 0);
    assert(n < original.connections.size());
    return std::next(original.connections.begin(), n)->second;
}

void neat_genepool::random_mutation(genome& original) {
    const double weight_mutation_prob = .8;
    const double weight_perturbation_prob = .99;
    const double new_connection_prob = 0.15;
    const double new_node_prob = 0.05;
    const double enable_prob = 0.2;
    const double disable_prob = 0.4;
    const double gate_prob = 0.01;

    std::uniform_real_distribution unif(.0, 1.);

    for (int i = 0; i < 10; i++)
        if (!original.connections.empty() && unif(mt) < weight_mutation_prob) {
            auto& connection = select_random_connection(original);

            if (unif(mt) < weight_perturbation_prob) {
                connection.weight += (unif(mt) - .5);
            } else {
                connection.weight = (4 * unif(mt) - 2);
            }
        }

    if (unif(mt) < new_connection_prob) {
        mutate_add_random_connection(original);
    }

    if (unif(mt) < new_node_prob) {
        mutate_add_node_at_random(original);
    }

    if (unif(mt) < enable_prob) {
        flip_random_connection(original, true);
    }
    if (unif(mt) < disable_prob) {
        flip_random_connection(original, false);
    }

    if (unif(mt) < gate_prob) {
        mutate_add_random_gate(original);
    }

//    int connections = 0;
//    for (const auto& kv : original.connections)
//        if (kv.second.enabled) connections++;
//
//    assert(connections > 0);
}

int neat_genepool::select_random_from_node(const genome& g, const std::vector<int>& except) {
    std::vector<int> nodes_from;
    std::vector<bool> node_counted(node_genes.size(), false);

    for (const int node : except) node_counted[node] = true;

    for (int i = 0; i <= inputs; i++) {
        if (!node_counted[i]) nodes_from.push_back(i);
        node_counted[i] = true;
    }
    for (const auto& c : g.connections)
        for (const int node : {c.second.from, c.second.to})
            if (!node_counted[node]) {
                node_counted[node] = true;
                if (node >= inputs + outputs + 1) nodes_from.push_back(node);
            }

    if (nodes_from.empty()) return -1;
    std::uniform_int_distribution unif(0);
    auto from = nodes_from[unif(mt) % nodes_from.size()];
    assert(node_genes[from].type != OUTPUT);
    return from;
}

int neat_genepool::select_random_to_node(const genome& g, int from) {
    std::vector<int> nodes_to;
    std::vector<bool> node_counted(node_genes.size(), false);
    node_counted[from] = true;
    assert(node_genes[from].type != OUTPUT);
    for (int i = inputs + 1; i < inputs + 1 + outputs; i++) {
        node_counted[i] = true;
        if (genome_has_connection(g, from, i)) continue;
        nodes_to.push_back(i);
    }
    nn_graph nn(g, inputs, outputs);
    std::vector<bool> visited(nn.g.size());
    nn.dfs(from, visited);

    for (const auto& c : g.connections)
        for (int node : {c.second.from, c.second.to})
            if (!node_counted[node] && !visited[node] && node >= inputs + outputs + 1) {
                node_counted[node] = true;
                if (!genome_has_connection(g, from, node)) nodes_to.push_back(node);
            }

    if (nodes_to.empty()) return -1;
    auto unif = std::uniform_int_distribution(0);
    int to = nodes_to[unif(mt) % nodes_to.size()];
    assert(!nn.has_path(to, from));
    assert(node_genes[to].type != INPUT);
    return to;
}

bool neat_genepool::genome_has_connection(const genome& g, int from, int to) const {
    auto iter = connections_map.find(std::make_pair(from, to));
    if (iter == connections_map.end()) return false;
    return g.contains(iter->second);
}

void neat_genepool::flip_random_connection(genome& g, bool to_value) {
    std::vector<connection> connections;
    for (auto& kv : g.connections) if (kv.second.enabled != to_value) connections.push_back(kv.second);
    auto unif = std::uniform_int_distribution(0);
    if (connections.empty()) return;
    connections[unif(mt) % connections.size()].enabled = to_value;
}

double neat_genepool::distance(const genome& g1, const genome& g2) const {
    int disjoint = 0;
    int coincident = 0;
    double weight_diff = 0;
    for (const auto& kv : g1.connections) {
        if (g2.contains(kv.first)) {
            coincident++;
            weight_diff += std::abs(kv.second.weight - g2.connections.at(kv.first).weight);
        } else {
            disjoint++;
        }
    }
    for (const auto& kv : g2.connections)
        if (!g1.contains(kv.first))
            disjoint++;

    //disjoint /= std::max(g1.connections.size(), g2.connections.size());
    weight_diff /= coincident;
    return disjoint + 0.01 * weight_diff;
}

void neat_genepool::mutate_add_random_gate(genome& original) {

    std::uniform_real_distribution unif;
    bool combination_found = false;
    std::vector<int> from1_except({0});
    while (!combination_found) {
        int from1 = select_random_from_node(original, from1_except);
        if (from1 < 0) return;
        int from2 = select_random_from_node(original, {0, from1});
        int to = select_random_to_node(original, from1);
        if (to < 0 || from2 < 0) {
            from1_except.push_back(from1);
            continue;
        }
        if (from2 == to) continue;
        assert(from1 != 0);
        assert(from2 != 0);
        assert(to != 0);
        assert(from1 != from2);
        assert(from1 != to);
        assert(from2 != to);
        nn_graph nn(original, inputs, outputs);
        if (nn.has_path(to, from2)) continue;
        else combination_found = true;
        int node = node_genes.size();
        node_genes.push_back({HIDDEN});

        mutate_add_connection(original, 0, node, unif(mt) * 1. - .5);
        mutate_add_connection(original, from1, node, unif(mt) * 1. - .5);
        mutate_add_connection(original, from2, node, unif(mt) * 1. - .5);
        mutate_add_connection(original, node, to, unif(mt) * 1. - .5);
    }
}

void neat_algorithm::add_generation() {
    int current_population = 0;
    int species_counted = 0;

    for (auto iter = all_species.begin(); iter != all_species.end();) {
        species_counted++;
        if ((iter->num_stale_generations >= 10 && species_counted > 3) || species_counted > 10)
            all_species.erase(iter);
        else {
            iter->population.resize(1);
            iter->num_stale_generations++;
            current_population += iter->population.size();
            iter++;
        }
    }
    new_species.clear();
    std::vector<genome> offspring;
    int offspring_count = population_size - current_population;
    offspring.reserve(offspring_count);
    std::uniform_real_distribution unif(0., 1.);
    for (int i = 0; i < offspring_count; i++) {
        if (unif(mt) < crossover_prob) {
            if (unif(mt) < 0.1) {
                int s1 = random_index(3, all_species.size());
                int s2 = random_index(3, all_species.size());
                if (s1 > s2) {
                    int tmp = s1; s1 = s2; s2 = tmp;
                } else if (s1 == s2) {
                    if (s1 == 0) {
                        if (all_species.size() > 0) s2 = 1;
                    }
                    else s1 = s2 - 1;
                }
                s1 = std::min<int>(s1, all_species.size() - 1);
                s2 = std::min<int>(s2, all_species.size() - 1);
                assert(all_species[s1].population[0].id <= last_id);
                assert(all_species[s2].population[0].id <= last_id);
                genome child = pool.crossover(all_species[s1].population[0], all_species[s2].population[0]);
                offspring.push_back(child);
            } else {
                int s = random_index(1, all_species.size());
                int p1 = random_index(2, all_species[s].population.size());
                int p2 = random_index(2, all_species[s].population.size());
                if (p1 > p2) { int tmp = p1; p1 = p2; p2 = tmp; }
                assert(all_species[s].population[p1].id <= last_id);
                assert(all_species[s].population[p2].id <= last_id);
                genome child = pool.crossover(all_species[s].population[p1], all_species[s].population[p2]);
                offspring.push_back(child);
            }
        } else {
            int s = random_index(4, all_species.size());
            int p1 = random_index(4, all_species[s].population.size());
            genome child = all_species[s].population[p1];
            offspring.push_back(child);
        }
        assert(offspring.back().connections.size() <= pool.connection_genes.size());
        pool.random_mutation(offspring.back());
        if (offspring.back().active_connetions_count() == 0) {
            offspring.pop_back();
            i--;
            continue;
        }
        offspring.back().id = last_id++;
        assign_species(offspring.back());
    }
    std::function<int(const species&, const species&)> comp = [&] (const species& s1, const species& s2) {
        return comparator(s1.population.front(), s2.population.front());
    };
    for (int i = 0; i < all_species.size(); i++) {
        if (all_species[i].num_stale_generations == 0)
            binary_search_reposition(all_species, i, comp);
    }
    for (const species& s : new_species) {
        binary_search_insert(all_species, s, comp);
    }
}

neat_algorithm::neat_algorithm(int inputs, int outputs, int population_size, genome_comparator comparator)
    : pool(neat_genepool(inputs, outputs)), population_size(population_size), comparator(comparator) {

    for (int i = 0; i < population_size; i++) {
        auto g = genome{last_id++};
        for (int j = 0; j < 1; j++) pool.mutate_add_random_connection(g);
        population.push_back(g);
        assign_species(g);
    }
    std::function<int(const species&, const species&)> comp = [&] (const species& s1, const species& s2) {
        return comparator(s1.population.front(), s2.population.front());
    };
    for (const species& s : new_species) {
        binary_search_insert(all_species, s, comp);
    }
}

void neat_algorithm::assign_species(const genome& g) {
    for (int i = 0; i < all_species.size(); i++) {
        if (pool.distance(all_species[i].population.front(), g) <= 5) {
            all_species[i].insert(g);
            return;
        }
    }
    for (int i = 0; i < new_species.size(); i++) {
        if (pool.distance(new_species[i].population.front(), g) <= 5) {
            new_species[i].insert(g);
            return;
        }
    }
    auto& s = new_species.emplace_back();
    s.id = last_species_id++;
    s.comparator = comparator;
    s.population.push_back(g);
    s.num_stale_generations = 0;
}

int neat_algorithm::random_index(double p, int max) {
    double rnd = std::uniform_real_distribution(0., 1.)(mt);
    auto ret = std::floor(std::pow(rnd, p) * max);
    assert(ret < max);
    return ret;
}

void species::insert(const genome& g) {
    if (binary_search_insert(population, g, comparator) == 0)
        num_stale_generations = 0;
}

//
// Created by leon on 12/9/19.
//

#include <cassert>
#include "neat.h"

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

std::vector<double> nn_graph::evaluate(const std::vector<double>& input) {
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

void nn_graph::evaluate(int node, std::vector<double>& all_values, std::vector<bool>& evaluated) {
    for (const auto& con : g[node]) if (con.enabled) {
        if (!evaluated[con.from]) evaluate(con.from, all_values, evaluated);
        all_values[node] += con.weight * all_values[con.from];
    }
    all_values[node] = 1.0 / (1.0 + std::exp(-all_values[node]));
    evaluated[node] = true;
}

bool nn_graph::has_path(int from, int to) {
    std::vector<bool> visited(g.size(), false);
    dfs(to, visited);
    return visited[from];
}

void nn_graph::dfs(int start, std::vector<bool>& visited) {
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

bool neat_genepool::mutate_add_random_connection(genome& original) {
    std::vector<int> nodes_from;
    std::vector<int> nodes_to;
    std::vector<bool> node_counted(node_genes.size(), false);

    for (int i = 0; i <= inputs; i++) {
        node_counted[i] = true;
        nodes_from.push_back(i);
    }
    for (int i = inputs + 1; i < inputs + outputs + 1; i++) {
        nodes_to.push_back(i);
        node_counted[i] = true;
    }
    for (const auto& con : original.connections) {
        if (con.second.from >= inputs + outputs + 1 && !node_counted[con.second.from]) {
            nodes_from.push_back(con.second.from);
            nodes_to.push_back(con.second.from);
            node_counted[con.second.from] = true;
        }
        if (con.second.to >= inputs + outputs + 1 && !node_counted[con.second.to]) {
            nodes_from.push_back(con.second.to);
            nodes_to.push_back(con.second.to);
            node_counted[con.second.to] = true;
        }
    }
    std::uniform_int_distribution dis(0);
    std::uniform_real_distribution dis2(-1.0, 1.0);
    nn_graph nn(original, inputs, outputs);
    int retries_left = 10;
    while (true) {
        if (retries_left-- == 0) return false;
        int from = nodes_from[dis(mt) % nodes_from.size()];
        if (node_genes[from].type == OUTPUT) continue;
        int to = nodes_to[dis(mt) % nodes_to.size()];
        if (to == from || node_genes[to].type == INPUT) continue;
        if (nn.has_path(to, from))
            continue;
        if (!mutate_add_connection(original, from, to, dis2(mt))) continue;
        for (auto& kv : original.connections) {
            assert(connection_genes[kv.first].from == kv.second.from);
            assert(connection_genes[kv.first].to == kv.second.to);
        }
        return true;
    }
    return false;
}

bool neat_genepool::mutate_add_connection(genome& original, int from, int to, double weight) {
    int innovation_number = -1;
    for (int i = 0; i < connection_genes.size(); i++) {
        if (connection_genes[i].from == from && connection_genes[i].to == to) {
            innovation_number = i;
            break;
        }
    }
    if (innovation_number < 0) {
        innovation_number = connection_genes.size();
        connection_genes.push_back({from, to});
    }

    if (original.connections.find(innovation_number) != original.connections.end()) {
        if (original.connections[innovation_number].enabled)
            return false;
        else {
            original.connections[innovation_number].weight = weight;
            original.connections[innovation_number].enabled = true;
            return true;
        }
    } else {
        original.connections[innovation_number] = {innovation_number, from, to, weight, true};
        return true;
    }
}

void neat_genepool::mutate_add_node_at_random(genome& original) {
    std::uniform_int_distribution dis(0);
    int size = original.connections.size();
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
    conn.enabled = false;
    int node = node_genes.size();
    node_genes.push_back({HIDDEN});
    mutate_add_connection(original, connection_genes[conn.connection_gene_id].from, node, 1);
    mutate_add_connection(original, node, connection_genes[conn.connection_gene_id].to, conn.weight);
    original.connections.erase(connection_to_break);
}

genome neat_genepool::crossover(const genome& g1, const genome& g2) {

    std::uniform_int_distribution dis(0, 1);
    genome child{};

    for (auto& kv : g1.connections) {
        int i = kv.first;
        auto conn1 = kv.second;
        if (g2.connections.find(i) != g2.connections.end()) {
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
    int size = original.connections.size();
    int connection = dis(mt) % size;
    for (auto it = original.connections.begin(); it != original.connections.end(); it++)
        if (connection-- == 0)
            return it->second;
}

void neat_genepool::random_mutation(genome& original) {
    const double weight_mutation_prob = .8;
    const double weight_perturbation_prob = .9;
    const double new_connection_prob = 0.05;
    const double new_node_prob = 0.03;
    std::uniform_real_distribution unif(.0, 1.);

    if (unif(mt) < weight_mutation_prob) {
        auto& connection = select_random_connection(original);

        if (unif(mt) < weight_perturbation_prob) {
            connection.weight += (2 * unif(mt) - 1);
        } else {
            connection.weight = (2 * unif(mt) - 1);
        }
    }

    if (unif(mt) < new_connection_prob) {
        mutate_add_random_connection(original);
    }

    if (unif(mt) < new_node_prob) {
        mutate_add_node_at_random(original);
    }
}

//
// Created by leon on 12/9/19.
//

#include "neat.h"

using namespace chess::neural::neat;

nn_graph::nn_graph(const genome& genome_, int inputs, int outputs) : inputs(inputs), outputs(outputs) {
    int size = 0;
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

    for (int i = 0; i < input.size(); i++) {
        all_values[i] = input[i];
        evaluated[i] = true;
    }

    std::vector<double> output(outputs, 0);
    for (int i = 0; i < outputs; i++) {
        evaluate(inputs + i, all_values, evaluated);
        output[i] = all_values[inputs + i];
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
    std::vector<bool> visited;
    visited.resize(g.size());
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
    for (int i = 0; i < inputs; i++) {
        node_genes.push_back({INPUT});
    }
    for (int i = 0; i < outputs; i++) {
        node_genes.push_back({OUTPUT});
    }
}

bool neat_genepool::mutate_add_random_connection(genome& original) {
    std::unordered_set<int> node_set;
    for (const auto& con : original.connections) {
        node_set.insert(con.second.from);
        node_set.insert(con.second.to);
    }
    std::vector<int> nodes(node_set.begin(), node_set.end());
    int num_nodes = nodes.size();
    bool success = false;
    std::uniform_int_distribution dis(0);
    std::uniform_real_distribution dis2(-1.0, 1.0);
    nn_graph nn(original, inputs, outputs);
    int retries_left = 100;
    while (true) {
        if (retries_left-- == 0) return false;
        int from = dis(mt) % num_nodes;
        if (node_genes[nodes[from]].type == OUTPUT) continue;
        int to = dis(mt) % num_nodes;
        if (to == from || node_genes[nodes[to]].type == INPUT) continue;
        if (nn.has_path(to, from)) continue;
        mutate_add_connection(original, from, to, dis2(mt));
        return true;
    }
}

void neat_genepool::mutate_add_connection(genome& original, int from, int to, double weight) {
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

    bool found = false;
    for (int i = 0; i < original.connections.size(); i++) {
        if (original.connections[i].connection_gene_id == innovation_number) {
            original.connections[i].weight += weight;
            found = true;
            break;
        }
    }

    if (!found) {
        original.connections[innovation_number] = {innovation_number, from, to, weight, true};
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
}

genome neat_genepool::crossover(const genome& g1, const genome& g2) {

    std::uniform_int_distribution dis(0, 1);
    genome child{};
    for (int i = 0; i < connection_genes.size(); i++) {
        const int j = i;
        if (g1.connections.find(i) != g1.connections.end()) {
            if (g2.connections.find(i) != g2.connections.end()) {
                if (dis(mt)) child.connections[i] = g1.connections.at(i);
                else child.connections[i] = g2.connections.at(i);
            } else {
                child.connections[i] = g1.connections.at(i);
            }
        } else {
            if (g2.connections.find(i) != g2.connections.end()) {
                child.connections[i] = g2.connections.at(i);
            }
        }
    }
    return child;
}

//
// Created by leon on 12/16/19.
//

#include <cmath>
#include <chess/neuralnet/neat_network.h>

using namespace chess::neural::neat;

neat_network::neat_network(int num_inputs, int num_outputs, int num_hidden) :
        num_inputs(num_inputs), num_outputs(num_outputs), num_hidden(num_hidden),
        connections(1 + num_inputs + num_outputs + num_hidden, std::vector<std::pair<int, double>>())
        {
}

constexpr int neat_network::num_nodes() {
    return 1 + num_inputs + num_outputs + num_hidden;
}

void neat_network::add_connection(int from, int to, double weight) {
    connections[to].emplace_back(from, weight);
}

std::vector<bool> neat_network::dfs(int start_node) {
    std::vector<bool> visited(1 + num_inputs + num_outputs + num_hidden, false);
    dfs(start_node, visited);
    return visited;
}

void neat_network::dfs(int start_node, std::vector<bool>& visited) {
    for (const auto& node : connections[start_node]) {
        if (visited[node.first]) continue;
        visited[node.first] = true;
        dfs(node.first, visited);
    }
}

std::vector<double> neat_network::evaluate(const std::vector<double>& inputs) {
    std::vector<double> all_values(num_nodes());
    std::vector<bool> visited(num_nodes(), false);
    std::vector<double> outputs(num_outputs);

    all_values[0] = 1;
    visited[0] = true;

    for (int i = 0; i < num_inputs; i++) {
        all_values[1 + i] = inputs[i];
        visited[1 + i] = true;
    }

    for (int o = 0; o < num_outputs; o++) {
        int node = 1 + num_inputs + o;
        if (!visited[node]) evaluate(node, all_values, visited);
        outputs[o] = all_values[node];
    }
    return outputs;
}

void neat_network::evaluate(int node, std::vector<double>& all_values, std::vector<bool>& visited) {
    visited[node] = true;
    double sum = .0;
    for (const auto& conn : connections[node]) {
        if (!visited[conn.first]) evaluate(conn.first, all_values, visited);
        sum += conn.second * all_values[conn.first];
    }
    all_values[node] = 2 / (1 + std::exp(-sum)) - 1;
}

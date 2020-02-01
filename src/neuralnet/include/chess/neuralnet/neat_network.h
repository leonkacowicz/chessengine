//
// Created by leon on 12/16/19.
//

#ifndef CHESSENGINE_NEAT_NETWORK_H
#define CHESSENGINE_NEAT_NETWORK_H

#include <vector>

namespace chess::neural::neat {


class neat_network {

public:
    const int num_inputs;
    const int num_outputs;
    const int num_hidden;

    std::vector<std::vector<std::pair<int, double>>> connections; // connections[x] list the nodes connecting into node x

    neat_network(int num_inputs, int num_outputs, int num_hidden);
    constexpr int num_nodes();
    void add_connection(int from, int to, double weight);
    std::vector<bool> dfs(int start_node);
    void dfs(int start_node, std::vector<bool>& visited);
    std::vector<double> evaluate(const std::vector<double>& inputs);
    void evaluate(int node, std::vector<double>& all_values, std::vector<bool>& visited);
};

}


#endif //CHESSENGINE_NEAT_NETWORK_H

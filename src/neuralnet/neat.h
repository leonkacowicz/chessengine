//
// Created by leon on 12/9/19.
//

#ifndef CHESSENGINE_NEAT_H
#define CHESSENGINE_NEAT_H


#include <vector>
#include <map>
#include <random>
#include <unordered_map>
#include <unordered_set>

namespace chess::neural::neat {

    enum node_type {
        INPUT, OUTPUT, HIDDEN
    };

    struct node_gene {
        node_type type;
    };

    struct connection_gene {
        int from;
        int to;
        int new_connection_base_index;
    };

    struct connection {
        int connection_gene_id;
        int from;
        int to;
        double weight;
        bool enabled;
    };

    struct genome {
        std::unordered_map<int, connection> connections;
        bool contains(int connection) const {
            return connections.find(connection) != connections.end();
        }
    };

    struct nn_graph {
        int inputs;
        int outputs;
        std::vector<std::vector<connection>> g;

        nn_graph(const genome& genome_, int inputs, int outputs);

        std::vector<double> evaluate(const std::vector<double>& input);

        void evaluate(int node, std::vector<double>& all_values, std::vector<bool>& evaluated);

        bool has_path(int from, int to);

        void dfs(int start, std::vector<bool>& visited);
    };

    struct pair_hash
    {
        template <class T1, class T2>
        std::size_t operator() (const std::pair<T1, T2> &pair) const
        {
            return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
        }
    };

    class neat_genepool {

        std::random_device rd;
        std::mt19937 mt{rd()};

    public:
        int inputs;
        int outputs;
        std::vector<node_gene> node_genes;
        std::vector<connection_gene> connection_genes;
        std::unordered_map<std::pair<int, int>, int, pair_hash> connections_map;
        int innovation = -1;

        neat_genepool(int inputs, int outputs);

        int mutate_add_random_connection(genome& original);

        void mutate_add_node_at_random(genome& original);

        int mutate_add_connection(genome& original, int from, int to, double weight);

        void mutate_add_node(genome& original, int connection_to_break);

        genome crossover(const genome& g1, const genome& g2);

        connection& select_random_connection(genome& original);

        int select_random_from_node(const genome& g, const std::vector<int>& except);

        int select_random_to_node(const genome& g, int from);

        void random_mutation(genome& original);

        bool genome_has_connection(const genome& g, int from, int to) const;

        void flip_random_connection(genome& g, bool to_value);
    };
}
#endif //CHESSENGINE_NEAT_H

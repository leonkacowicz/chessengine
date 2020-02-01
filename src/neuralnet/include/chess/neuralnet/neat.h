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
#include <functional>

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
    int id;
    std::unordered_map<int, connection> connections;

    bool contains(int connection) const {
        if (connections.empty()) return false;
        return connections.find(connection) != connections.end();
    }

    int active_connetions_count() const {
        int count = 0;
        for (const auto& kv : connections) if (kv.second.enabled) count++;
        return count;
    }
};

struct nn_graph {
    int inputs;
    int outputs;
    std::vector<std::vector<connection>> g;

    nn_graph(const genome& genome_, int inputs, int outputs);

    std::vector<double> evaluate(const std::vector<double>& input) const;

    void evaluate(int node, std::vector<double>& all_values, std::vector<bool>& evaluated) const;

    bool has_path(int from, int to) const;

    void dfs(int start, std::vector<bool>& visited) const;
};

struct pair_hash {
    template<class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& pair) const {
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

    void mutate_add_random_gate(genome& original);

    genome crossover(const genome& g1, const genome& g2);

    connection& select_random_connection(genome& original);

    int select_random_from_node(const genome& g, const std::vector<int>& except);

    int select_random_to_node(const genome& g, int from);

    void random_mutation(genome& original);

    bool genome_has_connection(const genome& g, int from, int to) const;

    void flip_random_connection(genome& g, bool to_value);

    double distance(const genome& g1, const genome& g2) const;
};

typedef std::function<int(const genome&, const genome&)> genome_comparator;

template<typename T>
int binary_search_insert(std::vector<T>& v, const T& t, std::function<int(const T&, const T&)> comp);

template<typename T>
int binary_search_insert(std::vector<T>& v, const T& t, std::function<int(const T&, const T&)> comp) {
    int begin = 0;
    int end = static_cast<int>(v.size()) - 1;

    while (begin <= end) {
        int pos = (begin + end) / 2;
        int c = comp(t, v[pos]);
        if (c == 0) {
            v.insert(v.begin() + pos, t);
            return pos;
        } else if (c < 0) end = pos - 1;
        else begin = pos + 1;
    }
    v.insert(v.begin() + begin, t);
    return begin;
}

template<typename T>
int binary_search_reposition(std::vector<T>& v, int index, std::function<int(const T&, const T&)> comp) {
//        std::vector<int> indices(index, 0);
//        for (int i = 0; i < index; i++) indices[i] = i;
    std::vector<T> subvector(v.begin(), v.begin() + index + 1);
    int pos = binary_search_insert(subvector, v[index], comp);
    for (int i = pos; i <= index; i++) {
        v[i] = subvector[i];
    }
    return pos;
}

struct species {
    int id;
    genome_comparator comparator;
    std::vector<genome> population;
    int num_stale_generations;

    void insert(const genome& g);
};

class neat_algorithm {
    const double crossover_prob = 0.5;

    std::random_device rd;
    std::mt19937 mt{rd()};
public:
    int last_id = 0;
    int last_species_id = 0;
    int population_size;
    neat_genepool pool;
    std::vector<species> all_species;
    std::vector<species> new_species;
    std::vector<genome> population;
    genome_comparator comparator;

    neat_algorithm(int inputs, int outputs, int population_size, genome_comparator comparator);

    void add_generation();

    void assign_species(const genome& g);

    int random_index(double p, int max);
};
}
#endif //CHESSENGINE_NEAT_H

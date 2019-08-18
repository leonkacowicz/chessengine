//
// Created by leon on 2019-08-11.
//

#ifndef CHESSENGINE_TRANSPOSITION_TABLE_H
#define CHESSENGINE_TRANSPOSITION_TABLE_H

#include <cstdint>
#include <vector>
#include <move.h>

struct node {
    uint64_t hash;
    int depth;
    int lower_bound;
    int upper_bound;
    std::vector<move> variation;
};

template<size_t size>
class transposition_table {
    node nodes[size];

public:
    void save(uint64_t hash, int depth, int lower_bound, int upper_bound, std::vector<move>* variation) {
        int idx = hash % size;
        auto& n = nodes[idx];
        if (n.hash == hash && n.depth < depth) return;
        if (n.depth == depth && n.lower_bound == n.upper_bound && lower_bound != upper_bound) return;
        n.lower_bound = lower_bound;
        n.upper_bound = upper_bound;
        n.hash = hash;
        n.depth = depth;
        if (variation == nullptr)
            n.variation.clear();
        else n.variation.assign(variation->begin(), variation->end());
    }

    bool load(uint64_t hash, int depth, node ** n) {
        node& m = nodes[hash % size];
        if (m.hash == hash && m.depth >= depth) {
            *n = &m;
            return true;
        } else {
            return false;
        }
    }
};


#endif //CHESSENGINE_TRANSPOSITION_TABLE_H

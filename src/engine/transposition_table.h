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
    int lower_bound;
    int upper_bound;
    std::vector<move> variation;
};

template<size_t size>
class transposition_table {
    node nodes[size];

public:
    void save(uint64_t hash, int lower_bound, int upper_bound, std::vector<move>* variation) {
        int idx = hash % size;
        nodes[idx].lower_bound = lower_bound;
        nodes[idx].upper_bound = upper_bound;
        nodes[idx].hash = hash;
        if (variation == nullptr)
            nodes[idx].variation.clear();
        else nodes[idx].variation.assign(variation->begin(), variation->end());
    }

    bool load(uint64_t hash, node ** n) {
        node& m = nodes[hash % size];
        if (m.hash == hash) {
            *n = &m;
            return true;
        } else {
            return false;
        }
    }
};


#endif //CHESSENGINE_TRANSPOSITION_TABLE_H

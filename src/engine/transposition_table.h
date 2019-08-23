//
// Created by leon on 2019-08-11.
//

#ifndef CHESSENGINE_TRANSPOSITION_TABLE_H
#define CHESSENGINE_TRANSPOSITION_TABLE_H

#include <cstdint>
#include <vector>
#include <move.h>

enum tt_node_type {
    EXACT, ALPHA, BETA
};

struct tt_node {
    uint64_t hash;
    int depth;
    int value;
    tt_node_type type;
    move bestmove;
};

template<size_t size>
class transposition_table {
    tt_node nodes[size];

public:
    void save(uint64_t hash, int depth, int value, tt_node_type type, move bestmove) {
        auto& n = nodes[hash % size];
        if (n.hash == hash && n.depth > depth) return;
        n.hash = hash;
        n.depth = depth;
        n.value = value;
        n.type = type;
        n.bestmove = bestmove;
    }

    bool load(uint64_t hash, int depth, tt_node * n) {
        tt_node& m = nodes[hash % size];
        if (m.hash == hash && m.depth >= depth) {
            *n = m;
            return true;
        } else {
            return false;
        }
    }
};


#endif //CHESSENGINE_TRANSPOSITION_TABLE_H

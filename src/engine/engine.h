//
// Created by leon on 7/28/19.
//

#ifndef CHESSENGINE_ENGINE_H
#define CHESSENGINE_ENGINE_H


#include <move.h>
#include <board.h>
#include <unordered_map>
#include "evaluator.h"
#include "zobrist.h"
#include "transposition_table.h"

class engine {
    static constexpr int max_depth = 8;

    std::vector<std::pair<move, move>> killers;
    int nodes = 0;
    int current_depth = -1;
    int cache_hit_count = 0;
    int history[2][64][64];
    transposition_table<100000000> tt;
    move bestmove;

public:
    engine();
    move search_iterate(const board& b);

    int search_widen(const board& b, int depth, int val);

    int search_root(const board& b, int depth, int alpha, int beta);

    template<bool is_pv>
    int search(const board& b, int depth, int alpha, int beta);

    void sort_moves(color c, std::vector<move>& moves, int first, move tt_move);

    void set_killer_move(move m, int ply);

    void log_score(const board& b, int val);

    int qsearch(const board& b, int ply, int alpha, int beta);
};


#endif //CHESSENGINE_ENGINE_H

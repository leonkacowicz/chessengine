//
// Created by leon on 7/28/19.
//

#ifndef CHESSENGINE_ENGINE_H
#define CHESSENGINE_ENGINE_H


#include <move.h>
#include <board.h>
#include <unordered_map>
#include <chrono>
#include "evaluator.h"
#include "zobrist.h"
#include "transposition_table.h"

class engine {
    static constexpr int max_depth = 10;

    std::vector<std::pair<move, move>> killers;
    int nodes = 0;
    int qnodes = 0;
    int current_depth = -1;
    int cache_hit_count = 0;
    int history[2][64][64];
    transposition_table<100000000> tt;
    move bestmove;
    bool can_do_null_move = true;
    bool time_over = false;

    evaluator& eval;

public:
    engine(evaluator& e);

    move timed_search(const board& b, const std::chrono::milliseconds& time);

    move search_iterate(const board& b);

    int search_widen(const board& b, int depth, int val);

    int search_root(const board& b, int depth, int alpha, int beta);

    template<bool is_pv>
    int search(const board& b, int depth, int ply, int alpha, int beta);

    void set_killer_move(move m, int ply);

    void log_score(const board& b, int val);

    int qsearch(const board& b, int ply, int alpha, int beta);

    std::vector<std::pair<move, int>>
    get_move_scores(const board& b, int ply, const std::vector<move>& moves, const move tt_move);

    void sort_moves(std::vector<std::pair<move, int>>& moves, int first);
};


#endif //CHESSENGINE_ENGINE_H

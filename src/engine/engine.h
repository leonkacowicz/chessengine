//
// Created by leon on 7/28/19.
//

#ifndef CHESSENGINE_ENGINE_H
#define CHESSENGINE_ENGINE_H


#include <move.h>
#include <board.h>
#include <unordered_map>
#include <chrono>
#include <game.h>
#include <zobrist.h>
#include "evaluator.h"
#include "transposition_table.h"

class engine {
    static constexpr int max_depth = 30;

    std::vector<std::pair<move, move>> killers;
    int nodes = 0;
    int qnodes = 0;
    int current_depth = -1;
    int cache_hit_count = 0;
    int history[2][64][64];
    transposition_table<10'000'000> tt;
    move bestmove;
    bool can_do_null_move = true;
    bool time_over = false;

    evaluator& eval;

public:
    engine(evaluator& e);

    move timed_search(game& g, const std::chrono::milliseconds& time);

    move search_iterate(game& g);

    int search_widen(game& g, int depth, int val);

    int search_root(game& g, int depth, int alpha, int beta);

    template<bool is_pv>
    int search(game& g, int depth, int ply, int alpha, int beta);

    void set_killer_move(move m, int ply);

    void log_score(const board& b, int val);

    int qsearch(const board& b, int ply, int alpha, int beta);

    std::vector<std::pair<move, int>>
    get_move_scores(const board& b, int ply, const std::vector<move>& moves, const move tt_move);

    void sort_moves(std::vector<std::pair<move, int>>& moves, int first);
};


#endif //CHESSENGINE_ENGINE_H

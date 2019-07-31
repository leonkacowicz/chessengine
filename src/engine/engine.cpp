//
// Created by leon on 7/28/19.
//

#include <deque>
#include <algorithm>

#include "engine.h"
#include "evaluator.h"
#include "zobrist.h"
std::unordered_map<uint64_t, node> transp;

void log_score(int val, move m, int depth) {
    auto mate = 32000 - std::abs(val);
    if (mate < 30) {
        std::cout << "info depth " << depth << " score mate " << (val < 0 ? "-" : "") << mate
                << " " << m.to_long_move() << std::endl;
    } else {
        std::cout << "info depth " << depth << " score cp " << val << " " << m.to_long_move() << std::endl;
    }
}

int negamax(const board& b, int depth, std::vector<move>& moves, move* selected, int alpha, int beta, int *cache_hit, int *total_nodes) {

    (*total_nodes)++;
    auto hash = zobrist::hash(b);
    if (moves.empty()) {
        // Game ended in less moves than "depth"
        if (b.under_check(b.side_to_play)) {
            int val = -32000;
            transp[hash] = {val};
            return val;
        } else {
            transp[hash] = {0};
            return 0;
        }
    }

    if (transp.find(hash + depth) != transp.end()) {
        (*cache_hit)++;
        auto n = transp[hash + depth];
        if (selected != nullptr) *selected = n.m;
        return n.val;
    }

    if (depth == 0) {
        // base case
        int val = evaluator::evaluate(b);
        val = (b.side_to_play == WHITE ? val : -val);
        transp[hash] = {val};
        return val;
    }

    int best = -32001;
    move best_m;
    for (move& m : moves) {
        board bnew = b;
        bnew.make_move(m);

        auto bnew_moves = bnew.get_legal_moves(bnew.side_to_play);

        int val;
        val = -negamax(bnew, depth - 1, bnew_moves, nullptr, -beta, -alpha, cache_hit, total_nodes);
        if (val > best) {
            best = val;
            best_m = m;
            if (selected != nullptr) {
                log_score(val, m, depth);
            }
        }
        alpha = std::max(alpha, val);
        if (alpha >= beta) {
            transp[hash + depth] = {val, m};
            return best;
        }
    }

    transp[hash + depth] = {best, best_m};
    if (selected != nullptr) *selected = best_m;
    return best;
}

move engine::get_move(const board& b) {
    int plys = 5;
    std::vector<move> legal_moves = b.get_legal_moves(b.side_to_play);
    if (legal_moves.empty()) return {};
    std::vector<move> seq(plys + 1);
    int alpha = -32001;
    int beta = 32001;
    for (int depth = 1; depth <= plys; depth++) {
        std::cerr << "Trying starting with move " << seq[depth - 1].to_long_move() << std::endl;
        seq[depth] = seq[depth - 1];
        move selected = legal_moves.front();
        int cache_hit = 0;
        int total_nodes = 0;

        if (b.side_to_play == WHITE)
            negamax(b, depth, legal_moves, &selected, alpha, beta, &cache_hit, &total_nodes);
        else
            negamax(b, depth, legal_moves, &selected, -beta, -alpha, &cache_hit, &total_nodes);

        seq[depth] = selected;
        std::cout << "info cachehit " << cache_hit << std::endl;
        std::cout << "info nodes " << total_nodes << std::endl;
        std::cout << "info currmove "; for (move m : legal_moves) std::cout << " " << m.to_long_move(); std::cout << std::endl;
        legal_moves.erase(std::find_if(legal_moves.begin(), legal_moves.end(), [&] (const move m) { return m == seq[depth]; }));
        legal_moves.insert(legal_moves.begin(), seq[depth]);
        std::cout << "info currmove "; for (move m : legal_moves) std::cout << " " << m.to_long_move(); std::cout << std::endl;

        std::cerr << "Current best move " << seq[depth].to_long_move() << std::endl;
    }
    if (seq[plys].special == NULL_MOVE) {
        return legal_moves[0];
    }
    return seq[plys];
}

engine::engine() {
    zobrist::init();
    transp.clear();
}

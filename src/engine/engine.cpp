//
// Created by leon on 7/28/19.
//

#include <deque>
#include <algorithm>

#include "engine.h"
#include "evaluator.h"
#include "zobrist.h"
std::unordered_map<uint64_t, node> transp;

void log_score(int val, std::vector<move>& variation, int depth) {
    auto mate = 32000 - std::abs(val);
    if (mate < 30) {
        std::cout << "info depth " << depth << " score mate " << (val < 0 ? "-" : "") << mate;
    } else {
        std::cout << "info depth " << depth << " score cp " << val;
    }

    for (auto m = variation.end() - 1; m != variation.begin(); m--)
        std::cout << " " << m->to_long_move();
    std::cout << std::endl;
}

int negamax(const board& b, int depth, std::vector<move>& moves, std::vector<move>& variation, int alpha, int beta, int *cache_hit, int *total_nodes) {

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
        if (!n.beta_cut && depth > 0) {
            variation[depth] = n.m;
        }
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
    bool first = true;
    auto move_pos = std::find_if(moves.begin(), moves.end(), [&](const move& m) { return m == variation[depth]; });
    if (move_pos == moves.end()){
        //std::cerr << "Principal Variation move not found in legal moves... weird" << std::endl;
    } else if (move_pos != moves.begin()) {
        moves.erase(move_pos);
        moves.insert(moves.begin(), variation[depth]);
    }
    for (move& m : moves) {
        board bnew = b;
        bnew.make_move(m);
        auto bnew_moves = bnew.get_legal_moves(bnew.side_to_play);

        int val;
        if (first) {
            val = -negamax(bnew, depth - 1, bnew_moves, variation, -beta, -alpha, cache_hit, total_nodes);
            variation[depth] = m;
            first = false;
        } else {
            std::vector<move> newvar = variation;
            val = -negamax(bnew, depth - 1, bnew_moves, newvar, -alpha - 1, -alpha, cache_hit, total_nodes);
            if (val > alpha && val < beta) {
                val = -negamax(bnew, depth - 1, bnew_moves, variation, -beta, -alpha, cache_hit, total_nodes);
            }
        }
        if (val > alpha) {
            alpha = val;
            variation[depth] = m;
            if (depth == variation.size() - 1) {
                log_score(val, variation, depth);
            }
        }
        if (alpha >= beta) {
            transp[hash + depth] = {beta, m, true};
            return alpha;
        }
    }
    transp[hash + depth] = {alpha, variation[depth], false};
    return alpha;
}

move engine::get_move(const board& b) {
    int plys = 6;
    std::vector<move> legal_moves = b.get_legal_moves(b.side_to_play);
    if (legal_moves.empty()) return {};
    std::vector<move> seq;
    int alpha = -32001;
    int beta = 32001;
    seq.emplace_back();
    for (int depth = 1; depth <= plys; depth++) {
        std::cerr << "Trying starting with move " << seq[depth - 1].to_long_move() << std::endl;
        seq.insert(seq.begin(), move());
        int cache_hit = 0;
        int total_nodes = 0;

        int val;
        if (b.side_to_play == WHITE)
            val = negamax(b, depth, legal_moves, seq, alpha, beta, &cache_hit, &total_nodes);
        else
            val = negamax(b, depth, legal_moves, seq, -beta, -alpha, &cache_hit, &total_nodes);

        std::cout << "info cachehit " << cache_hit << std::endl;
        std::cout << "info nodes " << total_nodes << std::endl;
        log_score(val, seq, depth);
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

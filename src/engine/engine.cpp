//
// Created by leon on 7/28/19.
//

#include <deque>
#include <algorithm>
#include <move_gen.h>

#include "engine.h"
#include "evaluator.h"
#include "zobrist.h"
#include "transposition_table.h"

transposition_table<10485760> transp;
//std::unordered_map<uint64_t, node> transp;

void log_score(int val, std::vector<move>& variation, int depth, int nodes) {
    auto mate = 32000 - std::abs(val);
    if (mate < 30) {
        std::cout << "info depth " << depth << " score mate " << " nodes " << nodes << (val < 0 ? "-" : "") << mate;
    } else {
        std::cout << "info depth " << depth << " score cp " << val << " nodes " << nodes ;
    }

    std::cout << " pv";
    for (int i = 0; i < variation.size(); i++) {
        std::cout << " " << to_long_move(variation[i]);
    }
    std::cout << std::endl;
}

int qsearch(const board& b, int alpha, int beta, int seldepth = 0) {
    auto moves = move_gen(b).generate();
    if (moves.empty()) {
        if (b.under_check(b.side_to_play)) return b.side_to_play == WHITE ? -MATE : MATE;
        else return 0;
    }
    int val = evaluator::evaluate(b);
    val = (b.side_to_play == WHITE ? val : -val);

    if (val >= beta) return beta;
    if (val > alpha) alpha = val;

    for (move m : moves) {
        if (b.piece_of_color[opposite(b.side_to_play)] & get_bb(move_dest(m))) {
            board bnew = b; bnew.make_move(m);
            val = -qsearch(bnew, -beta, -alpha, seldepth + 1);
            if (val >= beta) return beta;
            if (val > alpha) alpha = val;
        }
    }
    return alpha;
}

bool bring_last_best_move_to_front(std::vector<move>* moves, const std::vector<move>* prev_depth_var) {
    if (prev_depth_var != nullptr && !prev_depth_var->empty()) {
        auto last_best_move_pos = std::find(moves->begin(), moves->end(), (*prev_depth_var)[0]);
        if (last_best_move_pos != moves->end()) {
            moves->erase(last_best_move_pos);
            moves->insert(moves->begin(), (*prev_depth_var)[0]);
            return true;
        }
    }
    return false;
}

template<bool is_pv>
int search(const board& b, int depth, int alpha, int beta, std::vector<move>* variation, const std::vector<move>* prev_depth_var) {
    auto hash = zobrist::hash(b, 0);
    auto moves = move_gen(b).generate();
    if (moves.empty()) {
        if (b.under_check(b.side_to_play)) return b.side_to_play == WHITE ? -MATE : MATE;
        else return 0;
    }

    node* n;
    if (transp.load(hash, &n) && n->depth >= depth) {
        if (n->upper_bound == n->lower_bound) {
            if (is_pv && variation != nullptr && !n->variation.empty()) {
                variation->insert(variation->end(), n->variation.begin(), n->variation.end());
                if(!variation->back()) {
                    std::cout << to_long_move(variation->back()) << std::endl;
                    assert(false);
                }
            }
            return n->lower_bound;
        }
        else if (!is_pv && n->lower_bound >= beta) {
            return n->lower_bound;
        } else if (!is_pv && n->upper_bound <= alpha) {
            return n->upper_bound;
        }
    }

    if (depth == 0) {
        int val = qsearch(b, alpha, beta);
        transp.save(hash, depth, val, val, nullptr);
        return val;
    }

    std::vector<move> var;
    if (!bring_last_best_move_to_front(&moves, prev_depth_var) && depth > 5) {
//        // do a search of depth = depth - 1 to have a guess on what to try first
        search<true>(b, depth - 1, -INF, INF, &var, nullptr);
        bring_last_best_move_to_front(&moves, &var);
        variation = &var;
    }

    bool first = true;
    std::vector<std::vector<move>> variations;
    int best_idx = -1;
    int best = -INF;
    for (int i = 0; i < moves.size(); i++) {
        move m = moves[i];
        board bnew = b; bnew.make_move(m);
        variations.emplace_back();
        variations.back().push_back(m);

        int val;
        if (first) {
            std::vector<move> prev_depth_var_next;
            if (prev_depth_var != nullptr && !prev_depth_var->empty()) prev_depth_var_next.assign(prev_depth_var->begin() + 1, prev_depth_var->end());
            val = -search<true>(bnew, depth - 1, -beta, -alpha, &(variations.back()), &prev_depth_var_next);
            first = false;

        } else {
            if (-search<false>(bnew, depth - 1, -alpha - 1, -alpha, nullptr, nullptr) > alpha) {
                val = -search<true>(bnew, depth - 1, -beta, -alpha, &(variations.back()), nullptr);
            } else {
                continue;
            }
        }
        best = std::max(best, val);
        if (val > alpha) {
            if (val >= beta) {
                transp.save(hash, depth, val, INF, nullptr);
                return beta;
            }
            alpha = val;
            best_idx = i;
            best = val;
        }
    }
    if (best_idx >= 0) {
        if (is_pv && variation != nullptr) {
            variation->insert(variation->end(), variations[best_idx].begin(), variations[best_idx].end());
        }
        transp.save(hash, depth, alpha, alpha, &variations[best_idx]);
    } else if (best > -INF) {
        transp.save(hash, depth, -INF, best, nullptr);
    }
    return alpha;
}

int search_widen(const board& b, int depth, int val, std::vector<move>* variation, const std::vector<move>* prev_depth_var) {
    int alpha = val - 50;
    int beta = val + 50;
    std::vector<move> new_variation;
    int tmp = search<true>(b, depth, alpha, beta, &new_variation, prev_depth_var);
    if (tmp <= alpha || tmp >= beta)
        return search_widen(b, depth, tmp, variation, prev_depth_var);

    variation->insert(variation->begin(), new_variation.begin(), new_variation.end());
    return tmp;
}

move engine::search_iterate(const board& b) {
    const int max_depth = 8;
    auto legal_moves = move_gen(b).generate();
    if (legal_moves.empty()) return null_move;

    std::vector<move> seq[max_depth + 1];
    int cache_hit = 0;
    int total_nodes = 0;
    int val = search<true>(b, 1, -INF, +INF, &seq[1], nullptr);
    log_score(val, seq[1], 1, total_nodes);
    for (int depth = 2; depth <= max_depth; depth++) {
        val = search_widen(b, depth, val, &seq[depth], &seq[depth - 1]);
        std::cout << "info cachehit " << cache_hit << std::endl;
        log_score(val, seq[depth], depth, total_nodes);
    }
    return seq[max_depth][0];
}

engine::engine() {
    zobrist::init();
}

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

move engine::search_iterate(const board& b) {

    auto legal_moves = move_gen(b).generate();
    if (legal_moves.empty()) return null_move;

    bestmove = legal_moves[0];
    current_depth = 1;
    int val = search_root(b, current_depth, -INF, +INF);

    for (current_depth = 2; current_depth <= max_depth; current_depth++) {
        val = search_widen(b, current_depth, val);
    }
    return bestmove;
}

int engine::search_widen(const board& b, int depth, int val) {
    const int alpha = val - 50;
    const int beta = val + 50;
    const int tmp = search_root(b, depth, alpha, beta);
    if (alpha < tmp && tmp < beta) return tmp;
    return search_root(b, depth, -INF, INF);
}

int engine::search_root(const board& b, int depth, int alpha, int beta) {

    uint64_t hash = zobrist::hash(b, 0);
    auto legal_moves = move_gen(b).generate();
    int val;
    int best = -1;
    int bestval = -INF;
    for (int i = 0; i < legal_moves.size(); i++) {
        sort_moves(b.side_to_play, legal_moves, i, bestmove);
        board bnew = b;
        bnew.make_move(legal_moves[i]);
        if (best == -1) {
            val = -search<true>(bnew, depth - 1, -beta, -alpha);
        } else {
            val = -search<false>(bnew, depth - 1, -alpha - 1, -alpha);
            if (val > alpha)
                val = -search<true>(bnew, depth - 1, -beta, -alpha);
            else
                continue;
        }
        if (val > bestval) {
            bestval = val;
            best = i;
        }
        if (val > alpha) {
            best = i;
            bestmove = legal_moves[i];
            if (val >= beta) {
                tt.save(hash, depth, beta, BETA, legal_moves[i]);
                log_score(b, beta);
                return beta;
            }

            alpha = val;
            tt.save(hash, depth, alpha, ALPHA, legal_moves[i]);
            log_score(b, alpha);
        }
    }
    assert(best > -1);
    tt.save(hash, depth, alpha, EXACT, legal_moves[best]);
    return alpha;
}

template<bool is_pv>
int engine::search(const board& b, int depth, int alpha, int beta) {

    uint64_t hash = zobrist::hash(b, 0);
    int ply = current_depth - depth;
    int mate_value = MATE - ply;

    if (alpha < -mate_value) alpha = -mate_value;
    if (beta > mate_value - 1) beta = mate_value - 1;
    if (alpha >= beta) return alpha;

    tt_node node;
    move tt_move = null_move;
    if (tt.load(hash, depth, alpha, beta, &node)) {
        tt_move = node.bestmove;
        if (!is_pv || (alpha < node.value && node.value < beta)) {
            if (std::abs(node.value) > MATE - max_depth) {
                int val;
                if (node.value > 0) val = node.value - ply;
                else val = node.value + ply;
                assert(val >= -INF);
                return val;
            } else {
                return node.value;
            }
        }
    }

    bool in_check = b.under_check(b.side_to_play);
    if (depth <= 0 && !in_check)
        return qsearch(b, ply, alpha, beta);

    int val;
    auto legal_moves = move_gen(b).generate();


    if (legal_moves.empty()) {
        val = 0;
        if (in_check) {
            val = -MATE + ply;
            assert(val >= -INF);
        }
        tt.save(hash, INF, val, EXACT, null_move);
        return val;
    }

    sort_moves(b.side_to_play, legal_moves, 0, tt_move);
    bool raised_alpha = false;
    int best = -1;
    int bestval = -INF;
    tt_node_type new_tt_node_type = ALPHA;
    for (int i = 0; i < legal_moves.size(); i++) {
        // sort moves here after we have a score per move
        board bnew = b;
        move m = legal_moves[i];
        bnew.make_move(m);

        if (!raised_alpha) {
            val = -search<is_pv>(bnew, depth - 1, -beta, -alpha);
            assert(val >= -INF);
        } else {
            val = -search<false>(bnew, depth - 1, -alpha - 1, -alpha);
            assert(val >= -INF);
            if (val > alpha)
                val = -search<true>(bnew, depth - 1, -beta, -alpha);
            else
                continue;
            assert(val >= -INF);
        }

        if (val > bestval) {
            bestval = val;
            best = i;
        }

        if (val > alpha) {
            best = i;

            if (val >= beta) {

                square dest = move_dest(m);
                if (b.piece_at(get_bb(dest)) == NO_PIECE && dest != b.en_passant && move_type(m) < PROMOTION_QUEEN) {
                    set_killer_move(m, ply);
                }

                new_tt_node_type = BETA;
                alpha = beta;
                break;

            }

            raised_alpha = true;
            alpha = val;
        }
    }
    assert(best > -1);
    assert(alpha >= -INF);
    tt.save(hash, depth, alpha, new_tt_node_type, legal_moves[best]);
    return alpha;
}

void engine::sort_moves(color c, std::vector<move>& moves, int first, move tt_move) {
    int high = first;
    int high_value = -INF;
    for (int i = first; i < moves.size(); i++) {
        if (moves[i] == tt_move) {
            high = i;
            break;
        }
    }
    if (high > first) {
        move tmp = moves[first];
        moves[first] = moves[high];
        moves[high] = tmp;
    }
}

engine::engine() {

}

void engine::set_killer_move(move m, int ply) {
//    if (killers.size() <= ply) {
//        killers.resize(ply, std::make_pair(null_move, null_move));
//    }
//
//    if (killers[ply].first != m) {
//        killers[ply].second = killers[ply].first;
//        killers[ply].first = m;
//    }
}

int engine::qsearch(const board& b, int ply, int alpha, int beta) {
    nodes++;

    uint64_t hash = zobrist::hash(b, 0);
    tt_node node;
    move tt_move = null_move;
    int val;
    if (tt.load(hash, 0, alpha, beta, &node)) {
        val = node.value;
        //tt_move = node.bestmove;
    } else {
        val = evaluator::evaluate(b);
        if (b.side_to_play == BLACK) val = -val;
        tt.save(hash, 0, val, EXACT, tt_move);
    }

    if (val >= beta) return beta;
    if (val > alpha) alpha = val;

    auto legal_moves = move_gen(b).generate();

    if (legal_moves.empty()) {
        val = 0;
        if (b.under_check(b.side_to_play)) {
            val = -MATE + ply;
            assert(val >= -INF);
        }
        tt.save(hash, 1000, val, EXACT, null_move); // depth = 1000 means this evaluation is absolute, not depth dependent
        return val;
    }

    sort_moves(b.side_to_play, legal_moves, 0, tt_move);

    for (int i = 0; i < legal_moves.size(); i++) {
        move m = legal_moves[i];
        if (b.piece_at(get_bb(move_dest(m))) != NO_PIECE || move_type(m) >= PROMOTION_QUEEN || move_dest(m) == b.en_passant) {
            board bnew = b;
            bnew.make_move(m);
            val = -qsearch(bnew, ply + 1, -beta, -alpha);

            if (val > alpha) {
                if (val >= beta) return beta;
                alpha = val;
            }
        }
    }
    assert(alpha >= -INF);
    return alpha;
}

void engine::log_score(const board& b, int val) {
    int mate = MATE - std::abs(val);
    if (mate < 30) {
        if (val < 0)
            std::cout << "info depth " << current_depth << " score mate -" << mate << " nodes " << nodes << (val < 0 ? "-" : "");
        else
            std::cout << "info depth " << current_depth << " score mate " << mate << " nodes " << nodes << (val < 0 ? "-" : "");

    } else {
        std::cout << "info depth " << current_depth << " score cp " << val << " nodes " << nodes ;
    }

    std::cout << " pv " << to_long_move(bestmove);

    tt_node node{};
    board b2 = b;
    b2.make_move(bestmove);

    for (int i = 1; i < current_depth; i++) {
        auto hash = zobrist::hash(b2, 0);
        if (tt.load(hash, 0, &node) && node.bestmove != null_move) {
            std::cout << " " << to_long_move(node.bestmove);
            b2.make_move(node.bestmove);
        } else {
            break;
        }
    }

    std::cout << std::endl;
}


//
// Created by leon on 7/28/19.
//

#include <deque>
#include <algorithm>
#include <move_gen.h>
#include <thread>
#include <mutex>
#include <game.h>

#include "engine.h"
#include "evaluator.h"
#include "zobrist.h"
#include "transposition_table.h"

move engine::search_iterate(game& g) {

    time_over = false;
    auto legal_moves = move_gen(g.states.back().b).generate();
    if (legal_moves.empty()) return null_move;

    for (int c = 0; c < 2; c++)
        for (int i = 0; i < 64; i++)
            for (int j = 0; j < 64; j++)
                history[c][i][j] /= 8;
    nodes = 0;
    qnodes = 0;
    cache_hit_count = 0;
    // assuming last search was last played move, we can shift the killer move list one ply to the left and hope it will be still valid
    for (int i = 0; i < killers.size() - 1 && killers.size() > 0; i++) {
        killers[i] = killers[i + 1];
    }

    bestmove = legal_moves[0];
    current_depth = 1;
    int val = search_root(g, current_depth, -INF, +INF);

    for (current_depth = 2; current_depth <= max_depth; current_depth++) {
        if (val >= MATE - current_depth) return bestmove;
        val = search_widen(g, current_depth, val);
    }
    return bestmove;
}

int engine::search_widen(game& g, int depth, int val) {
    const int alpha = val - 50;
    const int beta = val + 50;
    const int tmp = search_root(g, depth, alpha, beta);
    if (alpha < tmp && tmp < beta) return tmp;
    return search_root(g, depth, -INF, INF);
}

int engine::search_root(game& g, int depth, int alpha, int beta) {
    if (time_over) return 0;
    auto b = g.states.back().b;
    auto hash = g.states.back().hash;
    tt_node node;
    move current_bestmove = bestmove;
    if (tt.load(hash, depth, alpha, beta, &node) && node.type == EXACT) {
        current_bestmove = node.bestmove;
    }
    auto legal_moves = get_move_scores(b, 0, move_gen(b).generate(), current_bestmove);
    int val;
    int best = -1;
    int bestval = -INF;
    for (int i = 0; i < legal_moves.size(); i++) {
        sort_moves(legal_moves, i);
        move m = legal_moves[i].first;
        g.do_move(m);
        if (best == -1) {
            val = -search<true>(g, depth - 1, 1, -beta, -alpha);
            g.undo_last_move();
        } else {
            int tmp = -search<false>(g, depth - 1, 1, -alpha - 1, -alpha);
            if (time_over) {
                g.undo_last_move();
                return 0;
            }
            if (tmp > alpha) {
                val = -search<true>(g, depth - 1, 1, -beta, -alpha);
                g.undo_last_move();
                if (time_over) return 0;
            }
            else {
                g.undo_last_move();
                continue;
            }
        }
        if (val > bestval) {
            bestval = val;
            best = i;
        }
        if (val > alpha) {
            best = i;
            current_bestmove = m;
            if (val > beta) {
                tt.save(hash, depth, beta, BETA, m);
                log_score(b, beta);
                return beta;
            }

            alpha = val;
            tt.save(hash, depth, alpha, ALPHA, m);
            log_score(b, alpha);
        }
    }
    assert(best > -1);
    bestmove = current_bestmove;
    tt.save(hash, depth, alpha, EXACT, bestmove);
    return alpha;
}

template<bool is_pv>
int engine::search(game& g, int depth, int ply, int alpha, int beta) {
    if (time_over) return 0;
    auto state = g.states.back();
    auto b = state.b;
    uint64_t hash = state.hash;
    int mate_value = MATE - ply;

    if (alpha < -mate_value) alpha = -mate_value;
    if (beta > mate_value - 1) beta = mate_value - 1;
    if (alpha >= beta) return alpha;

    tt_node node;
    move tt_move = null_move;
    int val;
    if (tt.load(hash, depth, alpha, beta, &node)) {
        cache_hit_count++;
        tt_move = node.bestmove;
        val = node.value;
        if (!is_pv || (alpha < val && val < beta)) {
            if (std::abs(val) > MATE - 100) {
                if (val > 0) val = val - ply;
                else val = val + ply;
            }
            return val;
        }
    }

    bool in_check = b.under_check(b.side_to_play);
    if (in_check) depth++;
    if (depth <= 0 && !in_check)
        return qsearch(b, ply, alpha, beta);

    nodes++;

    auto legal_moves = get_move_scores(b, ply, move_gen(b).generate(), tt_move);
    if (legal_moves.empty()) {
        val = 0;
        if (in_check) {
            val = -MATE + ply;
            tt.save(hash, INF, -MATE, EXACT, null_move);
        } else {
            tt.save(hash, INF, 0, EXACT, null_move);
        }
        return val;
    }
    if (g.is_draw()) return 0;

    if (depth < 3
        && !is_pv
        && !in_check
        && abs(beta - 1) > -MATE + 100)
    {
        int static_eval = eval.eval(b) * (b.side_to_play == BLACK ? -1 : 1);

        int eval_margin = 120 * depth;
        if (static_eval - eval_margin >= beta)
            return static_eval - eval_margin;
    }


    if (!is_pv && !in_check && depth > 2 && can_do_null_move && eval.eval(b) * (b.side_to_play == BLACK ? -1 : 1) >= beta) {
        g.do_null_move();
        can_do_null_move = false;
        int nmval;
        if (depth > 6)
            nmval = -search<is_pv>(g, depth - 4, ply + 1, -beta, -beta + 1);
        else
            nmval = -search<is_pv>(g, depth - 3, ply + 1, -beta, -beta + 1);
        can_do_null_move = true;
        g.undo_last_move();
        if (time_over) return 0;
        if (nmval >= beta) return beta;
    }


    bool raised_alpha = false;
    int best = -1;
    int bestval = -INF;
    tt_node_type new_tt_node_type = ALPHA;
    for (int i = 0; i < legal_moves.size(); i++) {
        sort_moves(legal_moves, i);
        move m = legal_moves[i].first;

        g.do_move(m);
        if (!raised_alpha) {
            val = -search<is_pv>(g, depth - 1, ply + 1, -beta, -alpha);
            g.undo_last_move();
            if (time_over) {
                return 0;
            }
        } else {
            int tmp = -search<false>(g, depth - 1, ply + 1, -alpha - 1, -alpha);
            if (time_over) {
                g.undo_last_move();
                return 0;
            }
            if (tmp > alpha) {
                val = -search<true>(g, depth - 1, ply + 1, -beta, -alpha);
                g.undo_last_move();
                if (time_over) return 0;
            }
            else {
                g.undo_last_move();
                continue;
            }
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
                    history[b.side_to_play][move_origin(m)][move_dest(m)] += depth * depth;
                    set_killer_move(m, ply);
                }
                new_tt_node_type = BETA;
                alpha = beta;
                break;
            }
            raised_alpha = true;
            new_tt_node_type = EXACT;
            alpha = val;
        }
    }
    if (alpha > MATE - 100) {
        tt.save(hash, depth, alpha + ply, new_tt_node_type, legal_moves[best].first);
    } else if (alpha < -MATE + 100) {
        tt.save(hash, depth, alpha - ply, new_tt_node_type, legal_moves[best].first);
    } else {
        tt.save(hash, depth, alpha, new_tt_node_type, legal_moves[best].first);
    }
    return alpha;
}

std::vector<std::pair<move, int>> engine::get_move_scores(const board& b, int ply, const std::vector<move>& moves, const move tt_move) {
    std::vector<std::pair<move, int>> ret;
    for (auto m : moves) {
        int score = history[b.side_to_play][move_origin(m)][move_dest(m)];
        board bnew = b;
        bnew.make_move(m);
        if (bnew.under_check(bnew.side_to_play)) score += 400000000;
        piece captured = b.piece_at(get_bb(move_dest(m)));
        if (captured != NO_PIECE || (b.piece_at(get_bb(move_origin(m))) == PAWN && move_dest(m) == b.en_passant)) {
            score += 100000100;
            if (captured == QUEEN) score += 800;
            else if (captured == ROOK) score += 400;
            else if (captured == BISHOP) score += 235;
            else if (captured == KNIGHT) score += 225;
        }
        if (b.piece_at(get_bb(move_origin(m))) == PAWN && (get_rank(move_dest(m)) % 8) == 0) score += 90000000;
        if (m == tt_move) score += 200000000;
        if (killers.size() > ply) {
            if (killers[ply].first == m) score += 80000000;
            if (killers[ply].second == m) score += 79999999;
        }
        ret.emplace_back(m, score);
    }
    return ret;
}

void engine::sort_moves(std::vector<std::pair<move, int>>& moves, int first) {
    int high = first;
    int high_value = -INF;
    for (int i = first; i < moves.size(); i++) {
        if (moves[i].second > high_value) {
            high = i;
            high_value = moves[i].second;
        }
    }
    if (high > first) {
        auto tmp = moves[first];
        moves[first] = moves[high];
        moves[high] = tmp;
    }
}

engine::engine(evaluator& e) : eval(e) {
    for (int c = 0; c < 2; c++)
        for (int i = 0; i < 64; i++)
            for (int j = 0; j < 64; j++)
                history[c][i][j] = 0;
}

void engine::set_killer_move(move m, int ply) {
    if (killers.size() <= ply) {
        killers.resize(ply + 1, std::make_pair(null_move, null_move));
    }

    if (killers[ply].first != m) {
        killers[ply].second = killers[ply].first;
        killers[ply].first = m;
    }
}

int engine::qsearch(const board& b, int ply, int alpha, int beta) {
    if (time_over) return 0;
    nodes++;
    qnodes++;
    uint64_t hash = zobrist::hash(b);
    tt_node node;
    move tt_move = null_move;
    int val;
    if (tt.load(hash, 0, &node)) {
        val = node.value;
        tt_move = node.bestmove;
    } else {
        val = eval.eval(b);
        if (b.side_to_play == BLACK) val = -val;
        tt.save(hash, 0, val, EXACT, tt_move);
    }

    if (val >= beta) return beta;
    if (val > alpha) alpha = val;

    auto legal_moves = get_move_scores(b, ply, move_gen(b).generate(), tt_move);
    if (legal_moves.empty()) {
        val = 0;
        if (b.under_check(b.side_to_play)) {
            val = -MATE + ply;
            tt.save(hash, 1000, -MATE, EXACT, tt_move);
        } else {
            tt.save(hash, 1000, 0, EXACT, tt_move);
        }
        return val;
    }

    for (int i = 0; i < legal_moves.size(); i++) {
        sort_moves(legal_moves, i);
        move m = legal_moves[i].first;
        if (b.piece_at(get_bb(move_dest(m))) != NO_PIECE || move_type(m) >= PROMOTION_QUEEN || (move_dest(m) == b.en_passant && b.piece_at(get_bb(move_origin(m))) == PAWN)) {
            board bnew = b;
            bnew.make_move(m);
            val = -qsearch(bnew, ply + 1, -beta, -alpha);
            if (time_over) return 0;
            if (val > alpha) {
                if (val >= beta) return beta;
                alpha = val;
            }
        }
    }
    return alpha;
}

void engine::log_score(const board& b, int val) {
    int mate = MATE - std::abs(val);
    std::cout << "info depth " << current_depth;
    if (mate < 30) {
        if (val < 0)
            std::cout << " score mate -" << mate;
        else
            std::cout << " score mate " << mate;

    } else {
        std::cout << " score cp " << val;
    }
    std::cout << " nodes " << nodes;
    std::cout << " qnodes " << qnodes;
    std::cout << " tthit " << cache_hit_count;
    std::cout << " pv " << to_long_move(bestmove);
    tt_node node{};
    board b2 = b;
    b2.make_move(bestmove);

    for (int i = 1; i < current_depth; i++) {
        auto hash = zobrist::hash(b2);
        if (tt.load(hash, 0, &node) && node.bestmove != null_move) {
            std::cout << " " << to_long_move(node.bestmove);
            b2.make_move(node.bestmove);
        } else {
            break;
        }
    }

    std::cout << std::endl;
}

move engine::timed_search(game& g, const std::chrono::milliseconds& time) {
    std::timed_mutex mutex;
    mutex.lock();
    time_over = false;
    std::thread thr([&] () {
        search_iterate(g);
        mutex.unlock();
    });
    if (!mutex.try_lock_for(time)) {
        std::cerr << "Timeout" << std::endl;
        time_over = true;
    }
    thr.join();
    return bestmove;
}


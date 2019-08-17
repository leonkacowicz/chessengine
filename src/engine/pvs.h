//
// Created by leon on 2019-08-14.
//

#ifndef CHESSENGINE_PVS_H
#define CHESSENGINE_PVS_H

#include <vector>
#include <board.h>
#include <move_gen.h>
#include <algorithm>
#include <memory>
#include "evaluator.h"

enum search_type {
    PV, NON_PV, QUIESCENCE
};

struct pvs_node {
    board position;
    int lower_bound = -INF;
    int upper_bound = INF;
    bool is_terminal = false;
    move move_done;
    pvs_node* parent = nullptr;
    pvs_node* next = nullptr;
    int beta_cutoffs = 0;
    std::vector<std::unique_ptr<pvs_node>> children;

    pvs_node(const board& b) : position(b) {}

    bool operator<(const pvs_node& other) const {
        if (other.lower_bound == other.upper_bound && other.lower_bound > lower_bound) return true;
        if (other.lower_bound == other.upper_bound && other.upper_bound > upper_bound) return true;
        if (upper_bound < other.lower_bound) return true;
        if (lower_bound > other.upper_bound) return false;
        if (other.lower_bound > lower_bound) return true;
        return upper_bound < other.upper_bound;
    }
};

class pvs {
    pvs_node root;

public:
    pvs(const board& board) : root(board) {}

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

    bool check_if_node_is_terminal(pvs_node* node, std::vector<move>& moves) {
        if (moves.empty()) {
            node->is_terminal = true;
            if (node->position.under_check(node->position.side_to_play)) { // CHECK MATE
                node->lower_bound = node->upper_bound = root.position.side_to_play == node->position.side_to_play ? -MATE : MATE;
            } else { // STALE MATE
                node->lower_bound = node->upper_bound = 0;
            }
            return true;
        }
        return false;
    }

    void expand_node(pvs_node* node, std::vector<move>& moves) {
        for (int i = 0; i < moves.size(); i++) {
            board bnew = node->position;
            bnew.make_move(moves[i]);
            node->children.push_back(std::make_unique<pvs_node>(bnew));
            node->children.back()->move_done = moves[i];
            node->children.back()->parent = node;
        }
    }

    bool move_is_quiet(pvs_node& node) {
        return false;
    }

    template <search_type type>
    int search(pvs_node* node, int depth, int alpha = -INF, int beta = INF) {
        if (node->children.empty() && (depth > 0 || type == QUIESCENCE) && !node->is_terminal) {
            move_gen generator(node->position);
            auto& moves = generator.generate();
            if (check_if_node_is_terminal(node, moves)) return node->lower_bound;
            expand_node(node, moves);
            return search<type>(node, depth);
        } else {
            if (node->is_terminal) return node->lower_bound; // already calculated
            if (depth == 0 && type != QUIESCENCE) {
                // do quiescence search
                node->lower_bound = node->upper_bound = evaluator::evaluate(node->position) * (node->position.side_to_play == root.position.side_to_play ? 1 : -1);
                return node->lower_bound;
            }

            int best = -INF;
            int best_index = -1;
            int val;
            node->lower_bound = -INF;
            node->upper_bound = INF;
            node->beta_cutoffs = 0;
            for (int i = 0; i < node->children.size(); i++) {
                if (type == QUIESCENCE && move_is_quiet(*node->children[i])) continue;
                if (best == -INF || type == QUIESCENCE) {
                    val = -search<type>(node->children[i].get(), depth - 1, -beta, -alpha);
                } else {
                    val = -search<NON_PV>(node->children[i].get(), depth - 1, -alpha - 1, -alpha);
                    if (val > alpha)
                        val = -search<PV>(node->children[i].get(), depth - 1, -beta, -alpha);
                }
                node->beta_cutoffs += node->children[i]->beta_cutoffs;
                if (val > best) {
                    best = val;
                    if (best > alpha) {
                        alpha = best;
                        if (alpha >= beta) {
                            node->upper_bound = std::min(node->upper_bound, beta);
                            node->beta_cutoffs += 1;
                            node->children.clear();
                            return beta;
                        }
                    }
                    best_index = i;
                    node->lower_bound = std::max(node->lower_bound, val);
                }
            }
            node->upper_bound = alpha;
            if (best_index > -1) {
                node->next = node->children[best_index].get();
            }
            std::sort(node->children.begin(), node->children.end());
            return alpha;
        }
    }

    int tree_size(pvs_node* tree) {
        if (tree->children.empty()) return 1;
        int size = 0;
        for (int i = 0; i < tree->children.size(); i++) size += tree_size(tree->children[i].get());
        return size;
    }

    move search_move(int depth) {
        for (int i = 1; i <= depth; i++) {
            search<PV>(&root, i);
            printf("%d\n", tree_size(&root));
        }
        return root.children[0]->move_done;
    }
};


#endif //CHESSENGINE_PVS_H

//
// Created by leon on 2019-08-12.
//

#ifndef CHESSENGINE_MCTS_H
#define CHESSENGINE_MCTS_H


#include <color.h>
#include <move.h>
#include <queue>
#include <board.h>
#include <move_gen.h>
#include <random>
#include <memory>
#include <algorithm>
#include "evaluator.h"

struct mcts_node {
    int num_victories = 0;
    int num_games = 0;
    board board_state;
    move prev_move = null_move;
    std::vector<mcts_node> children;
    mcts_node* parent = nullptr;
    bool node_is_terminal = false;

    mcts_node(const board & b) : board_state(b) {}

    double order_value() const {
        return double(num_victories)/double(num_games) + 1.004142 * std::sqrt(std::log(parent->num_games * .5) / double(.5 * num_games));
    }

    bool operator<(const mcts_node& other) const {
        //return num_victories * other.num_games < other.num_victories * num_games;
        if (parent == nullptr) return false;
        if (num_games == 0) return false;
        return order_value() < other.order_value();
    }
};

class mcts {
    std::default_random_engine gen;
    std::uniform_int_distribution<int> dis{0, 100000};
public:
    mcts_node root;
    mcts(const board& board) : root(board) { }

    int select_best(const mcts_node& parent, const mcts_node** node) {
        int depth = 0;
        const mcts_node* current = &parent;
        double value;
        while (true) {
            if (current->children.empty()) {
                *node = current;
                return depth;
            }

            double max_value = -INFINITY;
            const mcts_node* next = nullptr;
            for (const mcts_node& n : current->children) {
                if (n.num_games == 0) continue;
                value = double(n.num_victories)/double(n.num_games);
                if (value > max_value) {
                    max_value = value;
                    next = &n;
                }
            }
            if (next == nullptr) {
                *node = current;
                return depth;
            }
            current = next;
            depth++;
            //return 1 + select_best(*next, node);
        }
    }

    int select(const mcts_node& parent, const mcts_node** node) {
        int depth = 0;
        const mcts_node* current = &parent;

        while (true) {
            if (current->children.empty()) {
                *node = current;
                return depth;
            }

            double max_value = -INFINITY;
            const mcts_node* next = nullptr;
            for (const mcts_node& n : current->children) {
                if (n.num_games == 0) {
                    *node = &n;
                    return depth;
                }
                auto value = n.order_value();
                if (value > max_value) {
                    max_value = value;
                    next = &n;
                }
            }
            current = next;
            depth++;
        }
    }

    void expand(mcts_node* node) {
        auto gen = move_gen(node->board_state);
        auto& moves = gen.generate();
        node->children.reserve(moves.size());
        if (moves.empty()) {
            std::cout << "Found terminal node!\n\n\n";
        }
        for (move& m : moves) {
            board bnew = node->board_state;
            bnew.make_move(m);
            mcts_node new_child(bnew);
            //std::cout << to_long_move(m) << std::endl;
            //new_child.board_state.print();
            new_child.num_games = 0;
            new_child.num_victories = 0;
            new_child.prev_move = m;
            new_child.parent = node;
            node->children.push_back(new_child);
        }
    }

    int simulate(const mcts_node& node) {
        board b = node.board_state;
//        int evaluate = evaluator().evaluate(b);
//        evaluate = evaluate > 150 ? 1 : evaluate < -150 ? -1 : 0;
//        if (b.side_to_play == WHITE) return -evaluate;
//        if (b.side_to_play == BLACK) return evaluate;

        for (int i = 0; i < 100; i++) {
            auto mgen = move_gen(b);
            std::vector<move>& moves = mgen.generate();
            if (moves.empty()) {
                if (b.under_check(b.side_to_play)) {
                    // result is reversed, because this is result is the outcome of the parent making this move, so this result is associated with the node
                    // that represents the outcome of the move
                    if (b.side_to_play == node.board_state.side_to_play) {
                        return 1;
                    } else {
                        return -1;
                    }
                }
                return 0;
            }
            b.make_move(moves[dis(gen) % moves.size()]); // make some random move;
        }
        return 0;
    }

    void backprogate(mcts_node* node, int result) {
        while (node != nullptr) {
            node->num_games += 2;
            node->num_victories += result + 1;
            node = node->parent;
            result = -result;
        }
    }

    void print_tree(const mcts_node& root, int indent = 0) {
        if (indent == 2) return;
        for (int i = 0; i < indent; i++) std::cout << "    ";
        std::cout << to_long_move(root.prev_move) << ": " << root.num_victories << " / " << root.num_games << std::endl;
        int num_untried = 0;
        for (auto const& child : root.children) {
            if (child.num_games > 0)
                print_tree(child, indent + 1);
            else num_untried++;
        }
        if (indent < 1 && num_untried) {
            for (int i = 0; i < indent; i++) std::cout << "    ";
            std::cout << num_untried << " untried moves" << std::endl;
        }
    }

    void print_variation(mcts_node* node) {
        std::vector<move> moves;
        double prob = 0;
        while (node->parent != nullptr) {
            moves.push_back(node->prev_move);
            prob = double(node->num_victories) / double(node->num_games);
            node = node->parent;
        }

        printf("Win prob: %.2f moving ", 100 * prob);
        for (int i = moves.size(); i > 0; i--) std::cout << to_long_move(moves[i - 1]) << " ";
        std::cout << std::endl;
    }

    move get_best_move(int depth) {
        expand(&root);
        int last_depth = -1;
        while (true) {
            mcts_node* current;
            auto current_depth = select(root, (const mcts_node**) &current);
            if (current_depth >= depth) {
                select_best(root, (const mcts_node**) &current);
                while (current->parent->parent != nullptr) current = current->parent;
                return current->prev_move;
            }

            if (current->num_games == 0) {
                int result = simulate(*current);
                backprogate(current, result);
            } else {
                expand(current);
                int result = simulate(*current);
                backprogate(current, result);
            }
            select_best(root, (const mcts_node**) &current);
            printf("Current depth: %d\n", current_depth);
            print_variation(current);
            print_tree(root);
            if (current_depth > last_depth) {
                last_depth = current_depth;
                std::cout.flush();
            }
        }
    }
};


#endif //CHESSENGINE_MCTS_H

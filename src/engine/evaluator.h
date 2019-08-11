//
// Created by leon on 7/28/19.
//

#ifndef CHESSENGINE_EVALUATOR_H
#define CHESSENGINE_EVALUATOR_H


#include <board.h>
#include <random>

enum value : int {
    INF = 32001, MATE = 32000, DRAW = 0, CERTAIN_VICTORY = 10000
};

class evaluator {
//    std::default_random_engine gen;
//    std::uniform_int_distribution<int> dis{0, 100};

public:
    static int evaluate(const board& b);
};


#endif //CHESSENGINE_EVALUATOR_H

//
// Created by leon on 7/28/19.
//

#ifndef CHESSENGINE_EVALUATOR_H
#define CHESSENGINE_EVALUATOR_H


#include <board.h>
#include <random>

class evaluator {
//    std::default_random_engine gen;
//    std::uniform_int_distribution<int> dis{0, 100};
    int rnd = 0;
public:
    int evaluate(const board& b);
};


#endif //CHESSENGINE_EVALUATOR_H

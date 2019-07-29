//
// Created by leon on 7/28/19.
//

#include <random>
#include "engine.h"

move engine::get_move() const {

    // return first legal move known;
    const std::vector<move> legal_moves = b.get_legal_moves(b.side_to_play);
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dis(0, legal_moves.size() - 1);
    return legal_moves[dis(gen)];
}

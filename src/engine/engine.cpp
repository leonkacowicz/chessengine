//
// Created by leon on 7/28/19.
//

#include "engine.h"

move engine::get_move() const {

    // return first legal move known;
    return b.get_legal_moves(b.side_to_play).front();
}

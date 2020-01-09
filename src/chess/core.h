//
// Created by leon on 2019-11-16.
//

#ifndef CHESSENGINE_CORE_H
#define CHESSENGINE_CORE_H


namespace chess::core {

void init();

enum piece : int {
    PAWN = 0, KNIGHT, BISHOP, ROOK, QUEEN, KING, NO_PIECE
};

enum color : char {
    WHITE = 0, BLACK = 1
};

constexpr color opposite(color c) {
    return static_cast<color>((unsigned)c ^ 1u);
}

}

#endif //CHESSENGINE_CORE_H

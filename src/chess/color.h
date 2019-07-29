//
// Created by leon on 07/05/19.
//

#ifndef CHESSENGINE_COLOR_H
#define CHESSENGINE_COLOR_H

enum color : char {
    WHITE = 0, BLACK = 1
};

constexpr color opposite(color c) {
    return c == BLACK ? WHITE : BLACK;
}

#endif //CHESSENGINE_COLOR_H

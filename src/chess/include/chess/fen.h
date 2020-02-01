//
// Created by leon on 2019-07-29.
//

#ifndef CHESSENGINE_FEN_H
#define CHESSENGINE_FEN_H


#include <chess/board.h>

namespace chess::core::fen {
    char fen_char(piece p, color c);
    std::string to_string(const board& b, int full_move_counter = 1);
    board board_from_fen(const std::string&);
}


#endif //CHESSENGINE_FEN_H

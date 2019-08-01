//
// Created by leon on 2019-07-29.
//

#ifndef CHESSENGINE_FEN_H
#define CHESSENGINE_FEN_H


#include "board.h"

class fen {
public:

    static char fen_char(piece p, color c) {
        const char offset = (c == BLACK ? 'a' - 'A' : (char)0);
        switch (p) {
            case PAWN: return 'P' + offset;
            case KNIGHT: return 'N' + offset;
            case BISHOP: return 'B' + offset;
            case ROOK: return 'R' + offset;
            case QUEEN: return 'Q' + offset;
            case KING: return 'K' + offset;
            default: return '.';
        }
    }

    static std::string to_string(const board& b, int full_move_counter = 1) {
        std::stringstream res;
        for (int r = 7; r >= 0; r--) {
            int rank_counter = 0;
            for (int f = 0; f < 8; f++) {
                bitboard sq(square(f, r));
                piece p = b.piece_at(sq);
                if (p == NONE) {
                    rank_counter++;
                    continue;
                } else {
                    if (rank_counter > 0) {
                        res << rank_counter;
                        rank_counter = 0;
                    }
                    color c = b.color_at(sq);
                    res << fen_char(p, c);
                }
            }
            if (rank_counter > 0) res << rank_counter;
            if (r > 0) res << '/';
        }
        res << ' ' << (b.side_to_play == WHITE ? 'w' : 'b') << ' ';
        res << (b.can_castle_king_side[WHITE] ? 'K' : '-');
        res << (b.can_castle_queen_side[WHITE] ? 'Q' : '-');
        res << (b.can_castle_king_side[BLACK] ? 'k' : '-');
        res << (b.can_castle_queen_side[BLACK] ? 'q' : '-');

        res << ' ';
        if (b.en_passant == square::none) res << '-';
        else res << b.en_passant.to_string();
        res << ' ';

        res << (int)b.half_move_counter;
        res << ' ' << full_move_counter;
        return res.str();
    }
};


#endif //CHESSENGINE_FEN_H
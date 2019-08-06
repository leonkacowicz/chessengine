//
// Created by leon on 2019-07-29.
//

#ifndef CHESSENGINE_FEN_H
#define CHESSENGINE_FEN_H


#include "board.h"

class fen {
public:

    static char fen_char(piece p, color c) {
        const char offset = (char)(c == BLACK ? 'a' - 'A' : 0);
        switch (p) {
            case PAWN: return (char)('P' + offset);
            case KNIGHT: return (char)('N' + offset);
            case BISHOP: return (char)('B' + offset);
            case ROOK: return (char)('R' + offset);
            case QUEEN: return (char)('Q' + offset);
            case KING: return (char)('K' + offset);
            default: return '.';
        }
    }

    static std::string to_string(const board& b, int full_move_counter = 1) {
        std::stringstream res;
        for (int r = 7; r >= 0; r--) {
            int rank_counter = 0;
            for (int f = 0; f < 8; f++) {
                auto sq = get_bb(f, r);
                piece p = b.piece_at(sq);
                if (p == NO_PIECE) {
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
        if (b.en_passant == SQ_NONE) res << '-';
        else res << b.en_passant;
        res << ' ';

        res << (int)b.half_move_counter;
        res << ' ' << full_move_counter;
        return res.str();
    }
};


#endif //CHESSENGINE_FEN_H

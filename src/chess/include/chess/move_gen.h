//
// Created by leon on 2019-08-03.
//

#ifndef CHESSENGINE_MOVE_GEN_H
#define CHESSENGINE_MOVE_GEN_H

#include <cassert>
#include "board.h"
#include "magic_bitboard.h"


namespace chess::core {
class move_gen {
    const board& b;
    move moves_array[220];
    move* last_move = moves_array;
    bitboard checkers = 0;
    bitboard king = 0;
    bitboard pinned = 0;
    color us;
    color them;
    bitboard our_piece = 0;
    bitboard their_piece = 0;
    bitboard any_piece = 0;
    bitboard block_mask = 0;

    enum evasiveness {
        EVASIVE, NON_EVASIVE
    };

    inline bool square_attacked(square sq);

    inline void generate_king_moves();

    inline void reset();

    template<evasiveness e>
    inline void generate_non_king_moves();

    inline void scan_board();

    template<evasiveness e>
    inline void rook_moves(bitboard origin);

    template<evasiveness e>
    inline void bishop_moves(bitboard origin);

    template<evasiveness e>
    inline void knight_moves(bitboard origin);

    template<evasiveness e, shift_direction d>
    inline void pawn_moves(bitboard origin);

    template<evasiveness e>
    inline void pawn_captures(bool promotion, square origin_sq, bitboard cap);

    template<color us>
    inline void castle_moves();

    inline void add_move(square from, square to, special_move special);

    inline void add_move(square from, square to);

public:

    explicit move_gen(const board& b);

    std::vector<move> generate();
};
}

#endif //CHESSENGINE_MOVE_GEN_H
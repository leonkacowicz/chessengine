#ifndef CHESSENGINE_BOARD_H
#define CHESSENGINE_BOARD_H

#include <vector>
#include "core.h"
#include "bitboard.h"
#include "square.h"
#include "move.h"

namespace chess::core {
class board {
public:
    void move_piece(square from, square to, piece p, color c);

    bitboard piece_of_color[2] = {0, 0};
    bitboard piece_of_type[5] = {0, 0, 0, 0, 0};
    square king_pos[2] = {SQ_NONE, SQ_NONE};
    square en_passant = SQ_NONE;
    bool can_castle_king_side[2] = {false, false};
    bool can_castle_queen_side[2] = {false, false};
    color side_to_play = WHITE;
    char half_move_counter = 0;

    board();

    bool under_check(color c) const;
    bool under_check() const { return under_check(side_to_play); };

    void set_initial_position();

    std::string to_string() const;

    void print() const;

    void put_piece(piece p, color c, square s);

    void set_king_position(color c, square position);

    void make_move(square from, square to);

    void make_move(const move m);

    board simulate(const square from, const square to, piece p, color c) const;

    std::string move_in_pgn(const move m, const std::vector<move>& legal_moves) const;

    bool resets_half_move_counter(const move m);

    bool operator==(const board& other) const;

    inline piece piece_at(bitboard p) const {
        return (p == get_bb(king_pos[WHITE])) ? KING :
               (p == get_bb(king_pos[BLACK])) ? KING :
               piece_of_type[PAWN] & p ? PAWN :
               piece_of_type[KNIGHT] & p ? KNIGHT :
               piece_of_type[BISHOP] & p ? BISHOP :
               piece_of_type[ROOK] & p ? ROOK :
               piece_of_type[QUEEN] & p ? QUEEN : NO_PIECE;
    }

    inline color color_at(bitboard p) const {
        return piece_of_color[BLACK] & p ? BLACK : WHITE;
    }

    board flip_colors() const;
};
}
#endif //CHESSENGINE_BOARD_H

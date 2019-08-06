#ifndef CHESSENGINE_BOARD_H
#define CHESSENGINE_BOARD_H

#include <vector>
#include <functional>
#include "bitboard.h"
#include "square.h"
#include "move.h"
#include "piece.h"
#include "color.h"

class board {
public:
    template<shift_direction d>
    bitboard shift_attacks(const bitboard origin, const bitboard in_range) const;

    template<shift_direction d>
    void shift_moves(const bitboard origin, const bitboard in_range, std::vector<move>& moves, piece p, color c) const;


    void add_rook_moves(bitboard origin, std::vector<move>& moves, piece p, color c) const;
    void add_king_moves(bitboard origin, std::vector<move>& moves) const;
    void add_bishop_moves(bitboard origin, std::vector<move>& moves, piece p, color c) const;
    void add_knight_moves(bitboard origin, std::vector<move>& moves) const;
    void add_pawn_moves(bitboard origin, std::vector<move>& moves) const;
    void add_castle_moves(color c, std::vector<move> &moves) const;

    void move_piece(square from, square to, piece p, color c);


    bitboard piece_of_color[2] = {0, 0};
    bitboard piece_of_type[5] = {0, 0, 0, 0, 0};
    square king_pos[2] = { SQ_NONE, SQ_NONE };
    square en_passant;
    bool can_castle_king_side[2] = { false, false };
    bool can_castle_queen_side[2] = { false, false };
    color side_to_play = WHITE;
    char half_move_counter = 0;
    board() {}

    board(const std::string& fen);

    bool under_check(color c) const;
    void set_initial_position();
    std::string to_string() const;
    void print() const;

    void put_piece(piece p, color c, square s);
    void set_king_position(color c, square position);

    std::vector<move> get_legal_moves(color c) const;

    void make_move(const move m);

    board simulate(const square from, const square to, piece p, color c) const;

    std::string move_in_pgn(const move m, const std::vector<move>& legal_moves) const;

    bool resets_half_move_counter(const move m);

    bool operator==(const board& other) const;

    inline piece piece_at(bitboard p) const {
        return (p == bb(king_pos[WHITE])) ? KING :
               (p == bb(king_pos[BLACK])) ? KING :
               piece_of_type[PAWN] & p ? PAWN :
               piece_of_type[KNIGHT] & p ? KNIGHT :
               piece_of_type[BISHOP] & p ? BISHOP :
               piece_of_type[ROOK] & p ? ROOK :
               piece_of_type[QUEEN] & p ? QUEEN : NONE;
    }
    inline color color_at(bitboard p) const {
        return piece_of_color[BLACK] & p ? BLACK : WHITE;
    }
};

#endif //CHESSENGINE_BOARD_H

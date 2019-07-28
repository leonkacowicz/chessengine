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
    bitboard piece_of_color[2] = {0, 0};
    bitboard piece_of_type[5] = {0, 0, 0, 0, 0};
    square king_pos[2] = { square::none, square::none };
    square en_passant;
    bool can_castle_king_side[2] = { false, false };
    bool can_castle_queen_side[2] = { false, false };

    bitboard range_attacks(const bitboard origin, const std::function<bitboard(bitboard)>& shift, const bitboard in_range) const;
    bitboard knight_attacks(const bitboard origin) const;
    bitboard king_attacks(const bitboard origin) const;
    bitboard pawn_attacks(const bitboard origin, const color attacker) const;

    void range_moves(const bitboard origin, const std::function<bitboard(bitboard)>& shift, const bitboard in_range, std::vector<move>& moves) const;
    void add_rook_moves(bitboard origin, std::vector<move>& moves) const;
    void add_king_moves(bitboard origin, std::vector<move>& moves) const;
    void add_bishop_moves(bitboard origin, std::vector<move>& moves) const;
    void add_knight_moves(bitboard origin, std::vector<move>& moves) const;
    void add_pawn_moves(bitboard origin, std::vector<move>& moves) const;
    void add_castle_moves(color c, std::vector<move> &moves) const;

    void move_piece(square from, square to);

    piece piece_at(bitboard p) const;
    color color_at(bitboard p) const;
public:
    color side_to_play = WHITE;

    board() {}

    board(const std::string& fen);

    bool under_check(color c) const;
    bool is_checkmate() const;
    bool is_stalemate() const;
    void set_initial_position();
    std::string to_string() const;
    void print() const;

    void put_piece(piece p, color c, square s);
    void set_king_position(color c, square position);

    std::vector<move> get_legal_moves(color c) const;

    void make_move(const move m);
};

#endif //CHESSENGINE_BOARD_H

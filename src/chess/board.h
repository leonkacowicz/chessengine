#ifndef CHESSENGINE_BOARD_H
#define CHESSENGINE_BOARD_H

#include <vector>
#include "bitboard.h"
#include "square.h"
#include "move.h"
#include "piece.h"
#include "color.h"

class board {
    bitboard piece_of_color[2];
    bitboard piece_of_type[5];
    bitboard attacks[2];

    struct {
        square up;
        square down;
        square upleft;
        square upright;
        square left;
        square right;
        square downleft;
        square downright;
    } pinned[2];
    square king_pos[2];

    bool en_passant;
    bool can_castle_king_side[2];
    bool side_to_play;

    void add_rook_moves(bitboard origin, std::vector<move>& moves) const;
    void add_king_moves(bitboard origin, std::vector<move>& moves) const;
    void add_bishop_moves(bitboard origin, std::vector<move>& moves) const;
    void add_knight_moves(bitboard origin, std::vector<move>& moves) const;
    void add_pawn_moves(bitboard origin, std::vector<move>& moves) const;
    void add_castle_moves(std::vector<move>& moves) const;

    void calculate_bishop_attacks(bitboard origin);
    void calculate_rook_attacks(bitboard origin);
    void calculate_knight_attacks(bitboard origin);
    void calculate_king_attacks(bitboard origin);
    void calculate_pawn_attacks(bitboard origin);

    template <piece p>
    void add_possible_moves(bitboard origin, std::vector<move>& moves) const;

public:

    board() :
        piece_of_color{0, 0},
        piece_of_type{0, 0, 0, 0, 0}
    {
        set_king_position(WHITE, "e1");
        set_king_position(BLACK, "e8");
        calculate_attacks();
    }

    board(const std::string& fen);

    bool is_checkmate() const;
    bool is_stalemate() const;
    void set_initial_position();
    void calculate_attacks();
    std::string to_string() const;
    void print() const;

    void put_piece(piece p, color c, square s);
    void set_king_position(color c, square position);

    bitboard get_attacks(color color) const;
    std::vector<move> get_legal_moves(color c) const;

};

#endif //CHESSENGINE_BOARD_H

#ifndef CHESSENGINE_BOARD_H
#define CHESSENGINE_BOARD_H

#include <vector>
#include "bitboard.h"
#include "square.h"
#include "Move.h"
#include "piece.h"
#include "color.h"

class Board {
    bitboard piece_of_color[2];
    bitboard piece_of_type[5];
    bitboard attacks[2];
    bitboard pinned;
    square king_pos[2];

    bool en_passant;
    bool can_castle_king_side[2];
    bool side_to_play:1;

    void addRookPossibleMoves(bitboard board, std::vector<Move> &moves) const;
    void addKingPossibleMoves(bitboard origin, std::vector<Move> &moves) const;

    void calculate_bishop_attacks(bitboard origin);
    void calculate_rook_attacks(bitboard origin);
    void calculate_king_attacks(bitboard origin);

    template <piece p>
    void add_possible_moves(bitboard origin, std::vector<Move>& moves) const;

public:

    Board() :
        piece_of_color{0, 0},
        piece_of_type{0, 0, 0, 0, 0}
    {
        set_king_position(WHITE, square("e1"));
        set_king_position(BLACK, square("e8"));
        calculate_attacks();
    }

    Board(const std::string& fen);

    bool is_checkmate() const;
    bool is_stalemate() const;
    void set_initial_position();
    void calculate_attacks();
    std::string to_string() const;
    void print() const;

    void put_piece(piece p, color c, square s);
    void set_king_position(color c, square position);

    bitboard get_attacks(color color) const;
    std::vector<Move> get_legal_moves(color c) const;

};


#endif //CHESSENGINE_BOARD_H

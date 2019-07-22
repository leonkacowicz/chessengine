#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "color.h"
#include "Board.h"
#include "Move.h"

using namespace std;

bool Board::is_checkmate() const {
    return false;
}

bool Board::is_stalemate() const {
    return !is_checkmate() && get_legal_moves(side_to_play ? BLACK : WHITE).empty();
}

template<>
void Board::add_possible_moves<PAWN>(bitboard origin, std::vector<Move>& moves) const {

}

template<>
void Board::add_possible_moves<KNIGHT>(bitboard origin, std::vector<Move>& moves) const {

}

template<>
void Board::add_possible_moves<BISHOP>(bitboard origin, std::vector<Move>& moves) const {

}

template<>
void Board::add_possible_moves<ROOK>(bitboard origin, std::vector<Move>& moves) const {

}

template<>
void Board::add_possible_moves<QUEEN>(bitboard origin, std::vector<Move>& moves) const {

}

template<>
void Board::add_possible_moves<KING>(bitboard origin, std::vector<Move>& moves) const {

}

string Board::to_string() const {
    auto ret = stringstream();
    for (int y = 7; y >= 0; y--) {
        ret << " " << (y+1) << "  ";
        for (int x = 0; x <= 7; x++) {
            bitboard position(x, y);
            char c = '.';
            if (piece_of_color[WHITE][position] || piece_of_color[BLACK][position]) {
                if (piece_of_type[BISHOP][position]) c = 'B';
                else if (piece_of_type[ROOK][position]) c = 'R';
                else if (piece_of_type[KNIGHT][position]) c = 'N';
                else if (piece_of_type[QUEEN][position]) c = 'Q';
                else if (piece_of_type[PAWN][position]) c = 'P';
                else c = 'K';
                if (piece_of_color[BLACK][position]) c += 'a' - 'A';
            }
            ret << ' ' << c;
        }
        ret << endl;
    }
    ret << endl << "     a b c d e f g h" << endl;

    return ret.str();
}

void Board::print() const {
    cout << to_string();
}

void Board::set_initial_position()
{
    piece_of_type[ROOK] =   0x8100000000000081;
    piece_of_type[KNIGHT] = 0x4200000000000042;
    piece_of_type[BISHOP] = 0x2400000000000024;
    piece_of_type[QUEEN] =  0x1000000000000010;
    piece_of_type[PAWN] =   0x00FF00000000FF00;
    piece_of_color[BLACK] = 0xFFFF000000000000;
    piece_of_color[WHITE] = 0x000000000000FFFF;
}

void Board::calculate_attacks() {
    for (auto position = bitboard(1); !position.isEmpty(); position <<= 1)
    {
        if (position[king_pos[WHITE]] || position[king_pos[BLACK]])
            calculate_king_attacks(position);

        if (!piece_of_color[WHITE][position] && !piece_of_color[BLACK][position]) continue;

        if (piece_of_type[ROOK][position])
            calculate_rook_attacks(position);
    }
}

std::vector<Move> Board::get_legal_moves(color c) const {
    std::vector<Move> moves;

    for (auto sq = bitboard(1); !sq.isEmpty(); sq <<= 1)
    {
        if (!piece_of_color[c][sq]) continue;
        if (king_pos[c] == sq.get_square()) add_possible_moves<KING>(sq, moves);
        if (piece_of_type[PAWN][sq]) add_possible_moves<PAWN>(sq, moves);
        if (piece_of_type[KNIGHT][sq]) add_possible_moves<KNIGHT>(sq, moves);
        if (piece_of_type[BISHOP][sq]) add_possible_moves<BISHOP>(sq, moves);
        if (piece_of_type[ROOK][sq]) add_possible_moves<ROOK>(sq, moves);
        if (piece_of_type[QUEEN][sq]) add_possible_moves<QUEEN>(sq, moves);
    }

    return moves;
}

void Board::calculate_king_attacks(const bitboard origin) {
    color attackerColor = piece_of_color[BLACK][origin] ? BLACK : WHITE;

    if (!rank_8[origin]) {
        auto north = origin.shift_up(1);
        if (!piece_of_color[attackerColor][north])
            attacks[attackerColor] |= north;

        if (!file_a[origin]) {
            auto northwest = origin.shift_up_left(1);
            if (!piece_of_color[attackerColor][northwest])
                attacks[attackerColor] |= northwest;
        }

        if (!file_h[origin]) {
            auto northeast = origin.shift_up_right(1);
            if(!piece_of_color[attackerColor][northeast])
                attacks[attackerColor] |= northeast;
        }
    }
    if (!rank_1[origin]) {
        auto south = origin.shift_down(1);
        if (!piece_of_color[attackerColor][south])
            attacks[attackerColor] |= south;

        if (!file_a[origin]) {
            auto southwest = origin.shift_down_left(1);
            if (!piece_of_color[attackerColor][southwest])
                attacks[attackerColor] |= southwest;
        }

        if (!file_h[origin]) {
            auto southeast = origin.shift_down_right(1);
            if (!piece_of_color[attackerColor][southeast])
                attacks[attackerColor] |= southeast;
        }
    }
    if (!file_a[origin]) {
        auto west = origin.shift_left(1);
        if (!piece_of_color[attackerColor][west])
            attacks[attackerColor] |= west;
    }
    if (!file_h[origin]) {
        auto east = origin.shift_right(1);
        if (!piece_of_color[attackerColor][east])
            attacks[attackerColor] |= east;
    }
}

void Board::calculate_rook_attacks(bitboard origin) {
    // naive implementation
    color attackerColor = piece_of_color[BLACK][origin] ? BLACK : WHITE;
    color oppositeColor = opposite(attackerColor);

    bitboard square = origin;
    while (!rank_8[square]) {
        square <<= 8;
        if (piece_of_color[attackerColor][square]) break;
        attacks[attackerColor] |= square;
        if (piece_of_color[oppositeColor][square]) break;
    }

    square = origin;
    while (!rank_1[square]) {
        square >>= 8;
        if (piece_of_color[attackerColor][square]) break;
        attacks[attackerColor] |= square;
        if (piece_of_color[oppositeColor][square]) break;
    }

    square = origin;
    while (!file_a[square]) {
        square >>= 1;
        if (piece_of_color[attackerColor][square]) break;
        attacks[attackerColor] |= square;
        if (piece_of_color[oppositeColor][square]) break;
    }

    square = origin;
    while (!file_h[square]) {
        square <<= 1;
        if (piece_of_color[attackerColor][square]) break;
        attacks[attackerColor] |= square;
        if (piece_of_color[oppositeColor][square]) break;
    }
}

std::vector<Move> Board::pseudo_legal_rook_moves(const bitboard origin, color attackerColor) const noexcept {
    std::vector<Move> ret;
    color oppositeColor = opposite(attackerColor);
    bitboard square = origin;
    while (!rank_8[square]) {
        square = square.shift_up(1);
        if (piece_of_color[attackerColor][square]) break;
        ret.emplace_back(origin.get_square(), square.get_square());
        if (piece_of_color[oppositeColor][square]) break;
    }

    square = origin;
    while (!rank_1[square]) {
        square = square.shift_down(1);
        if (piece_of_color[attackerColor][square]) break;
        ret.emplace_back(origin.get_square(), square.get_square());
        if (piece_of_color[oppositeColor][square]) break;
    }

    square = origin;
    while (!file_a[square]) {
        square = square.shift_left(1);
        if (piece_of_color[attackerColor][square]) break;
        ret.emplace_back(origin.get_square(), square.get_square());
        if (piece_of_color[oppositeColor][square]) break;
    }

    square = origin;
    while (!file_h[square]) {
        square = square.shift_right(1);
        if (piece_of_color[attackerColor][square]) break;
        ret.emplace_back(origin.get_square(), square.get_square());
        if (piece_of_color[oppositeColor][square]) break;
    }
}

void Board::put_piece(piece p, color c, square s) {

    if ((s == king_pos[WHITE] && (p != KING || c != WHITE))
        || (s == king_pos[BLACK] && (p != KING || c != BLACK)) ) {
        __throw_runtime_error("Can't put other piece where king is.");
    }

    if (p == KING) {
        return set_king_position(c, s);
    }

    bitboard bb(s);
    bitboard bbi = ~bb;

    piece_of_type[PAWN] &= bbi;
    piece_of_type[KNIGHT] &= bbi;
    piece_of_type[BISHOP] &= bbi;
    piece_of_type[ROOK] &= bbi;
    piece_of_type[QUEEN] &= bbi;
    piece_of_type[p] |= bb;
    piece_of_color[c] |= bb;
    piece_of_color[opposite(c)] &= bbi;
}


void Board::set_king_position(color c, square position) {

    piece_of_color[c] &= ~(bitboard(king_pos[c]));
    king_pos[c] = position;
    auto bb = bitboard(position);
    auto bbi = ~bb;
    piece_of_color[c] |= bb;
    piece_of_color[opposite(c)] &= bbi;
    piece_of_type[PAWN] &= bbi;
    piece_of_type[KNIGHT] &= bbi;
    piece_of_type[BISHOP] &= bbi;
    piece_of_type[ROOK] &= bbi;
    piece_of_type[QUEEN] &= bbi;
}

bitboard Board::get_attacks(color color) const {
    return attacks[color];
}

void Board::addRookPossibleMoves(bitboard origin, std::vector<Move> & moves) const {
    color attackerColor = piece_of_color[BLACK][origin] ? BLACK : WHITE;
    color oppositeColor = opposite(attackerColor);

    bitboard square = origin;
    while (!rank_8[square]) {
        square <<= 8;
        if (piece_of_color[attackerColor][square]) break;
        moves.emplace_back(origin.get_square(), square.get_square());

        if (piece_of_color[oppositeColor][square]) break;
    }

    square = origin;
    while (!rank_1[square]) {
        square >>= 8;
        if (piece_of_color[attackerColor][square]) break;
        moves.emplace_back(origin.get_square(), square.get_square());
        if (piece_of_color[oppositeColor][square]) break;
    }

    square = origin;
    while (!file_a[square]) {
        square <<= 1;
        if (piece_of_color[attackerColor][square]) break;
        moves.emplace_back(origin.get_square(), square.get_square());
        if (piece_of_color[oppositeColor][square]) break;
    }

    square = origin;
    while (!file_h[square]) {
        square >>= 1;
        if (piece_of_color[attackerColor][square]) break;
        moves.emplace_back(origin.get_square(), square.get_square());
        if (piece_of_color[oppositeColor][square]) break;
    }
}

void Board::addKingPossibleMoves(bitboard origin, std::vector<Move>& moves) const {


}

Board::Board(const std::string& fen) {

    using std::string;
    using std::stringstream;

    stringstream ss(fen);

    string piecePlacement;
    getline(ss, piecePlacement, ' ');

    string side_to_move;
    getline(ss, side_to_move, ' ');

    string castling;
    getline(ss, castling, ' ');

    string en_passant;
    getline(ss, en_passant, ' ');

    string half_move_clock;
    getline(ss, half_move_clock, ' ');

    string full_move_counter;
    getline(ss, full_move_counter, ' ');

    side_to_play = side_to_move == "b";
}

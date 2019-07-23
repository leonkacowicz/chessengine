#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "color.h"
#include "board.h"
#include "move.h"

using std::string;
using std::stringstream;
using std::cout;
using std::endl;

bool board::is_checkmate() const {
    return false;
}

bool board::is_stalemate() const {
    return !is_checkmate() && get_legal_moves(side_to_play ? BLACK : WHITE).empty();
}

template<>
void board::add_possible_moves<PAWN>(bitboard origin, std::vector<move>& moves) const {

}

template<>
void board::add_possible_moves<KNIGHT>(bitboard origin, std::vector<move>& moves) const {

}

template<>
void board::add_possible_moves<BISHOP>(bitboard origin, std::vector<move>& moves) const {

}

template<>
void board::add_possible_moves<ROOK>(bitboard origin, std::vector<move>& moves) const {

}

template<>
void board::add_possible_moves<QUEEN>(bitboard origin, std::vector<move>& moves) const {

}

template<>
void board::add_possible_moves<KING>(bitboard origin, std::vector<move>& moves) const {

}

string board::to_string() const {
    auto ret = stringstream();
    for (int y = 7; y >= 0; y--) {
        ret << " " << (y + 1) << "  ";
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

void board::print() const {
    cout << to_string();
}

void board::set_initial_position() {
    piece_of_type[ROOK] = 0x8100000000000081;
    piece_of_type[KNIGHT] = 0x4200000000000042;
    piece_of_type[BISHOP] = 0x2400000000000024;
    piece_of_type[QUEEN] = 0x1000000000000010;
    piece_of_type[PAWN] = 0x00FF00000000FF00;
    piece_of_color[BLACK] = 0xFFFF000000000000;
    piece_of_color[WHITE] = 0x000000000000FFFF;
}

void board::calculate_attacks() {
    attacks[0] = 0;
    attacks[1] = 0;
    for (auto position = bitboard(1); !position.isEmpty(); position <<= 1) {
        if (position[king_pos[WHITE]] || position[king_pos[BLACK]])
            calculate_king_attacks(position);

        if (!piece_of_color[WHITE][position] && !piece_of_color[BLACK][position]) continue;

        if (piece_of_type[ROOK][position] || piece_of_type[QUEEN][position])
            calculate_rook_attacks(position);

        if (piece_of_type[BISHOP][position] || piece_of_type[QUEEN][position])
            calculate_bishop_attacks(position);

        if (piece_of_type[KNIGHT][position])
            calculate_knight_attacks(position);
    }
}

std::vector<move> board::get_legal_moves(color c) const {
    std::vector<move> moves;

    for (auto sq = bitboard(1); !sq.isEmpty(); sq <<= 1) {
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

void board::calculate_king_attacks(const bitboard origin) {
    color attackerColor = piece_of_color[BLACK][origin] ? BLACK : WHITE;

    if (!rank_8[origin]) {
        attacks[attackerColor] |= origin.shift_up(1);

        if (!file_a[origin]) {
            attacks[attackerColor] |= origin.shift_up_left(1);
        }

        if (!file_h[origin]) {
            attacks[attackerColor] |= origin.shift_up_right(1);
        }
    }
    if (!rank_1[origin]) {
        auto south = origin.shift_down(1);
        attacks[attackerColor] |= south;

        if (!file_a[origin]) {
            attacks[attackerColor] |= origin.shift_down_left(1);
        }

        if (!file_h[origin]) {
            attacks[attackerColor] |= origin.shift_down_right(1);
        }
    }
    if (!file_a[origin]) {
        attacks[attackerColor] |= origin.shift_left(1);
    }
    if (!file_h[origin]) {
        attacks[attackerColor] |= origin.shift_right(1);
    }
}

void board::calculate_bishop_attacks(bitboard origin){
    color attacker = piece_of_color[BLACK][origin] ? BLACK : WHITE;
    color opponent = opposite(attacker);
    auto attacker_piece = piece_of_color[attacker];
    auto opponent_piece = piece_of_color[opponent];
    bitboard opponent_king = king_pos[opponent];
    auto any_piece = attacker_piece | opponent_piece;

    bitboard sq = origin;
    while(!rank_8[sq] && !file_a[sq]) {
        sq = sq.shift_up_left(1);
        attacks[attacker] |= sq;
        if (attacker_piece[sq]) break;
        if (opponent_piece[sq]) {
            if (!pinned[sq]) {
                auto sq2 = sq;
                while(!rank_8[sq2] && !file_a[sq2]) {
                    sq2 = sq2.shift_up_left(1);
                    if (sq2 == opponent_king) {
                        pinned |= sq;
                        break;
                    } else if (any_piece[sq2]) break;
                }
            }
            break;
        }
    }

    while(!rank_8[sq] && !file_h[sq]) {
        sq = sq.shift_up_right(1);
        attacks[attacker] |= sq;
        if (attacker_piece[sq]) break;
        if (opponent_piece[sq]) {
            if (!pinned[sq]) {
                auto sq2 = sq;
                while(!rank_8[sq2] && !file_h[sq2]) {
                    sq2 = sq2.shift_up_right(1);
                    if (sq2 == opponent_king) {
                        pinned |= sq;
                        break;
                    } else if (any_piece[sq2]) break;
                }
            }
            break;
        }
    }

    while(!rank_1[sq] && !file_a[sq]) {
        sq = sq.shift_down_left(1);
        attacks[attacker] |= sq;
        if (attacker_piece[sq]) break;
        if (opponent_piece[sq]) {
            if (!pinned[sq]) {
                auto sq2 = sq;
                while(!rank_1[sq2] && !file_a[sq2]) {
                    sq2 = sq2.shift_down_left(1);
                    if (sq2 == opponent_king) {
                        pinned |= sq;
                        break;
                    } else if (any_piece[sq2]) break;
                }
            }
            break;
        }
    }

    while(!rank_1[sq] && !file_h[sq]) {
        sq = sq.shift_down_right(1);
        attacks[attacker] |= sq;
        if (attacker_piece[sq]) break;
        if (opponent_piece[sq]) {
            if (!pinned[sq]) {
                auto sq2 = sq;
                while(!rank_1[sq2] && !file_h[sq2]) {
                    sq2 = sq2.shift_down_right(1);
                    if (sq2 == opponent_king) {
                        pinned |= sq;
                        break;
                    } else if (any_piece[sq2]) break;
                }
            }
            break;
        }
    }
}

void board::calculate_rook_attacks(bitboard origin) {
    color attackerColor = piece_of_color[BLACK][origin] ? BLACK : WHITE;
    color oppositeColor = opposite(attackerColor);

    bitboard square = origin;
    while (!rank_8[square]) {
        square = square.shift_up(1);
        attacks[attackerColor] |= square;
        if (piece_of_color[attackerColor][square]) break;
        if (piece_of_color[oppositeColor][square]) { // find out if piece is pinned
            auto square2 = square;
            if (!pinned[square])
                while (!rank_8[square2]) {
                    square2 = square2.shift_up(1);
                    if (square2 == king_pos[oppositeColor]) {
                        pinned |= square;
                        break;
                    } else if (piece_of_color[BLACK][square2] || piece_of_color[WHITE][square2]) {
                        break;
                    }
                }
            break;
        }
    }

    square = origin;
    while (!rank_1[square]) {
        square = square.shift_down(1);
        attacks[attackerColor] |= square;
        if (piece_of_color[attackerColor][square]) break;
        if (piece_of_color[oppositeColor][square]) { // find out if piece is pinned
            auto square2 = square;
            if (!pinned[square])
                while (!rank_1[square2]) {
                    square2 = square2.shift_down(1);
                    if (square2 == king_pos[oppositeColor]) {
                        pinned |= square;
                        break;
                    } else if (piece_of_color[BLACK][square2] || piece_of_color[WHITE][square2]) {
                        break;
                    }
                }
            break;
        }
    }

    square = origin;
    while (!file_a[square]) {
        square = square.shift_left(1);
        attacks[attackerColor] |= square;
        if (piece_of_color[attackerColor][square]) break;
        if (piece_of_color[oppositeColor][square]) { // find out if piece is pinned
            auto square2 = square;
            if (!pinned[square])
                while (!file_a[square2]) {
                    square2 = square2.shift_left(1);
                    if (square2 == king_pos[oppositeColor]) {
                        pinned |= square;
                        break;
                    } else if (piece_of_color[BLACK][square2] || piece_of_color[WHITE][square2]) {
                        break;
                    }
                }
            break;
        }
    }

    square = origin;
    while (!file_h[square]) {
        square = square.shift_right(1);
        attacks[attackerColor] |= square;
        if (piece_of_color[attackerColor][square]) break;
        if (piece_of_color[oppositeColor][square]) { // find out if piece is pinned
            auto square2 = square;
            if (!pinned[square])
                while (!file_h[square2]) {
                    square2 = square2.shift_right(1);
                    if (square2 == king_pos[oppositeColor]) {
                        pinned |= square;
                        break;
                    } else if (piece_of_color[BLACK][square2] || piece_of_color[WHITE][square2]) {
                        break;
                    }
                }
            break;
        }
    }
}

void board::calculate_knight_attacks(bitboard origin) {
    color c = piece_of_color[BLACK][origin] ? BLACK : WHITE;

    if (!file_a[origin] && !rank_8[origin] && !rank_7[origin]) {
        attacks[c] |= origin.shift_left(1).shift_up(2);
    }

    if (!file_a[origin] && !rank_1[origin] && !rank_2[origin]) {
        attacks[c] |= origin.shift_left(1).shift_down(2);
    }

    if (!file_h[origin] && !rank_8[origin] && !rank_7[origin]) {
        attacks[c] |= origin.shift_right(1).shift_up(2);
    }

    if (!file_h[origin] && !rank_1[origin] && !rank_2[origin]) {
        attacks[c] |= origin.shift_right(1).shift_down(2);
    }

    if (!file_a[origin] && !file_b[origin] && !rank_8[origin]) {
        attacks[c] |= origin.shift_left(2).shift_up(1);
    }

    if (!file_a[origin] && !file_b[origin] && !rank_1[origin]) {
        attacks[c] |= origin.shift_left(2).shift_down(1);
    }

    if (!file_h[origin] && !file_g[origin] && !rank_8[origin]) {
        attacks[c] |= origin.shift_right(2).shift_up(1);
    }

    if (!file_h[origin] && !file_g[origin] && !rank_1[origin]) {
        attacks[c] |= origin.shift_right(2).shift_down(1);
    }
}


void board::put_piece(piece p, color c, square s) {

    if ((s == king_pos[WHITE] && (p != KING || c != WHITE))
        || (s == king_pos[BLACK] && (p != KING || c != BLACK))) {
        std::__throw_runtime_error("Can't put other piece where king is.");
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

void board::set_king_position(color c, square position) {

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

bitboard board::get_attacks(color color) const {
    return attacks[color];
}

void board::add_rook_moves(bitboard board, std::vector<move>& moves) const {
    color attackerColor = piece_of_color[BLACK][board] ? BLACK : WHITE;
    color oppositeColor = opposite(attackerColor);

    bitboard square = board;
    while (!rank_8[square]) {
        square <<= 8;
        if (piece_of_color[attackerColor][square]) break;
        moves.emplace_back(board.get_square(), square.get_square());

        if (piece_of_color[oppositeColor][square]) break;
    }

    square = board;
    while (!rank_1[square]) {
        square >>= 8;
        if (piece_of_color[attackerColor][square]) break;
        moves.emplace_back(board.get_square(), square.get_square());
        if (piece_of_color[oppositeColor][square]) break;
    }

    square = board;
    while (!file_a[square]) {
        square <<= 1;
        if (piece_of_color[attackerColor][square]) break;
        moves.emplace_back(board.get_square(), square.get_square());
        if (piece_of_color[oppositeColor][square]) break;
    }

    square = board;
    while (!file_h[square]) {
        square >>= 1;
        if (piece_of_color[attackerColor][square]) break;
        moves.emplace_back(board.get_square(), square.get_square());
        if (piece_of_color[oppositeColor][square]) break;
    }
}

void board::add_king_moves(bitboard origin, std::vector<move>& moves) const {
    color c = piece_of_color[BLACK][origin] ? BLACK : WHITE;
    auto allowed = ~attacks[opposite(c)];

    auto origin_square = origin.get_square();
    if (!rank_8[origin]) {
        auto up = origin.shift_up(1);
        if (allowed[up]) moves.emplace_back(origin_square, up.get_square());
        if (!file_a[origin]) {
            auto upleft = origin.shift_up_left(1);
            if (allowed[upleft]) moves.emplace_back(origin_square, upleft.get_square());
        }
        if (!file_h[origin]) {
            auto upright = origin.shift_up_right(1);
            if (allowed[upright]) moves.emplace_back(origin_square, upright.get_square());
        }
    }

    if (!rank_1[origin]) {
        auto down = origin.shift_down(1);
        if (allowed[down]) moves.emplace_back(origin_square, down.get_square());

        if (!file_a[origin]) {
            auto downleft = origin.shift_down_left(1);
            if (allowed[downleft]) moves.emplace_back(origin_square, downleft.get_square());
        }

        if (!file_h[origin]) {
            auto downright = origin.shift_down_right(1);
            if (allowed[downright]) moves.emplace_back(origin_square, downright.get_square());
        }
    }
    if (!file_a[origin]) {
        auto left = origin.shift_left(1);
        if (allowed[left]) moves.emplace_back(origin_square, left.get_square());
    }
    if (!file_h[origin]) {
        auto right = origin.shift_right(1);
        if (allowed[right]) moves.emplace_back(origin_square, right.get_square());
    }
}

board::board(const std::string& fen) {

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

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
    add_pawn_moves(origin, moves);
}

template<>
void board::add_possible_moves<KNIGHT>(bitboard origin, std::vector<move>& moves) const {
    add_knight_moves(origin, moves);
}

template<>
void board::add_possible_moves<BISHOP>(bitboard origin, std::vector<move>& moves) const {
    add_bishop_moves(origin, moves);
}

template<>
void board::add_possible_moves<ROOK>(bitboard origin, std::vector<move>& moves) const {
    add_rook_moves(origin, moves);
}

template<>
void board::add_possible_moves<QUEEN>(bitboard origin, std::vector<move>& moves) const {
    add_possible_moves<BISHOP>(origin, moves);
    add_possible_moves<ROOK>(origin, moves);
}

template<>
void board::add_possible_moves<KING>(bitboard origin, std::vector<move>& moves) const {
    add_king_moves(origin, moves);
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

        if (piece_of_type[PAWN][position])
            calculate_pawn_attacks(position);
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
    auto inside_range = ~(rank_8 | file_a);
    while(inside_range[sq]) {
        sq = sq.shift_up_left(1);
        attacks[attacker] |= sq;
        if (attacker_piece[sq]) break;
        if (opponent_piece[sq]) {
            if (pinned[opponent].downright == square::none) {
                auto sq2 = sq;
                while(inside_range[sq2]) {
                    sq2 = sq2.shift_up_left(1);
                    if (sq2 == opponent_king) {
                        pinned[opponent].downright = sq.get_square();
                        break;
                    } else if (any_piece[sq2]) break;
                }
            }
            break;
        }
    }

    sq = origin;
    inside_range = ~(rank_8 | file_h);
    while(inside_range[sq]) {
        sq = sq.shift_up_right(1);
        attacks[attacker] |= sq;
        if (attacker_piece[sq]) break;
        if (opponent_piece[sq]) {
            if (pinned[opponent].downleft == square::none) {
                auto sq2 = sq;
                while(inside_range[sq2]) {
                    sq2 = sq2.shift_up_right(1);
                    if (sq2 == opponent_king) {
                        pinned[opponent].downleft = sq.get_square();
                        break;
                    } else if (any_piece[sq2]) break;
                }
            }
            break;
        }
    }

    sq = origin;
    inside_range = ~(rank_1 | file_a);
    while(inside_range[sq]) {
        sq = sq.shift_down_left(1);
        attacks[attacker] |= sq;
        if (attacker_piece[sq]) break;
        if (opponent_piece[sq]) {
            if (pinned[opponent].upright == square::none) {
                auto sq2 = sq;
                while(inside_range[sq2]) {
                    sq2 = sq2.shift_down_left(1);
                    if (sq2 == opponent_king) {
                        pinned[opponent].upright = sq.get_square();
                        break;
                    } else if (any_piece[sq2]) break;
                }
            }
            break;
        }
    }

    sq = origin;
    inside_range = ~(rank_1 | file_h);
    while(inside_range[sq]) {
        sq = sq.shift_down_right(1);
        attacks[attacker] |= sq;
        if (attacker_piece[sq]) break;
        if (opponent_piece[sq]) {
            if (pinned[opponent].upleft == square::none) {
                auto sq2 = sq;
                while(inside_range[sq2]) {
                    sq2 = sq2.shift_down_right(1);
                    if (sq2 == opponent_king) {
                        pinned[opponent].upleft = sq.get_square();
                        break;
                    } else if (any_piece[sq2]) break;
                }
            }
            break;
        }
    }
}

void board::calculate_rook_attacks(bitboard origin) {
    color attacker = piece_of_color[BLACK][origin] ? BLACK : WHITE;
    color opponent = opposite(attacker);
    auto attacker_piece = piece_of_color[attacker];
    auto opponent_piece = piece_of_color[opponent];
    bitboard opponent_king = king_pos[opponent];
    auto any_piece = attacker_piece | opponent_piece;

    bitboard sq = origin;
    auto inside_range = ~rank_8;
    while (inside_range[sq]) {
        sq = sq.shift_up(1);
        attacks[attacker] |= sq;
        if (attacker_piece[sq]) break;
        if (opponent_piece[sq]) { // find out if piece is pinned
            auto sq2 = sq;
            if (pinned[opponent].down == square::none)
                while (inside_range[sq2]) {
                    sq2 = sq2.shift_up(1);
                    if (sq2 == opponent_king) {
                        pinned[opponent].down = sq.get_square();
                        break;
                    } else if (any_piece[sq2]) {
                        break;
                    }
                }
            break;
        }
    }

    sq = origin;
    inside_range = ~rank_1;
    while (inside_range[sq]) {
        sq = sq.shift_down(1);
        attacks[attacker] |= sq;
        if (attacker_piece[sq]) break;
        if (opponent_piece[sq]) { // find out if piece is pinned
            auto square2 = sq;
            if (pinned[opponent].up == square::none)
                while (inside_range[square2]) {
                    square2 = square2.shift_down(1);
                    if (square2 == king_pos[opponent]) {
                        pinned[opponent].up = sq.get_square();
                        break;
                    } else if (piece_of_color[BLACK][square2] || piece_of_color[WHITE][square2]) {
                        break;
                    }
                }
            break;
        }
    }

    sq = origin;
    inside_range = ~file_a;
    while (inside_range[sq]) {
        sq = sq.shift_left(1);
        attacks[attacker] |= sq;
        if (piece_of_color[attacker][sq]) break;
        if (piece_of_color[opponent][sq]) { // find out if piece is pinned
            auto square2 = sq;
            if (pinned[opponent].right == square::none)
                while (inside_range[square2]) {
                    square2 = square2.shift_left(1);
                    if (square2 == king_pos[opponent]) {
                        pinned[opponent].right = sq.get_square();
                        break;
                    } else if (piece_of_color[BLACK][square2] || piece_of_color[WHITE][square2]) {
                        break;
                    }
                }
            break;
        }
    }

    sq = origin;
    inside_range = ~file_h;
    while (inside_range[sq]) {
        sq = sq.shift_right(1);
        attacks[attacker] |= sq;
        if (piece_of_color[attacker][sq]) break;
        if (piece_of_color[opponent][sq]) { // find out if piece is pinned
            auto sq2 = sq;
            if (pinned[opponent].left == square::none)
                while (inside_range[sq2]) {
                    sq2 = sq2.shift_right(1);
                    if (sq2 == king_pos[opponent]) {
                        pinned[opponent].left = sq.get_square();
                        break;
                    } else if (any_piece[sq2]) {
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

void board::calculate_pawn_attacks(bitboard origin) {
    color attacker = piece_of_color[BLACK][origin] ? BLACK : WHITE;

    if (!file_a[origin]) {
        if (attacker == WHITE)
            attacks[attacker] |= origin.shift_up_left(1);
        else
            attacks[attacker] |= origin.shift_down_left(1);
    }

    if (!file_h[origin]) {
        if (attacker == WHITE)
            attacks[attacker] |= origin.shift_up_right(1);
        else
            attacks[attacker] |= origin.shift_down_right(1);
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

void board::add_rook_moves(bitboard origin, std::vector<move>& moves) const {
    color attacker = piece_of_color[BLACK][origin] ? BLACK : WHITE;
    auto origin_sq = origin.get_square();
    auto attacker_piece = piece_of_color[attacker];
    auto opponent_piece = piece_of_color[opposite(attacker)];

    bitboard s = origin;
    while (!rank_8[s]) {
        s = s.shift_up(1);
        if (attacker_piece[s]) break;
        moves.emplace_back(origin_sq, s.get_square());
        if (opponent_piece[s]) break;
    }

    s = origin;
    while (!rank_1[s]) {
        s = s.shift_down(1);
        if (attacker_piece[s]) break;
        moves.emplace_back(origin_sq, s.get_square());
        if (opponent_piece[s]) break;
    }

    s = origin;
    while (!file_a[s]) {
        s = s.shift_left(1);
        if (attacker_piece[s]) break;
        moves.emplace_back(origin_sq, s.get_square());
        if (opponent_piece[s]) break;
    }

    s = origin;
    while (!file_h[s]) {
        s = s.shift_right(1);
        if (attacker_piece[s]) break;
        moves.emplace_back(origin_sq, s.get_square());
        if (opponent_piece[s]) break;
    }
}

void board::add_bishop_moves(bitboard origin, std::vector<move>& moves) const {
    color attacker = piece_of_color[BLACK][origin] ? BLACK : WHITE;
    auto origin_sq = origin.get_square();
    if (pinned[attacker].up == origin_sq) return; // piece can't move at all
    if (pinned[attacker].down == origin_sq) return; // piece can't move at all
    if (pinned[attacker].left == origin_sq) return; // piece can't move at all
    if (pinned[attacker].right == origin_sq) return; // piece can't move at all

    auto attacker_piece = piece_of_color[attacker];
    auto opponent_piece = piece_of_color[opposite(attacker)];

    bitboard s = origin;

    if (pinned[attacker].upright != origin_sq && pinned[attacker].downleft != origin_sq) {
        auto not_in_file_a_rank_8 = ~(file_a | rank_8);
        while (not_in_file_a_rank_8[s]) {
            s = s.shift_up_left(1);
            if (attacker_piece[s]) break;
            moves.emplace_back(origin_sq, s.get_square());
            if (opponent_piece[s]) break;
        }
    }

    if (pinned[attacker].upleft != origin_sq && pinned[attacker].downright != origin_sq) {
        s = origin;
        auto not_in_file_a_rank_1 = ~(file_a | rank_1);
        while (not_in_file_a_rank_1[s]) {
            s = s.shift_down_left(1);
            if (attacker_piece[s]) break;
            moves.emplace_back(origin_sq, s.get_square());
            if (opponent_piece[s]) break;
        }
    }

    if (pinned[attacker].upleft != origin_sq && pinned[attacker].downright != origin_sq) {
        s = origin;
        auto not_in_file_h_rank_8 = ~(file_h | rank_8);
        while (not_in_file_h_rank_8[s]) {
            s = s.shift_up_right(1);
            if (attacker_piece[s]) break;
            moves.emplace_back(origin_sq, s.get_square());
            if (opponent_piece[s]) break;
        }
    }

    if (pinned[attacker].upright != origin_sq && pinned[attacker].downleft != origin_sq) {
        s = origin;
        auto not_in_file_h_rank_1 = ~(file_h | rank_1);
        while (not_in_file_h_rank_1[s]) {
            s = s.shift_down_right(1);
            if (attacker_piece[s]) break;
            moves.emplace_back(origin_sq, s.get_square());
            if (opponent_piece[s]) break;
        }
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

void board::add_pawn_moves(bitboard origin, std::vector<move>& moves) const {
    color attacker = piece_of_color[BLACK][origin] ? BLACK : WHITE;

    // add normal moves, captures, en passant, and promotions
}

void board::add_knight_moves(bitboard origin, std::vector<move>& moves) const {
    // to-do
}

void board::add_castle_moves(std::vector<move>& moves) const {
    // to-do
}

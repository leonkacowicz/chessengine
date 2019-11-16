#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>
#include "color.h"
#include "board.h"
#include "move.h"
#include "magic_bitboard.h"

using namespace chess::core;
using std::string;
using std::stringstream;
using std::cout;
using std::endl;

bool board::under_check(color c) const {
    square sq = king_pos[c];
    if (sq == SQ_NONE) return false;
    bitboard king = get_bb(sq);
    bitboard their_piece = piece_of_color[opposite(c)];
    bitboard any_piece = piece_of_color[BLACK] | piece_of_color[WHITE];
    if (knight_attacks(sq) & their_piece & piece_of_type[KNIGHT]) return true;
    if (pawn_attacks_bb[c][sq] & their_piece & piece_of_type[PAWN]) return true;
    if (king_attacks(sq) & get_bb(king_pos[opposite(c)])) return true;
    if (attacks_from_rook(sq, any_piece ^ king) & their_piece & (piece_of_type[ROOK] | piece_of_type[QUEEN])) return true;
    if (attacks_from_bishop(sq, any_piece ^ king) & their_piece & (piece_of_type[BISHOP] | piece_of_type[QUEEN])) return true;
    return false;
}

string board::to_string() const {
    stringstream ret;
    for (int y = 7; y >= 0; y--) {
        ret << " " << (y + 1) << "  ";
        for (int x = 0; x <= 7; x++) {
            bitboard sq = get_bb(x, y);
            if (sq & piece_of_color[WHITE]) {
                if (sq & piece_of_type[BISHOP]) ret << " ♗";
                else if (sq & piece_of_type[ROOK]) ret << " ♖";
                else if (sq & piece_of_type[KNIGHT]) ret << " ♘";
                else if (sq & piece_of_type[QUEEN]) ret << " ♕";
                else if (sq & piece_of_type[PAWN]) ret << " ♙";
                else if (sq & get_bb(king_pos[WHITE])) ret << " ♔";
                else ret << " X";
            }
            else if (sq & piece_of_color[BLACK]) {
                if (sq & piece_of_type[BISHOP]) ret << " ♝";
                else if (sq & piece_of_type[ROOK]) ret << " ♜";
                else if (sq & piece_of_type[KNIGHT]) ret << " ♞";
                else if (sq & piece_of_type[QUEEN]) ret << " ♛";
                else if (sq & piece_of_type[PAWN]) ret << " ♟";
                else if (sq & get_bb(king_pos[BLACK])) ret << " ♚";
                else ret << " x";
            } else {
                ret << " ◦";
            }
        }
        ret << std::endl;
    }
    ret << std::endl << "     a◦b◦c◦d◦e◦f◦g◦h" << std::endl;
    return ret.str();
}

void board::print() const {
    std::cout << std::endl << to_string() << std::endl ;
}

void board::set_initial_position() {
    const bitboard player_first_rank = rank_1 | rank_8;
    piece_of_type[ROOK] = (file_a | file_h) & player_first_rank;
    piece_of_type[KNIGHT] = (file_b | file_g) & player_first_rank;
    piece_of_type[BISHOP] = (file_c | file_f) & player_first_rank;
    piece_of_type[QUEEN] = file_d & player_first_rank;
    piece_of_type[PAWN] = rank_2 | rank_7;
    piece_of_color[BLACK] = rank_7 | rank_8;
    piece_of_color[WHITE] = rank_1 | rank_2;
    king_pos[WHITE] = SQ_E1;
    king_pos[BLACK] = SQ_E8;
    can_castle_king_side[0] = true;
    can_castle_king_side[1] = true;
    can_castle_queen_side[0] = true;
    can_castle_queen_side[1] = true;
    side_to_play = WHITE;
    en_passant = SQ_NONE;
}

std::vector<move> board::get_legal_moves(color c) const {
    std::vector<move> moves;
    //moves.reserve(60);
    for (auto sq = bitboard(1); sq; sq <<= 1u) {
        if (!(sq & piece_of_color[c])) continue;
        if (king_pos[c] == get_square(sq)) add_king_moves(sq, moves);
        else if (piece_of_type[PAWN] & sq) add_pawn_moves(sq, moves);
        else if (piece_of_type[KNIGHT] & sq) add_knight_moves(sq, moves);
        else if (piece_of_type[ROOK] & sq) add_rook_moves(sq, moves, ROOK, c);
        else if (piece_of_type[BISHOP] & sq) add_bishop_moves(sq, moves, BISHOP, c);
        else if (piece_of_type[QUEEN] & sq) {
            add_rook_moves(sq, moves, QUEEN, c);
            add_bishop_moves(sq, moves, QUEEN, c);
        }
    }
    if (can_castle_king_side[c] || can_castle_queen_side[c]) {
        add_castle_moves(c, moves);
    }
    //moves.shrink_to_fit();
    return moves;
}

void board::put_piece(piece p, color c, square s) {

    if ((s == king_pos[WHITE] && (p != KING || c != WHITE))
        || (s == king_pos[BLACK] && (p != KING || c != BLACK))) {
        std::__throw_runtime_error("Can't put other piece where king is.");
    }

    if (p == KING) {
        return set_king_position(c, s);
    }

    bitboard bbo = get_bb(s);
    bitboard bbi = ~bbo;

    piece_of_type[PAWN] &= bbi;
    piece_of_type[KNIGHT] &= bbi;
    piece_of_type[BISHOP] &= bbi;
    piece_of_type[ROOK] &= bbi;
    piece_of_type[QUEEN] &= bbi;
    piece_of_color[opposite(c)] &= bbi;
    piece_of_type[p] |= bbo;
    piece_of_color[c] |= bbo;
}

void board::set_king_position(color c, square position) {
    piece_of_color[c] &= ~(get_bb(king_pos[c]));
    king_pos[c] = position;
    auto bbo = get_bb(position);
    auto bbi = ~bbo;
    piece_of_color[c] |= bbo;
    piece_of_color[opposite(c)] &= bbi;
    piece_of_type[PAWN] &= bbi;
    piece_of_type[KNIGHT] &= bbi;
    piece_of_type[BISHOP] &= bbi;
    piece_of_type[ROOK] &= bbi;
    piece_of_type[QUEEN] &= bbi;
}

void board::add_rook_moves(bitboard origin, std::vector<move>& moves, piece p, color c) const {
    shift_moves<UP>(origin, rank_8_i, moves, p, c);
    shift_moves<DOWN>(origin, rank_1_i, moves, p, c);
    shift_moves<LEFT>(origin, file_a_i, moves, p, c);
    shift_moves<RIGHT>(origin, file_h_i, moves, p, c);
}

void board::add_bishop_moves(bitboard origin, std::vector<move>& moves, piece p, color c) const {
    shift_moves<UP_LEFT>(origin, file_a_i_rank_8_i, moves, p, c);
    shift_moves<UP_RIGHT>(origin, file_h_i_rank_8_i, moves, p, c);
    shift_moves<DOWN_LEFT>(origin, file_a_i_rank_1_i, moves, p, c);
    shift_moves<DOWN_RIGHT>(origin, file_h_i_rank_1_i, moves, p, c);
}

void board::add_king_moves(bitboard origin, std::vector<move>& moves) const {
    color c = (piece_of_color[BLACK] & origin) ? BLACK : WHITE;
    auto origin_square = get_square(origin);
    bitboard dest[8];
    int N = 0;

    if (rank_8_i & origin) {
        dest[N++] = shift<UP>(origin);
        if (file_a_i & origin) dest[N++] = shift<UP_LEFT>(origin);
        if (file_h_i & origin) dest[N++] = shift<UP_RIGHT>(origin);
    }
    if (rank_1_i & origin) {
        dest[N++] = shift<DOWN>(origin);
        if (file_a_i & origin) dest[N++] = shift<DOWN_LEFT>(origin);
        if (file_h_i & origin) dest[N++] = shift<DOWN_RIGHT>(origin);
    }
    if (file_a_i & origin) dest[N++] = shift<LEFT>(origin);
    if (file_h_i & origin) dest[N++] = shift<RIGHT>(origin);

    for (int i = 0; i < N; i++) {
        if (!(piece_of_color[c] & dest[i])) {
            assert(get_square(dest[i]) != king_pos[opposite(c)]);
            if (!simulate(origin_square, get_square(dest[i]), KING, c).under_check(c))
                moves.push_back(get_move(origin_square, get_square(dest[i])));
        }
    }
}

board::board(const std::string& fen) {

    using std::string;
    using std::stringstream;

    std::stringstream ss(fen);

    std::string pieces;
    getline(ss, pieces, ' ');

    std::string side_to_move;
    getline(ss, side_to_move, ' ');

    std::string castling;
    getline(ss, castling, ' ');

    std::string enpassant;
    getline(ss, enpassant, ' ');

    std::string half_move_clock;
    getline(ss, half_move_clock, ' ');

    std::string full_move_counter;
    getline(ss, full_move_counter, ' ');


    int r = 7;
    int f = 0;
    for (char ch : pieces) {
        bitboard sq = file[f] & rank[r];
        if (ch == '/') {
            r--;
            f = 0;
            continue;
        }
        if (ch >= '1' && ch <= '7') {
            f += ch - '0';
            continue;
        }

        if (ch == 'p' || ch == 'P') piece_of_type[PAWN] |= sq;
        if (ch == 'n' || ch == 'N') piece_of_type[KNIGHT] |= sq;
        if (ch == 'b' || ch == 'B') piece_of_type[BISHOP] |= sq;
        if (ch == 'r' || ch == 'R') piece_of_type[ROOK] |= sq;
        if (ch == 'q' || ch == 'Q') piece_of_type[QUEEN] |= sq;
        if (ch == 'k') set_king_position(BLACK, get_square(sq));
        if (ch == 'K') set_king_position(WHITE, get_square(sq));
        if (ch == 'p' || ch == 'n' || ch == 'b' || ch == 'r' || ch == 'q' || ch == 'k') piece_of_color[BLACK] |= sq;
        if (ch == 'P' || ch == 'N' || ch == 'B' || ch == 'R' || ch == 'Q' || ch == 'K') piece_of_color[WHITE] |= sq;
        f++;
    }

    side_to_play = side_to_move == "b" ? BLACK : WHITE;

    can_castle_king_side[WHITE] = castling.find('K', 0) != std::string::npos;
    can_castle_queen_side[WHITE] = castling.find('Q', 0) != std::string::npos;;
    can_castle_king_side[BLACK] = castling.find('k', 0) != std::string::npos;;
    can_castle_queen_side[BLACK] = castling.find('q', 0) != std::string::npos;;

    this->en_passant = enpassant == "-" ? SQ_NONE : get_square(enpassant.c_str());

    if (!half_move_clock.empty())
        half_move_counter = (char)std::stoi(half_move_clock);
}

board board::simulate(const square from, const square to, const piece p, const color c) const {
    board simulated = *this;
    simulated.move_piece(from, to, p, c);
    return simulated;
}

void board::add_pawn_moves(bitboard origin, std::vector<move>& moves) const {
    const color c = piece_of_color[BLACK] & origin ? BLACK : WHITE;
    auto opponent = opposite(c);
    const bitboard opponent_piece = piece_of_color[opponent];
    // add normal moves, captures, en passant, and promotions
    const bitboard fwd = c == WHITE ? shift<UP>(origin) : shift<DOWN>(origin);
    const bool promotion = (rank_1 | rank_8) & fwd;
    const square origin_sq = get_square(origin);
    const bitboard empty = ~(piece_of_color[WHITE] | piece_of_color[BLACK]);

    if (empty & fwd) {
        const square dest = get_square(fwd);
        if (!simulate(origin_sq, dest, PAWN, c).under_check(c)) {
            if (promotion) {
                moves.push_back(get_move(origin_sq, dest, special_move::PROMOTION_QUEEN));
                moves.push_back(get_move(origin_sq, dest, special_move::PROMOTION_ROOK));
                moves.push_back(get_move(origin_sq, dest, special_move::PROMOTION_BISHOP));
                moves.push_back(get_move(origin_sq, dest, special_move::PROMOTION_KNIGHT));
            } else {
                moves.push_back(get_move(origin_sq, dest));
            }
        }
        if (c == WHITE && (rank_2 & origin)) {
            const bitboard fwd2 = shift<UP>(fwd);
            if ((empty & fwd2) && !simulate(origin_sq, get_square(fwd2), PAWN, c).under_check(c))
                moves.push_back(get_move(origin_sq, get_square(fwd2)));
        } else if (c == BLACK && (rank_7 & origin)) {
            const bitboard fwd2 = shift<DOWN>(fwd);
            if ((empty & fwd2) && !simulate(origin_sq, get_square(fwd2), PAWN, c).under_check(c))
                moves.push_back(get_move(origin_sq, get_square(fwd2)));
        }
    }

    auto enpassant_bb = get_bb(en_passant);
    if (file_a_i & origin) {
        const bitboard cap = shift<LEFT>(fwd);
        if ((opponent_piece & cap) || cap == enpassant_bb) {
            const square dest = get_square(cap);
            auto bnew = simulate(origin_sq, dest, PAWN, c);
            if (cap == enpassant_bb) {
                auto en_passant_bbi = ~enpassant_bb;
                bnew.piece_of_color[opponent] &= en_passant_bbi; // remove captured piece
                bnew.piece_of_type[PAWN] &= en_passant_bbi;
            }
            if (!bnew.under_check(c)) {
                if (promotion) {
                    moves.push_back(get_move(origin_sq, dest, special_move::PROMOTION_QUEEN));
                    moves.push_back(get_move(origin_sq, dest, special_move::PROMOTION_KNIGHT));
                    moves.push_back(get_move(origin_sq, dest, special_move::PROMOTION_ROOK));
                    moves.push_back(get_move(origin_sq, dest, special_move::PROMOTION_BISHOP));
                } else {
                    moves.push_back(get_move(origin_sq, dest));
                }
            }
        }
    }

    if (file_h_i & origin) {
        const bitboard cap = shift<RIGHT>(fwd);
        if ((opponent_piece & cap) || cap == enpassant_bb) {
            const square dest = get_square(cap);
            auto bnew = simulate(origin_sq, dest, PAWN, c);
            if (cap == enpassant_bb) {
                auto en_passant_bbi = ~enpassant_bb;
                bnew.piece_of_color[opponent] &= en_passant_bbi; // remove captured piece
                bnew.piece_of_type[PAWN] &= en_passant_bbi;
            }
            if (!bnew.under_check(c)) {
                if (promotion) {
                    moves.push_back(get_move(origin_sq, dest, special_move::PROMOTION_QUEEN));
                    moves.push_back(get_move(origin_sq, dest, special_move::PROMOTION_KNIGHT));
                    moves.push_back(get_move(origin_sq, dest, special_move::PROMOTION_ROOK));
                    moves.push_back(get_move(origin_sq, dest, special_move::PROMOTION_BISHOP));
                } else {
                    moves.push_back(get_move(origin_sq, dest));
                }
            }
        }
    }
}

void board::add_knight_moves(bitboard origin, std::vector<move>& moves) const {
    color c = piece_of_color[BLACK] & origin ? BLACK : WHITE;
    auto origin_square = get_square(origin);
    bitboard dest[8];
    int N = 0;

    if (file_a_i_rank_8_i_rank_7_i & origin) dest[N++] = shift<UP_LEFT + UP>(origin);
    if (file_a_i_rank_1_i_rank_2_i & origin) dest[N++] = shift<DOWN_LEFT + DOWN>(origin);
    if (file_h_i_rank_8_i_rank_7_i & origin) dest[N++] = shift<UP_RIGHT + UP>(origin);
    if (file_h_i_rank_1_i_rank_2_i & origin) dest[N++] = shift<DOWN_RIGHT + DOWN>(origin);
    if (file_a_i_rank_8_i_file_b_i & origin) dest[N++] = shift<UP_LEFT + LEFT>(origin);
    if (file_a_i_rank_1_i_file_b_i & origin) dest[N++] = shift<DOWN_LEFT + LEFT>(origin);
    if (file_h_i_rank_8_i_file_g_i & origin) dest[N++] = shift<UP_RIGHT + RIGHT>(origin);
    if (file_h_i_rank_1_i_file_g_i & origin) dest[N++] = shift<DOWN_RIGHT + RIGHT>(origin);

    for (int i = 0; i < N; i++) {
        if ((dest[i] & piece_of_color[c]) != 0) continue;
        if (!simulate(origin_square, get_square(dest[i]), KNIGHT, c).under_check(c)) {
            moves.push_back(get_move(origin_square, get_square(dest[i])));
        }
    }
}

/**
 * Does not support Chess960 yet!!!
 * @param c
 * @param moves
 */
void board::add_castle_moves(color c, std::vector<move> &moves) const {
    const bitboard anypiece = piece_of_color[WHITE] | piece_of_color[BLACK];
    bool check_checked = false;

    const bitboard king = get_bb(king_pos[c]);
    board simulated = *this;
    if (can_castle_king_side[c]) {
        if (under_check(c)) return;
        else check_checked = true;
        if (!(anypiece & (shift<RIGHT>(king) | shift<2 * RIGHT>(king)))) {
            simulated.set_king_position(c, get_square(shift<RIGHT>(king)));
            if (!simulated.under_check(c)) {
                simulated.set_king_position(c, get_square(shift<2 * RIGHT>(king)));
                if (!simulated.under_check(c)) {
                    const square dest = c == BLACK ? SQ_G8 : SQ_G1;
                    const special_move castle_king_side = c == WHITE ? CASTLE_KING_SIDE_WHITE : CASTLE_KING_SIDE_BLACK;
                    moves.push_back(get_move(king_pos[c], dest, castle_king_side));
                }
            }
        }
    }
    if (can_castle_queen_side[c]) {
        if (!check_checked) if (under_check(c)) return;
        if (!(anypiece & (shift<LEFT>(king) | shift<2 * LEFT>(king) | shift<3 * LEFT>(king)))) {
            simulated.set_king_position(c, get_square(shift<LEFT>(king)));
            if (!simulated.under_check(c)) {
                const square dest = c == BLACK ? SQ_C8 : SQ_C1;
                simulated.set_king_position(c, dest);
                if (!simulated.under_check(c)) {
                    special_move castle_queen_side = c == WHITE ? CASTLE_QUEEN_SIDE_WHITE : CASTLE_QUEEN_SIDE_BLACK;
                    moves.push_back(get_move(king_pos[c], dest, castle_queen_side));
                }
            }
        }
    }
}

void board::move_piece(square from, square to, piece p, color c) {
    if (king_pos[WHITE] == from) {
        set_king_position(WHITE, to);
        return;
    }

    if (king_pos[BLACK] == from) {
        set_king_position(BLACK, to);
        return;
    }

    bitboard ns = ~(get_bb(from));
    piece_of_type[p] &= ns;
    piece_of_color[c] &= ns;

    if ((to == king_pos[WHITE] && (p != KING || c != WHITE))
        || (to == king_pos[BLACK] && (p != KING || c != BLACK))) {
        std::cerr << "ERROR trying to put piece of type " << p << " on square " << to << std::endl;
        std::cerr << (side_to_play == WHITE ? "WHITE to play" : "BLACK to play") << std::endl;
        std::cerr << std::endl << to_string() << std::endl;
    }
    put_piece(p, c, to);
}

/*
 * According to LiChess behavior, losing castling rights, and change enpassant status does not reset the counter
 */
bool board::resets_half_move_counter(const move m) {
    return (piece_of_type[PAWN] & get_bb(move_origin(m))) // moving a pawn
            || (piece_of_color[BLACK] & get_bb(move_dest(m))) // captures piece
            || (piece_of_color[WHITE] & get_bb(move_dest(m))) // captures piece
            //|| move_type(m) != 0
            // || en_passant != SQ_NONE // will change en passant status
            // || (((file_a | file_e) & rank_1)[move_origin(m)] && can_castle_queen_side[WHITE]) // will lose castling right
            // || (((file_h | file_e) & rank_1)[move_origin(m)] && can_castle_king_side[WHITE]) // will lose castling right
            // || (((file_a | file_e) & rank_8)[move_origin(m)] && can_castle_queen_side[BLACK]) // will lose castling right
            // || (((file_h | file_e) & rank_8)[move_origin(m)] && can_castle_king_side[BLACK]) // will lose castling right
            ;
}

void board::make_move(const move m) {
    //assert(move_type(m) != NULL_MOVE);
    piece p = piece_at(get_bb(move_origin(m)));
    color c = color_at(get_bb(move_origin(m)));
    square new_en_passant = SQ_NONE;

    if (resets_half_move_counter(m)) half_move_counter = 0;
    else half_move_counter += 1;

    if (move_type(m) == 0) {
        move_piece(move_origin(m), move_dest(m), p, c);
        if (p == PAWN) {
            if (move_dest(m) == en_passant) {
                auto bbi = ~(get_bb(get_file(move_dest(m)), get_rank(move_origin(m))));
                piece_of_color[opposite(c)] &= bbi;
                piece_of_type[PAWN] &= bbi;
            }
            if (get_rank(move_dest(m)) - get_rank(move_origin(m)) == 2) {
                new_en_passant = get_square(get_file(move_origin(m)), get_rank(move_origin(m)) + 1);
            } else if (get_rank(move_origin(m)) - get_rank(move_dest(m)) == 2) {
                new_en_passant = get_square(get_file(move_origin(m)), get_rank(move_origin(m)) - 1);
            }
        }
    } else if (move_type(m) == special_move::CASTLE_KING_SIDE_WHITE) {
        move_piece(SQ_E1, SQ_G1, KING, c);
        move_piece(SQ_H1, SQ_F1, ROOK, c);
    } else if (move_type(m) == special_move::CASTLE_KING_SIDE_BLACK) {
        move_piece(SQ_E8, SQ_G8, KING, c);
        move_piece(SQ_H8, SQ_F8, ROOK, c);
    } else if (move_type(m) == special_move::CASTLE_QUEEN_SIDE_WHITE) {
        move_piece(SQ_E1, SQ_C1, KING, c);
        move_piece(SQ_A1, SQ_D1, ROOK, c);
    } else if (move_type(m) == special_move::CASTLE_QUEEN_SIDE_BLACK) {
        move_piece(SQ_E8, SQ_C8, KING, c);
        move_piece(SQ_A8, SQ_D8, ROOK, c);
    } else if (move_type(m) == special_move::PROMOTION_QUEEN) {
        auto bbi = ~(get_bb(move_origin(m)));
        piece_of_color[c] &= bbi;
        piece_of_type[PAWN] &= bbi;
        put_piece(QUEEN, c, move_dest(m));
    } else if (move_type(m) == special_move::PROMOTION_KNIGHT) {
        auto bbi = ~(get_bb(move_origin(m)));
        piece_of_color[c] &= bbi;
        piece_of_type[PAWN] &= bbi;
        put_piece(KNIGHT, c, move_dest(m));
    } else if (move_type(m) == special_move::PROMOTION_ROOK) {
        auto bbi = ~(get_bb(move_origin(m)));
        piece_of_color[c] &= bbi;
        piece_of_type[PAWN] &= bbi;
        put_piece(ROOK, c, move_dest(m));
    } else if (move_type(m) == special_move::PROMOTION_BISHOP) {
        auto bbi = ~(get_bb(move_origin(m)));
        piece_of_color[c] &= bbi;
        piece_of_type[PAWN] &= bbi;
        put_piece(BISHOP, c, move_dest(m));
    }
    if (move_origin(m) == SQ_A1 || move_dest(m) == SQ_A1 || move_origin(m) == SQ_E1) can_castle_queen_side[WHITE] = false;
    if (move_origin(m) == SQ_H1 || move_dest(m) == SQ_H1 || move_origin(m) == SQ_E1) can_castle_king_side[WHITE] = false;
    if (move_origin(m) == SQ_A8 || move_dest(m) == SQ_A8 || move_origin(m) == SQ_E8) can_castle_queen_side[BLACK] = false;
    if (move_origin(m) == SQ_H8 || move_dest(m) == SQ_H8 || move_origin(m) == SQ_E8) can_castle_king_side[BLACK] = false;

    en_passant = new_en_passant;
    side_to_play = opposite(side_to_play);
}

std::string board::move_in_pgn(const move m, const std::vector<move>& legal_moves) const {

    auto origin_bb = get_bb(move_origin(m));
    color c = color_at(origin_bb);
    piece p = piece_at(origin_bb);
    stringstream ss;
    ss << (p == KING ? "K" : p == QUEEN ? "Q" : p == ROOK ? "R" : p == BISHOP ? "B" : p == KNIGHT ? "N" : "");

    if (p == KING) {
        if (move_type(m) == CASTLE_KING_SIDE_WHITE || move_type(m) == CASTLE_KING_SIDE_BLACK) return "O-O";
        if (move_type(m) == CASTLE_QUEEN_SIDE_WHITE || move_type(m) == CASTLE_QUEEN_SIDE_BLACK) return "O-O-O";
    }

    bool is_capture = (piece_of_color[opposite(c)] & get_bb(move_dest(m))) || (p == PAWN && get_file(move_origin(m)) != get_file(move_dest(m)));

    if ((p == PAWN && is_capture) || std::find_if(begin(legal_moves), end(legal_moves), [&] (const move& other) {
        return move_dest(other) == move_dest(m)
               && (piece_of_type[p] & get_bb(move_origin(other)))
               && get_file(move_origin(other)) != get_file(move_origin(m));
    }) != end(legal_moves)) {
        ss << get_file_char(move_origin(m));
    }

    if (std::find_if(begin(legal_moves), end(legal_moves), [&] (const move& other) {
        return move_dest(other) == move_dest(m)
               && (piece_of_type[p] & get_bb(move_origin(other)))
               && get_rank(move_origin(other)) != get_rank(move_origin(m));
    }) != end(legal_moves)) {
        ss << get_rank_char(move_origin(m));
    }

    if (is_capture) ss << "x";

    ss << move_dest(m);

    if (move_type(m) == PROMOTION_QUEEN) ss << "=Q";
    if (move_type(m) == PROMOTION_KNIGHT) ss << "=N";
    if (move_type(m) == PROMOTION_ROOK) ss << "=R";
    if (move_type(m) == PROMOTION_BISHOP) ss << "=B";

    board simulated = *this;
    simulated.make_move(m);
    if (simulated.under_check(opposite(c))) {
        if (simulated.get_legal_moves(opposite(c)).empty()) {
            ss << "#";
        } else {
            ss << "+";
        }
    }
    return ss.str();
}

/**
 * This is for 3-fold-repetition purposes
 */
bool board::operator==(const board &other) const {
    return side_to_play == other.side_to_play
        && en_passant == other.en_passant
        && piece_of_color[WHITE] == other.piece_of_color[WHITE]
        && piece_of_color[BLACK] == other.piece_of_color[BLACK]
        && piece_of_type[PAWN] == other.piece_of_type[PAWN]
        && piece_of_type[BISHOP] == other.piece_of_type[BISHOP]
        && piece_of_type[ROOK] == other.piece_of_type[ROOK]
        && piece_of_type[KNIGHT] == other.piece_of_type[KNIGHT]
        && piece_of_type[QUEEN] == other.piece_of_type[QUEEN]
        && king_pos[WHITE] == other.king_pos[WHITE]
        && king_pos[BLACK] == other.king_pos[BLACK]
        && can_castle_king_side[BLACK] == other.can_castle_king_side[BLACK]
        && can_castle_queen_side[BLACK] == other.can_castle_queen_side[BLACK]
        && can_castle_king_side[WHITE] == other.can_castle_king_side[WHITE]
        && can_castle_queen_side[WHITE] == other.can_castle_queen_side[WHITE]
    ;
}

template<shift_direction d>
bitboard board::shift_attacks(const bitboard origin, const bitboard in_range) const {
    bitboard any_piece = piece_of_color[WHITE] | piece_of_color[BLACK];
    bitboard sq = origin;
    bitboard attacks = 0;
    while (in_range & sq) {
        sq = shift<d>(sq);
        attacks |= sq;
        if (any_piece & sq) break;
    }
    return attacks;
}

template<shift_direction d>
void board::shift_moves(const bitboard origin, const bitboard in_range, std::vector<move>& moves, piece p, color c) const {
    bitboard player_piece = piece_of_color[c];
    bitboard sq = origin;
    while (in_range & sq) {
        sq = shift<d>(sq);
        if (player_piece & sq) return; // blocked by own piece
        if(sq == get_bb(king_pos[opposite(c)])) throw std::runtime_error("capture king"); // found opponent king
        // simulate move
        if (!simulate(get_square(origin), get_square(sq), p, c).under_check(c)) {
            moves.push_back(get_move(get_square(origin), get_square(sq)));
        }
        if (piece_of_color[opposite(c)] & sq) break; // captured opponent piece: stop there
    }
}

void board::make_move(square from, square to) {
    this->make_move(get_move(from, to));
}


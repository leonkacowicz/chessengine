#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <chess/core.h>
#include <chess/board.h>
#include <chess/move.h>
#include <chess/magic_bitboard.h>
#include <chess/move_gen.h>

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
            bitboard bb = get_bb(x, y);
            if (bb & piece_of_color[WHITE]) {
                if (bb & piece_of_type[BISHOP]) ret << " ♗";
                else if (bb & piece_of_type[ROOK]) ret << " ♖";
                else if (bb & piece_of_type[KNIGHT]) ret << " ♘";
                else if (bb & piece_of_type[QUEEN]) ret << " ♕";
                else if (bb & piece_of_type[PAWN]) ret << " ♙";
                else if (bb & get_bb(king_pos[WHITE])) ret << " ♔";
                else ret << " X";
            }
            else if (bb & piece_of_color[BLACK]) {
                if (bb & piece_of_type[BISHOP]) ret << " ♝";
                else if (bb & piece_of_type[ROOK]) ret << " ♜";
                else if (bb & piece_of_type[KNIGHT]) ret << " ♞";
                else if (bb & piece_of_type[QUEEN]) ret << " ♛";
                else if (bb & piece_of_type[PAWN]) ret << " ♟";
                else if (bb & get_bb(king_pos[BLACK])) ret << " ♚";
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

void board::put_piece(piece p, color c, square s) {
    assert(p != NO_PIECE);
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

board board::simulate(const square from, const square to, const piece p, const color c) const {
    board simulated = *this;
    simulated.move_piece(from, to, p, c);
    return simulated;
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
        if (move_gen(simulated).generate().empty()) {
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

void board::make_move(square from, square to) {
    this->make_move(get_move(from, to));
}

board::board() {
}

board board::flip_colors() const {
    board b;
    for (square sq = SQ_A1; sq < SQ_NONE; ++sq) {
        bitboard bb = get_bb(sq);
        piece p = piece_at(bb);
        if (p == NO_PIECE) continue;
        color c = color_at(bb);
        b.put_piece(p, opposite(c), get_square(get_file(sq), 7 - get_rank(sq)));
    }
    b.can_castle_queen_side[color::WHITE] = can_castle_queen_side[color::BLACK];
    b.can_castle_queen_side[color::BLACK] = can_castle_queen_side[color::WHITE];
    b.can_castle_king_side[color::WHITE] = can_castle_king_side[color::BLACK];
    b.can_castle_king_side[color::BLACK] = can_castle_king_side[color::WHITE];
    if (en_passant != SQ_NONE) {
        b.en_passant = get_square(get_file(en_passant), 7 - get_rank(en_passant));
    }
    b.side_to_play = opposite(side_to_play);
    return b;
}


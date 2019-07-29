#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <functional>
#include <algorithm>
#include "color.h"
#include "board.h"
#include "move.h"

using std::string;
using std::stringstream;
using std::cout;
using std::endl;

bitboard board::range_attacks(const bitboard origin, const std::function<bitboard(bitboard)>& shift, const bitboard in_range) const {
    bitboard any_piece = piece_of_color[WHITE] | piece_of_color[BLACK];
    bitboard sq = origin;
    bitboard attacks = 0;
    while (in_range[sq]) {
        sq = shift(sq);
        attacks |= sq;
        if (any_piece[sq]) break;
    }
    return attacks;
}

void board::range_moves(const bitboard origin, const std::function<bitboard(bitboard)>& shift, const bitboard in_range, std::vector<move>& moves) const {
    color c = piece_of_color[WHITE][origin] ? WHITE : BLACK;
    bitboard player_piece = piece_of_color[c];
    bitboard sq = origin;
    while (in_range[sq]) {
        sq = shift(sq);
        if (player_piece[sq]) return; // blocked by own piece
        if (sq == king_pos[opposite(c)]) return; // found opponent king
        // simulate move
        board newboard = *this;
        newboard.move_piece(origin.get_square(), sq.get_square());
        if (!newboard.under_check(c)) {
            moves.emplace_back(origin.get_square(), sq.get_square());
        }
    }
}

bool board::under_check(color c) const {

    bitboard king = king_pos[c];
    if (king == 0) return false;
    // checks if attacked by rook or queen in horizontal or vertical directions
    bitboard opponent_piece = piece_of_color[opposite(c)];
    bitboard attacker = (piece_of_type[ROOK] | piece_of_type[QUEEN]) & opponent_piece;
    if ((range_attacks(king, [](bitboard b) { return b.shift_up(1); }, ~rank_8) & attacker) != 0) return true;
    if ((range_attacks(king, [](bitboard b) { return b.shift_down(1); }, ~rank_1) & attacker) != 0) return true;
    if ((range_attacks(king, [](bitboard b) { return b.shift_left(1); }, ~file_a) & attacker) != 0) return true;
    if ((range_attacks(king, [](bitboard b) { return b.shift_right(1); }, ~file_h) & attacker) != 0) return true;

    // checks if attacked by bishop or queen in diagonal directions
    attacker = (piece_of_type[BISHOP] | piece_of_type[QUEEN]) & opponent_piece;
    if ((range_attacks(king, [](bitboard b) { return b.shift_up_left(1); }, ~(rank_8 | file_a)) & attacker) != 0) return true;
    if ((range_attacks(king, [](bitboard b) { return b.shift_up_right(1); }, ~(rank_8 | file_h)) & attacker) != 0) return true;
    if ((range_attacks(king, [](bitboard b) { return b.shift_down_left(1); }, ~(rank_1 | file_a)) & attacker) != 0) return true;
    if ((range_attacks(king, [](bitboard b) { return b.shift_down_right(1); }, ~(rank_1 | file_h)) & attacker) != 0) return true;

    // check if attacked by knight
    if ((knight_attacks(king) & piece_of_type[KNIGHT] & opponent_piece) != 0) return true;

    // check if attacked by king
    if ((king_attacks(king)[king_pos[opposite(c)]])) return true;

    // calls pawn_attacks pretending king is pawn, and if it were a pawn, then if it attacks a pawn, it means that it is also attacked by that pawn
    return pawn_attacks(king, c)[piece_of_type[PAWN] & opponent_piece];
}

bool board::is_checkmate() const {
    return false;
}

bool board::is_stalemate() const {
    return !is_checkmate() && get_legal_moves(side_to_play ? BLACK : WHITE).empty();
}

string board::to_string() const {
    stringstream ret;
    for (int y = 7; y >= 0; y--) {
        ret << " " << (y + 1) << "  ";
        for (int x = 0; x <= 7; x++) {
            bitboard position(x, y);
            if (piece_of_color[WHITE][position]) {
                if (piece_of_type[BISHOP][position]) ret << " ♗";
                else if (piece_of_type[ROOK][position]) ret << " ♖";
                else if (piece_of_type[KNIGHT][position]) ret << " ♘";
                else if (piece_of_type[QUEEN][position]) ret << " ♕";
                else if (piece_of_type[PAWN][position]) ret << " ♙";
                else ret << " ♔";
            }
            else if (piece_of_color[BLACK][position]) {
                if (piece_of_type[BISHOP][position]) ret << " ♝";
                else if (piece_of_type[ROOK][position]) ret << " ♜";
                else if (piece_of_type[KNIGHT][position]) ret << " ♞";
                else if (piece_of_type[QUEEN][position]) ret << " ♛";
                else if (piece_of_type[PAWN][position]) ret << " ♟";
                else ret << " ♚";
            } else {
                ret << " .";
            }
        }
        ret << std::endl;
    }
    ret << std::endl << "     a b c d e f g h" << std::endl;
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
    king_pos[WHITE] = "e1";
    king_pos[BLACK] = "e8";
    can_castle_king_side[0] = true;
    can_castle_king_side[1] = true;
    can_castle_queen_side[0] = true;
    can_castle_queen_side[1] = true;
    side_to_play = WHITE;
}

std::vector<move> board::get_legal_moves(color c) const {
    std::vector<move> moves;

    for (auto sq = bitboard(1); !sq.isEmpty(); sq <<= 1) {
        if (!piece_of_color[c][sq]) continue;
        if (king_pos[c] == sq.get_square()) add_king_moves(sq, moves);
        if (piece_of_type[PAWN][sq]) add_pawn_moves(sq, moves);
        if (piece_of_type[KNIGHT][sq]) add_knight_moves(sq, moves);
        if (piece_of_type[BISHOP][sq]) add_bishop_moves(sq, moves);
        if (piece_of_type[ROOK][sq]) add_rook_moves(sq, moves);
        if (piece_of_type[QUEEN][sq]) {
            add_bishop_moves(sq, moves);
            add_rook_moves(sq, moves);
        }
    }
    if (can_castle_king_side[c] || can_castle_queen_side[c]) {
        add_castle_moves(c, moves);
    }
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

void board::add_rook_moves(bitboard origin, std::vector<move>& moves) const {
    range_moves(origin, [] (bitboard b) { return b.shift_up(1); }, ~rank_8, moves);
    range_moves(origin, [] (bitboard b) { return b.shift_down(1); }, ~rank_1, moves);
    range_moves(origin, [] (bitboard b) { return b.shift_left(1); }, ~file_a, moves);
    range_moves(origin, [] (bitboard b) { return b.shift_right(1); }, ~file_h, moves);
}

void board::add_bishop_moves(bitboard origin, std::vector<move>& moves) const {
    range_moves(origin, [] (bitboard b) { return b.shift_up_left(1); }, ~(file_a | rank_8), moves);
    range_moves(origin, [] (bitboard b) { return b.shift_down_right(1); }, ~(file_h | rank_1), moves);
    range_moves(origin, [] (bitboard b) { return b.shift_down_left(1); }, ~(file_a | rank_1), moves);
    range_moves(origin, [] (bitboard b) { return b.shift_up_right(1); }, ~(file_h | rank_8), moves);
}

void board::add_king_moves(bitboard origin, std::vector<move>& moves) const {
    color c = piece_of_color[BLACK][origin] ? BLACK : WHITE;
    auto origin_square = origin.get_square();
    bitboard in_range[8];
    int N = 0;

    if (!rank_8[origin]) {
        in_range[N++] = origin.shift_up(1);
        if (!file_a[origin]) in_range[N++] = origin.shift_up_left(1);
        if (!file_h[origin]) in_range[N++] = origin.shift_up_right(1);
    }
    if (!rank_1[origin]) {
        in_range[N++] = origin.shift_down(1);
        if (!file_a[origin]) in_range[N++] = origin.shift_down_left(1);
        if (!file_h[origin]) in_range[N++] = origin.shift_down_right(1);
    }
    if (!file_a[origin]) in_range[N++] = origin.shift_left(1);
    if (!file_h[origin]) in_range[N++] = origin.shift_right(1);

    board simulated = *this;
    for (int i = 0; i < N; i++, simulated = *this) {
        simulated.set_king_position(c, in_range[i].get_square());
        if (!simulated.under_check(c)) moves.emplace_back(origin_square, in_range[i].get_square());
    }
}

board::board(const std::string& fen) {

    using std::string;
    using std::stringstream;

    std::stringstream ss(fen);

    std::string piecePlacement;
    getline(ss, piecePlacement, ' ');

    std::string side_to_move;
    getline(ss, side_to_move, ' ');

    std::string castling;
    getline(ss, castling, ' ');

    std::string en_passant;
    getline(ss, en_passant, ' ');

    std::string half_move_clock;
    getline(ss, half_move_clock, ' ');

    std::string full_move_counter;
    getline(ss, full_move_counter, ' ');

    side_to_play = side_to_move == "b" ? BLACK : WHITE;
}

board board::simulate(const square from, const square to) const {
    board simulated = *this;
    simulated.move_piece(from, to);
    return simulated;
}

void board::add_pawn_moves(bitboard origin, std::vector<move>& moves) const {
    const color c = piece_of_color[BLACK][origin] ? BLACK : WHITE;
    const bitboard opponent_piece = piece_of_color[opposite(c)];
    // add normal moves, captures, en passant, and promotions
    const bitboard fwd = c == WHITE ? origin.shift_up(1) : origin.shift_down(1);
    const bool promotion = (rank_1 | rank_8)[fwd];
    const square origin_sq = origin.get_square();
    const bitboard empty = ~(piece_of_color[WHITE] | piece_of_color[BLACK]);

    if (empty[fwd]) {
        const square dest = fwd.get_square();
        if (!simulate(origin_sq, dest).under_check(c)) {
            if (promotion) {
                moves.emplace_back(origin_sq, dest, special_move::PROMOTION_QUEEN);
                moves.emplace_back(origin_sq, dest, special_move::PROMOTION_ROOK);
                moves.emplace_back(origin_sq, dest, special_move::PROMOTION_BISHOP);
                moves.emplace_back(origin_sq, dest, special_move::PROMOTION_KNIGHT);
            } else {
                moves.emplace_back(origin_sq, dest);
            }
        }
        if (c == WHITE && rank_2[origin]) {
            const bitboard fwd2 = fwd.shift_up(1);
            if (empty[fwd2])
                moves.emplace_back(origin_sq, fwd2.get_square());
        } else if (c == BLACK && rank_7[origin]) {
            const bitboard fwd2 = fwd.shift_down(1);
            if (empty[fwd2])
                moves.emplace_back(origin_sq, fwd2.get_square());
        }
    }

    if (!file_a[origin]) {
        const bitboard cap = fwd.shift_left(1);
        if (opponent_piece[cap] || cap == en_passant) {
            const square dest = cap.get_square();
            if (!simulate(origin_sq, dest).under_check(c)) {
                if (promotion) {
                    moves.emplace_back(origin_sq, dest, special_move::PROMOTION_QUEEN);
                    moves.emplace_back(origin_sq, dest, special_move::PROMOTION_ROOK);
                    moves.emplace_back(origin_sq, dest, special_move::PROMOTION_BISHOP);
                    moves.emplace_back(origin_sq, dest, special_move::PROMOTION_KNIGHT);
                } else {
                    moves.emplace_back(origin_sq, dest);
                }
            }
        }
    }

    if (!file_h[origin]) {
        const bitboard cap = fwd.shift_right(1);
        if (opponent_piece[cap] || cap == en_passant) {
            const square dest = cap.get_square();
            if (!simulate(origin_sq, dest).under_check(c)) {
                if (promotion) {
                    moves.emplace_back(origin_sq, dest, special_move::PROMOTION_QUEEN);
                    moves.emplace_back(origin_sq, dest, special_move::PROMOTION_ROOK);
                    moves.emplace_back(origin_sq, dest, special_move::PROMOTION_BISHOP);
                    moves.emplace_back(origin_sq, dest, special_move::PROMOTION_KNIGHT);
                } else {
                    moves.emplace_back(origin_sq, dest);
                }
            }
        }
    }
}

void board::add_knight_moves(bitboard origin, std::vector<move>& moves) const {
    color c = piece_of_color[BLACK][origin] ? BLACK : WHITE;
    auto origin_square = origin.get_square();
    bitboard in_range[8];
    int N = 0;

    if (!file_a[origin] && !rank_8[origin] && !rank_7[origin]) in_range[N++] = origin.shift_left(1).shift_up(2);
    if (!file_a[origin] && !rank_8[origin] && !file_b[origin]) in_range[N++] = origin.shift_left(2).shift_up(1);
    if (!file_a[origin] && !rank_1[origin] && !rank_2[origin]) in_range[N++] = origin.shift_left(1).shift_down(2);
    if (!file_a[origin] && !rank_1[origin] && !file_b[origin]) in_range[N++] = origin.shift_left(2).shift_down(1);
    if (!file_h[origin] && !rank_8[origin] && !rank_7[origin]) in_range[N++] = origin.shift_right(1).shift_up(2);
    if (!file_h[origin] && !rank_8[origin] && !file_g[origin]) in_range[N++] = origin.shift_right(2).shift_up(1);
    if (!file_h[origin] && !rank_1[origin] && !rank_2[origin]) in_range[N++] = origin.shift_right(1).shift_down(2);
    if (!file_h[origin] && !rank_1[origin] && !file_g[origin]) in_range[N++] = origin.shift_right(2).shift_down(1);

    board simulated = *this;
    for (int i = 0; i < N; i++, simulated = *this) {
        if ((in_range[i] & piece_of_color[c]) != 0) continue;
        simulated.move_piece(origin_square, in_range[i].get_square());
        if (!simulated.under_check(c)) {
            moves.emplace_back(origin_square, in_range[i].get_square());
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
    if (under_check(c)) return;
    const bitboard king = bitboard(king_pos[c]);
    board simulated = *this;
    if (can_castle_king_side[c]) {
        if (!(anypiece[king.shift_right(1)] || anypiece[king.shift_right(2)])) {
            simulated.set_king_position(c, king.shift_right(1).get_square());
            if (!simulated.under_check(c)) {
                simulated.set_king_position(c, king.shift_right(2).get_square());
                if (!simulated.under_check(c)) {
                    const square dest = square(6, (c == BLACK) * 7);
                    const special_move castle_king_side = c == WHITE ? CASTLE_KING_SIDE_WHITE : CASTLE_KING_SIDE_BLACK;
                    moves.emplace_back(king_pos[c], dest, castle_king_side);
                }
            }
        }
    }
    if (can_castle_queen_side[c]) {
        if (!(anypiece[king.shift_left(1)] || anypiece[king.shift_left(2)])) {
            simulated.set_king_position(c, king.shift_left(1).get_square());
            if (!simulated.under_check(c)) {
                simulated.set_king_position(c, king.shift_left(2).get_square());
                if (!simulated.under_check(c)) {
                    const square dest = square(2, (c == BLACK) * 7);
                    special_move castle_queen_side = c == WHITE ? CASTLE_QUEEN_SIDE_WHITE : CASTLE_QUEEN_SIDE_BLACK;
                    moves.emplace_back(king_pos[c], dest, castle_queen_side);
                }
            }
        }
    }
}

bitboard board::knight_attacks(const bitboard origin) const {

    bitboard attacks;

    if (!file_a[origin] && !rank_8[origin] && !rank_7[origin]) {
        attacks |= origin.shift_left(1).shift_up(2);
    }

    if (!file_a[origin] && !rank_1[origin] && !rank_2[origin]) {
        attacks |= origin.shift_left(1).shift_down(2);
    }

    if (!file_h[origin] && !rank_8[origin] && !rank_7[origin]) {
        attacks |= origin.shift_right(1).shift_up(2);
    }

    if (!file_h[origin] && !rank_1[origin] && !rank_2[origin]) {
        attacks |= origin.shift_right(1).shift_down(2);
    }

    if (!file_a[origin] && !file_b[origin] && !rank_8[origin]) {
        attacks |= origin.shift_left(2).shift_up(1);
    }

    if (!file_a[origin] && !file_b[origin] && !rank_1[origin]) {
        attacks |= origin.shift_left(2).shift_down(1);
    }

    if (!file_h[origin] && !file_g[origin] && !rank_8[origin]) {
        attacks |= origin.shift_right(2).shift_up(1);
    }

    if (!file_h[origin] && !file_g[origin] && !rank_1[origin]) {
        attacks |= origin.shift_right(2).shift_down(1);
    }

    return attacks;
}

bitboard board::king_attacks(const bitboard origin) const {
    bitboard attacks;
    if (!rank_8[origin]) {
        attacks |= origin.shift_up(1);

        if (!file_a[origin]) {
            attacks |= origin.shift_up_left(1);
        }

        if (!file_h[origin]) {
            attacks |= origin.shift_up_right(1);
        }
    }
    if (!rank_1[origin]) {
        auto south = origin.shift_down(1);
        attacks |= south;

        if (!file_a[origin]) {
            attacks |= origin.shift_down_left(1);
        }

        if (!file_h[origin]) {
            attacks |= origin.shift_down_right(1);
        }
    }
    if (!file_a[origin]) {
        attacks |= origin.shift_left(1);
    }
    if (!file_h[origin]) {
        attacks |= origin.shift_right(1);
    }
    
    return attacks;
}

/**
 * Assumes pawn cannot be in promotion rank. Does not account for en passant.
 */
bitboard board::pawn_attacks(const bitboard origin, const color attacker) const {
    bitboard attacks;
    if (attacker == WHITE) {
        if (!file_a[origin]) {
            attacks |= origin.shift_up_left(1);
        }
        if (!file_h[origin]) {
            attacks |= origin.shift_up_right(1);
        }
    } else {
        if (!file_a[origin]) {
            attacks |= origin.shift_down_left(1);
        }
        if (!file_h[origin]) {
            attacks |= origin.shift_down_right(1);
        }
    }
    return attacks;
}

void board::move_piece(square from, square to) {
    if (king_pos[WHITE] == from) {
        set_king_position(WHITE, to);
        return;
    }

    if (king_pos[BLACK] == from) {
        set_king_position(BLACK, to);
        return;
    }

    bitboard ns = ~(bitboard(from));
    piece p = piece_at(from);
    color c = color_at(from);
    piece_of_type[p] &= ns;
    piece_of_color[c] &= ns;
    put_piece(p, c, to);
}

/*
 * According to LiChess behavior, losing castling rights, and change enpassant status does not reset the counter
 */
bool board::resets_half_move_counter(const move m) {
    return  (piece_of_type[PAWN][m.origin]) // moving a pawn
            || piece_of_color[BLACK][m.destination] // captures piece
            || piece_of_color[WHITE][m.destination] // captures piece
            //|| m.special != 0
            // || en_passant != square::none // will change en passant status
            // || (((file_a | file_e) & rank_1)[m.origin] && can_castle_queen_side[WHITE]) // will lose castling right
            // || (((file_h | file_e) & rank_1)[m.origin] && can_castle_king_side[WHITE]) // will lose castling right
            // || (((file_a | file_e) & rank_8)[m.origin] && can_castle_queen_side[BLACK]) // will lose castling right
            // || (((file_h | file_e) & rank_8)[m.origin] && can_castle_king_side[BLACK]) // will lose castling right
            ;
}

void board::make_move(const move m) {
    piece p = piece_at(m.origin);
    color c = color_at(m.origin);
    en_passant = square::none;

    if (m.special == 0) {
        move_piece(m.origin, m.destination);
        if (p == PAWN) {
            if (m.destination == en_passant) {
                auto bbi = ~(bitboard(square(m.destination.get_file(), m.origin.get_rank())));
                piece_of_color[opposite(c)] &= bbi;
                piece_of_type[PAWN] &= bbi;
            }
            if (m.destination.get_rank() - m.origin.get_rank() == 2) {
                en_passant = square(m.origin.get_file(), m.origin.get_rank() + 1);
            } else if (m.origin.get_rank() - m.destination.get_rank() == 2) {
                en_passant = square(m.origin.get_file(), m.origin.get_rank() - 1);
            }
        }
        if (p == KING) {
            can_castle_king_side[c] = false;
            can_castle_queen_side[c] = false;
        }
    } else if (m.special == special_move::CASTLE_KING_SIDE_WHITE) {
        can_castle_king_side[c] = false;
        can_castle_queen_side[c] = false;
        move_piece("e1", "g1");
        move_piece("h1", "f1");
    } else if (m.special == special_move::CASTLE_KING_SIDE_BLACK) {
        can_castle_king_side[c] = false;
        can_castle_queen_side[c] = false;
        move_piece("e8", "g8");
        move_piece("h8", "f8");
    } else if (m.special == special_move::CASTLE_QUEEN_SIDE_WHITE) {
        can_castle_king_side[c] = false;
        can_castle_queen_side[c] = false;
        move_piece("e1", "c1");
        move_piece("a1", "d1");
    } else if (m.special == special_move::CASTLE_QUEEN_SIDE_BLACK) {
        can_castle_king_side[c] = false;
        can_castle_queen_side[c] = false;
        move_piece("e8", "c8");
        move_piece("a8", "d8");
    } else if (m.special == special_move::PROMOTION_QUEEN) {
        auto bbi = ~(bitboard(m.origin));
        piece_of_color[c] &= bbi;
        piece_of_type[PAWN] &= bbi;
        put_piece(QUEEN, c, m.destination);
    } else if (m.special == special_move::PROMOTION_KNIGHT) {
        auto bbi = ~(bitboard(m.origin));
        piece_of_color[c] &= bbi;
        piece_of_type[PAWN] &= bbi;
        put_piece(KNIGHT, c, m.destination);
    } else if (m.special == special_move::PROMOTION_ROOK) {
        auto bbi = ~(bitboard(m.origin));
        piece_of_color[c] &= bbi;
        piece_of_type[PAWN] &= bbi;
        put_piece(ROOK, c, m.destination);
    } else if (m.special == special_move::PROMOTION_BISHOP) {
        auto bbi = ~(bitboard(m.origin));
        piece_of_color[c] &= bbi;
        piece_of_type[PAWN] &= bbi;
        put_piece(BISHOP, c, m.destination);
    }
    side_to_play = opposite(side_to_play);
}

piece board::piece_at(bitboard p) const {
    if (p == king_pos[WHITE]) return KING;
    if (p == king_pos[BLACK]) return KING;

    return piece_of_type[PAWN][p] ? PAWN :
        piece_of_type[KNIGHT][p] ? KNIGHT :
        piece_of_type[BISHOP][p] ? BISHOP :
        piece_of_type[ROOK][p] ? ROOK : QUEEN;
}

color board::color_at(bitboard p) const {
    return piece_of_color[BLACK][p] ? BLACK : WHITE;
}

std::string board::move_in_pgn(const move m, const std::vector<move>& legal_moves) const {

    color c = color_at(m.origin);
    piece p = piece_at(m.origin);
    stringstream ss;
    ss << (p == KING ? "K" : p == QUEEN ? "Q" : p == ROOK ? "R" : p == BISHOP ? "B" : p == KNIGHT ? "N" : "");

    if (p == KING) {
        if (m.special == CASTLE_KING_SIDE_WHITE || m.special == CASTLE_KING_SIDE_BLACK) return "O-O";
        if (m.special == CASTLE_QUEEN_SIDE_WHITE || m.special == CASTLE_QUEEN_SIDE_BLACK) return "O-O-O";
        return "K" + m.destination.to_string();
    }

    bool is_capture = piece_of_color[opposite(c)][m.destination] || (p == PAWN && m.origin.get_file() != m.destination.get_file());

    if ((p == PAWN && is_capture) || std::find_if(begin(legal_moves), end(legal_moves), [&] (const move& other) {
        return other.destination == m.destination
               && piece_of_type[p][other.origin]
               && other.origin.get_file() != m.origin.get_file();
    }) != end(legal_moves)) {
        ss << m.origin.get_file_char();
    }

    if (std::find_if(begin(legal_moves), end(legal_moves), [&] (const move& other) {
        return other.destination == m.destination
               && piece_of_type[p][other.origin]
               && other.origin.get_rank() != m.origin.get_rank();
    }) != end(legal_moves)) {
        ss << m.origin.get_rank_char();
    }

    if (is_capture) ss << "x";

    ss << m.destination.to_string();

    if (m.special == PROMOTION_QUEEN) ss << "=Q";
    if (m.special == PROMOTION_KNIGHT) ss << "=N";
    if (m.special == PROMOTION_ROOK) ss << "=R";
    if (m.special == PROMOTION_BISHOP) ss << "=B";

    board simulated = *this;
    simulated.make_move(m);
    if (simulated.under_check(opposite(c))) {
        if (simulated.get_legal_moves(opposite(c)).empty()) {
            ss << "#";
        } else {
            ss << "+";
        }
    }
    string s;
    ss >> s;
    return s;
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


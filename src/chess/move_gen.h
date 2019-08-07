//
// Created by leon on 2019-08-03.
//

#ifndef CHESSENGINE_MOVE_GEN_H
#define CHESSENGINE_MOVE_GEN_H

#include <cassert>
#include "board.h"
#include "magic_bitboard.h"

enum pin_direction {
    VERTICAL, HORIZONTAL, DIAGONAL_P, DIAGONAL_S
};

enum evasiveness {
    EVASIVE, NON_EVASIVE
};

enum class move_list_type {
    VECTOR, ARRAY
};

class move_gen {
    const board& b;
    std::vector<move> moves;
    bitboard checkers;
    bitboard attacked;
    char num_checkers;
    bitboard king;
    bitboard pinned[4] = {0, 0, 0, 0}; // one per direction (we need this to know which direction the piece is pinned, you can still move towards or away from pinner while still protecting the king)
    bitboard pinned_any_dir = 0;
    color us;
    color them;
    bitboard our_piece = 0;
    bitboard their_piece = 0;
    bitboard any_piece = 0;
    bitboard block_mask = 0;

    int num_our_pieces = 0;
    bitboard our_pieces[16];
public:

    move_gen(const board& b) : b(b) {
        us = b.side_to_play;
        them = opposite(us);
        our_piece = b.piece_of_color[us];
        their_piece = b.piece_of_color[them];
        any_piece = our_piece | their_piece;
        moves.reserve(100);
    }

    std::vector<move>& generate();

    void generate_king_moves() {
        bitboard attacks = king_attacks(king);
        square s;
        while (attacks) {
            s = pop_lsb(&attacks);
            bitboard sbb = get_bb(s);
            if (((our_piece | attacked) & sbb) == 0) {
                add_move(b.king_pos[us], s);
            }
        }
    }

    void reset() {
        checkers = 0;
        num_checkers = 0;
        attacked = 0;
        block_mask = 0;
        king = get_bb(b.king_pos[b.side_to_play]);
        num_our_pieces = 0;
        for (int i = 0; i < 4; i++) pinned[i] = 0;
    }

    template <evasiveness e>
    void generate_non_king_moves() {
        for (int i = 0; i < num_our_pieces; i++) {
            bitboard sq = our_pieces[i];
            if (sq & (b.piece_of_type[ROOK] | b.piece_of_type[QUEEN])) rook_moves<e>(sq);
            if (sq & (b.piece_of_type[BISHOP] | b.piece_of_type[QUEEN])) bishop_moves<e>(sq);
            else if ((sq & b.piece_of_type[KNIGHT]) && !(pinned_any_dir & sq)) knight_moves<e>(sq);
            else if (sq & b.piece_of_type[PAWN]) {
                if (us == WHITE) pawn_moves<e, UP>(sq); else pawn_moves<e, DOWN>(sq);
            }
        }
    }

    void scan_board() {
        bitboard non_slider_attack;
        for (bitboard sq(1); sq != 0; sq <<= 1uL) {
            if (their_piece & sq) {
                if (sq & (b.piece_of_type[ROOK] | b.piece_of_type[QUEEN])) rook_attacks(sq);
                if (sq & (b.piece_of_type[BISHOP] | b.piece_of_type[QUEEN])) bishop_attacks(sq);
                else if (sq & b.piece_of_type[KNIGHT]) {
                    non_slider_attack = knight_attacks(sq);
                    attacked |= non_slider_attack;
                    if ((non_slider_attack & king) != 0) {
                        num_checkers++;
                        checkers |= sq;
                    }
                }
                else if (sq & b.piece_of_type[PAWN]) {
                    non_slider_attack = pawn_attacks(sq, them);
                    attacked |= non_slider_attack;
                    if ((non_slider_attack & king) != 0) {
                        num_checkers++;
                        checkers |= sq;
                    }
                }
            } else if (sq & our_piece) {
                our_pieces[num_our_pieces++] = sq;
            }
        }
        non_slider_attack = king_attacks(b.king_pos[them]);
        attacked |= non_slider_attack;
    }

    template <evasiveness e>
    void rook_moves(const bitboard origin) {
        square origin_sq = get_square(origin);
        bitboard attacks = attacks_from_rook(origin_sq, b.piece_of_color[WHITE] | b.piece_of_color[BLACK]);
        attacks &= ~our_piece;
        if (e == EVASIVE) attacks &= (checkers | block_mask); // remove squares that don't block the checker or capture it

        if (!(pinned_any_dir & origin)) {
            // piece not pinned, anything will be legal
            while (attacks) {
                square dest = pop_lsb(&attacks);
                add_move(origin_sq, dest);
            }
        } else {
            while (attacks) {
                square dest = pop_lsb(&attacks);
                if (line[origin_sq][dest] & king) // if piece is pinned, it can only move away from or towards the king, but not any other direction
                    add_move(origin_sq, dest);
            }
        }
    }

    template <evasiveness e>
    void bishop_moves(const bitboard origin) {
        square origin_sq = get_square(origin);
        bitboard attacks = attacks_from_bishop(origin_sq, b.piece_of_color[WHITE] | b.piece_of_color[BLACK]);
        attacks &= ~our_piece;
        if (e == EVASIVE) attacks &= (checkers | block_mask); // remove squares that don't block the checker or capture it

        if (!(pinned_any_dir & origin)) {
            // piece not pinned, anything will be legal
            while (attacks) {
                square dest = pop_lsb(&attacks);
                add_move(origin_sq, dest);
            }
        } else {
            while (attacks) {
                square dest = pop_lsb(&attacks);
                if (line[origin_sq][dest] & king) // if piece is pinned, it can only move away from or towards the king, but not any other direction
                    add_move(origin_sq, dest);
            }
        }
    }

    void rook_attacks(const bitboard origin) {
        square origin_sq = get_square(origin);
        bitboard attacks = attacks_from_rook(origin_sq, any_piece & ~king);
        attacked |= attacks;
        if (attacks & king) {
            // under check
            num_checkers++;
            checkers |= origin;
            block_mask |= line_segment[origin_sq][b.king_pos[us]];
        } else {
            bitboard path = line_segment[origin_sq][b.king_pos[us]];
            bitboard blockers = path & our_piece & ~king;
            if (num_squares(blockers) == 1) {
                // only 1 piece blocking the attack, therefore it's pinned
                pinned_any_dir |= blockers;
            }
        }
    }

    void bishop_attacks(const bitboard origin) {
        square origin_sq = get_square(origin);
        bitboard attacks = attacks_from_bishop(origin_sq, any_piece & ~king);
        attacked |= attacks;
        if (attacks & king) {
            // under check
            num_checkers++;
            checkers |= origin;
            block_mask |= line_segment[origin_sq][b.king_pos[us]];
        } else {
            bitboard path = line_segment[origin_sq][b.king_pos[us]];
            bitboard blockers = path & our_piece & ~king;
            if (num_squares(blockers) == 1) {
                // only 1 piece blocking the attack, therefore it's pinned
                pinned_any_dir |= blockers;
            }
        }
    }

    template <evasiveness e>
    void knight_moves(bitboard origin) {
        bitboard attacks = knight_attacks(origin);
        square s;
        while (attacks) {
            s = pop_lsb(&attacks);
            bitboard sbb = get_bb(s);
            if (our_piece & sbb) continue;
            if (e == NON_EVASIVE || ((checkers | block_mask) & sbb)) {
                add_move(get_square(origin), s);
            }
        }
    }

    template <evasiveness e, shift_direction d>
    void pawn_moves(bitboard origin) {
        // add normal moves, captures, en passant, and promotions
        const bitboard fwd = shift<d>(origin);
        const bool promotion = (rank_1 | rank_8) & fwd;
        const square origin_sq = get_square(origin);
        const bitboard empty = ~(our_piece | their_piece);

        square dest = get_square(fwd);
        if ((empty & fwd) && (!(pinned_any_dir & origin) || (line[origin_sq][dest] & king))) {
            if (e == NON_EVASIVE || (block_mask & fwd)) {
                if (promotion) {
                    add_move(origin_sq, dest, special_move::PROMOTION_QUEEN);
                    add_move(origin_sq, dest, special_move::PROMOTION_ROOK);
                    add_move(origin_sq, dest, special_move::PROMOTION_BISHOP);
                    add_move(origin_sq, dest, special_move::PROMOTION_KNIGHT);
                } else {
                    add_move(origin_sq, dest);
                }
            }
            if ((d == UP && (rank_2 & origin)) || (d == DOWN && (rank_7 & origin))) {
                const bitboard fwd2 = shift<d>(fwd);
                if (empty & fwd2)
                    if (e == NON_EVASIVE || (block_mask & fwd2))
                        add_move(origin_sq, get_square(fwd2));  // moves.emplace_back(origin_sq, get_square(fwd2));
            }
        }

        if (origin & file_a_i) {
            bitboard cap = shift<LEFT>(fwd);
            dest = get_square(cap);
            if (!(pinned_any_dir & origin) || (line[origin_sq][dest] & king)) {
                pawn_captures<e>(promotion, origin_sq, cap);
            }
        }

        if (origin & file_h_i) {
            bitboard cap = shift<RIGHT>(fwd);
            dest = get_square(cap);
            if (!(pinned_any_dir & origin) || (line[origin_sq][dest] & king)) {
                pawn_captures<e>(promotion, origin_sq, cap);
            }
        }
    }

    template<evasiveness e>
    inline void pawn_captures(const bool promotion, const square origin_sq, const bitboard cap) {
        if (their_piece & cap) {
            const square dest = get_square(cap);
            if (e == NON_EVASIVE || (checkers & cap)) {
                if (promotion) {
                    add_move(origin_sq, dest, special_move::PROMOTION_QUEEN);
                    add_move(origin_sq, dest, special_move::PROMOTION_ROOK);
                    add_move(origin_sq, dest, special_move::PROMOTION_BISHOP);
                    add_move(origin_sq, dest, special_move::PROMOTION_KNIGHT);
                } else {
                    //moves.emplace_back(origin_sq, dest);
                    add_move(origin_sq, dest);
                }
            }
        } else if (cap & get_bb(b.en_passant)) {
            const square dest = get_square(cap);
            if (e == EVASIVE || get_rank(b.king_pos[us]) == get_rank(origin_sq)) {
                // this is a corner case that needs to be simulated
                board bnew = b.simulate(origin_sq, get_square(cap), PAWN, us);
                auto en_passant_bbi = ~(get_bb(get_file(b.en_passant), get_rank(origin_sq)));
                bnew.piece_of_color[them] &= en_passant_bbi; // remove captured piece
                bnew.piece_of_type[PAWN] &= en_passant_bbi;
                if (!bnew.under_check(us)) {
                    add_move(origin_sq, dest);
                }
            } else {
                add_move(origin_sq, dest);
            }
        }
    }

    template<color us>
    void castle_moves() {
        const bitboard anypiece = our_piece | their_piece;
        if (b.can_castle_king_side[us]) {
            auto path = shift<RIGHT>(king) | shift<2 * RIGHT>(king);
            if ((anypiece & path) == 0) {
                if ((attacked & path) == 0) {
                    if (us == BLACK)
                        add_move(b.king_pos[us], SQ_G8, CASTLE_KING_SIDE_BLACK);
                    else
                        add_move(b.king_pos[us], SQ_G1, CASTLE_KING_SIDE_WHITE);
                }
            }
        }
        if (b.can_castle_queen_side[us]) {
            auto path = shift<LEFT>(king) | shift<2 * LEFT>(king);
            if ((anypiece & (path | shift<LEFT>(path))) == 0) {
                if ((attacked & path) == 0) {
                    if (us == BLACK)
                        add_move(b.king_pos[us], SQ_C8, CASTLE_QUEEN_SIDE_BLACK);
                    else
                        add_move(b.king_pos[us], SQ_C1, CASTLE_QUEEN_SIDE_WHITE);
                }
            }
        }
    }

    inline void add_move(square from, square to, special_move special = NOT_SPECIAL);
};

inline std::vector<move>& move_gen::generate() {
    reset();
    scan_board();
    //print_bb(attacked);
    //print_bb(pinned_any_dir);

    generate_king_moves();
    if (num_checkers == 2) {
        // only legal moves will be moving the king away from check
    } else if (num_checkers == 1) {
        // only generate moves that remove check
        generate_non_king_moves<EVASIVE>();
    } else {
        generate_non_king_moves<NON_EVASIVE>(); // generate any move that does not put the king in check
        if (us == WHITE) castle_moves<WHITE>();
        else castle_moves<BLACK>();
    }
    return moves;
}

inline void move_gen::add_move(square from, square to, special_move special) {
    moves.emplace_back(from, to, special);
}

#endif //CHESSENGINE_MOVE_GEN_H
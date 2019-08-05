//
// Created by leon on 2019-08-03.
//

#ifndef CHESSENGINE_MOVE_GEN_H
#define CHESSENGINE_MOVE_GEN_H

#include <cassert>
#include "board.h"

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
    bitboard block_mask = 0;

    int num_our_pieces = 0;
    bitboard our_pieces[16];
public:

    move_gen(const board& b) : b(b) {
        us = b.side_to_play;
        them = opposite(us);
        our_piece = b.piece_of_color[us];
        their_piece = b.piece_of_color[them];
        moves.reserve(100);
//        moves.clear();
    }

    std::vector<move>& generate();

    void generate_king_moves() {
        bitboard dest[8];
        int N = 0;
        if (rank_8_i & king) {
            dest[N++] = shift<UP>(king);
            if (file_a_i& king) dest[N++] = shift<UP_LEFT>(king);
            if (file_h_i& king) dest[N++] = shift<UP_RIGHT>(king);
        }
        if (rank_1_i& king) {
            dest[N++] = shift<DOWN>(king);
            if (file_a_i& king) dest[N++] = shift<DOWN_LEFT>(king);
            if (file_h_i& king) dest[N++] = shift<DOWN_RIGHT>(king);
        }
        if (file_a_i& king) dest[N++] = shift<LEFT>(king);
        if (file_h_i& king) dest[N++] = shift<RIGHT>(king);

        for (int i = 0; i < N; i++) {
            if (((our_piece | attacked) & dest[i]) == 0) {
                //moves.emplace_back(b.king_pos[us], get_square(dest[i]));
                add_move(b.king_pos[us], get_square(dest[i]));

            }
        }
    }

    void reset() {
        checkers = 0;
        num_checkers = 0;
        attacked = 0;
        block_mask = 0;
        king = bb(b.king_pos[b.side_to_play]);
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
        non_slider_attack = king_attacks(bb(b.king_pos[them]));
        attacked |= non_slider_attack;
    }

    template <evasiveness e>
    void rook_moves(const bitboard origin) {
        if ((origin & (pinned[HORIZONTAL] | pinned[DIAGONAL_P] | pinned[DIAGONAL_S])) == 0) {
            shift_moves<e, UP>(origin, rank_8_i);
            shift_moves<e, DOWN>(origin, rank_1_i);
        }
        if ((origin & (pinned[VERTICAL] | pinned[DIAGONAL_P] | pinned[DIAGONAL_S])) == 0) {
            shift_moves<e, LEFT>(origin, file_a_i);
            shift_moves<e, RIGHT>(origin, file_h_i);
        }
    }

    template <evasiveness e>
    void bishop_moves(const bitboard origin) {
        if ((origin & (pinned[HORIZONTAL] | pinned[VERTICAL] | pinned[DIAGONAL_S])) == 0) {
            shift_moves<e, UP_LEFT>(origin, file_a_i_rank_8_i);
            shift_moves<e, DOWN_RIGHT>(origin, file_h_i_rank_1_i);
        }
        if ((origin & (pinned[HORIZONTAL] | pinned[VERTICAL] | pinned[DIAGONAL_P])) == 0) {
            shift_moves<e, UP_RIGHT>(origin, file_h_i_rank_8_i);
            shift_moves<e, DOWN_LEFT>(origin, file_a_i_rank_1_i);
        }
    }

    void rook_attacks(const bitboard origin) {
        shift_attacks<UP>(origin, rank_8_i);
        shift_attacks<DOWN>(origin, rank_1_i);
        shift_attacks<LEFT>(origin, file_a_i);
        shift_attacks<RIGHT>(origin, file_h_i);
    }

    void bishop_attacks(const bitboard origin) {
        shift_attacks<UP_LEFT>(origin, file_a_i_rank_8_i);
        shift_attacks<UP_RIGHT>(origin, file_h_i_rank_8_i);
        shift_attacks<DOWN_LEFT>(origin, file_a_i_rank_1_i);
        shift_attacks<DOWN_RIGHT>(origin, file_h_i_rank_1_i);
    }

    template<shift_direction d>
    void shift_attacks(const bitboard origin, const bitboard in_range) {
        auto sq = origin;
        bitboard pin = 0;
        bitboard potential_block_mask = 0;
        while (in_range & sq) {
            sq = shift<d>(sq);
            if (pin == 0) {
                attacked |= sq;
                potential_block_mask |= sq;
            }
            if (king & sq) {
                if (pin == 0) {
                    num_checkers++;
                    checkers |= origin;
                    block_mask |= potential_block_mask;
                } else {
                    pinned_any_dir |= pin;
                    if (d == UP || d == DOWN) {
                        pinned[VERTICAL] |= pin;
                    }
                    if (d == LEFT || d == RIGHT) {
                        pinned[HORIZONTAL] |= pin;
                    }
                    if (d == UP_LEFT || d == DOWN_RIGHT) {
                        pinned[DIAGONAL_P] |= pin;
                    }
                    if (d == UP_RIGHT || d == DOWN_LEFT) {
                        pinned[DIAGONAL_S] |= pin;
                    }
                }
            } else if (their_piece & sq) {
                return;
            } else if (our_piece & sq) {
                if (pin == 0) {
                    // ray passed first time through a piece of our side, potential pin
                    pin = sq;
                } else {
                    // second time our piece on the way: this means no piece is pinned in this direction, and no check in this direction
                    return;
                }
            }
        }
    }

    template<evasiveness e, shift_direction d>
    void shift_moves(const bitboard origin, const bitboard in_range) {
        bitboard sq = origin;
        while (in_range & sq) {
            sq = shift<d>(sq);
            if (our_piece & sq) return; // blocked by own piece
            if (e == NON_EVASIVE || (sq & (checkers | block_mask))) {
                // to evade a single check, we must block the checker or capture it
                //moves.emplace_back(get_square(origin), get_square(sq));
                add_move(get_square(origin), get_square(sq));
            }
            if (their_piece & sq) break; // captured opponent piece: stop there
        }
    }

    template <evasiveness e>
    void knight_moves(bitboard origin) {
        int N = 0;
        bitboard dest[8];
        if (file_a_i_rank_8_i_rank_7_i & origin) dest[N++] = shift<UP_UP_LEFT>(origin);
        if (file_a_i_rank_1_i_rank_2_i & origin) dest[N++] = shift<DOWN_DOWN_LEFT>(origin);
        if (file_h_i_rank_8_i_rank_7_i & origin) dest[N++] = shift<UP_UP_RIGHT>(origin);
        if (file_h_i_rank_1_i_rank_2_i & origin) dest[N++] = shift<DOWN_DOWN_RIGHT>(origin);
        if (file_a_i_rank_8_i_file_b_i & origin) dest[N++] = shift<UP_LEFT_LEFT>(origin);
        if (file_a_i_rank_1_i_file_b_i & origin) dest[N++] = shift<DOWN_LEFT_LEFT>(origin);
        if (file_h_i_rank_8_i_file_g_i & origin) dest[N++] = shift<UP_RIGHT_RIGHT>(origin);
        if (file_h_i_rank_1_i_file_g_i & origin) dest[N++] = shift<DOWN_RIGHT_RIGHT>(origin);

        for (int i = 0; i < N; i++) {
            if (our_piece & dest[i]) continue;
            if (e == NON_EVASIVE || ((checkers | block_mask) & dest[i])) {
                //moves.emplace_back(get_square(origin), get_square(dest[i]));
                add_move(get_square(origin), get_square(dest[i]));
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

        if ((empty & fwd) && (origin & (pinned[HORIZONTAL] | pinned[DIAGONAL_S] | pinned[DIAGONAL_P])) == 0) {
            const square dest = get_square(fwd);
            if (e == NON_EVASIVE || (block_mask & fwd)) {
                if (promotion) {
//                    moves.emplace_back(origin_sq, dest, special_move::PROMOTION_QUEEN);
//                    moves.emplace_back(origin_sq, dest, special_move::PROMOTION_ROOK);
//                    moves.emplace_back(origin_sq, dest, special_move::PROMOTION_BISHOP);
//                    moves.emplace_back(origin_sq, dest, special_move::PROMOTION_KNIGHT);
                    add_move(origin_sq, dest, special_move::PROMOTION_QUEEN);
                    add_move(origin_sq, dest, special_move::PROMOTION_ROOK);
                    add_move(origin_sq, dest, special_move::PROMOTION_BISHOP);
                    add_move(origin_sq, dest, special_move::PROMOTION_KNIGHT);
                } else {
                    add_move(origin_sq, dest);
                    //moves.emplace_back(origin_sq, dest);
                }
            }
            if ((d == UP && (rank_2 & origin)) || (d == DOWN && (rank_7 & origin))) {
                const bitboard fwd2 = shift<d>(fwd);
                if (empty & fwd2)
                    if (e == NON_EVASIVE || (block_mask & fwd2))
                        add_move(origin_sq, get_square(fwd2));  // moves.emplace_back(origin_sq, get_square(fwd2));
            }
        }

        if ((origin & file_a_i) && !(origin & (pinned[VERTICAL] | pinned[HORIZONTAL] | pinned[d == UP ? DIAGONAL_S : DIAGONAL_P]))) {
            const bitboard cap = shift<LEFT>(fwd);
            pawn_captures<e>(promotion, origin_sq, cap);
        }

        if ((origin & file_h_i) && !(origin & (pinned[VERTICAL] | pinned[HORIZONTAL] | pinned[d == UP ? DIAGONAL_P : DIAGONAL_S]))) {
            const bitboard cap = shift<RIGHT>(fwd);
            pawn_captures<e>(promotion, origin_sq, cap);
        }
    }

    template<evasiveness e>
    inline void pawn_captures(const bool promotion, const square origin_sq, const bitboard cap) {
        if (their_piece & cap) {
            const square dest = get_square(cap);
            if (e == NON_EVASIVE || (checkers & cap)) {
                if (promotion) {
//                    moves.emplace_back(origin_sq, dest, PROMOTION_QUEEN);
//                    moves.emplace_back(origin_sq, dest, PROMOTION_KNIGHT);
//                    moves.emplace_back(origin_sq, dest, PROMOTION_ROOK);
//                    moves.emplace_back(origin_sq, dest, PROMOTION_BISHOP);
                    add_move(origin_sq, dest, special_move::PROMOTION_QUEEN);
                    add_move(origin_sq, dest, special_move::PROMOTION_ROOK);
                    add_move(origin_sq, dest, special_move::PROMOTION_BISHOP);
                    add_move(origin_sq, dest, special_move::PROMOTION_KNIGHT);
                } else {
                    //moves.emplace_back(origin_sq, dest);
                    add_move(origin_sq, dest);
                }
            }
        } else if (cap & bb(b.en_passant)) {
            const square dest = get_square(cap);
            if (e == EVASIVE || b.king_pos[us].get_rank() == origin_sq.get_rank()) {
                // this is a corner case that needs to be simulated
                board bnew = b.simulate(origin_sq, get_square(cap), PAWN, us);
                auto en_passant_bbi = ~(bb(b.en_passant.get_file(), origin_sq.get_rank()));
                bnew.piece_of_color[them] &= en_passant_bbi; // remove captured piece
                bnew.piece_of_type[PAWN] &= en_passant_bbi;
                if (!bnew.under_check(us)) {
                    //moves.emplace_back(origin_sq, dest);
                    add_move(origin_sq, dest);
                }
            } else {
                //moves.emplace_back(origin_sq, dest);
                add_move(origin_sq, dest);
            }
        }
    }

    template<color us>
    void castle_moves() {
        const bitboard anypiece = our_piece | their_piece;
        if (b.can_castle_king_side[us]) {
            auto path = shift<RIGHT>(king) | shift<RIGHT_RIGHT>(king);
            if ((anypiece & path) == 0) {
                if ((attacked & path) == 0) {
                    if (us == BLACK)
                        add_move(b.king_pos[us], square(6, 7), CASTLE_KING_SIDE_BLACK);
                    else
                        add_move(b.king_pos[us], square(6, 0), CASTLE_KING_SIDE_WHITE);
//                    moves.emplace_back(b.king_pos[us], square(6, (us == BLACK) * 7),
//                            (us == WHITE) ? CASTLE_KING_SIDE_WHITE : CASTLE_KING_SIDE_BLACK);
                }
            }
        }
        if (b.can_castle_queen_side[us]) {
            auto path = shift<LEFT>(king) | shift<LEFT_LEFT>(king);
            if ((anypiece & (path | shift<LEFT>(path))) == 0) {
                if ((attacked & path) == 0) {
//                    moves.emplace_back(b.king_pos[us], square(2, (us == BLACK) * 7),
//                            (us == WHITE) ? CASTLE_QUEEN_SIDE_WHITE : CASTLE_QUEEN_SIDE_BLACK);
                    if (us == BLACK)
                        add_move(b.king_pos[us], square(2, 7), CASTLE_QUEEN_SIDE_BLACK);
                    else
                        add_move(b.king_pos[us], square(2, 0), CASTLE_QUEEN_SIDE_WHITE);
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
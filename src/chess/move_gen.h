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
        moves.reserve(250);
        moves.clear();
    }

    std::vector<move>& generate() {
        reset();
        scan_board();
        generate_king_moves();
        if (num_checkers == 2) {
            return moves; // only legal moves will be moving the king away from check
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

    void generate_king_moves() {
        bitboard dest[8];
        int N = 0;
        if (rank_8_i[king]) {
            dest[N++] = king.shift<UP>();
            if (file_a_i[king]) dest[N++] = king.shift<UP_LEFT>();
            if (file_h_i[king]) dest[N++] = king.shift<UP_RIGHT>();
        }
        if (rank_1_i[king]) {
            dest[N++] = king.shift<DOWN>();
            if (file_a_i[king]) dest[N++] = king.shift<DOWN_LEFT>();
            if (file_h_i[king]) dest[N++] = king.shift<DOWN_RIGHT>();
        }
        if (file_a_i[king]) dest[N++] = king.shift<LEFT>();
        if (file_h_i[king]) dest[N++] = king.shift<RIGHT>();

        for (int i = 0; i < N; i++) {
            if (((our_piece | attacked) & dest[i]) == 0) {
                moves.emplace_back(b.king_pos[us], dest[i].get_square());
            }
        }
    }

    void reset() {
        checkers = 0;
        num_checkers = 0;
        attacked = 0;
        block_mask = 0;
        king = b.king_pos[b.side_to_play];
        num_our_pieces = 0;
        for (int i = 0; i < 4; i++) pinned[i] = 0;
    }

    template <evasiveness e>
    void generate_non_king_moves() {
        for (int i = 0; i < num_our_pieces; i++) {
            bitboard sq = our_pieces[i];
            if (b.piece_of_type[ROOK][sq] || b.piece_of_type[QUEEN][sq]) rook_moves<e>(sq);
            if (b.piece_of_type[BISHOP][sq] || b.piece_of_type[QUEEN][sq]) bishop_moves<e>(sq);
            else if (b.piece_of_type[KNIGHT][sq] && !pinned_any_dir[sq]) knight_moves<e>(sq);
            else if (b.piece_of_type[PAWN][sq]) {
                if (us == WHITE) pawn_moves<e, UP>(sq); else pawn_moves<e, DOWN>(sq);
            }
        }
    }

    void scan_board() {
        bitboard non_slider_attack;
        for (bitboard sq(1); sq != 0; sq <<= 1) {
            if (their_piece[sq]) {
                if (b.piece_of_type[ROOK][sq] || b.piece_of_type[QUEEN][sq]) rook_attacks(sq);
                if (b.piece_of_type[BISHOP][sq] || b.piece_of_type[QUEEN][sq]) bishop_attacks(sq);
                if (b.piece_of_type[KNIGHT][sq]) {
                    non_slider_attack = bitboard::knight_attacks(sq);
                    attacked |= non_slider_attack;
                    if ((non_slider_attack & king) != 0) {
                        num_checkers++;
                        checkers |= sq;
                    }
                }
                if (b.piece_of_type[PAWN][sq]) {
                    non_slider_attack = bitboard::pawn_attacks(sq, them);
                    attacked |= non_slider_attack;
                    if ((non_slider_attack & king) != 0) {
                        num_checkers++;
                        checkers |= sq;
                    }
                }
            } else if (our_piece[sq]) {
                our_pieces[num_our_pieces++] = sq;
            }
        }
        non_slider_attack = bitboard::king_attacks(b.king_pos[them]);
        attacked |= non_slider_attack;
    }

    template <evasiveness e>
    void rook_moves(const bitboard origin) {
        if (!origin[pinned[HORIZONTAL] | pinned[DIAGONAL_P] | pinned[DIAGONAL_S]]) {
            shift_moves<e, UP>(origin, rank_8_i);
            shift_moves<e, DOWN>(origin, rank_1_i);
        }
        if (!origin[pinned[VERTICAL] | pinned[DIAGONAL_P] | pinned[DIAGONAL_S]]) {
            shift_moves<e, LEFT>(origin, file_a_i);
            shift_moves<e, RIGHT>(origin, file_h_i);
        }
    }

    template <evasiveness e>
    void bishop_moves(const bitboard origin) {
        if (!origin[pinned[HORIZONTAL] | pinned[VERTICAL] | pinned[DIAGONAL_S]]) {
            shift_moves<e, UP_LEFT>(origin, file_a_i_rank_8_i);
            shift_moves<e, DOWN_RIGHT>(origin, file_h_i_rank_1_i);
        }
        if (!origin[pinned[VERTICAL] | pinned[DIAGONAL_P] | pinned[DIAGONAL_S]]) {
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
        bitboard pin;
        bitboard potential_block_mask;
        while (in_range[sq]) {
            sq = sq.shift<d>();
            if (pin.empty()) {
                attacked |= sq;
                potential_block_mask |= sq;
            }
            if (king[sq]) {
                if (pin.empty()) {
                    num_checkers++;
                    checkers |= origin;
                    block_mask |= potential_block_mask;
                    return;
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
            } else if (their_piece[sq]) {
                return;
            } else if (our_piece[sq]) {
                if (pin.empty()) {
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
        while (in_range[sq]) {
            sq = sq.shift<d>();
            if (our_piece[sq]) return; // blocked by own piece
            if (e == NON_EVASIVE || checkers[sq] || block_mask[sq]) {
                // to evade a single check, we must block the checker or capture it
                moves.emplace_back(origin.get_square(), sq.get_square());
            }
            if (their_piece[sq]) break; // captured opponent piece: stop there
        }
    }

    template <evasiveness e>
    void knight_moves(bitboard origin) {
        int N = 0;
        bitboard dest[8];
        if (file_a_i_rank_8_i_rank_7_i[origin]) dest[N++] = origin.shift<UP_UP_LEFT>();
        if (file_a_i_rank_1_i_rank_2_i[origin]) dest[N++] = origin.shift<DOWN_DOWN_LEFT>();
        if (file_h_i_rank_8_i_rank_7_i[origin]) dest[N++] = origin.shift<UP_UP_RIGHT>();
        if (file_h_i_rank_1_i_rank_2_i[origin]) dest[N++] = origin.shift<DOWN_DOWN_RIGHT>();
        if (file_a_i_rank_8_i_file_b_i[origin]) dest[N++] = origin.shift<UP_LEFT_LEFT>();
        if (file_a_i_rank_1_i_file_b_i[origin]) dest[N++] = origin.shift<DOWN_LEFT_LEFT>();
        if (file_h_i_rank_8_i_file_g_i[origin]) dest[N++] = origin.shift<UP_RIGHT_RIGHT>();
        if (file_h_i_rank_1_i_file_g_i[origin]) dest[N++] = origin.shift<DOWN_RIGHT_RIGHT>();

        for (int i = 0; i < N; i++) {
            if (our_piece[dest[i]]) continue;
            if (e == NON_EVASIVE || (checkers| block_mask)[dest[i]])
                moves.emplace_back(origin.get_square(), dest[i].get_square());
        }
    }


    template <evasiveness e, shift_direction d>
    void pawn_moves(bitboard origin) {
        // add normal moves, captures, en passant, and promotions
        const bitboard fwd = origin.shift<d>();
        const bool promotion = (rank_1 | rank_8)[fwd];
        const square origin_sq = origin.get_square();
        const bitboard empty = ~(our_piece | their_piece);

        if (empty[fwd] && !origin[pinned[HORIZONTAL] | pinned[DIAGONAL_S] | pinned[DIAGONAL_P]]) {
            const square dest = fwd.get_square();
            if (e == NON_EVASIVE || block_mask[fwd]) {
                if (promotion) {
                    moves.emplace_back(origin_sq, dest, special_move::PROMOTION_QUEEN);
                    moves.emplace_back(origin_sq, dest, special_move::PROMOTION_ROOK);
                    moves.emplace_back(origin_sq, dest, special_move::PROMOTION_BISHOP);
                    moves.emplace_back(origin_sq, dest, special_move::PROMOTION_KNIGHT);
                } else {
                    moves.emplace_back(origin_sq, dest);
                }
            }
            if ((d == UP && rank_2[origin]) || (d == DOWN && rank_7[origin])) {
                const bitboard fwd2 = fwd.shift<d>();
                if (empty[fwd2])
                    if (e == NON_EVASIVE || block_mask[fwd2])
                        moves.emplace_back(origin_sq, fwd2.get_square());
            }
        }

        if (!file_a[origin] && !origin[pinned[VERTICAL] | pinned[HORIZONTAL] | pinned[d == UP ? DIAGONAL_S : DIAGONAL_P]]) {
            const bitboard cap = fwd.shift<LEFT>();
            pawn_captures<e>(promotion, origin_sq, cap);
        }

        if (!file_h[origin] && !origin[pinned[VERTICAL] | pinned[HORIZONTAL] | pinned[d == UP ? DIAGONAL_P : DIAGONAL_S]]) {
            const bitboard cap = fwd.shift<RIGHT>();
            pawn_captures<e>(promotion, origin_sq, cap);
        }
    }

    template<evasiveness e>
    inline void pawn_captures(const bool promotion, const square origin_sq, const bitboard cap) {
        if (their_piece[cap]) {
            const square dest = cap.get_square();
            if (e == NON_EVASIVE || checkers[cap]) {
                if (promotion) {
                    moves.emplace_back(origin_sq, dest, PROMOTION_QUEEN);
                    moves.emplace_back(origin_sq, dest, PROMOTION_KNIGHT);
                    moves.emplace_back(origin_sq, dest, PROMOTION_ROOK);
                    moves.emplace_back(origin_sq, dest, PROMOTION_BISHOP);
                } else {
                    moves.emplace_back(origin_sq, dest);
                }
            }
        } else if (cap[b.en_passant]) {
            const square dest = cap.get_square();
            if (b.king_pos[us].get_rank() == origin_sq.get_rank()) {
                // this is a corner case that needs to be simulated
                board bnew = b.simulate(origin_sq, cap.get_square(), PAWN, us);
                auto en_passant_bbi = ~(bitboard(b.en_passant.get_file(), origin_sq.get_rank()));
                bnew.piece_of_color[them] &= en_passant_bbi; // remove captured piece
                bnew.piece_of_type[PAWN] &= en_passant_bbi;
                if (!b.under_check(us)) {
                    moves.emplace_back(origin_sq, dest);
                }
            } else {
                moves.emplace_back(origin_sq, dest);
            }
        }
    }

    template<color us>
    void castle_moves() {
        const bitboard anypiece = our_piece | their_piece;
        if (b.can_castle_king_side[us]) {
            auto path = king.shift<RIGHT>() | king.shift<RIGHT_RIGHT>();
            if ((anypiece & path) == 0) {
                if ((attacked & path) == 0) {
                    moves.emplace_back(b.king_pos[us], square(6, (us == BLACK) * 7),
                            (us == WHITE) ? CASTLE_KING_SIDE_WHITE : CASTLE_KING_SIDE_BLACK);
                }
            }
        }
        if (b.can_castle_queen_side[us]) {
            auto path = king.shift<LEFT>() | king.shift<LEFT_LEFT>();
            if ((anypiece & (path | path.shift<LEFT>())) == 0) {
                if ((attacked & path) == 0) {
                    moves.emplace_back(b.king_pos[us], square(2, (us == BLACK) * 7),
                            (us == WHITE) ? CASTLE_QUEEN_SIDE_WHITE : CASTLE_QUEEN_SIDE_BLACK);
                }
            }
        }
    }
};


#endif //CHESSENGINE_MOVE_GEN_H


//    void calc_checks_pins() {
//        reset();
//        color c = b.side_to_play;
//        color opponent = opposite(c);
//        square king_sq = b.king_pos[c];
//
//        assert(king != 0);
//        // checks if attacked by rook or queen in horizontal or vertical directions
//        bitboard opponent_piece = b.piece_of_color[opponent];
//
//        bitboard attacker = (b.piece_of_type[ROOK] | b.piece_of_type[QUEEN]) & opponent_piece;
//        if (file[king_sq.get_file()][attacker]) {
//            calc_shift_checks_pins<UP>(king, rank_8_i, attacker);
//            calc_shift_checks_pins<DOWN>(king, rank_1_i, attacker);
//        }
//        if (rank[king_sq.get_rank()][attacker]) {
//            calc_shift_checks_pins<LEFT>(king, file_a_i, attacker);
//            calc_shift_checks_pins<RIGHT>(king, file_h_i, attacker);
//        }
//
//        // checks if attacked by bishop or queen in diagonal directions
//        attacker = (b.piece_of_type[BISHOP] | b.piece_of_type[QUEEN]) & opponent_piece;
//        calc_shift_checks_pins<UP_LEFT>(king, file_a_i_rank_8_i, attacker);
//        calc_shift_checks_pins<UP_RIGHT>(king, file_h_i_rank_8_i, attacker);
//        calc_shift_checks_pins<DOWN_LEFT>(king, file_a_i_rank_1_i, attacker);
//        calc_shift_checks_pins<DOWN_RIGHT>(king, file_h_i_rank_1_i, attacker);
//
//        // check if attacked by knight
//        auto non_sliding_checker = bitboard::knight_attacks(king) & b.piece_of_type[KNIGHT] & opponent_piece;
//        if (non_sliding_checker != 0) {
//            // it's impossible to be in checks by 2 knights at the same time
//            num_checkers++;
//            checkers |= non_sliding_checker;
//        } else {
//            // it's impossible to be in checks by 2 non-sliding pieces at once
//            non_sliding_checker = bitboard::pawn_attacks(king, c) & b.piece_of_type[PAWN] & opponent_piece;
//            if (non_sliding_checker != 0) {
//                num_checkers++;
//                checkers |= non_sliding_checker;
//            }
//        }
//    }
//
//    template<shift_direction d>
//    void calc_shift_checks_pins(bitboard origin, bitboard limit, bitboard attackers) {
//
//        auto sq = origin;
//        bitboard ray = origin;
//        bitboard pin;
//        while (limit[sq]) {
//            sq = sq.shift<d>();
//            ray |= sq;
//            if (b.piece_of_color[b.side_to_play][sq]) {
//                if (pin.empty()) {
//                    // ray passed first time through a piece of our side, potential pin
//                    pin = sq;
//                } else {
//                    // second time our piece on the way: this means no piece is pinned in this direction, and no check in this direction
//                    return;
//                }
//            } else if ((b.piece_of_color[opposite(b.side_to_play)] & attackers & sq) != 0) {
//                // found attacker giving check or pinning
//                if (pin.empty()) {
//                    num_checkers++;
//                    attackers |= sq;
//                    attacked |= ray;
//                } else {
//                    pinned[d] = pin;
//                }
//                return;
//            }
//        }
//        // if we reach here, nothing "interesting" was found
//    }

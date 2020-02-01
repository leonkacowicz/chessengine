//
// Created by leon on 2019-08-03.
//

#include <chess/move_gen.h>

using namespace chess::core;

move_gen::move_gen(const board& b) : b(b) {
    us = b.side_to_play;
    them = opposite(us);
    our_piece = b.piece_of_color[us];
    their_piece = b.piece_of_color[them];
    any_piece = our_piece | their_piece;
}

bool move_gen::square_attacked(square sq) {
    if (knight_attacks(sq) & their_piece & b.piece_of_type[KNIGHT]) return true;
    if (pawn_attacks_bb[us][sq] & their_piece & b.piece_of_type[PAWN]) return true;
    if (king_attacks(sq) & get_bb(b.king_pos[them])) return true;
    if (attacks_from_rook(sq, any_piece ^ king) & their_piece & (b.piece_of_type[ROOK] | b.piece_of_type[QUEEN])) return true;
    if (attacks_from_bishop(sq, any_piece ^ king) & their_piece & (b.piece_of_type[BISHOP] | b.piece_of_type[QUEEN])) return true;
    return false;
}

std::vector<move> move_gen::generate() {

    reset();
    scan_board();
    //print_bb(attacked);
    //print_bb(pinned);

    generate_king_moves();
    int num_checkers = num_squares(checkers);
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
    std::vector<move> moves(moves_array, last_move);
    //moves.assign(moves_array, last_move);
    return moves;
}

void move_gen::generate_king_moves() {
    bitboard attacks = king_attacks(king) & ~our_piece;
    square sq;
    while (attacks) {
        sq = pop_lsb(&attacks);
        if (!square_attacked(sq)) {
            add_move(b.king_pos[us], sq);
        }
    }
}

void move_gen::scan_board() {
    //print_bb(remaining);
    square king_sq = b.king_pos[us];

    checkers |= knight_attacks(king_sq) & their_piece & b.piece_of_type[KNIGHT];
    checkers |= pawn_attacks_bb[us][king_sq] & their_piece & b.piece_of_type[PAWN];
    checkers |= king_attacks(king_sq) & get_bb(b.king_pos[them]);
    bitboard rook_checkers = attacks_from_rook(king_sq, any_piece ^ king) & their_piece & (b.piece_of_type[ROOK] | b.piece_of_type[QUEEN]);
    checkers |= rook_checkers;
    bitboard remaining = rook_checkers;
    while (remaining) {
        square sq = pop_lsb(&remaining);
        block_mask |= line_segment[king_sq][sq];
    }

    bitboard bishop_checkers = attacks_from_bishop(king_sq, any_piece ^ king) & their_piece & (b.piece_of_type[BISHOP] | b.piece_of_type[QUEEN]);
    checkers |= bishop_checkers;
    remaining = bishop_checkers;
    while (remaining) {
        square sq = pop_lsb(&remaining);
        block_mask |= line_segment[king_sq][sq];
    }


    bitboard ray = attacks_from_rook(king_sq, 0);
    remaining = their_piece & (b.piece_of_type[ROOK] | b.piece_of_type[QUEEN]) & ray;
    while (remaining) {
        square sq = pop_lsb(&remaining);
        bitboard path = ray & line_segment[sq][king_sq] & any_piece;
        if (num_squares(path) == 1) {
            pinned |= path;
        }
    }

    ray = attacks_from_bishop(king_sq, 0);
    remaining = their_piece & (b.piece_of_type[BISHOP] | b.piece_of_type[QUEEN]) & ray;
    while (remaining) {
        square sq = pop_lsb(&remaining);
        bitboard path = ray & line_segment[sq][king_sq] & any_piece;
        if (num_squares(path) == 1) {
            pinned |= path;
        }
    }
}

template<move_gen::evasiveness e>
void move_gen::rook_moves(const bitboard origin) {
    square origin_sq = get_square(origin);
    bitboard attacks = attacks_from_rook(origin_sq, any_piece);
    attacks &= ~our_piece;
    if (e == EVASIVE) attacks &= (checkers | block_mask); // remove squares that don't block the checker or capture it

    if (!(pinned & origin)) {
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

template<move_gen::evasiveness e>
void move_gen::bishop_moves(const bitboard origin) {
    square origin_sq = get_square(origin);
    bitboard attacks = attacks_from_bishop(origin_sq, any_piece);
    attacks &= ~our_piece;
    if constexpr (e == EVASIVE) attacks &= (checkers | block_mask); // remove squares that don't block the checker or capture it

    if (!(pinned & origin)) {
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

template<move_gen::evasiveness e>
void move_gen::knight_moves(bitboard origin) {
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

template<move_gen::evasiveness e, shift_direction d>
void move_gen::pawn_moves(bitboard origin) {
    // add normal moves, captures, en passant, and promotions
    const bitboard fwd = shift<d>(origin);
    const bool promotion = (rank_1 | rank_8) & fwd;
    const square origin_sq = get_square(origin);

    square dest = get_square(fwd);
    if ((~any_piece & fwd) && (!(pinned & origin) || (line[origin_sq][dest] & king))) {
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
            if (~any_piece & fwd2)
                if (e == NON_EVASIVE || (block_mask & fwd2))
                    add_move(origin_sq, get_square(fwd2));  // moves.emplace_back(origin_sq, get_square(fwd2));
        }
    }

    if (origin & file_a_i) {
        bitboard cap = shift<LEFT>(fwd);
        dest = get_square(cap);
        if (!(pinned & origin) || (line[origin_sq][dest] & king)) {
            pawn_captures<e>(promotion, origin_sq, cap);
        }
    }

    if (origin & file_h_i) {
        bitboard cap = shift<RIGHT>(fwd);
        dest = get_square(cap);
        if (!(pinned & origin) || (line[origin_sq][dest] & king)) {
            pawn_captures<e>(promotion, origin_sq, cap);
        }
    }
}

template<move_gen::evasiveness e>
void move_gen::generate_non_king_moves() {
    bitboard remaining = our_piece;
    //print_bb(remaining);
    while (remaining) {
        bitboard sq = get_bb(pop_lsb(&remaining));
        //print_bb(sq);
        if (sq & (b.piece_of_type[ROOK] | b.piece_of_type[QUEEN])) rook_moves<e>(sq);
        if (sq & (b.piece_of_type[BISHOP] | b.piece_of_type[QUEEN])) bishop_moves<e>(sq);
        else if ((sq & b.piece_of_type[KNIGHT]) && !(pinned & sq)) knight_moves<e>(sq);
        else if (sq & b.piece_of_type[PAWN]) {
            if (us == WHITE) pawn_moves<e, UP>(sq); else pawn_moves<e, DOWN>(sq);
        }
    }
}

template<move_gen::evasiveness e>
void move_gen::pawn_captures(const bool promotion, const square origin_sq, const bitboard cap) {
    if (their_piece & cap) {
        const square dest = get_square(cap);
        if (e == NON_EVASIVE || (checkers & cap)) {
            if (promotion) {
                add_move(origin_sq, dest, special_move::PROMOTION_QUEEN);
                add_move(origin_sq, dest, special_move::PROMOTION_ROOK);
                add_move(origin_sq, dest, special_move::PROMOTION_BISHOP);
                add_move(origin_sq, dest, special_move::PROMOTION_KNIGHT);
            } else {
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
void move_gen::castle_moves() {
    const bitboard anypiece = our_piece | their_piece;
    if (b.can_castle_king_side[us]) {
        auto path = shift<RIGHT>(king) | shift<2 * RIGHT>(king);
        if ((anypiece & path) == 0) {
            if (us == BLACK && !square_attacked(SQ_F8) && !square_attacked(SQ_G8)) {
                add_move(b.king_pos[us], SQ_G8, CASTLE_KING_SIDE_BLACK);
            } else if (us == WHITE && !square_attacked(SQ_F1) && !square_attacked(SQ_G1)) {
                add_move(b.king_pos[us], SQ_G1, CASTLE_KING_SIDE_WHITE);
            }
        }
    }
    if (b.can_castle_queen_side[us]) {
        auto path = shift<LEFT>(king) | shift<2 * LEFT>(king);
        if ((anypiece & (path | shift<LEFT>(path))) == 0) {
            if (us == BLACK && !square_attacked(SQ_C8) && !square_attacked(SQ_D8)) {
                add_move(b.king_pos[us], SQ_C8, CASTLE_QUEEN_SIDE_BLACK);
            } else if (us == WHITE && !square_attacked(SQ_C1) && !square_attacked(SQ_D1)){
                add_move(b.king_pos[us], SQ_C1, CASTLE_QUEEN_SIDE_WHITE);
            }
        }
    }
}

void move_gen::reset() {
    checkers = 0;
    block_mask = 0;
    king = get_bb(b.king_pos[b.side_to_play]);
}

inline void move_gen::add_move(square from, square to, special_move special) {
    *last_move++ = get_move(from, to, special);
}

inline void move_gen::add_move(square from, square to) {
    *last_move++ = get_move(from, to);
}

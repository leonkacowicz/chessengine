//
// Created by leon on 22/06/18.
//

#include <chess/bitboard.h>

namespace chess::core {
bitboard piece_attacks_bb[6][64];
bitboard pawn_attacks_bb[2][64];
bitboard line[64][64];
bitboard line_segment[64][64];

void init_king_attacks_bb() {
    for (square sq = SQ_A1; sq <= SQ_H8; ++sq) {
        bitboard origin = get_bb(sq);
        bitboard attack = 0;
        if (rank_8_i & origin) {
            attack |= shift<UP>(origin);
            if (file_a_i & origin) attack |= shift<UP_LEFT>(origin);
            if (file_h_i & origin) attack |= shift<UP_RIGHT>(origin);
        }
        if (rank_1_i & origin) {
            attack |= shift<DOWN>(origin);
            if (file_a_i & origin) attack |= shift<DOWN_LEFT>(origin);
            if (file_h_i & origin) attack |= shift<DOWN_RIGHT>(origin);
        }
        if (file_a_i & origin) attack |= shift<LEFT>(origin);
        if (file_h_i & origin) attack |= shift<RIGHT>(origin);
        piece_attacks_bb[KING][sq] = attack;
    }
}

void init_knight_attacks_bb() {
    for (square sq = SQ_A1; sq <= SQ_H8; ++sq) {
        bitboard origin = get_bb(sq);
        bitboard attack = 0;
        if (file_a_i_rank_8_i_rank_7_i & origin) attack |= shift<UP_LEFT + UP>(origin);
        if (file_a_i_rank_1_i_rank_2_i & origin) attack |= shift<DOWN_LEFT + DOWN>(origin);
        if (file_h_i_rank_8_i_rank_7_i & origin) attack |= shift<UP_RIGHT + UP>(origin);
        if (file_h_i_rank_1_i_rank_2_i & origin) attack |= shift<DOWN_RIGHT + DOWN>(origin);
        if (file_a_i_rank_8_i_file_b_i & origin) attack |= shift<UP_LEFT + LEFT>(origin);
        if (file_a_i_rank_1_i_file_b_i & origin) attack |= shift<DOWN_LEFT + LEFT>(origin);
        if (file_h_i_rank_8_i_file_g_i & origin) attack |= shift<UP_RIGHT + RIGHT>(origin);
        if (file_h_i_rank_1_i_file_g_i & origin) attack |= shift<DOWN_RIGHT + RIGHT>(origin);
        piece_attacks_bb[KNIGHT][sq] = attack;
    }
}

void init_pawn_attacks_bb() {
    for (square sq = SQ_A8; sq <= SQ_H8; ++sq) pawn_attacks_bb[WHITE][sq] = 0;
    for (square sq = SQ_A1; sq <= SQ_H1; ++sq) pawn_attacks_bb[BLACK][sq] = 0;

    for (square sq = SQ_A1; sq <= SQ_H7; ++sq) {
        bitboard origin = get_bb(sq);
        bitboard attack = 0;
        if (file_a_i & origin) attack |= shift<UP_LEFT>(origin);
        if (file_h_i & origin) attack |= shift<UP_RIGHT>(origin);
        pawn_attacks_bb[WHITE][sq] = attack;
    }

    for (square sq = SQ_A2; sq <= SQ_H8; ++sq) {
        bitboard origin = get_bb(sq);
        bitboard attack = 0;
        if (file_a_i & origin) attack |= shift<DOWN_LEFT>(origin);
        if (file_h_i & origin) attack |= shift<DOWN_RIGHT>(origin);
        pawn_attacks_bb[BLACK][sq] = attack;
    }
}

/**
 * This only calculates attacks of a rook on an empty board. For actual attack bitboard, we'll need *magic bitboard*
 * This will be used as mask and
 */
void init_rook_theoretical_attacks_bb() {
    for (square sq = SQ_A1; sq <= SQ_H8; ++sq) {
        bitboard origin = get_bb(sq);
        piece_attacks_bb[ROOK][sq] = (file[get_file(sq)] | rank[get_rank(sq)]) & ~origin;
    }
}

/**
 * Returns a horizontal line on rank r, between and NOT INCLUDING files f1 and f2
 * This is intended to be used as a block mask for check evasions
 */
bitboard horizontal_line_segment(int r, int f1, int f2) {
    if (f1 > f2) {
        const int tmp = f1;
        f1 = f2;
        f2 = tmp;
    }
    bitboard ret = 0;
    while (++f1 < f2) {
        ret |= get_bb(f1, r);
    }
    return ret;
}

/**
 * Returns a vertical line on file f, between and NOT INCLUDING ranks r1 and r2
 * This is intended to be used as a block mask for check evasions
 */
bitboard vertical_line_segment(int f, int r1, int r2) {
    if (r1 > r2) {
        const int tmp = r1;
        r1 = r2;
        r2 = tmp;
    }
    bitboard ret = 0;
    while (++r1 < r2) {
        ret |= get_bb(f, r1);
    }
    return ret;
}

/**
 * Returns a diagonal line between and NOT INCLUDING squares sq1 and sq2, if they are in the same diagonal (result is undefined otherwise)
 * This is intended to be used as a block mask for check evasions
 */
bitboard diagonal_line_segment(square sq1, square sq2) {
    if (get_file(sq1) > get_file(sq2)) {
        const square tmp = sq1;
        sq1 = sq2;
        sq2 = tmp; // make sq1 the leftmost square
    }
    const shift_direction d = get_rank(sq2) > get_rank(sq1) ? UP_RIGHT : DOWN_RIGHT;

    bitboard sq2bb = get_bb(sq2);
    bitboard ret = 0;
    if (d == UP_RIGHT) {
        bitboard sqbb = shift<UP_RIGHT>(get_bb(sq1));
        while (!(sqbb & sq2bb)) {
            ret |= sqbb;
            sqbb = shift<UP_RIGHT>(sqbb);
        }
    } else {
        bitboard sqbb = shift<DOWN_RIGHT>(get_bb(sq1));
        while (!(sqbb & sq2bb)) {
            ret |= sqbb;
            sqbb = shift<DOWN_RIGHT>(sqbb);
        }
    }
    return ret;
}

/**
 * Returns the WHOLE diagonal that passes through squares sq1 and sq2, assuming it exists
 * Used to check if pinned piece movement is allowed
 */
bitboard diagonal_line(square sq1, square sq2) {
    if (get_file(sq1) > get_file(sq2)) {
        const square tmp = sq1;
        sq1 = sq2;
        sq2 = tmp; // make sq1 the leftmost square
    }
    const shift_direction d = get_rank(sq2) > get_rank(sq1) ? UP_RIGHT : DOWN_RIGHT;
    bitboard sqbb = get_bb(sq1);
    bitboard ret = sqbb;
    if (d == UP_RIGHT)
        while (sqbb & file_h_i_rank_8_i) {
            sqbb = shift<UP_RIGHT>(sqbb);
            ret |= sqbb;
        }
    if (d == DOWN_RIGHT)
        while (sqbb & file_h_i_rank_1_i) {
            sqbb = shift<DOWN_RIGHT>(sqbb);
            ret |= sqbb;
        }

    sqbb = get_bb(sq1);
    if (d == UP_RIGHT)
        while (sqbb & file_a_i_rank_1_i) {
            sqbb = shift<DOWN_LEFT>(sqbb);
            ret |= sqbb;
        }
    if (d == DOWN_RIGHT)
        while (sqbb & file_a_i_rank_8_i) {
            sqbb = shift<UP_LEFT>(sqbb);
            ret |= sqbb;
        }
    return ret;
}

void init_line_bitboards() {
    for (square sq1 = SQ_A1; sq1 <= SQ_H8; ++sq1) {
        for (square sq2 = sq1; sq2 <= SQ_H8; ++sq2) {
            if (sq1 == sq2) line[sq1][sq2] = 0;
            else if (get_rank(sq1) == get_rank(sq2)) line[sq1][sq2] = rank[get_rank(sq1)];
            else if (get_file(sq1) == get_file(sq2)) line[sq1][sq2] = file[get_file(sq1)];
            else if (rank_dist(sq1, sq2) == file_dist(sq1, sq2)) line[sq1][sq2] = diagonal_line(sq1, sq2);
            else line[sq1][sq2] = 0;
            line[sq2][sq1] = line[sq1][sq2];
        }
    }
}

void init_line_segment_bitboards() {
    for (square sq1 = SQ_A1; sq1 <= SQ_H8; ++sq1) {
        for (square sq2 = sq1; sq2 <= SQ_H8; ++sq2) {
            if (sq1 == sq2)
                line_segment[sq1][sq2] = 0;
            else if (get_rank(sq1) == get_rank(sq2))
                line_segment[sq1][sq2] = horizontal_line_segment(get_rank(sq1), get_file(sq1), get_file(sq2));
            else if (get_file(sq1) == get_file(sq2))
                line_segment[sq1][sq2] = vertical_line_segment(get_file(sq1), get_rank(sq1), get_rank(sq2));
            else if (rank_dist(sq1, sq2) == file_dist(sq1, sq2))
                line_segment[sq1][sq2] = diagonal_line_segment(sq1, sq2);
            else
                line_segment[sq1][sq2] = 0;
            line_segment[sq2][sq1] = line_segment[sq1][sq2];
        }
    }
}

void init_bitboards() {
    init_king_attacks_bb();
    init_knight_attacks_bb();
    init_pawn_attacks_bb();
    init_rook_theoretical_attacks_bb();
    init_line_bitboards();
    init_line_segment_bitboards();
}
}
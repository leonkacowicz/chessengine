//
// Created by leon on 22/06/18.
//

#include "bitboard.h"
#include "piece.h"

namespace chess {
    namespace core {
        bitboard piece_attacks_bb[6][64];
        bitboard pawn_attacks_bb[2][64];


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
            for (square sq = SQ_A1; sq <= SQ_H8; ++sq) pawn_attacks_bb[WHITE][sq] = 0;
            for (square sq = SQ_A1; sq <= SQ_H8; ++sq) pawn_attacks_bb[BLACK][sq] = 0;

            for (square sq = SQ_A2; sq <= SQ_H7; ++sq) {
                bitboard origin = get_bb(sq);
                bitboard attack = 0;
                if (file_a_i & origin) attack |= shift<UP_LEFT>(origin);
                if (file_h_i & origin) attack |= shift<UP_RIGHT>(origin);
                pawn_attacks_bb[WHITE][sq] = attack;
            }

            for (square sq = SQ_A2; sq <= SQ_H7; ++sq) {
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

        void init_bitboards() {
            init_king_attacks_bb();
            init_knight_attacks_bb();
            init_pawn_attacks_bb();
            init_rook_theoretical_attacks_bb();
        }
    }
}
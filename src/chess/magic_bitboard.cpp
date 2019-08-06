//
// Created by leon on 2019-08-05.
//

#include <vector>
#include <random>
#include "magic_bitboard.h"
#include "bitboard.h"

namespace chess {
    namespace core {

        bitboard rook_table[0x20000];
        magic rook_wizardry[64];

        template<shift_direction d>
        bitboard calc_shift_attacks(const bitboard origin, const bitboard range, const bitboard occupancy) {
            auto sq = origin;
            bitboard attack = 0;
            while (range & sq) {
                sq = shift<d>(sq);
                attack |= sq;
                if (occupancy & sq) break;
            }
            return attack;
        }

        bitboard calc_rook_attacks(const bitboard origin, const bitboard occupancy) {
            return calc_shift_attacks<UP>(origin, rank_8_i, occupancy) |
                    calc_shift_attacks<DOWN>(origin, rank_1_i, occupancy) |
                    calc_shift_attacks<LEFT>(origin, file_a_i, occupancy) |
                    calc_shift_attacks<RIGHT>(origin, file_h_i, occupancy);
        }

        std::vector<bitboard> partitions(const bitboard attack_mask) {
            std::vector<bitboard> occupancy;

            bitboard bb = 0;
            do {
                occupancy.push_back(bb);
                bb -= attack_mask;
                bb &= attack_mask;
            } while (bb != 0);
            return occupancy;
        }

        void init_magic_bitboards() {
            std::default_random_engine gen;
            std::uniform_int_distribution<uint64_t> dis(0, (uint64_t)-1);

            int last_size = 0;

            for (square sq = SQ_A1; sq <= SQ_H8; ++sq) {
                bitboard board_interior = ((rank_8_i & rank_1_i) | rank[get_rank(sq)]) & ((file_a_i & file_h_i) | file[get_file(sq)]);
                print_bb(board_interior);
                magic& m = rook_wizardry[sq];

                m.attack_mask = piece_attacks_bb[ROOK][sq] & board_interior;
                print_bb(m.attack_mask);
                std::vector<bitboard> occupancies = partitions(m.attack_mask);
                std::vector<bitboard> attacks;
                attacks.reserve(occupancies.size());
                for (int i = 0; i < occupancies.size(); i++) {
                    attacks.push_back(calc_rook_attacks(get_bb(sq), occupancies[i]));
                }
                m.attack_table = &rook_table[last_size];
                m.shift = 64 - num_squares(m.attack_mask);
                last_size += occupancies.size();
                bool found = false;
                std::cout.flush();
                for (int attempt = 0; attempt < 1000000; attempt++) {
                    // get some random magic number
                    do {
                        auto r1 = dis(gen);
                        auto r2 = dis(gen);
                        auto r3 = dis(gen);
                        auto r4 = dis(gen);
                        m.magic_number = r1 & r2 & r3;
                    } while (!m.magic_number);


                    // fill the table
                    for (int i = 0; i < occupancies.size(); i++) {
                        m.attack_table[(occupancies[i] * m.magic_number) >> m.shift] = attacks[i];
                    }

                    // check if it works for all occupancies
                    found = true;
                    for (int i = 0; i < occupancies.size(); i++) {
                        if (m.attack_table[(occupancies[i] * m.magic_number) >> m.shift] != attacks[i]) {
                            found = false;
                            break;
                        }
                    }
                    if (found) break;
                }
                assert(found);
            }
        }
    }
}
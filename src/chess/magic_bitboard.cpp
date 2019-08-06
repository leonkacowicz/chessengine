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
            std::uniform_int_distribution<uint64_t> dis(0, (uint64_t) -1);

            // this is not necessary for the function to work, this is just the answer calculated without it
            // it's just pasted here to speed up the start-up, otherwise it will take a few seconds calculating it
            // through trial and error
            uint64_t pre_calculated_magic_numbers[] =
                    {11745405970126275136uL, 4629718013955293184uL, 2522042454769672192uL, 36037594261229568uL,
                     72062000808595712uL, 216177180198109256uL, 36030996126106240uL, 1297037388484345924uL,
                     4615204456711225379uL, 4611756395225679938uL, 1153343785793488004uL, 4611826824669298816uL,
                     324399945027290112uL, 11540192607254610944uL, 577023736650138657uL, 6917810504798912768uL,
                     2522157078575841312uL, 302306324631290696uL, 1267187419455489uL, 9529759198540204032uL,
                     722302174205448232uL, 8071154770552816640uL, 2343010900296876552uL, 11611126463319507073uL,
                     9511639317513125888uL, 9223688705867796480uL, 144399209972375552uL, 2515686901481481uL,
                     18160637851467792uL, 327074000248571920uL, 4828018796663411216uL, 1152940204894486596uL,
                     141012458536992uL, 1441715932404654656uL, 4613973039128776705uL, 1152957788641562624uL,
                     432349964429953024uL, 281484254512128uL, 1197402589499394uL, 36029382241812780uL,
                     9385502173753933824uL, 5206161341307461634uL, 580964489378168849uL, 72902087738589216uL,
                     144194352980197504uL, 18296440422465538uL, 2882310358603694208uL, 4521483931090953uL,
                     576882965859152128uL, 864972743022477568uL, 4611704169663840768uL, 216744537018630272uL,
                     598987548681044096uL, 4400194125952uL, 114844092756657152uL, 2018747896014356992uL,
                     2322718314733825uL, 37390114636034uL, 78886146972586241uL, 9299950827538229505uL,
                     38843564516460546uL, 2882585279510628353uL, 180425531007827969uL, 1167559045284954210uL};
            int last_size = 0;

            for (square sq = SQ_A1; sq <= SQ_H8; ++sq) {
                bitboard board_interior =
                        ((rank_8_i & rank_1_i) | rank[get_rank(sq)]) & ((file_a_i & file_h_i) | file[get_file(sq)]);
                //print_bb(board_interior);
                magic& m = rook_wizardry[sq];

                m.attack_mask = piece_attacks_bb[ROOK][sq] & board_interior;
                //print_bb(m.attack_mask);
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
                m.magic_number = pre_calculated_magic_numbers[sq];
                for (int attempt = 0; attempt < 1000000; attempt++) {
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

                    // get some random magic number
                    do {
                        auto r1 = dis(gen);
                        auto r2 = dis(gen);
                        auto r3 = dis(gen);
                        auto r4 = dis(gen);
                        m.magic_number = r1 & r2 & r3;
                    } while (!m.magic_number);
                }
                assert(found);
                // std::cout << ", " << m.magic_number;
            }
        }
    }
}
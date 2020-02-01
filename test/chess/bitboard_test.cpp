#include <gtest/gtest.h>
#include <chess/square.h>
#include <chess/bitboard.h>
#include <random>

using namespace chess::core;

TEST(bitboard_test, size_of_class) {
    std::cout << "\nSize of class: " << sizeof(bitboard) << " bytes" << std::endl;
}

TEST(bitboard_test, bitboard_consistency) {
    for (int f = 0; f < 8; f++) {
        for (int r = 0; r < 8; r++) {
            bitboard bbo = get_bb(f, r);

            ASSERT_EQ((bbo & file[f] & rank[r]) != 0, true);
            for (int k = 0; k < 8; k++) {
                if (k != f) {
                    EXPECT_EQ(bbo & file[k], 0);
                }
                if (k != r) {
                    EXPECT_EQ(bbo & rank[k], 0);
                }
            }
        }
    }
}

TEST(bitboard_test, bitboard_square_conversion) {
    for (int f = 0; f < 8; f++) {
        for (int r = 0; r < 8; r++) {
            square sq = get_square(f, r);
            bitboard bbo = get_bb(sq);

            ASSERT_EQ((bbo & file[f] & rank[r]) != 0, true);
            for (int k = 0; k < 8; k++) {
                if (k != f) {
                    EXPECT_EQ(bbo & file[k], 0);
                }
                if (k != r) {
                    EXPECT_EQ(bbo & rank[k], 0);
                }
            }
        }
    }
}

TEST(bitboard_test, bitboard_square_conversion_inv) {
    for (int f = 0; f < 8; f++) {
        for (int r = 0; r < 8; r++) {
            bitboard bbo = get_bb(f, r);
            auto sq = get_square(bbo);
            ASSERT_EQ(get_file(sq), f);
            ASSERT_EQ(get_rank(sq), r);
        }
    }
}

TEST(bitboard_test, costants_and_ops) {
    for (int f = 0; f < 7; f++) {
        EXPECT_EQ(shift<RIGHT>(file[f]), file[f + 1]);
    }

    for (int f = 7; f > 0; f--) {
        EXPECT_EQ(shift<LEFT>(file[f]), file[f - 1]);
    }

    for (int r = 0; r < 7; r++) {
        EXPECT_EQ(shift<UP>(rank[r]), rank[r + 1]);
    }

    for (int r = 7; r > 0; r--) {
        EXPECT_EQ(shift<DOWN>(rank[r]), rank[r - 1]);
    }
}

TEST(bitboard_test, test_shift_up_right) {
    for (int r = 0; r < 7; r++) {
        for (int f = 0; f < 7; f++) {
            EXPECT_EQ(shift<UP_RIGHT>(get_bb(f, r)), get_bb(f + 1, r + 1));
        }
    }
}

TEST(bitboard_test, test_shift_up_left) {
    for (int r = 0; r < 7; r++) {
        for (int f = 1; f < 8; f++) {
            EXPECT_EQ(shift<UP_LEFT>(get_bb(f, r)), get_bb(f - 1, r + 1));
        }
    }
}

TEST(bitboard_test, test_shift_down_right) {
    for (int r = 1; r < 8; r++) {
        for (int f = 0; f < 7; f++) {
            EXPECT_EQ(shift<DOWN_RIGHT>(get_bb(f, r)), get_bb(f + 1, r - 1));
        }
    }
}

TEST(bitboard_test, test_shift_down_left) {
    for (int r = 1; r < 8; r++) {
        for (int f = 1; f < 8; f++) {
            EXPECT_EQ(shift<DOWN_LEFT>(get_bb(f, r)), get_bb(f - 1, r - 1));
        }
    }
}

TEST(bitboard_test, bitboard_square_conv) {
    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++) {
            square expected = get_square(x, y);
            bitboard bitBoard = get_bb(expected);
            square calculated = get_square(bitBoard);
            ASSERT_EQ(expected, calculated);
        }

    ASSERT_EQ(get_bb(SQ_NONE), 0);
}

TEST(bitboard_test, bitboard_poplsb) {

    std::default_random_engine gen;
    std::uniform_int_distribution<uint64_t> dis(1, (uint64_t)-1);
    dis(gen);

    for (int i = 0; i < 100000; i++) {
        bitboard b = dis(gen);
        bitboard b_prev = b;
        square s = pop_lsb(&b);
        ASSERT_LT(s, SQ_NONE);
        ASSERT_NE(b, b_prev);
        ASSERT_EQ(num_squares(b ^ b_prev), 1);
        ASSERT_EQ(b | get_bb(s), b_prev);
    }

    // now sparse
    for (int i = 0; i < 100000; i++) {
        bitboard b = dis(gen);
        b = b & dis(gen);
        if (!b) { --i; continue; }
        bitboard b_prev = b;
        square s = pop_lsb(&b);
        ASSERT_LT(s, SQ_NONE);
        ASSERT_NE(b, b_prev);
        ASSERT_EQ(num_squares(b ^ b_prev), 1);
        ASSERT_EQ(b | get_bb(s), b_prev);
    }

    // now sparser
    for (int i = 0; i < 100000; i++) {
        bitboard b = dis(gen);
        b = b & dis(gen);
        b = b & dis(gen);
        if (!b) { --i; continue; }
        bitboard b_prev = b;
        square s = pop_lsb(&b);
        ASSERT_LT(s, SQ_NONE);
        ASSERT_NE(b, b_prev);
        ASSERT_EQ(num_squares(b ^ b_prev), 1);
        ASSERT_EQ(b | get_bb(s), b_prev);
    }
}
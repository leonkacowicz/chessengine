#include <gtest/gtest.h>
#include <square.h>
#include <bitboard.h>

TEST(bitboard_test, size_of_class) {
    std::cout << "\nSize of class: " << sizeof(bitboard) << " bytes" << std::endl;
}

TEST(bitboard_test, bitboard_consistency) {
    for (int f = 0; f < 8; f++) {
        for (int r = 0; r < 8; r++) {
            bitboard bbo = bb(f, r);

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
            square sq(f, r);
            bitboard bbo = bb(sq);

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
            bitboard bbo = bb(f, r);
            auto sq = get_square(bbo);
            ASSERT_EQ(sq.get_file(), f);
            ASSERT_EQ(sq.get_rank(), r);
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
            EXPECT_EQ(shift<UP_RIGHT>(bb(f, r)), bb(f + 1, r + 1));
        }
    }
}

TEST(bitboard_test, test_shift_up_left) {
    for (int r = 0; r < 7; r++) {
        for (int f = 1; f < 8; f++) {
            EXPECT_EQ(shift<UP_LEFT>(bb(f, r)), bb(f - 1, r + 1));
        }
    }
}

TEST(bitboard_test, test_shift_down_right) {
    for (int r = 1; r < 8; r++) {
        for (int f = 0; f < 7; f++) {
            EXPECT_EQ(shift<DOWN_RIGHT>(bb(f, r)), bb(f + 1, r - 1));
        }
    }
}

TEST(bitboard_test, test_shift_down_left) {
    for (int r = 1; r < 8; r++) {
        for (int f = 1; f < 8; f++) {
            EXPECT_EQ(shift<DOWN_LEFT>(bb(f, r)), bb(f - 1, r - 1));
        }
    }
}

TEST(bitboard_test, bitboard_square_conv) {
    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++) {
            square expected(x, y);
            bitboard bitBoard = bb(expected);
            square calculated = get_square(bitBoard);
            ASSERT_EQ(expected, calculated);
        }

    ASSERT_EQ(bb(square::none), 0);
}
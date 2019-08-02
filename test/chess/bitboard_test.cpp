#include <gtest/gtest.h>
#include <square.h>
#include <bitboard.h>

TEST(bitboard_test, size_of_class) {
    std::cout << "\nSize of class: " << sizeof(bitboard) << " bytes" << std::endl;
}

TEST(bitboard_test, bitboard_consistency) {
    for (int f = 0; f < 8; f++) {
        for (int r = 0; r < 8; r++) {
            bitboard bb(f, r);

            ASSERT_EQ(bb[file[f] & rank[r]], true);
            for (int k = 0; k < 8; k++) {
                if (k != f) {
                    EXPECT_EQ(bb & file[k], 0);
                }
                if (k != r) {
                    EXPECT_EQ(bb & rank[k], 0);
                }
            }
        }
    }
}

TEST(bitboard_test, bitboard_square_conversion) {
    for (int f = 0; f < 8; f++) {
        for (int r = 0; r < 8; r++) {
            square sq(f, r);
            bitboard bb(sq);

            ASSERT_EQ(bb[file[f] & rank[r]], true);
            for (int k = 0; k < 8; k++) {
                if (k != f) {
                    EXPECT_EQ(bb & file[k], 0);
                }
                if (k != r) {
                    EXPECT_EQ(bb & rank[k], 0);
                }
            }
        }
    }
}

TEST(bitboard_test, bitboard_square_conversion_inv) {
    for (int f = 0; f < 8; f++) {
        for (int r = 0; r < 8; r++) {
            bitboard bb(f, r);
            auto sq = bb.get_square();
            ASSERT_EQ(sq.get_file(), f);
            ASSERT_EQ(sq.get_rank(), r);
        }
    }
}

TEST(bitboard_test, costants_and_ops) {
    for (int f = 0; f < 7; f++) {
        EXPECT_EQ(file[f].shift<RIGHT>(), file[f + 1]);
    }

    for (int f = 7; f > 0; f--) {
        EXPECT_EQ(file[f].shift<LEFT>(), file[f - 1]);
    }

    for (int r = 0; r < 7; r++) {
        EXPECT_EQ(rank[r].shift<UP>(), rank[r + 1]);
    }

    for (int r = 7; r > 0; r--) {
        EXPECT_EQ(rank[r].shift<DOWN>(), rank[r - 1]);
    }
}

TEST(bitboard_test, test_shift_up_right) {
    for (int r = 0; r < 7; r++) {
        for (int f = 0; f < 7; f++) {
            EXPECT_EQ(bitboard(f, r).shift<UP_RIGHT>(), bitboard(f + 1, r + 1));
        }
    }
}

TEST(bitboard_test, test_shift_up_left) {
    for (int r = 0; r < 7; r++) {
        for (int f = 1; f < 8; f++) {
            EXPECT_EQ(bitboard(f, r).shift<UP_LEFT>(), bitboard(f - 1, r + 1));
        }
    }
}

TEST(bitboard_test, test_shift_down_right) {
    for (int r = 1; r < 8; r++) {
        for (int f = 0; f < 7; f++) {
            EXPECT_EQ(bitboard(f, r).shift<DOWN_RIGHT>(), bitboard(f + 1, r - 1));
        }
    }
}

TEST(bitboard_test, test_shift_down_left) {
    for (int r = 1; r < 8; r++) {
        for (int f = 1; f < 8; f++) {
            EXPECT_EQ(bitboard(f, r).shift<DOWN_LEFT>(), bitboard(f - 1, r - 1));
        }
    }
}

TEST(bitboard_test, bitboard_square_conv) {
    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++) {
            square expected(x, y);
            bitboard bitBoard = bitboard(expected);
            square calculated = bitBoard.get_square();
            ASSERT_EQ(expected, calculated);
        }

    ASSERT_EQ(bitboard(square::none), 0);
}
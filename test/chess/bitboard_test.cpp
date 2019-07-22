#include <gtest/gtest.h>
#include <square.h>
#include <bitboard.h>

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
            auto sq = bb.asSquarePosition();
            ASSERT_EQ(sq.get_file(), f);
            ASSERT_EQ(sq.get_rank(), r);
        }
    }
}

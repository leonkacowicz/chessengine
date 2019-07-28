#include <gtest/gtest.h>
#include <bitboard.h>

TEST(no_op, no_op_test) {
    // do nothing
}

int main(int argc, char **argv) {
    bitboard::initializePositions();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
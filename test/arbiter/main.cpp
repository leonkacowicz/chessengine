#include <gtest/gtest.h>
#include <bitboard.h>

int main(int argc, char **argv) {
    chess::core::init_bitboards();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
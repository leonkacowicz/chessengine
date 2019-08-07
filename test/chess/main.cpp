#include <gtest/gtest.h>
#include <bitboard.h>
#include <magic_bitboard.h>

int main(int argc, char **argv) {
    chess::core::print_bb(0);
    chess::core::init_bitboards();
    chess::core::init_magic_bitboards();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
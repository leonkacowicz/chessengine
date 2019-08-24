#include <gtest/gtest.h>
#include <bitboard.h>
#include <magic_bitboard.h>
#include <zobrist.h>

int main(int argc, char **argv) {
    chess::core::init_bitboards();
    chess::core::init_magic_bitboards();
    zobrist::init();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
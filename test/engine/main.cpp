#include <gtest/gtest.h>
#include <core.h>

int main(int argc, char **argv) {
    chess::core::init();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
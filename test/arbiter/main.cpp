#include <string>
#include <gtest/gtest.h>
#include <spawn.h>
#include <process.h>

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
//
// Created by leon on 2019-11-24.
//

#include <gtest/gtest.h>
#include <algorithm.h>

using namespace chess::optimizer;

TEST(algorithm_test, add_generation_test) {

    algorithm a(3, 4);
    a.run();
}
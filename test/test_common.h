//
// Created by leonkaco on 7/23/19.
//

#ifndef CHESSENGINE_TEST_COMMON_H
#define CHESSENGINE_TEST_COMMON_H
#include <gtest/gtest.h>

#define ASSERT_NOT_CONTAINS(X, Y) ASSERT_EQ(std::find(begin(X), end(X), Y), end(X))
#define ASSERT_CONTAINS(X, Y) ASSERT_NE(std::find(begin(X), end(X), Y), end(X))


#endif //CHESSENGINE_TEST_COMMON_H
#ifndef CHESSENGINE_TEST_COMMON_H
#define CHESSENGINE_TEST_COMMON_H
#include <gtest/gtest.h>

#define ASSERT_NOT_CONTAINS(X, Y) ASSERT_EQ(std::find(begin(X), end(X), Y), end(X))
#define ASSERT_CONTAINS(X, Y) ASSERT_NE(std::find(begin(X), end(X), Y), end(X))
#define ASSERT_CONTAINS_ALL(X, Y...) for(auto& e : Y) { ASSERT_NE(std::find(begin(X), end(X), e), end(X)); }


#endif //CHESSENGINE_TEST_COMMON_H

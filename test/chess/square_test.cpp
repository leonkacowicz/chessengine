#include <gtest/gtest.h>
#include <chess/square.h>

using namespace chess::core;

TEST(square_test, sanity_check_null_square_is_not_equal_to_any_other) {
    for (char file = 'a'; file <= 'h'; file++) {
        for (char rank = '1'; rank <= '8'; rank++) {
            square sq = get_square(file, rank);
            ASSERT_FALSE(sq == SQ_NONE);
        }
    }
}

TEST(square_test, sanity_check_square_is_equal_to_its_spec) {
    for (int file = 0; file < 8; file++) {
        for (int rank = 0; rank < 8; rank++) {
            square sq = get_square(file, rank);
            std::stringstream ss;
            ss << char('a' + file) << char('1' + rank);
            square sq2 = get_square(ss.str().c_str());
            ASSERT_EQ(sq, sq2);
            ASSERT_EQ(get_file(sq), file);
            ASSERT_EQ(get_file(sq2), file);
            ASSERT_EQ(get_rank(sq), rank);
            ASSERT_EQ(get_rank(sq2), rank);
        }
    }
}
#include <gtest/gtest.h>
#include <square.h>

TEST(square_test, sanity_check_null_square_is_not_equal_to_any_other) {
    square none;
    for (char file = 'a'; file <= 'h'; file++) {
        for (char rank = '1'; rank <= '8'; rank++) {
            square sq(std::string({file, rank}));
            ASSERT_FALSE(sq == none);
        }
    }
}

TEST(square_test, sanity_check_square_is_equal_to_its_spec) {
    for (char file = 'a'; file <= 'h'; file++) {
        for (char rank = '1'; rank <= '8'; rank++) {
            square sq(std::string({file, rank}));
            square sq2(file - 'a', rank - '1');
            ASSERT_EQ(sq, sq2);
            ASSERT_EQ(sq.get_file(), file - 'a');
            ASSERT_EQ(sq2.get_file(), file - 'a');
            ASSERT_EQ(sq.get_rank(), rank - '1');
            ASSERT_EQ(sq2.get_rank(), rank - '1');
            ASSERT_EQ(sq.to_string(), std::string({file, rank}));
            ASSERT_EQ(sq2.to_string(), std::string({file, rank}));
        }
    }
}
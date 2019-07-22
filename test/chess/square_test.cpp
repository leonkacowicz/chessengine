#include <gtest/gtest.h>
#include <square.h>

TEST(square_test, sanity_check_null_square_is_not_equal_to_any_other) {
    square none;
    for (char file = 'a'; file <= 'h'; file++) {
        for (char rank = '1'; rank <= '8'; rank++) {
            square sq(std::string({file, rank}));
            std::cout << "Testing " << sq.to_string() << std::endl;
            ASSERT_FALSE(sq == none);
        }
    }
}
#include <gtest/gtest.h>
#include <chess/uci/uci.h>

using namespace chess::uci;

TEST(uci_parse_position_cmd, startpos) {
    auto position = parse_cmd_position({"position", "startpos"});
    EXPECT_EQ(position.initial_position, "startpos");
    EXPECT_TRUE(position.moves.empty());
}

TEST(uci_parse_position_cmd, startpos_with_moves) {
    auto position = parse_cmd_position({"position", "startpos", "moves", "e2e4", "e7e5"});
    EXPECT_EQ(position.initial_position, "startpos");
    EXPECT_EQ(position.moves.size(), 2);
    EXPECT_EQ(position.moves[0], "e2e4");
    EXPECT_EQ(position.moves[1], "e7e5");
}

TEST(uci_parse_position_cmd, fen_pos) {
    std::stringstream ss("position fen r1bqkb1r/pppp1ppp/2n2n2/4p3/4P3/2N2N2/PPPP1PPP/R1BQKB1R w KQkq - 0 1");
    std::vector<std::string> tokens;
    std::string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    auto position = parse_cmd_position(tokens);
    EXPECT_EQ(position.initial_position, "r1bqkb1r/pppp1ppp/2n2n2/4p3/4P3/2N2N2/PPPP1PPP/R1BQKB1R w KQkq - 0 1");
    EXPECT_TRUE(position.moves.empty());
}

TEST(uci_parse_position_cmd, fen_pos_with_moves) {
    std::stringstream ss("position fen r1bqkb1r/pppp1ppp/2n2n2/4p3/4P3/2N2N2/PPPP1PPP/R1BQKB1R w KQkq - 0 1 moves f1c4");
    std::vector<std::string> tokens;
    std::string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    auto position = parse_cmd_position(tokens);
    EXPECT_EQ(position.initial_position, "r1bqkb1r/pppp1ppp/2n2n2/4p3/4P3/2N2N2/PPPP1PPP/R1BQKB1R w KQkq - 0 1");
    EXPECT_EQ(position.moves.size(), 1);
    EXPECT_EQ(position.moves[0], "f1c4");
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
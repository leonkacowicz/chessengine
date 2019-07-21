#include <string>
#include <gtest/gtest.h>
#include <process.h>
#include <arbiter.h>
#include <thread>
#include <mutex>
#include <deque>

using namespace std;

TEST(arbiter_test, arbiter_can_start_players) {

    stringstream white_in, white_out, black_in, black_out("uciok\n");
    white_out << "uciok" << endl;
    white_out << "bestmove (none)" << endl;
    player white(color::WHITE, white_in, white_out);
    player black(color::BLACK, black_in, black_out);

    arbiter arb(white, black, 1000ms, 0ms);

    arb.start_players();
    arb.start_game();

    string line;
    getline(white_in, line);
    EXPECT_STREQ(line.c_str(), "uci");

    getline(black_in, line);
    EXPECT_STREQ(line.c_str(), "uci");
}

TEST(arbiter_test, arbiter_passes_moves_from_one_player_another) {

    stringstream white_in, white_out, black_in, black_out;
    player white(color::WHITE, white_in, white_out);
    player black(color::BLACK, black_in, black_out);

    white_out << "uciok" << endl;
    white_out << "bestmove e2e4" << endl;
    white_out << "bestmove a2a4" << endl;
    white_out << "bestmove h2h4" << endl;

    black_out << "uciok" << endl;
    black_out << "bestmove e7e5" << endl;
    black_out << "bestmove a7a5" << endl;
    black_out << "bestmove (none)" << endl;

    arbiter arb(white, black, 10000ms, 0ms);

    arb.start_players();
    arb.start_game();

    string line;
    getline(white_in, line);
    EXPECT_STREQ(line.c_str(), "uci");
    getline(white_in, line);
    EXPECT_STREQ(line.c_str(), "ucinewgame");
    getline(white_in, line);
    EXPECT_STREQ(line.c_str(), "position startpos");
    getline(white_in, line);
    EXPECT_STREQ(line.substr(0, 3).c_str(), "go ");
    getline(white_in, line);
    EXPECT_STREQ(line.c_str(), "position startpos moves e2e4 e7e5");
    getline(white_in, line);
    EXPECT_STREQ(line.substr(0, 3).c_str(), "go ");
    getline(white_in, line);
    EXPECT_STREQ(line.c_str(), "position startpos moves e2e4 e7e5 a2a4 a7a5");
    getline(white_in, line);
    EXPECT_STREQ(line.substr(0, 3).c_str(), "go ");
    
    getline(black_in, line);
    EXPECT_STREQ(line.c_str(), "uci");
    getline(black_in, line);
    EXPECT_STREQ(line.c_str(), "ucinewgame");
    getline(black_in, line);
    EXPECT_STREQ(line.c_str(), "position startpos moves e2e4");
    getline(black_in, line);
    EXPECT_STREQ(line.substr(0, 3).c_str(), "go ");
    getline(black_in, line);
    EXPECT_STREQ(line.c_str(), "position startpos moves e2e4 e7e5 a2a4");
    getline(black_in, line);
    EXPECT_STREQ(line.substr(0, 3).c_str(), "go ");
    getline(black_in, line);
    EXPECT_STREQ(line.c_str(), "position startpos moves e2e4 e7e5 a2a4 a7a5 h2h4");
    getline(black_in, line);
    EXPECT_STREQ(line.substr(0, 3).c_str(), "go ");
    cout << line;
}
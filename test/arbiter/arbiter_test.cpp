#include <string>
#include <gtest/gtest.h>
#include <chess/core.h>
#include <arbiter.h>
#include <player.h>
#include <mutex>
#include <deque>

using namespace std;
using namespace chess::core;

void expect_line(std::istream& stream, const std::string& expected) {
    std::string actual;
    while (std::getline(stream, actual) && actual.empty());
    EXPECT_EQ(expected, actual);
}

void expect_line_starting_with(std::istream& stream, const std::string& expected) {
    std::string actual;
    while (std::getline(stream, actual) && actual.empty());
    EXPECT_EQ(expected, actual.substr(0, expected.size()));
}

TEST(arbiter_test, arbiter_can_start_players) {

    std::stringstream white_in, white_out, black_in, black_out("uciok\nreadyok\n");
    white_out << "uciok" << std::endl;
    white_out << "readyok" << std::endl;
    white_out << "bestmove (none)" << std::endl;
    player white(color::WHITE, white_in, white_out);
    player black(color::BLACK, black_in, black_out);

    game_settings gs = {};
    arbiter arb(white, black, gs);

    arb.start_players("", "");
    arb.start_game();

    expect_line(white_in, "uci");
    expect_line(black_in, "uci");
}

TEST(arbiter_test, arbiter_passes_moves_from_one_player_another) {

    std::stringstream white_in, white_out, black_in, black_out;
    player white(color::WHITE, white_in, white_out);
    player black(color::BLACK, black_in, black_out);

    white_out << "uciok" << std::endl;
    white_out << "readyok" << std::endl;
    white_out << "bestmove e2e4" << std::endl;
    white_out << "bestmove a2a4" << std::endl;
    white_out << "bestmove h2h4" << std::endl;

    black_out << "uciok" << std::endl;
    black_out << "readyok" << std::endl;
    black_out << "bestmove e7e5" << std::endl;
    black_out << "bestmove a7a5" << std::endl;
    black_out << "bestmove (none)" << std::endl;

    arbiter arb(white, black, {});

    arb.start_players();
    arb.start_game();

    expect_line(white_in, "uci");
    expect_line(white_in, "isready");
    expect_line(white_in, "ucinewgame");
    expect_line(white_in, "position startpos");
    expect_line_starting_with(white_in, "go");
    expect_line(white_in, "position startpos moves e2e4 e7e5");
    expect_line_starting_with(white_in, "go");
    expect_line(white_in, "position startpos moves e2e4 e7e5 a2a4 a7a5");
    expect_line_starting_with(white_in, "go");

    expect_line(black_in, "uci");
    expect_line(black_in, "isready");
    expect_line(black_in, "ucinewgame");
    expect_line(black_in, "position startpos moves e2e4");
    expect_line_starting_with(black_in, "go");
    expect_line(black_in, "position startpos moves e2e4 e7e5 a2a4");
    expect_line_starting_with(black_in, "go");
    expect_line(black_in, "position startpos moves e2e4 e7e5 a2a4 a7a5 h2h4");
    expect_line_starting_with(black_in, "go");
}

TEST(arbiter_test, test_arbitraty_initial_pos_1) {

    std::stringstream white_in, white_out, black_in, black_out;
    player white(WHITE, white_in, white_out);
    player black(BLACK, black_in, black_out);
    game_settings settings;
    settings.white_settings.initial_time = 30000ms;
    settings.white_settings.time_increment = 0ms;
    settings.white_settings.move_time = 0ms;
    settings.initial_position = "startpos moves e2e4 e7e5";

    arbiter arb(white, black, settings);
    white_out << "uciok" << std::endl
              << "readyok" << std::endl;
    black_out << "uciok" << std::endl
              << "readyok" << std::endl;

    white_out << "bestmove (none)" << std::endl;
    arb.start_players();
    expect_line(white_in, "uci");
    expect_line(white_in, "isready");
    arb.start_game();
    expect_line(white_in, "ucinewgame");
    expect_line(white_in, "position startpos moves e2e4 e7e5");
    expect_line_starting_with(white_in, "go");
}

TEST(test_a, test_b) {
    std::stringstream ss("asdf bcbc lelel");

    std::string line;
    std::cout << bool(ss >> line) << "\n";
    std::cout << line << "\n";
    std::cout << ss.str().substr(line.size() + 1) << "\n";
    std::cout << bool(ss >> line) << "\n";
    std::cout << line << "\n";
    std::cout << bool(ss >> line) << "\n";
    std::cout << line << "\n";
}

TEST(time_format_test, time_format_test_milli) {
    std::cout << pretty_time() << std::endl;
}
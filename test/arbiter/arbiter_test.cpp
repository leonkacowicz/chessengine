#include <string>
#include <gtest/gtest.h>
#include <process.h>
#include <arbiter.h>
#include <thread>
#include <mutex>
#include <deque>

using namespace std;

TEST(arbiter_test, arbiter_can_start_players) {

    stringstream white_in, white_out("uciok\n"), black_in, black_out("uciok\n");
    player white(white_in, white_out);
    player black(black_in, black_out);

    arbiter arb(white, black, 10000, 0);

    auto arb_th = thread([&] {
        arb.start_players();
    });
    arb_th.detach();

    this_thread::sleep_for(std::chrono::milliseconds(1));
    string line;
    getline(white_in, line);
    ASSERT_STREQ(line.c_str(), "uci");

    getline(black_in, line);
    ASSERT_STREQ(line.c_str(), "uci");
}
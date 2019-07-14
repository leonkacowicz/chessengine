#include <string>
#include <gtest/gtest.h>
#include <spawn.h>
#include <process.h>

using namespace std;

TEST(process_test, start_process_streams) {
    process p("cat");

    string test_str("ksdfjaasdfjbasdkljfbadkljfb");

    p << test_str;
    p.send_eof();

    string received;
    p >> received;
    p.wait();

    ASSERT_GT(p.pid, 0);
    ASSERT_STREQ(test_str.c_str(), received.c_str());
}

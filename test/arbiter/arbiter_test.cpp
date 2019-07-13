#include <string>
#include <gtest/gtest.h>
#include <spawn.h>
#include <process.h>

using namespace std;

TEST(arbiter_test, start_process_streams) {
    process p("cat");

    string test_str("ksdfjaasdfjbasdkljfbadkljfb");

    //write(process.stdin_fd, test_str.c_str(), sizeof(char));
    p.stdin << test_str;
    p.send_eof();

    //read(process.stdout_fd, buf, test_str.size());
    string received;
    p.stdout >> received;
    p.wait();

    ASSERT_GT(p.pid, 0);
    ASSERT_STREQ(test_str.c_str(), received.c_str());
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
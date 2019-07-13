#include <iostream>
#include <string>

using namespace std;

struct process {
    int pid;
    int stdin_fd;
    int stdout_fd;
};

class arbiter {
    const string& white_player;
    const string& black_player;
    int white_time;
    int black_time;
    int increment;

    process white;
    process black;

public:
    arbiter(const string& white_player, const string& black_player, int total_seconds_per_player, int increment);
    void start_players();
    void hello();
};
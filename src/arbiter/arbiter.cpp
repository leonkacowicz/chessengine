#include <iostream>
#include <unistd.h>
#include "arbiter.h"

void arbiter::hello() {
    std::cout << "Hello I'm arbiter" << std::endl;
}

arbiter::arbiter(const string &white_player, const string &black_player, int total_seconds_per_player, int increment) :
    white_player(white_player), black_player(black_player),
    white_time(total_seconds_per_player), black_time(total_seconds_per_player),
    increment(increment) {

}

process start_process(const string& process) {

    struct process proc;
    const int PIPE_IN = 1;
    const int PIPE_OUT = 0;

    int parent_to_child[2];
    int child_to_parent[2];

    if (!pipe(parent_to_child)) {
        cerr << "Failed to create parent_to_child pipe." << endl;
        throw std::exception();
    }
    if (!pipe(child_to_parent)) {
        cerr << "Failed to create child_to_parent pipe." << endl;
        __throw_runtime_error("Failed to create child_to_parent pipe.");
    }

    int pid;
    if ((pid = fork()) == 0) {
        // child process
        dup2(parent_to_child[PIPE_OUT], STDIN_FILENO);
        dup2(child_to_parent[PIPE_IN], STDOUT_FILENO);

        char * const argv[1] = {nullptr};
        exit(execvp(process.c_str(), argv));

    } else if (pid == -1) {
        __throw_runtime_error("Failed to create child_to_parent pipe.");
    } else {
        proc.pid = pid;
        proc.stdout_fd = child_to_parent[PIPE_OUT];
        proc.stdin_fd = parent_to_child[PIPE_IN];
    }
    return proc;
}

void arbiter::start_players() {


}

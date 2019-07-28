
#include <memory>
#include <unistd.h>
#include <iostream>
#include <string>
#include <wait.h>
#include "process.h"

using namespace std;

process::process(const std::string & executable) : stdin(NULL), stdout(NULL) {
    const int PIPE_IN = 1;
    const int PIPE_OUT = 0;

    int parent_to_child[2];
    int child_to_parent[2];

    if (pipe(child_to_parent) != 0) {
        cerr << "Failed to create child_to_parent pipe." << std::endl;
        __throw_runtime_error("Failed to create child_to_parent pipe.");
    }
    if (pipe(parent_to_child) != 0) {
        cerr << "Failed to create parent_to_child pipe." << std::endl;
        __throw_runtime_error("Failed to create parent_to_child pipe.");
    }

    if ((pid = fork()) == 0) {
        // child process
        dup2(parent_to_child[PIPE_OUT], STDIN_FILENO);
        dup2(child_to_parent[PIPE_IN], STDOUT_FILENO);

        close(parent_to_child[PIPE_IN]);
        close(parent_to_child[PIPE_OUT]);
        close(child_to_parent[PIPE_IN]);
        close(child_to_parent[PIPE_OUT]);

        char * const argv[] = {(char *)executable.c_str(), (char *)0};
        if (execvp(argv[0], argv) == -1) {
            cerr << "Failed to launch process" << std::endl;
            exit(1);
        }
        exit(0);

    } else if (pid == -1) {
        __throw_runtime_error("Failed to child process.");
    } else {
        close(parent_to_child[PIPE_OUT]);
        close(child_to_parent[PIPE_IN]);

        write_buf = make_unique<__gnu_cxx::stdio_filebuf<char> >(parent_to_child[PIPE_IN], std::ios::out);
        read_buf = make_unique<__gnu_cxx::stdio_filebuf<char> >(child_to_parent[PIPE_OUT], std::ios::in);
        stdin.rdbuf(write_buf.get());
        stdout.rdbuf(read_buf.get());
    }
}

void process::send_eof() {
    write_buf->close();
}

int process::wait() {
    int status;
    waitpid(pid, &status, 0);
    return status;
}

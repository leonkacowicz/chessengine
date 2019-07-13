#ifndef CHESSENGINE_PROCESS_H
#define CHESSENGINE_PROCESS_H

#include <string>
#include <memory>
#include <ext/stdio_filebuf.h>

using namespace std;

class process {
    std::unique_ptr<__gnu_cxx::stdio_filebuf<char> > write_buf;
    std::unique_ptr<__gnu_cxx::stdio_filebuf<char> > read_buf;
public:
    int stdin_fd;
    int stdout_fd;
    int pid;
    std::ostream stdin;
    std::istream stdout;
    process(const string & executable);
    void send_eof();
    int wait();
};

#endif //CHESSENGINE_PROCESS_H

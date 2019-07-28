#ifndef CHESSENGINE_PROCESS_H
#define CHESSENGINE_PROCESS_H

#include <string>
#include <memory>
#include <ext/stdio_filebuf.h>

class process {
    std::unique_ptr<__gnu_cxx::stdio_filebuf<char> > write_buf;
    std::unique_ptr<__gnu_cxx::stdio_filebuf<char> > read_buf;
public:
    int pid;
    std::ostream stdin;
    std::istream stdout;
    process(const std::string & executable);
    void send_eof();
    int wait();

    template <typename T>
    process& operator<<(const T& obj) {
        stdin << obj;
        return *this;
    }

    template <typename T>
    process& operator>>(T& obj) {
        stdout >> obj;
        return *this;
    }
};

#endif //CHESSENGINE_PROCESS_H

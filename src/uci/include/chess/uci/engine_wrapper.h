//
// Created by leon on 2020-01-31.
//

#ifndef CHESSENGINE_ENGINE_WRAPPER_H
#define CHESSENGINE_ENGINE_WRAPPER_H

#include <string>
#include <istream>
#include <ostream>
#include <chess/uci/uci.h>

namespace chess::uci {

class engine_wrapper {
    std::istream& std_out;
    std::ostream& std_in;
public:
    engine_wrapper(std::ostream& in, std::istream& out);
    void send_uci();
    void send_isready();

    void wait_for_uciok();
    void wait_for_readyok();
    void send_option(const std::string& name, const std::string& value);
    void send_go(const cmd_go& cmd);
    void send_position(const cmd_position& cmd);
    void send_stop();

    void send_ucinewgame();
};

}
#endif //CHESSENGINE_ENGINE_WRAPPER_H

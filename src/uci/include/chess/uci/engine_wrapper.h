//
// Created by leon on 2020-01-31.
//

#ifndef CHESSENGINE_ENGINE_WRAPPER_H
#define CHESSENGINE_ENGINE_WRAPPER_H

#include <istream>
#include <ostream>
#include <chess/uci/uci.h>

namespace chess::uci {

class engine_wrapper {
    std::istream& std_out;
    std::ostream& std_in;
public:
    engine_wrapper(std::ostream& in, std::istream& out);
    void send_go_command(const cmd_go& cmd);
    void send_position_command(const cmd_go& cmd);
};

}
#endif //CHESSENGINE_ENGINE_WRAPPER_H

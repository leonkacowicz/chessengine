//
// Created by leonkaco on 2/5/20.
//

#include <string>
#include <chess/core.h>
#include <iostream>
#include <sstream>
#include <chess/board.h>
#include <chess/game.h>

int main() {
    chess::core::init();
    std::string detected_initial_pos("startpos");
    chess::core::game g;

    std::string line;
    while (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        std::string token;
        ss >> token;
        if (token == "--initial-pos") {
            ss >> token; // = equals sign
            detected_initial_pos = "";
            while (!ss.eof()) {
                if (detected_initial_pos.empty()) ss >> detected_initial_pos;
                else {
                    ss >> token;
                    detected_initial_pos += " " + token;
                }
            }
        }
    }
}
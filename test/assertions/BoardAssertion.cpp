//
// Created by leon on 19/06/18.
//

#include <iostream>
#include "BoardAssertion.h"

void Assertion<Board>::isNotCheckmate() const {
    if (board.isCheckmate())
        throw std::string("Test failed");

    std::cout << "asserted" << std::endl;
}



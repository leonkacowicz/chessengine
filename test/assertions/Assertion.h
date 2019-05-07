#ifndef CHESSENGINE_ASSERTION_H
#define CHESSENGINE_ASSERTION_H

#include "../../src/engine/Board.h"

template <typename T>
class Assertion {
};

template<typename T>
Assertion<T> assertThat(T value) {
    return Assertion<T>(value);
}




#endif //CHESSENGINE_ASSERTION_H

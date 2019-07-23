#include "square.h"

const square square::none = square();

std::ostream& operator<<(std::ostream& os, const square pos) {
    os << ('a' + pos.get_file()) << ('1' + pos.get_rank());
    return os;
}
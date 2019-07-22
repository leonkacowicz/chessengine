#include "Square.h"

std::ostream& operator<<(std::ostream& os, const Square pos) {
    os << ('a' + pos.get_file()) << ('1' + pos.get_rank());
    return os;
}
#include "Square.h"

std::ostream& operator<<(std::ostream& os, const Square pos) {
    os << ('a' + pos.getX()) << ('1' + pos.getY());
    return os;
}
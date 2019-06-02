#include "Square.h"

ostream& operator<<(ostream& os, const Square pos) {
    os << ('a' + pos.getX()) << ('1' + pos.getY());
    return os;
}
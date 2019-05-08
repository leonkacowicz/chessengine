#include "SquarePosition.h"

ostream& operator<<(ostream& os, const SquarePosition pos) {
    os << ('a' + pos.getX()) << ('1' + pos.getY());
    return os;
}
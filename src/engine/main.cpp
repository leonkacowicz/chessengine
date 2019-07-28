#include <iostream>
#include <string>
#include <algorithm>
#include <board.h>

using namespace std;

int main()
{
    board b;
    b.set_initial_position();
    b.print();
    std::cout << sizeof(b);
    return 0;
}
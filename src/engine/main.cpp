#include <iostream>
#include <string>
#include <algorithm>
#include <Board.h>

using namespace std;

int main()
{
    Board board;
    board.set_initial_position();
    board.print();
    cout << sizeof(Board);
    return 0;
}
#include <iostream>
#include <string>
#include <algorithm>
#include <Board.h>

using namespace std;

int main()
{
    Board board;
    board.setInitialPosition();
    board.printBoard();
    cout << sizeof(Board);
    return 0;
}
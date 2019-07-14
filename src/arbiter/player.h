#ifndef CHESSENGINE_PLAYER_H
#define CHESSENGINE_PLAYER_H

#include <string>
#include <vector>

using namespace std;

class player {

public:
    void set_position(const vector<string>& moves);
    void calculate_next_move(int white_time_millis, int black_time_millis, int white_increment, int black_increment);
    void get_next_move();

};


#endif //CHESSENGINE_PLAYER_H

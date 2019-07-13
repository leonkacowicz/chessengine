#include <iostream>
#include <unistd.h>
#include "arbiter.h"

void arbiter::hello() {
    std::cout << "Hello I'm arbiter" << std::endl;
}

arbiter::arbiter(const string &white_player, const string &black_player, int total_seconds_per_player, int increment) :
    white_player(white_player), black_player(black_player),
    white_time(total_seconds_per_player), black_time(total_seconds_per_player),
    increment(increment) {

}



void arbiter::start_players() {


}

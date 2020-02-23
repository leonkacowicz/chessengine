//
// Created by leon on 2020-02-22.
//

#ifndef CHESSENGINE_CONDITION_WAITER_H
#define CHESSENGINE_CONDITION_WAITER_H

#include <mutex>
#include <condition_variable>

class condition_waiter {
    std::mutex m;
    bool value = false;
    std::condition_variable cv;
public:
    void wait();
    bool wait_for(std::chrono::milliseconds time);
    void notify();
    void reset();
};


#endif //CHESSENGINE_CONDITION_WAITER_H

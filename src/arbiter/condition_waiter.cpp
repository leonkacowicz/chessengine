//
// Created by leon on 2020-02-22.
//

#include "condition_waiter.h"

void condition_waiter::wait() {
    std::unique_lock<std::mutex> lock(m);
    cv.wait(lock, [&] () { return value; });
}

void condition_waiter::notify() {
    std::lock_guard lock(m);
    value = true;
    cv.notify_one();
}

bool condition_waiter::wait_for(std::chrono::milliseconds time) {
    std::unique_lock<std::mutex> lock(m);
    value = false;
    return cv.wait_for(lock, time, [&] () { return value; });
}

void condition_waiter::reset() {
    std::lock_guard lock(m);
    value = false;
}

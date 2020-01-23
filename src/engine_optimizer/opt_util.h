//
// Created by leon on 2020-01-21.
//

#ifndef CHESSENGINE_OPT_UTIL_H
#define CHESSENGINE_OPT_UTIL_H

#include <future>

namespace chess::optimizer::util {

template <typename T, typename Comp>
static int parallel_max(const std::vector<T>& candidates, int l, int r, Comp comp) {
    assert(l < r);
    //std::printf("parallel_max(..., %d, %d, ...);\n", l, r);
    if (l == r - 1) {
        //std::printf("returning %d from parallel_max(..., %d, %d, ...);\n", l, l, r);
        return l;
    }
    int m = (l + r) / 2;
    std::future<int> max_l = std::async(parallel_max<T, Comp>, candidates, l, m, comp);
    std::future<int> max_r = std::async(parallel_max<T, Comp>, candidates, m, r, comp);

    int max_l_value = max_l.get();
    int max_r_value = max_r.get();
    int ret;
    if (comp(candidates[max_l_value], candidates[max_r_value])) ret = max_r_value;
    else ret = max_l_value;
    //std::printf("returning %d from parallel_max(..., %d, %d, ...);\n", ret, l, r);
    return ret;
}

}

#endif //CHESSENGINE_OPT_UTIL_H

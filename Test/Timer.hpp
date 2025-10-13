#pragma once

#include <chrono>
#include <iostream>

struct Timer {
    using clock_t = std::chrono::steady_clock;
    using time_t = std::chrono::time_point<clock_t>;

    time_t t0;

    Timer() : t0{ clock_t::now() } {
    }

    ~Timer() {
        time_t t1{ clock_t::now() };
        auto time_duration{ std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0) };
        std::cout << "Time consumed: " << time_duration.count() << "微秒\n";
    }
};


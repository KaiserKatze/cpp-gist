#pragma once

#include <chrono>
#include <iostream>

struct Timer {
    using time_t = std::chrono::time_point<std::chrono::system_clock>;

    time_t t0;

    Timer() : t0{ std::chrono::system_clock::now() } {
    }

    ~Timer() {
        time_t t1{ std::chrono::system_clock::now() };
        std::cout << "Time consumed: " << (t1 - t0) << '\n';
    }
};


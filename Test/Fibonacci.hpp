#pragma once

#include <vector>
#include <type_traits>
#include <concepts>
#include <iostream>

template <std::unsigned_integral T>
struct Fibonacci {
    std::vector<T> buffer{ 0,1 };
    T operator()(T n) {
        for (auto i = buffer.size(); i <= n; ++i) {
            buffer.push_back(buffer[i - 1] + buffer[i - 2]);
        }
        return buffer[n];
    }
};

void Test() {
    Fibonacci<uint32_t> fib;
    for (int i = 0; i < 10; ++i) {
        std::cout << fib(i) << ", ";
    }
}

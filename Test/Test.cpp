#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include <cassert>
#include <stack>
#include <queue>
#include <concepts>
#include <type_traits>
#include <iomanip>
#include <string>
#include <vector>
#include <compare> // std::strong_ordering (since C++20)
#include <memory>
#include <chrono>

#include <Windows.h>
#include <fcntl.h>
#include <io.h>

#define DEBUG
#include "MissionaryAndCannibal.hpp"

int main(int argc, char** argv) {
    // SetConsoleOutputCP(65001);

    const auto now{ std::chrono::system_clock::now() };
    const std::time_t now_time_t{ std::chrono::system_clock::to_time_t(now) };
    const std::tm* now_tm{ std::localtime(&now_time_t) };

    std::cout << "当前时间："
        << std::put_time(now_tm, "[%Y-%m-%d %H:%M:%S] ")
        << '\n';

    try {
        MissionaryAndCannibal::Solve();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    return 0;
}


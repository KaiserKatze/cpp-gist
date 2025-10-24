// #include <iostream>
// #include <sstream>
// #include <stdexcept>
// #include <cstring>
// #include <cassert>
// #include <stack>
// #include <queue>
// #include <concepts>
// #include <type_traits>
// #include <iomanip>
// #include <string>
// #include <vector>
// #include <compare> // std::strong_ordering (since C++20)
// #include <memory>
// #include <chrono>

// #include <Windows.h>
// #include <fcntl.h>
// #include <io.h>

#define DEBUG
#include "Timer.hpp"
#include "HonoiTower.hpp"

int main(int argc, char** argv) {
    // SetConsoleOutputCP(65001);

    HonoiTower honoiTower(4, 3);
    honoiTower.Solve();

    return 0;
}

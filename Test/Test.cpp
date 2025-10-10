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

#include <Windows.h>
#include <fcntl.h>
#include <io.h>

#define DEBUG
#include "RLYC.hpp"

int main(int argc, char** argv) {
    SetConsoleOutputCP(65001);

    RLYC::Solve();

    return 0;
}


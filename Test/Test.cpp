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
#include "BFS.hpp"
#include "Graph.hpp"

int main(int argc, char **argv)
{
    // SetConsoleOutputCP(65001);

    AdjacencyMatrixGraph<int, int> graph(9, false);
    for (size_t i{0}; i < 9; ++i)
    {
        graph[i] = i;
    }
    graph.Connect({
        {0,1,1},
        {1,2,1},
        {2,3,1},
        {2,8,1},
        {8,7,1},
        {3,6,1},
        {3,4,1},
        {4,5,1},
    });

    BreadthFirstSearch(graph).Traverse(0, [](auto v) {
        std::cout << v << ' ';
    });

    return 0;
}

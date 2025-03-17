#pragma once

#include <vector>
#include <utility>

auto MakeSpiral(size_t w /* 列数 */, size_t h /* 行数 */) {
    // 生成螺旋矩阵
    std::vector<std::vector<size_t>> res(h, std::vector<size_t>(w, 0)); // 初始化矩阵
    size_t left{ 0 }, right{ w - 1 }, top{ 0 }, bottom{ h - 1 };
    size_t n{ 0 };
    while (top <= bottom && left <= right) {
        // 向右遍历
        for (size_t i{ left }; i <= right; ++i) {
            res[top][i] = ++n;
            //std::cout << "res[" << top << "][" << i << "] = " << n << std::endl;
        }
        ++top;

        // 向下遍历
        for (size_t i{ top }; i <= bottom; ++i) {
            res[i][right] = ++n;
            //std::cout << "res[" << i << "][" << right << "] = " << n << std::endl;
        }
        --right;

        // 向左遍历
        if (top <= bottom) {
            for (size_t i{ right + 1 }; i > left;) {
                res[bottom][--i] = ++n;
                //std::cout << "res[" << bottom << "][" << i << "] = " << n << std::endl;
            }
            --bottom;
        }

        // 向上遍历
        if (left <= right) {
            for (size_t i{ bottom + 1 }; i > top;) {
                res[--i][left] = ++n;
                //std::cout << "res[" << i << "][" << left << "] = " << n << std::endl;
            }
            ++left;
        }
    }
    return res;
}

#ifdef DEBUG
#include <iostream>

void TestOutput() {
    auto res{ MakeSpiral(7, 6) };
    for (const auto& row : res) {
        for (int i : row) {
            std::cout
                << std::setw(2)
                << i << ' ';
        }
        std::cout << std::endl;
    }
}
#endif

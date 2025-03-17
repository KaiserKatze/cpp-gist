#pragma once

#include <vector>
#include <random>
#include <string>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cmath>

struct NumberTriangle {
    std::vector<size_t> data;  // 下三角阵
    const size_t n;  // 行数、列数
    const size_t m;  // 存储元素个数（不含右上角元素）

    NumberTriangle(size_t n /* 行数、列数 */)
        : n{ n }, m{ n * (n + 1) / 2 } {
        data.reserve(m + 1);
    }

    NumberTriangle(size_t n /* 行数、列数 */, std::vector<size_t>&& data)
        : n{ n }, m{ n * (n + 1) / 2 }, data{ data } {
    }

    void fill_random() {
        // 随机生成一个下三角矩阵
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(1, 100);
        for (size_t i{ 0 }; i < m; ++i) {
            data.push_back(distrib(gen));
        }
        data.push_back(0);  // 右上角元素
    }

    size_t& operator[](std::pair<size_t, size_t>&& ij) {
        size_t i;  // 行号
        size_t j;  // 列号
        std::tie(i, j) = ij;
        if (!(0 <= i && i < n && 0 <= j && j < n)) {
            throw std::invalid_argument{ "地址越界!" };
        }
        if (i < j) {  // 右上角全是零
            return data[m];  // 返回最后一个元素
        }
        const size_t k{ i * (i + 1) / 2 + j };
        return data[k];  // 返回左下角元素
    }

    size_t operator[](std::pair<size_t, size_t>&& ij) const {
        size_t i;  // 行号
        size_t j;  // 列号
        std::tie(i, j) = ij;
        if (i < j) {  // 右上角全是零
            return data[m];  // 返回最后一个元素
        }
        const size_t k{ i * (i + 1) / 2 + j };
        return data[k];  // 返回左下角元素
    }

    operator std::string() const {
        size_t max{ *std::max_element(data.cbegin(), data.cend()) };
        size_t nbits{ static_cast<size_t>(std::log10(max)) + 1 };
        std::ostringstream ss;
        for (size_t i{ 0 }; i < n; ++i) {
            for (size_t j{ 0 }; j <= i; ++j) {
                ss << ' ' << std::setw(nbits) << this->operator[]({ i, j });
            }
            ss << '\n';
        }
        return ss.str();
    }
};


// 计算从顶部到底部某点的一条路径，使得该路径所经过的数字总和最大。
// 每一步可以沿竖线向下走，或着沿右斜线向下走
// 状态转移方程：
// a[{x, y}] = max( a[{x+1, y}], a[{x+1,y+1}] ) + a[{x, y}]
#include <iostream>
struct ComputeRoute {
    NumberTriangle dp;  // dp[{i, j}] 表示第 i 行第 j 列的最大值，它的状态是由 dp[{i+1, j}] 和 dp[{i+1, j+1}] 转移而来
    const size_t n;

    ComputeRoute(const NumberTriangle& a) : n{ a.n }, dp(a) {
        // 向上逐层累加
        for (size_t i{ n - 1 }; i > 0;) {
            --i;
            for (size_t j{ 0 }; j <= i; ++j) {
                dp[{i, j}] += std::max(
                    dp[{i + 1, j}],
                    dp[{i + 1, j + 1}]
                );
            }
        }
    }
};

#ifdef DEBUG
#include <iostream>
void TestNumberTriangle(size_t n) {
    NumberTriangle nt(4, {1, 4, 6, 8, 3, 9, 5, 7, 2, 1});
    nt.fill_random();
    std::cout << "数塔：\n" << static_cast<std::string>(nt) << '\n';
    ComputeRoute cr(nt);
    std::cout << "状态：\n" << static_cast<std::string>(cr.dp) << '\n';
}
#endif

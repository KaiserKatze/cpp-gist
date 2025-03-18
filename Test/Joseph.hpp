#pragma once
// 约瑟夫环问题：
// n个人围成一个环，从 0 开始编号。
// 从编号为 0 的人开始报数，数到 (m-1) 的人出列，
// 继续从下一个人开始报数，直到剩下最后一人
// 目标是确定最后剩下的人的初始编号
// 【分析】
// 第一个出列的人的编号一定是 (m-1) % n，记下一个人的编号为 k
// 剩下的 n-1 个人的编号分别是 k, k+1, k+2, ..., n-2, n-1, 0, 1, 2, ..., k-3, k-2
// 从 k 开始重新编号，新的编号分别是 0, 1, 2, ..., n-k-2, n-k-1, n-k, ..., n-3, n-2
// 也就是说，新编号 x' 是旧编号 x 的函数，且 x' = (x-k) % (n-1)
// 【结果】
// 设 f(n, m) 表示最后幸存者的编号，则递推公式为：
// f(1, m) = 0
// f(n, m) = (f(n-1, m) + m) % n

#include <stdexcept>

size_t josephus(size_t n /* 人数 */, size_t m /* 报数次数 */) {
    if (n == 0) {
        throw std::invalid_argument{ "人数 n 不得为零!" };
    }
    size_t result{ 0 };
    for (size_t i{ 2 }; i <= n; ++i) {
        result = (result + m) % i;
    }
    return result;
}

#ifdef DEBUG
#include <iostream>

void TestJoseph(size_t n, size_t m) {
    std::cout << "josephus(" << n << ", " << m << ") -> "
        << josephus(n, m) << ".\n";
}
#endif

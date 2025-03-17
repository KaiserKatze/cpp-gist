#pragma once
// 判断、生成回文数

#include <utility>

int reverse(int x) {
    int res{ 0 };
    while (x != 0) {
        res = x % 10 + res * 10;
        x /= 10;
    }
    return res;
}

std::pair<size_t, int> MakePalindromic(int x) {
    // 将输入的 x 与它的反转值 reverse(x) 进行比较
    int rx{ reverse(x) };  // 进行第一次反转
    size_t step{ 0 };  // 统计操作次数
    while (x != rx) {  // 如果 x 不是回文数
        x += rx;
        rx = reverse(x);
        ++step;
    }
    return { step, x };
}


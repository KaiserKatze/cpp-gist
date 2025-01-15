#pragma once

#include "QuickSort.hpp"
#include "BubbleSort.hpp"
#include "SimpleSelectSort.hpp"
#include "MergeSort.hpp"
#include "VerifySort.hpp"

void TestSort() {
    int array[] = { 7,1,88,3,20,27,36,55,60,28,36,67,44,16,60 };
#ifdef DEBUG
    std::cout << "输入序列：(";
    for (auto x : array) {
        std::cout << x << ",";
    }
    std::cout << ").\n";
    std::cout << "元素个数：" << (std::end(array) - std::begin(array)) << ".\n";
#endif

    MergeSort(std::begin(array), std::end(array));
    Verify(std::begin(array), std::end(array));
}


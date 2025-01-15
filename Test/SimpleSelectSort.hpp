#pragma once

#include <algorithm> // std::swap

template <class RandomIt>
void SimpleSelectSort(RandomIt begin, RandomIt end) {
    // 简单选择排序
    for (; begin < end - 1; ++begin) { // 一共进行 n-1 趟
        RandomIt min_pos{ begin }; // 记录最小元素位置
        for (RandomIt p{ begin + 1 }; p < end; ++p) { // 查找最小元素
            if (*p < *min_pos) { // 当前元素比之前记录的最小元素还要小
                min_pos = p; // 更新最小元素的位置
            }
        }
        if (min_pos != begin) {
            std::swap(*min_pos, *begin); // 交换元素
        }
    }
}
// 性能分析：
// 空间效率：仅使用了常数个辅助存储单元，因此空间复杂度为 O(1)
// 时间效率：在简单选择排序过程中，元素移动的操作次数很少，不会超过 3(n-1) 次，最好的情况（表已经有序）是 0 次。
//         但是元素间比较的次数与序列的初始状态无关，始终是 n(n-1)/2 次。
//         因此时间复杂度始终是 O(n^2)。
// 稳定性：简单选择排序是不稳定的。
// 适用性：简单选择排序适用于顺序存储和链式存储的线性表，以及关键字较少的情况。


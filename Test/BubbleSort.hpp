#pragma once

#include <algorithm> // std::swap

template <class RandomIt>
void BubbleSort(RandomIt begin, RandomIt end) {
    // 冒泡排序
    for (; begin != end; ++begin) {
        bool flag{ false }; // 表示本趟冒泡是否交换
        for (RandomIt p{ end - 1 }; p > begin; --p) { // 一趟冒泡过程
            if (*(p - 1) > *p) { // 逆序（左边比右边大）
                std::swap(*(p - 1), *p); // 交换
                flag = true;
            }
        }
        if (!flag) { // 本趟遍历没有交换，说明表已经有序
            break;
        }
    }
}
// 性能分析：
// 空间效率：仅使用了常数个辅助存储单元，因此空间复杂度为 O(1)
// 时间效率：当初始序列有序时，比较次数为 n-1，移动次数为 0，从而最好情况下的时间复杂度为 O(n)。
//         当初始序列逆序时，需要 n-1 趟排序，第 i 趟排序要进行 n-i 次关键字的比较，而且每次比较厚都必须移动元素 3 次来交换元素位置；
//         这种情况下，比较次数为 n(n-1)/2，移动次数为 3n(n-1)/2；
//         从而最坏情况下的时间复杂度为 O(n^2)。
//         平均时间复杂度为 O(n^2)。
// 稳定性：冒泡排序是稳定的。
// 适用性：冒泡排序适用于顺序存储和链式存储的线性表。


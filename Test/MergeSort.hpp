#pragma once

#include <vector> // std::vector
#include <algorithm> // std::copy
#include <iterator> // std::iterator_traits

template <class RandomIt>
struct MergeSort {
    // 二路归并排序（按关键字从小到大的顺序排列）
    using ElemType = typename std::iterator_traits<RandomIt>::value_type;
    using Vector = std::vector<ElemType>;
    Vector duplica;

    MergeSort(RandomIt begin, RandomIt end)
        : duplica(std::distance(begin, end)) {
        Sort(begin, end);
    }

private:
    void Sort(RandomIt begin, RandomIt end) {
        if (begin + 1 < end) { // 如果表中只有1个元素，则它一定是有序的，因此下面只处理长度大于1的表
            RandomIt mid{ begin }; // 用 mid 把原表划分成左右两个子表
            std::advance(mid, std::distance(begin, end) / 2);
            Sort(begin, mid); // 对左子表进行归并排序
            Sort(mid, end); // 对右子表进行归并排序
            Merge(begin, mid, end); // 归并左右子表
        }
    }

    void Merge(RandomIt begin, RandomIt mid, RandomIt end) {
        auto left{ std::begin(duplica) }; // 左子表起点
        const auto left_limit{ left + (mid - begin) }; // 左子表终点
        auto right{ left_limit }; // 右子表起点
        const auto right_limit{ right + (end - mid) }; // 右子表终点
        std::copy(begin, end, left); // 复制全部元素
        while (left != left_limit && right != right_limit) {
            if (*left <= *right) { // 找出左右子表中最小的元素
                *begin = *left; // 复制到原表最左边（有序区右边）
                ++left;
            }
            else {
                *begin = *right;
                ++right;
            }
            ++begin;
        }
        // 由于左右子表长度可能不相等，故在上述 while 循环结束后，可能发生某一个子表还剩一些元素留在最后没有比较大小。
        // 因为每个子表各自是有序的，所以对于这种情况，只需要把剩余元素直接复制到原表最左边（有序区右边）即可。
        while (left != left_limit) { // 左子表还有剩余元素
            *begin = *left;
            ++begin;
            ++left;
        }
        while (right != right_limit) { // 右子表还有剩余元素
            *begin = *right;
            ++begin;
            ++right;
        }
    }
};
// 性能分析：
// 空间效率：在 Merge 操作中最多使用 n 个辅助存储单元，所以空间复杂度为 O(n)
// 时间效率：二路归并排序的每趟归并的时间复杂度是 O(n)，共需进行 ceil(log(n)) 趟归并，因此时间复杂度为 O(n log(n))。
//         对于 n 个元素进行 k 路归并排序的情况，归并的趟数 m 满足 k^m = n，从而 m = ceil(log(n) / log(k))，时间复杂度与二路归并排序相同。
// 稳定性：二路归并排序是稳定的。
// 适用性：归并排序适用于顺序存储和链式存储的线性表。


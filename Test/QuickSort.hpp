#pragma once

//@see: https://www.bilibili.com/video/BV1y4421Z7hK/

template <class RandomIt>
RandomIt Partition(RandomIt left, RandomIt right) {
    // 把表划分为两段，前半段的元素都比枢轴小，后半段的元素都比枢轴大
    const auto pivot{ *left }; // 复制当前表中第一个元素，作为枢轴（可以把枢轴原本占据的位置看作空位了）
    while (left != right) { // 无序区夹在两侧有序区中间，交替扫描无序区的右侧和左侧
        // 首先从右到左扫描，寻找比枢轴小的元素
        while (left != right && *right >= pivot) {
            --right;
        }
        *left = *right; // 把比枢轴小的元素移动到左端有序区右侧空位
        // 接着从左到右扫描，寻找比枢轴大的元素
        while (left != right && *left <= pivot) {
            ++left;
        }
        *right = *left; // 把比枢轴大的元素移动到右端有序区左侧空位
    } // 在上述 while 循环过程中，在表的左右两端各自会产生一个有序区
    *left = pivot; // 把枢轴存放到最终位置
    return left; // 返回枢轴的位置
}

template <class RandomIt>
void QuickSort(RandomIt begin, RandomIt end) {
    // 快速排序
    if (begin < end) { // 确保 begin 和 end 之间至少有一个元素
        const RandomIt pivot_pos{ Partition(begin, end - 1) }; // 分段
        QuickSort(begin, pivot_pos); // 递归调用，对左右两个子表分别排序
        QuickSort(pivot_pos + 1, end);
    }
}
// 性能分析：
// 空间效率：快速排序借助一个递归工作栈保存每层递归调用的必要信息，其容量与递归调用的最大层数一致。
//         最好情况下空间复杂度是 O(log n)；
//         最坏情况下要进行 n-1 次递归调用，空间复杂度是 O(n)；
//         平均情况下空间复杂度是 O(log n)。
// 时间效率：快速排序的运行时间与划分是否对称有关。
//         快速排序的最坏情况发生在两个区域分别包含 n-1 个元素和 0 个元素时，这时候时间复杂度是 O(n^2)。
//         最理想的状态下划分得到的两个子表的大小都不大于 n/2，这时候时间复杂度是 O(n log n)。
// 稳定性：快速排序是不稳定的。
// 适用性：快速排序仅适用于顺序存储的线性表。
// 特点：快速排序每一趟总有一个元素（即枢轴）会被放在它的最终位置上，不再移动。


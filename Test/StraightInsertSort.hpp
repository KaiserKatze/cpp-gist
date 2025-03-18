#pragma once

#include <iostream>

template <class ElemType>
void StraightInsertSort(ElemType* array, size_t start, size_t end) {
    // 直接插入排序（不带监视哨）
    for (size_t i{ start + 1 }; i < end; ++i) { // 从第二个元素开始排序
        ElemType* curr{ array + i }; // 正在检查的记录
        ElemType* prev{ curr - 1 }; // 前一个记录
        if (*curr < *prev) { // 当前记录比前一个记录更小（比较关键字）
            ElemType pivot{ *curr }; // 复制待插入元素 p（相当于暂存到监视哨）
            *curr = *prev; // 让前一个记录向后移动
            for (--prev; prev >= array + start && pivot < *prev; --prev) { // 从后向前寻找插入位置
                *(prev + 1) = *prev; // 记录逐个向后移动，直到找到可以插入元素 p 的位置
            }
            *(prev + 1) = pivot; // 把元素 p 插入到正确的位置
        }
    }
}


#pragma once

template <class ElemType>
void BinaryInsertSort(ElemType* array, size_t start, size_t end) {
    // 折半插入排序
    for (size_t i{ start + 1 }; i < end; ++i) { // 从第二个元素开始排序
        if (array[i] >= array[i - 1]) { // 当前记录不小于前一个记录，不需要移动，直接放在有序子表后面
            continue;
        }
        ElemType* left{ array + start }; // 初始化查找区间 [lo..hi]
        ElemType* right{ array + i - 1 };
        ElemType pivot{ array[i] }; // 复制待插入元素 pivot（相当于暂存到监视哨）
        while (left <= right) { // 在 [lo..hi] 中折半查找插入的位置
            ElemType* middle{ left + (right - left) / 2 }; // md 将 [lo..hi] 一分为二
            if (pivot < *middle) { // 插入点在左半段
                right = middle - 1; // 更新右端点
            }
            else { // 插入点在右半段
                left = middle + 1; // 更新左端点
            }
        }
        for (ElemType* j{ array + i - 1 }; j >= right + 1; --j) {
            *(j + 1) = *j; // 记录逐个向后移动，插入到正确的位置
        }
        *(right + 1) = pivot; // 插入元素 pivot
    }
}

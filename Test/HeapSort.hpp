#pragma once

#include <initializer_list>
#include <stdexcept>
#include <iostream>
#include <concepts>
#include <functional>
#include <stack>
#include <forward_list>
#include <list>
#include <cassert>
#include <type_traits>
#include <cstddef>
#include <vector>

template <class E, std::strict_weak_order<E, E> Compare = std::less<E>>
struct HeapSort {
    // 当 Compare 为 std::less<E> 时，建成大根堆（默认如此）
    // 当 Compare 为 std::greater<E> 时，建成小根堆
    HeapSort(std::vector<E> list) : heap{ std::move(list)}, r(*this) {
        // 堆排序（大根堆）的步骤：
        // 1. 按堆的定义，将待排序序列 r[1..n] 调整为大根堆（这个过程称为建初堆），交换 r[1] 和 r[n]，后者是关键字最大的记录
        // 2. 将 r[1..n-1] 重新调整为大根堆，交换 r[1] 和 r[n-1]，后者为关键字次大的记录
        // 3. 循环 n-1 次，直到交换了 r[1] 和 r[2] 为止，得到一个非递减有序序列 r[1..n]
        Sort();
    }
    auto begin() { return heap.begin(); }
    auto end() { return heap.end(); }
private:
    Compare compare{};
    std::vector<E> heap;
    struct agent {
        std::vector<E>& refHeap;
        agent(HeapSort& parent) : refHeap{ parent.heap } {}
        E& operator[](int s) { return refHeap[s - 1]; }
        auto size() const { return refHeap.size(); }
    } r;
    void Adjust(int s /* 根结点序号 */, int m) { // 筛选法调整堆
        E rc{ r[s] }; // 保存根结点
        for (int j = 2 * s; j <= m; j *= 2) { // 沿关键字较大的孩子结点向下筛选
            if (j < m && compare(r[j], r[j + 1])) // 从左右孩子中选出关键字较大者
                ++j; // j 为关键字较大的记录的下标
            if (!compare(rc, r[j])) // (rc >= r[j])
                // 以 r[s] 为根的子树已经是堆，不必做任何调整
                break; // rc 应插入在位置 s 上
            r[s] = r[j]; s = j; // 交换 r[s] 和 r[j]
            // 交换后，以 r[2s+1] 为根的子树仍是堆
            // 如果以 r[2s] 为根的子树不是堆，则重复以上过程
            // 将以 r[2s] 为根的子树调整为堆，直至进行到叶子结点为止
        }
        r[s] = rc;
    }
    void Setup() { // 建初堆
        auto n = r.size();
        for (auto i = n / 2; i > 0; --i) Adjust(i, n);
    }
    void Sort() { // 堆排序
        Setup();
        auto n = r.size();
        for (auto i = n; i > 1; --i) {
            auto x = r[1]; r[1] = r[i]; r[i] = x; // 将堆顶记录与当前未经排序子序列 r[1..i] 中最后一个记录互换
            Adjust(1, i - 1); // 将 r[1..i-1] 重新调整为大根堆
        }
    }
};

void TestHeapSort() {
    std::vector<int> hs0{ 49,38,65,97,76,13,27,49 };
    for (const auto x : hs0) {
        std::cout << x << '\n';
    }

    std::cout << "--------------\n";

    HeapSort hs1(hs0);
    for (const auto x : hs1) {
        std::cout << x << '\n';
    }

    std::cout << "--------------\n";

    HeapSort<int, std::greater<int>> hs2(hs0);
    for (const auto x : hs2) {
        std::cout << x << '\n';
    }
}

#pragma once

#include <cstring>
#include "HuffmanTree.hpp"

template <typename ElemType>
char** CreateHuffmanCode(const HuffmanTree<ElemType> ht, int n) {
    char** hc{ new char* [n + 1] }; // 分配存储 n 个字符编码的编码表空间
    char* cd{ new char[n] {0} }; // 分配临时存放每个字符编码的动态数组空间
    cd[n - 1] = '\0'; // 编码结束符
    for (int i = 1; i <= n; ++i) { // 逐个字符求哈夫曼编码
        int start{ n - 1 }; // start 开始时指向最后，即编码结束符位置
        int c{ i };
        int f{ ht[i].parent }; // f 指向结点 c 的双亲结点
        while (f != 0) {
            --start; // 回溯一次，start 向前指一个位置
            if (ht[f].lChild == c) {
                cd[start] = '0'; // 结点 c 是 f 的左孩子，生成代码 0
            }
            else {
                cd[start] = '1'; // 结点 c 是 f 的右孩子，生成代码 1
            }
            c = f;
            f = ht[f].parent; // 继续向上回溯
        }
        hc[i] = new char[n - start] {0};
        const char* src{ &cd[start] };
        const int len{ static_cast<int>(std::strlen(src)) };
        std::memcpy(hc[i], src, len);
    }
    delete[] cd;
    return hc;
}

#ifdef DEBUG
#include <iostream>

void TestHuffmanCode() {
    const int weights[]{ 5, 29, 7, 8, 14, 23, 3, 11 };
    const int n{ sizeof(weights) / sizeof(weights[0]) };
    HuffmanTree<int> ht{ CreateHuffmanTree(weights, n) };
    const int m{ 2 * n - 1 };
    std::cout << "哈夫曼树的带权路径长度 WPL=" << GetWeightedPathLength(ht, m) << std::endl;
    ht[0].data = 0; // 方便层次遍历
    TEST_TRAVERSE("哈夫曼树的先序序列：", ht, m, TraversePrOrderR);
    TEST_TRAVERSE("哈夫曼树的中序序列：", ht, m, TraverseInOrderR);
    TEST_TRAVERSE("哈夫曼树的后序序列：", ht, m, TraversePsOrderR);
    TEST_TRAVERSE("哈夫曼树的层次序列：", ht, m, TraverseTlOrderNR);

    char** hc{ CreateHuffmanCode(ht, n) };
    std::cout << "编码表：\n";
    for (int i = 1; i <= n; ++i) {
        std::cout << "hc[" << i << "]=" << hc[i] << '\0' << std::endl;
        delete hc[i];
    }
    delete[] hc;
    delete ht;

    return 0;
}

#endif

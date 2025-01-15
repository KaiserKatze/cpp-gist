#if 0

#include <iostream>
#include "HuffmanTree.h"

static int test() {
    const int weights[]{ 5,29,7,8,14,23,3,11 };

    const int n{ sizeof(weights) / sizeof(int) };
    auto* result{ CreateHuffmanTree(weights, n) };
    const int m{ 2 * n - 1 };
    TEST_TRAVERSE("哈夫曼树的中序序列：", result, m, TraverseInOrderR);
    TEST_TRAVERSE("哈夫曼树的先序序列：", result, m, TraversePrOrderR);

    std::cout << "哈夫曼树的带权路径长度 WPL=" << GetWeightedPathLength(result, m);

    delete result;

    return 0;
}

#endif
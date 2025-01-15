#if 0
#include <iostream>
#include "HuffmanCode.h"


int main() {
    const int weights[]{ 5, 29, 7, 8, 14, 23, 3, 11 };
    const int n{ sizeof(weights) / sizeof(weights[0]) };
    HuffmanTree<int> ht{ CreateHuffmanTree(weights, n) };
    const int m{ 2 * n - 1 };
    std::cout << "¹þ·òÂüÊ÷µÄ´øÈ¨Â·¾¶³¤¶È WPL=" << GetWeightedPathLength(ht, m) << std::endl;
    ht[0].data = 0; // ·½±ã²ã´Î±éÀú
    TEST_TRAVERSE("¹þ·òÂüÊ÷µÄÏÈÐòÐòÁÐ£º", ht, m, TraversePrOrderR);
    TEST_TRAVERSE("¹þ·òÂüÊ÷µÄÖÐÐòÐòÁÐ£º", ht, m, TraverseInOrderR);
    TEST_TRAVERSE("¹þ·òÂüÊ÷µÄºóÐòÐòÁÐ£º", ht, m, TraversePsOrderR);
    TEST_TRAVERSE("¹þ·òÂüÊ÷µÄ²ã´ÎÐòÁÐ£º", ht, m, TraverseTlOrderNR);

    char** hc{ CreateHuffmanCode(ht, n) };
    std::cout << "±àÂë±í£º\n";
    for (int i = 1; i <= n; ++i) {
        std::cout << "hc[" << i << "]=" << hc[i] << '\0' << std::endl;
        delete hc[i];
    }
    delete[] hc;
    delete ht;

    return 0;
}

#endif
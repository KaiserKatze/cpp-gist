#include "HuffmanTree.hpp"
#include <iostream>
#include <sstream>
#include <stack>
#include <queue>
#include <type_traits>

using WeightType = int;
static_assert(std::is_floating_point_v<WeightType> || std::is_integral_v<WeightType>); // 权重类型必须是数字类型

void TestHuffmanTree() {
    WeightType weight[]{ 5,29,7,8,14,23,3,11 };
    auto n{ sizeof(weight) / sizeof(weight[0]) };
    auto huffman_tree{ CreateHuffmanTree(weight, n) };
    auto idx_root_node{ 2 * n - 1 };
    std::cout
        << "Total length of input weight array: " << n << "\n"
        << "Traverse Huffman Tree from ht[" << idx_root_node << "]:\n\t";
    TraverseInOrderR(huffman_tree, idx_root_node, [](const auto& x) {
        if (x.is_leaf()) {
            std::cout << "<" << x.data << ">,";
        }
        else {
            std::cout << x.data << ",";
        }
    });
    std::cout << std::endl;
    // 计算各结点的深度
    auto depth{ new DepthType[2 * n] };
    for (size_t i{ 1 }; i < 2 * n; ++i) { // 初始化深度
        depth[i] = 0;
    }
    for (size_t i{ n + 1 }; i < 2 * n; ++i) {
        const auto& node{ huffman_tree[i] };
        // 中序遍历 node 对应的子树，让其中的每一个结点的深度加一
        TraverseInOrderR(huffman_tree, i, [&depth](const auto& x) {
            ++depth[x.lChild];
            ++depth[x.rChild];
        });
        const auto depth_left{ depth[node.lChild] };
        const auto depth_right{ depth[node.rChild] };
        depth[i] = (depth_left > depth_right ? depth_left : depth_right) + 1;
    }
    std::cout << "Depth of Huffman Tree: " << depth[idx_root_node] << "\n";
    std::cout << "Depth of each node:\n";
    for (size_t i{ 1 }; i < 2 * n; ++i) {
        std::cout << "\t{ weight: " << huffman_tree[i].data << ", depth: " << depth[i] << " },\n";
    }
    std::cout << std::endl;
    // 计算带权路径长度
    //WeightType WPL{ 0 };
    //std::stringstream ss;
    //for (size_t i{ 1 }; i <= n; ++i) {
    //    const auto& node{ huffman_tree[i] };
    //    const auto& depth_node{ depth[i] };
    //    WPL += node.data * depth_node;

    //    if (i > 1) {
    //        ss << "+";
    //    }
    //    ss << node.data << "×" << depth_node;
    //}
    auto WPL{ GetWeightedPathLength(huffman_tree, idx_root_node) };
    std::cout << "Weighted path length: " << WPL
        //<< " = " << ss.str()
        << "\n";
}
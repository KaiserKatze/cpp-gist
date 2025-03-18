#pragma once

#ifdef DEBUG
#   include <iostream>
#endif
#include <iterator> // std::distance
#include <vector>

// data structures

using DepthType = size_t;
static_assert(std::is_integral_v<DepthType>); // 深度类型必须是整数类型

template <typename WeightType>
struct HTNode { /* 哈夫曼树的结点 */
    WeightType data; /* 权值 */
    size_t parent;
    size_t lChild;
    size_t rChild;

    bool operator<(const HTNode& other) const {
        return data < other.data;
    }
    bool is_leaf() const {
        return lChild == rChild && rChild == 0;
    }
};

template <typename WeightType>
using HuffmanTree = HTNode<WeightType>*;

// declartion

template <typename WeightType>
HuffmanTree<WeightType> CreateHuffmanTree(const WeightType*, size_t);

template <typename WeightType, typename Callable>
void TraverseInOrderR(HuffmanTree<WeightType>, size_t, Callable);

template <typename WeightType, typename Callable>
void TraversePrOrderR(HuffmanTree<WeightType>, size_t, Callable);

template <typename WeightType>
WeightType GetWeightedPathLength(const HuffmanTree<WeightType>, size_t);

// definition

template <typename WeightType>
void Select(HuffmanTree<WeightType> ht, size_t range, size_t& a, size_t& b) {
    // 在 ht[k] (1<=k<=range) 中选择两个结点
    // 这两个结点满足：
    // - 其双亲域为 0
    // - 其权值最小
    // 返回这两个结点的序号
    a = b = 0;
    HTNode<WeightType>* d1{ nullptr }, * d2{ nullptr };
    for (size_t i{ 1 }; i <= range; ++i) {
        HTNode<WeightType>& node{ ht[i] };
        if (node.parent != 0) continue; // 该结点已有双亲，跳过
        if (d1 == nullptr || node < *d1) { // 当 d1 是空指针时，或当 node 比 d1 的权值更小时，移动 d1 和 d2 指针
            d2 = d1;
            d1 = &node;
        }
        else if (d2 == nullptr || node < *d2) { // 当 d2 是空指针时，或当 node 比 d2 的权值更小时，移动 d2 指针
            d2 = &node;
        }
    }
    a = d1 - ht;
    b = d2 - ht;
#ifdef DEBUG
    std::cout << "Nodes with the smallest weight:\n"
        << "\t#1: ht[" << a << "]=" << d1->data << "\n"
        << "\t#2: ht[" << b << "]=" << d2->data << "\n";
#endif
}

template <typename WeightType>
HuffmanTree<WeightType> CreateHuffmanTree(const WeightType* weights /* 权值数组 */, size_t n /* 棵数 */) {
    // 构造哈夫曼树
    if (n <= 1) return nullptr;
    const auto capacity{ 2 * n };
    HuffmanTree<WeightType> ht{ new HTNode<WeightType>[capacity] }; // 用 `ht[m]` 表示根结点
    for (size_t i{ 0 }; i < capacity; ++i) { // 初始化
        auto& node{ ht[i] };
        node.parent = 0;
        node.lChild = 0;
        node.rChild = 0;
    }
    for (size_t i{ 1 }; i <= n; ++i) { // 输入前 n 个单元中叶子结点的权值
        ht[i].data = weights[i - 1];
    } // 区间 [1..n] 内的结点是初始结点
    for (size_t i{ n + 1 }; i < capacity; ++i) {
        size_t s1{}, s2{}; // 权值最小的两个结点的序号
        Select(ht, i - 1, s1, s2);
        // 让权值最小的两个结点（第 s1 个结点、第 s2 个结点）指向第 i 个结点，把第 i 个结点作为亲结点
        ht[s1].parent = i;
        ht[s2].parent = i;
        // 设定第 i 个结点的左右孩子
        auto& parent{ ht[i] };
        parent.lChild = s1;
        parent.rChild = s2;
        // 设定第 i 个结点的权值 = 左右孩子的权值之和
        parent.data = ht[s1].data + ht[s2].data;
#ifdef DEBUG
        std::cout << "Create parent node:\n"
            << "\t#0: ht[" << i << "]={ "
            << "data: " << parent.data << ", "
            << "lChild: " << parent.lChild << ", "
            << "rChild: " << parent.rChild
            << " }\n";
#endif
    }
    return ht;
}

template <typename WeightType, typename Callable>
void TraversePrOrderR(HuffmanTree<WeightType> tree, size_t idx, Callable callback) {
    // 先序遍历哈夫曼树的递归算法
    if (tree && idx > 0) {                                  // 若二叉树非空
        callback(tree[idx]);                                // 访问根结点
        TraversePrOrderR(tree, tree[idx].lChild, callback); // 先序遍历左子树
        TraversePrOrderR(tree, tree[idx].rChild, callback); // 先序遍历右子树
    }
}

template <typename WeightType, typename Callable>
void TraverseInOrderR(HuffmanTree<WeightType> tree, size_t idx, Callable callback) {
    // 中序遍历哈夫曼树的递归算法
    if (tree && idx > 0) {                                         // 若二叉树非空
        TraverseInOrderR(tree, tree[idx].lChild, callback); // 中序遍历左子树
        callback(tree[idx]);                           // 访问根结点
        TraverseInOrderR(tree, tree[idx].rChild, callback); // 中序遍历右子树
    }
}

template <typename WeightType, typename Callable>
void TraversePsOrderR(HuffmanTree<WeightType> tree, size_t idx, Callable callback) {
    // 后序遍历哈夫曼树的递归算法
    if (tree && idx > 0) {                                  // 若二叉树非空
        TraversePsOrderR(tree, tree[idx].lChild, callback); // 后序遍历左子树
        TraversePsOrderR(tree, tree[idx].rChild, callback); // 后序遍历右子树
        callback(tree[idx]);                                // 访问根结点
    }
}

//template <typename WeightType, typename Callable>
//void TraverseTlOrderNR(HuffmanTree<WeightType> tree, size_t idx, Callable callback) {
//    // 层次遍历哈夫曼树的非递归算法
//    if (!tree) return;
//    size_t qsize{ 1 << idx };
//    int* queue{ new int[qsize] }; int p{ 0 }, q{ 0 };
//    queue[q++] = idx;
//    while (p >= 0 && p < q && q <= qsize) {
//        int nodeIndex{ queue[p++] };
//        callback(tree[nodeIndex]);
//        int lChildIndex{ tree[nodeIndex].lChild };
//        int rChildIndex{ tree[nodeIndex].rChild };
//        if (lChildIndex > 0 && lChildIndex <= idx)
//            queue[q++] = lChildIndex;
//        if (rChildIndex > 0 && lChildIndex <= idx)
//            queue[q++] = rChildIndex;
//    }
//    delete[] queue;
//}

template <typename WeightType>
WeightType GetWeightedPathLength(const HuffmanTree<WeightType> tree, size_t idx) {
    WeightType sum{};
    std::vector<DepthType> depth(idx + 1, 0); // 存储各个结点的层数（根结点的层数记为壹）
    TraversePrOrderR(tree, idx, [&](auto& c) {
        const WeightType weight{ c.data }; // 权值
        const auto offset{ &c - tree }; // 结点的序号
        const DepthType length{ depth[offset] = depth[c.parent] + 1 }; // 结点的路径长度
        if (c.lChild == 0 && c.rChild == 0) { // 叶子结点
            sum += weight * (length - 1); // 加上当前叶子结点的带权路径长度
        }
    });
    return sum;
}

#if DEBUG

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

#include <sstream>
#include <stack>
#include <queue>
#include <type_traits>

struct TestHuffmanTree {
    using WeightType = int;
    static_assert(std::is_floating_point_v<WeightType> || std::is_integral_v<WeightType>); // 权重类型必须是数字类型

    void Test() {
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

        delete huffman_tree;
    }
};

#endif

//==================================
// 拓扑排序

#pragma once

#include <stack>
#include <vector>
#include "Graph.hpp"

template <Number V, Number E>
auto TopologicalSort(const AdjacencyListGraph<V, E>& graph) {
    // 对采用邻接表存储结构的有向图进行拓扑排序
    const size_t size{ graph.Size() }; // 图中顶点个数
    std::vector<size_t> topo(size, -1); // 存储输出结果
    std::vector<size_t> inDegree(size, 0); // 存储各顶点的入度
    for (const auto& vnode : graph.vertices) { // 计算各顶点的入度
        for (const auto& enode : vnode.arcs) {
            inDegree[enode.adjvex]++;
        }
    }
    std::stack<size_t, std::list<size_t>> s; // 栈
    for (size_t i = 0; i < size; ++i) {
        if (inDegree[i] == 0) {
            s.push(i); // 入度为零的顶点入栈
        }
    }
    size_t m{ 0 }; // 对输出顶点计数，初始为零
    while (!s.empty()) {
        size_t i{ s.top() }; s.pop(); // 栈顶顶点 vi 出栈
        topo[m++] = i; // 将 vi 保存在拓扑序列中，计数加一
        for (const auto& enode : graph.vertices[i].arcs) {
            const size_t k{ enode.adjvex }; // vk 是 vi 的邻接点
            --inDegree[k]; // vi 的每个邻接点的入度减一
            if (inDegree[k] == 0) {// 若 vk 的入度减为零
                s.push(k); // 则让 vk 入栈
            }
        }
    }
    if (m < size) {
        topo.clear();
        return std::pair{ false, topo };
    }
    return std::pair{ true, topo };
}

void TestTopoSort() {
    AdjacencyListGraph<int, int> graph(6, true);
    for (size_t i = 0; i < 6; ++i)
        graph[i] = i + 1;
    graph.Connect({
        {0, 1, 2}, // a
        {0, 2, 5}, // b
        {1, 2, 1}, // c
        {1, 3, 3}, // d
        {2, 3, 3}, // e
        {2, 4, 4}, // f
        {2, 5, 1}, // g
        {3, 4, 1}, // h
        {3, 5, 4}, // i
        {4, 5, 1}, // j
        });

    {
        auto [status, topo] { TopologicalSort(graph) };
        std::cout << "LOG <<< 是否有环：" << std::boolalpha << !status << ".\n";
    }
}

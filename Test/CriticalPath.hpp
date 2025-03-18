//==================================
// 关键路径

#pragma once

#include <vector>
#include "Graph.hpp"
#include "TopoSort.hpp"

template <Number V, Number E>
auto CriticalPath(const AdjacencyListGraph<V, E>& graph) {
    // 在采用邻接表存储结构的有向图中找出关键路径
    std::vector<std::pair<size_t, size_t>> criticalPath; // 关键路径（邻接顶点对）
    auto [status, topo] { TopologicalSort(graph) }; // 可能在这里抛出异常（有向图有回路）
    if (!status) return std::pair{ false, criticalPath }; // 不是有向无环图，退出
    const size_t size{ graph.Size() };
    std::vector<E> ve(size, 0); // 事件的最早发生时间
    for (size_t i = 0; i < size; ++i) { // 正向拓扑排序
        const auto k{ topo[i] }; // 取得拓扑序列中的顶点序号 k
        for (const auto& enode : graph.vertices[k].arcs) {
            const auto j{ enode.adjvex }; // 邻接顶点序号
            const E weight{ enode.info }; // 边权
            ve[j] = std::max(ve[j], ve[k] + weight); // 更新顶点 j 的最早发生时间（取最大值）
        }
    }
    std::vector<E> vl(size, ve[size - 1]); // 事件的最迟发生时间
    for (size_t i = size; i > 0;) { // 反向拓扑排序
        --i;
        const auto k{ topo[i] }; // 取得拓扑序列中的顶点序号 k
        for (const auto& enode : graph.vertices[k].arcs) {
            const auto j{ enode.adjvex }; // 邻接顶点序号
            const E weight{ enode.info }; // 边权
            vl[k] = std::min(vl[k], vl[j] - weight); // 更新顶点 k 的最迟发生时间（取最小值）
        }
    }
    for (size_t i = 0; i < size; ++i) { // 计算活动的最早发生时间和最晚发生时间
        // 活动的最早发生时间是它的弧尾事件的最早发生时间
        // 活动的最晚发生时间是它的弧头事件的最晚发生时间，减去活动的持续时间（边权）
        for (const auto& enode : graph.vertices[i].arcs) {
            const auto j{ enode.adjvex }; // 邻接顶点序号
            const E weight{ enode.info }; // 边权
            const E e{ ve[i] }; // 活动 <vi,vj> 的最早开始时间
            const E l{ vl[j] - weight }; // 活动 <vi,vj> 的最迟开始时间
            if (e == l) // 如果`最早开始时间`等于`最迟开始时间`
                criticalPath.emplace_back(i, j); // 那么活动 <vi,vj> 就是关键活动
        }
    }
#ifdef DEBUG
    std::cout << "LOG <<< 事件的最早发生时间：\n";
    for (size_t i = 0; i < size; ++i) {
        std::cout << "\tve[" << i << "] = " << ve[i] << ".\n";
    }
    std::cout << "LOG <<< 事件的最迟发生时间：\n";
    for (size_t i = 0; i < size; ++i) {
        std::cout << "\tvl[" << i << "] = " << vl[i] << ".\n";
    }
#endif
    return std::pair{ true, criticalPath };
}

void TestCriticalPath() {
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
        auto [status, cp] { CriticalPath(graph) };
        if (status) {
            std::cout << "LOG <<< 关键路径为：\n";
            for (const auto& [v1, v2] : cp) {
                std::cout << "\t{ " << v1 << " -> " << v2 << " }.\n";
            }
        }
    }
}

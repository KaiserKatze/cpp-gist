#pragma once

#include <iostream>
#include "Graph.hpp"

template <Graph G>
struct MinimalSpanningTree_Kruskal {
    // 时间复杂度：O(e log(e))
    // 时间复杂度只与边数 e 有关，适合于顶点数较多、边数较少的图

    using ArcType = typename G::ArcType;
    using VexType = typename G::VexType;
    static constexpr ArcType arcInfinity{ std::numeric_limits<ArcType>::max() }; // 用某个数字来表示两个顶点之间的边权是无穷大，表示两个顶点之间没有边

    MinimalSpanningTree_Kruskal(const G& graph /* 邻接表表示 */) {
        const int size{ graph.Size() }; // 图中顶点个数
        vexset.resize(size);
        for (int i = 0; i < size; ++i) {
            vexset[i] = i; // 初始化，各顶点自成一个连通分量
        }
        edges.resize(size);
        graph.arcs.Replace(0, arcInfinity);
    }
    // TODO
private:
    struct Edge {
        VexType head; // 边的起点
        VexType tail; // 边的终点
        ArcType cost; // 边权
    };
    std::vector<Edge> edges; // 记录从 U 到 V-U 具有最小权值的边
    std::vector<int> vexset; // 记录各个顶点所属的连通分量
};

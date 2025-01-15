#pragma once

#include <iostream>
#include "Graph.hpp"


template <Graph G>
struct MinimalSpanningTree_Prim {
    // 时间复杂度：O(n^2)
    // 时间复杂度只与顶点数 n 有关，适合于边数较多、顶点数较少的图

    using ArcType = typename G::ArcType;
    using ArcTuple = std::tuple<size_t, size_t, ArcType>;
private:
    struct Cost {
        size_t adjvex; // 跨越 U 和 V-U 两个集合的最小边，在 U 中对应的顶点的序号
        ArcType lowcost; // 最小边上的权值（如果顶点已在集合 U 中，等于 0；否则大于 0）
    };

    std::vector<Cost> edges; // 记录从 U 到 V-U 具有最小权值的边
    static constexpr ArcType arcInfinity{ std::numeric_limits<ArcType>::max() }; // 用某个数字来表示两个顶点之间的边权是无穷大，表示两个顶点之间没有边
    std::vector<ArcTuple> mst; // 最小生成树

    size_t minCost() const {
        ArcType min_cost{ arcInfinity };
        size_t min_element{ 0 };
        for (size_t p{ 0 }; const auto& edge : edges) {
            if (edge.lowcost > 0 && edge.lowcost < min_cost) {
                min_cost = edge.lowcost;
                min_element = p;
            }
            ++p;
        }
        return min_element;
    }

public:
    MinimalSpanningTree_Prim(G& graph /* 邻接矩阵表示 */, const size_t v0 /* 初始顶点序号 */) {
        // 从顶点 `v0` 出发，构造无向网 `graph` 的最小生成树 `T`
        const size_t size{ graph.Size() }; // 图中顶点个数
        mst.reserve(size); // 初始化空树（输出结果）
        edges.resize(size); // 初始化（辅助数据结构）
        // 初始化 `edges`
        {
            const auto& row{ graph.arcs[v0] }; // 边表的第 `v0` 行，即第 `v0` 个顶点与其余各个顶点之间的边
            for (size_t j = 0; j < size; ++j) {
                edges[j] = { v0, row[j] };
            }
            edges[v0] = { v0, 0 }; // 一开始有 $U = \{ v0 \}$
        }

        for (size_t v = 0; v < size; ++v) {
            if (v == v0) continue; // 选择其余 n-1 个顶点，生成 n-1 条边（其中 n 是顶点总数）
            const size_t v1 = minCost(); // 从跨越 $U$ 和 $V-U$ 的各组边中，找出最小边 `edges[v1]`
#ifdef DEBUG
            // 打印 `edges`
            std::cout << "LOG <<< 第 [" << v << "] 轮 `edges`：\n";
            for (size_t j = 0; j < size; ++j) {
                auto [adjvex, lowcost] = edges[j];
                std::cout << "LOG <<< \t[" << j << "] = { adjvex: " << adjvex << ", lowcost: ";
                if (lowcost == arcInfinity) std::cout << "∞";
                else                        std::cout << lowcost;
                std::cout << " },\n";
            }
            // 打印刚刚选定的边
            auto u0n{ graph.vertices[edges[v1].adjvex] }; // u0n 是最小边的属于 U 的那个顶点的名称
            auto u1n{ graph.vertices[v1] }; // u1n 是最小边的属于 V-U 的那个顶点的名称
            std::cout << "LOG <<< 选中的最小边为： arc{ " << u0n << " -- " << u1n << " }，"
                "边权为：" << edges[v1].lowcost << ";\t<------------\n"; // 打印
#endif
            mst.push_back({ edges[v1].adjvex, v1, edges[v1].lowcost }); // 写入最小生成树
            edges[v1] = { v1, 0 }; // 第 v1 个顶点并入 U，即 $U := U \cup \{v1\}$
#ifdef DEBUG
            std::cout << "LOG <<< 更新路径长度：\n";
#endif
            for (size_t j = 0; j < size; ++j) {
                if (edges[j].lowcost == 0) {
#ifdef DEBUG
                    std::cout << "LOG <<< \t第 [" << j << "] 个顶点已并入 U 不更新路径长度.\n";
#endif
                    continue; // 已经并入 $U$ 中的点，不更新路径长度
                }
                const ArcType w1{ graph.arcs[v1][j] }; // 第 `v1` 个顶点与第 `j` 个顶点之间的边权
                const ArcType w2{ edges[j].lowcost }; // 第 `j` 个顶点与第 `edges[j].adjvex` 个顶点之间的边权
#ifdef DEBUG
                std::cout << "LOG <<< \t第 [" << j << "] 个顶点备选路径长度为 { " << w1 << ", " << w2 << " }.\n";
#endif
                // 新顶点并入 U 后，重新选择最小边
                if (w1 < w2) edges[j] = { v1, w1 };
            }
        }
    }

    const auto& GetTree() const {
        return mst;
    }
};

void Test() {
    AdjacencyMatrixGraph<int, int> graph(6, false);
    for (size_t i = 0; i < 6; ++i)
        graph[i] = i + 1;
    graph.Connect({
        {0, 1, 6},
        {0, 2, 1},
        {0, 3, 5},
        {1, 2, 5},
        {1, 4, 3},
        {2, 3, 5},
        {2, 4, 6},
        {2, 5, 4},
        {3, 5, 2},
        {4, 5, 6}
    });

    MinimalSpanningTree_Prim mst(graph, 0);

    int total_weight{ 0 };
    for (const auto& [v1, v2, w12] : mst.GetTree()) {
        auto u1{ graph.vertices[v1] }; // u0n 是最小边的属于 U 的那个顶点的名称
        auto u2{ graph.vertices[v2] }; // u1n 是最小边的属于 V-U 的那个顶点的名称
        std::cout << "LOG <<< 选中的最小边为： arc{ " << u1 << " -- " << u2 << " }，"
            "边权为：" << w12 << ";\t<------------\n"; // 打印
        total_weight += w12;
    }
    std::cout << "LOG <<< 总边权：" << total_weight << ".\n";
}

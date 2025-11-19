#pragma once

#include <vector>
#include "Graph.hpp"
#include <iostream>

template <Graph G>
struct ShortestPath_Dijkstra {
    using ArcType = typename G::ArcType;
    static constexpr ArcType arcInfinity{ std::numeric_limits<ArcType>::max() }; // 用某个数字来表示两个顶点之间的边权是无穷大，表示两个顶点之间没有边
    std::vector<bool> s; // 记录从起点 v0 到终点 vi 是否已被确定最短路径长度，`true` 表示确定
    std::vector<size_t> path; // 记录从起点 v0 到终点 vi 的当前最短路径上 vi 的直接前驱顶点序号
    std::vector<ArcType> d; // 记录从起点 v0 到终点 vi 的当前最短路径长度
    ShortestPath_Dijkstra(G& graph /* 图的邻接矩阵表示 */, size_t v0 /* 源点 */) {
        const size_t size{ graph.Size() }; // 图中顶点个数
        static constexpr auto max_path{ std::numeric_limits<typename decltype(path)::value_type>::max() };
        // 检查 graph 是否带有负边权（一般来说，Dijkstra 算法不适用于带有负边权的图，但是少数情况下仍旧可用，因此这里只是发出警告）
        for (const auto& row : graph.arcs.data) {
            for (const auto entry : row) {
                if (entry < 0) {
                    std::cerr << "ERR <<< 图中有负边权，可能无法使用 Dijkstra 算法求解最短路径！\n";
                }
            }
        }
        s.resize(size, false); // 把集合 S 初始化为空集
        s[v0] = true; // 将 v0 加入集合 S
        d.resize(size, arcInfinity); // 初始化路径长度
        path.resize(size, max_path); // 初始化最短路径
        for (size_t v = 0; v < size; ++v) {
            d[v] = graph.arcs[v0][v]; // v0 到各个终点的最短路径长度，初始化为边权
            if (d[v] < arcInfinity) path[v] = v0; // 如果 v0 和 v 之间有边，则 v 的前驱置为 v0
                                                  // 如果 v0 和 v 之间无边，则 v 的前驱置为空
        }
        d[v0] = 0; // 起点 v0 到它自己的距离为零
        // 初始化结束，开始主循环，每次求得 v0 到某个顶点 v 的最短路径，将 v 加入集合 S
        for (size_t i = 1 /* 这里 i 只起到计数作用，初始值就是 1，没有错，不要改 */; i < size; ++i) { // 对其余 n-1 个顶点，依次计算
            // 从集合 V-S 中找一个到 v0 的路径最短的点 v
            ArcType min_path{ arcInfinity }; // 路径最小权值
            size_t min_vertex{ max_path }; // 最小权值路径
            for (size_t w = 0; w < size; ++w) {
                if (!s[w] && d[w] < min_path) { // 若点 w 还不在终点集合 S 中，且 w 到 v0 的距离更短
                    min_vertex = w;
                    min_path = d[w];
                }
            }
            s[min_vertex] = true; // 将顶点 v 加入集合 S
#ifdef DEBUG // 打印辅助数据结构
            std::cout << "LOG <<< path[i=" << i << "]: ";
            for (auto prev_vertex : path) {
                if (prev_vertex == max_path) {
                    std::cout << R"(∞)" << " ";
                }
                else {
                    std::cout << prev_vertex << " ";
                }
            }
            std::cout << "\n";
            std::cout << "LOG <<< d[i=" << i << "]: ";
            for (const ArcType& weight : d) {
                if (weight == std::numeric_limits<ArcType>::max()) {
                    std::cout << R"(∞)" << " ";
                }
                else {
                    std::cout << weight << " ";
                }
            }
            std::cout << "\n";
            std::cout << "LOG <<< 选中的顶点为: v='" << graph[min_vertex] << "'(" << min_vertex << "), 对应的路径长度为: " << min_path << ".\n";
#endif
            // 更新从 v0 出发到集合 V-S 上所有顶点的最短路径长度
            for (size_t w = 0; w < size; ++w) {
                if (s[w]) continue; // 顶点 w 在集合 S 中，不在集合 V-S 中，跳过
                const ArcType& t1{ graph.arcs[min_vertex][w] }; // 顶点 v 到顶点 w 的边权
                ArcType t2{ t1 + d[min_vertex] }; // 从顶点 v0 出发，途径顶点 v，再到顶点 w 的路径的长度
                if constexpr (std::is_integral_v<ArcType>) { // 边权类型是整数，需要检查运算结果是否溢出
                    if (t2 <= t1) { // 期望的结果是 t2 > t1，t2 <= t1 说明运算结果溢出了！！！
                        t2 = arcInfinity; // 设为最大值
                    }
                }
                if (t2 < d[w]) { // 新的路径的长度小于之前记录的最短路径
                    d[w] = t2; // 更新最短路径
                    path[w] = min_vertex; // 更新前驱顶点
                }
            }
        }
    }
};

void TestDijkstra() {
    AdjacencyMatrixGraph<char, int> graph(6, false);
    for (size_t i = 0; i < 6; ++i)
        graph[i] = static_cast<char>('a' + i);
    graph.Connect({
        {0,1,2},
        {0,2,5},
        {1,2,1},
        {1,3,3},
        {2,3,3},
        {2,4,4},
        {3,4,1},
        {3,5,4},
        {4,5,1},
        {2,5,1},
        });

    ShortestPath_Dijkstra sp(graph, 0);
}

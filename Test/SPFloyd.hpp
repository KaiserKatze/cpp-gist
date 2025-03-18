#pragma once

#include <vector>
#include "Graph.hpp"

template <Graph G>
struct ShortestPath_Floyd {
    using ArcType = typename G::ArcType;
    static constexpr ArcType arcInfinity{ std::numeric_limits<ArcType>::max() }; // 用某个数字来表示两个顶点之间的边权是无穷大，表示两个顶点之间没有边
    ShortestPath_Floyd(const G& graph) {
        const int size{ graph.Size() }; // 图中顶点个数
        path.resize(size, std::vector(size, -1));
        d.resize(size, std::vector(size, arcInfinity));
        for (int i = 0; i < size; ++i)
            for (int j = 0; j < size; ++j)
                if (i != j && (d[i][j] = graph.arcs[i][j]) < arcInfinity)
                    path[i][j] = i; // 如果 i 和 j 之间有边，则将 j 的前驱置为 i
                else
                    path[i][j] = -1; // 如果 i 和 j 之间无边，则将 j 的前驱置为空
        for (int k = 0; k < size; ++k) // k 是中间点
            for (int i = 0; i < size; ++i) {
                if (i == k) continue;
                for (int j = 0; j < size; ++j) {
                    if (j == k) continue;
                    if (i == j) continue;
                    auto d_ik{ d[i][k] };
                    auto d_kj{ d[k][j] };
                    auto d_ij{ d[i][j] };
                    auto d_ik_kj{ d_ik + d_kj };
                    if (d_ik_kj < 0) d_ik_kj = arcInfinity; // 处理 arithemtic overflow
                    if (d_ik_kj < d_ij) {
                        d[i][j] = d_ik_kj; // 更新距离
                        path[i][j] = path[k][j]; // 把 j 的前驱改为 k
                    }
                }
            }
    }
    std::pair<ArcType, std::vector<int>> GetShortestPath(int vi, int vj) {
        ArcType distanceBetweenViAndVj{ d[vi][vj] };
        std::forward_list<int> pathBetweenViAndVj{ vj };
        for (int prev{ path[vi][vj] }; prev != vi; prev = path[vi][prev])
            pathBetweenViAndVj.push_front(prev);
        pathBetweenViAndVj.push_front(vi);
        return {
            distanceBetweenViAndVj,
            {
                pathBetweenViAndVj.cbegin(),
                pathBetweenViAndVj.cend()
            }
        };
    }
private:
    std::vector<std::vector<int>> path; // `path[i][j]` 记录最短路径上顶点 $v_j$ 的前一个顶点的序号
    std::vector<std::vector<ArcType>> d; // 记录顶点 $v_i$ 和 $v_j$ 之间的最短路径长度
};
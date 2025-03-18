#pragma once
#include "Graph.hpp"
#include <vector>

template <Graph G>
struct DepthFirstSearch { // 深度优先搜索
    DepthFirstSearch(G& g) : g{ g }, size{ g.Size() }, visited(size, false) {
    }
    template <typename Callable>
    void Traverse(int v /* 顶点序号 */, Callable callback) { // 从某个指定顶点开始，遍历连通图
        if (!(0 <= v && v < size)) throw "out of range";
        callback(g[v]); visited[v] = true; // 访问第 v 个顶点，置为已访问
        for (int w = FirstAdjacentVertex(g, v); w >= 0;
            w = NextAdjacentVertex(g, v, w))
            if (!visited[w])
                Traverse(w, callback); // 对 v 的尚未访问的邻接顶点 w 递归调用 DFS
    }
    template <typename Callable>
    void Traverse(Callable callback) { // 遍历非连通图
        for (int i = 0; i < size; ++i) if (!visited[i]) Traverse(i, callback);
    }
private:
    std::vector<bool> visited; // 顶点是否已访问的标记
    G& g; // 图的引用
    int size; // 图中顶点数
};
/*
 * 深度优先搜索遍历算法在遍历图时，对图中每个顶点，至多调用一次 DFS 函数。
 * 因此，遍历图的过程实质上是对每个顶点查找其邻接点的过程，其耗费的时间取决于图的存储结构。
 * 用邻接矩阵表示图时，查找每个顶点的邻接点的时间复杂度为 $O(n^2)$。
 * 用邻接表表示图时，时间复杂度为 $O(n+e)$。
 */

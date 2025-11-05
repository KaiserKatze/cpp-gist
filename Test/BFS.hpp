#pragma once
#include "Graph.hpp"
#include <queue>
#include <vector>

template <Graph G>
struct BreadthFirstSearch { // 广度优先搜索
    BreadthFirstSearch(G& g) : g{ g }, size{ g.Size() }, visited(size, false) {
    }
    template <typename Callable>
    void Traverse(size_t v /* 顶点序号 */, Callable callback) { // 从某个指定顶点开始，遍历连通图
        if (!(0 <= v && v < size)) throw "out of range";
        callback(g[v]); visited[v] = true; // 访问第 v 个顶点，置为已访问
        std::queue<int, std::list<int>> q; // 创建队列
        q.push(v); // 顶点 v 入队
        while (!q.empty()) { // 队列非空
            int u{ q.front() }; q.pop(); // 队头元素出队
            for (int w = FirstAdjacentVertex(g, u); w >= 0;
                w = NextAdjacentVertex(g, u, w))
                if (!visited[w]) {
                    callback(g[w]); visited[w] = true; // 访问第 w 个顶点，置为已访问
                    q.push(w);
                }
        }
    }
private:
    std::vector<bool> visited; // 顶点是否已访问的标记
    G& g; // 图的引用
    size_t size; // 图中顶点数
};
/*
 * 广度优先搜索遍历算法在遍历图时，每个顶点至多进一次队列。
 * 遍历图的过程实质上是通过边找邻接点的过程，
 * 因此广度优先搜索的时间复杂度和深度优先搜索相同，
 * 两种遍历方法的不同之处仅仅在于对顶点访问的顺序不同。
 */

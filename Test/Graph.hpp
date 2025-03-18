#pragma once
#ifndef GRAPH_H
#define GRAPH_H

#include "Matrix.hpp"
#include <vector>
#include <list>
#include <forward_list>
#include <queue>
#include <type_traits>
#include <algorithm> // std::min
#include <ranges>
#include <iostream>
#include <utility>
#include <memory>
#include <stack>

struct AbstractGraph {
    virtual size_t Size() const = 0; // 返回图中顶点个数
};

template <class G>
concept Graph = std::is_base_of_v<AbstractGraph, G>;

template <class T>
concept Number = std::is_arithmetic_v<T>;

template <Number V, Number E>
struct BaseGraph : AbstractGraph {
    using VexType = V;
    using ArcType = E;
};

template <Number V, Number E, E Infinity = std::numeric_limits<E>::max()>
struct AdjacencyMatrixGraph : BaseGraph<V, E> { // 用邻接矩阵表示的图
    std::vector<V> vertices; // 顶点表
    Matrix<E> arcs; // 边表
    size_t nArcs; // 边数
    const bool directed; // 是否有向图

    AdjacencyMatrixGraph(size_t nVertices /* 顶点个数 */, bool directed /* 是否有向图 */)
        : vertices(nVertices), arcs(nVertices, nVertices, Infinity), nArcs{ 0 }, directed{ directed } {
    }
    V& operator[](size_t index) {
        return vertices[index];
    }
    size_t Size() const { // 顶点数
        return vertices.size();
    }
    void Connect(size_t v1, size_t v2, E arcWeight) { // 构造新的边（弧）
        ++nArcs;
        arcs[v1][v2] = arcWeight;
        if (directed) return;
        arcs[v2][v1] = arcWeight;
    }
    void Connect(std::initializer_list<std::tuple<size_t, size_t, E>> data) {
        for (auto& [v1, v2, aw] : data) {
            Connect(v1, v2, aw);
        }
    }
};
/*
 * 邻接矩阵表示法的优点：
 * - 便于判断两个顶点之间有没有边
 * - 便于计算各个顶点的度（一行元素之和就是对应顶点的出度，一列元素之和就是对应顶点的入度）
 * 邻接矩阵表示法的缺点：
 * - 不便于增加和删除顶点
 * - 不便于统计边的数目（时间复杂度为 $O(n^2)$）
 * - 空间复杂度高（特别是对稀疏图而言）
 */

template <Number V, Number E>
struct AdjacencyListGraph : BaseGraph<V, E> { // 用邻接表表示的图
    // 为每个顶点建立一个单链表，与该顶点邻接的顶点都放在这个链表中
    // 链表的头结点中存放有关顶点的信息
    // 把这些头结点另行串成一个链表（称为表头结点表）
    struct ENode { // 边结点
        size_t adjvex; // 邻接点域（与顶点邻接的点在图中的位置）
        E info; // 数据域（存储和边相关的信息，如权值等）

        ENode() : adjvex{}, info{} {}
        ENode(size_t adjvex, E info) : adjvex{ adjvex }, info{ info } {}
    };
    struct VNode { // 顶点结点
        V data; // 数据域（存储顶点的名称或其他有关信息）
        std::forward_list<ENode> arcs; // 链域（与该顶点关联的所有边）

        VNode() : data{} {}
    };
    std::vector<VNode> vertices; // 顶点顺序表
    size_t nArcs; // 边数
    const bool directed; // 是否有向图

    AdjacencyListGraph(size_t nVertices /* 顶点个数 */, bool directed /* 是否有向图 */)
        : vertices(nVertices), nArcs{ 0 }, directed{ directed } {
    }
    V& operator[](size_t index) {
        return vertices[index].data;
    }
    size_t Size() const { // 顶点数
        return vertices.size();
    }
private:
    void _connect0(size_t v1, size_t v2, E arcWeight) {
        VNode& vnode1{ vertices[v1] };
        std::forward_list<ENode>& enodes1{ vnode1.arcs };
        enodes1.push_front({ v2, arcWeight });
    }
public:
    void Connect(size_t v1, size_t v2, E arcWeight) { // 构造新的边（弧）
        ++nArcs;
        _connect0(v1, v2, arcWeight);
        if (directed) return;
        _connect0(v2, v1, arcWeight);
    }
    void Connect(std::initializer_list<std::tuple<size_t, size_t, E>> data) {
        for (auto& [v1, v2, aw] : data) {
            Connect(v1, v2, aw);
        }
    }
};
/*
 * 邻接表表示法的优点：
 * - 适合用来表示稀疏图
 * - 便于增加和删除顶点
 * - 便于统计边的数目（时间复杂度为 $O(n+e)$）
 * - 空间效率高（空间复杂度为 $O(n+e)$）
 * 邻接表表示法的缺点：
 * - 不便于判断顶点之间有没有边（最坏情况下时间复杂度为 $O(n)$）
 * - 邻接表表示法不便于计算有向图中各个顶点的入度，逆邻接表表示法不便于计算有向图中各个顶点的出度
 */

template <Number V, Number E>
struct OrthogonalListGraph : BaseGraph<V, E> { // 用十字链表表示的图
    struct ArcBox { // 弧结点
        int tailvex; // 尾域（弧尾在图中的位置）
        int headvex; // 头域（弧头在图中的位置）
        ArcBox* hlink; // 链域（指向弧头相同的下一条弧）
        ArcBox* tlink; // 链域（指向弧尾相同的下一条弧）
        E info; // 弧的相关信息
    };
    struct VertexNode { // 顶点结点
        V data; // 数据域
        ArcBox* firstin; // 链域（指向以该顶点为弧头的第一个弧结点）
        ArcBox* firstout; // 链域（指向以该顶点为弧尾的第一个弧结点）
    };
    std::vector<VertexNode> vertices; // 表头向量
    int nArcs; // 边数

    V& operator[](int index) {
        if (0 <= index && index < vertices.Size())
            return vertices[index].data;
        throw "out of range";
    }
};
/*
 * 十字链表表示法可以看成是将有向图的邻接表和逆邻接表结合起来得到的一种链表
 */

template <Number V, Number E>
struct AdjacencyMultilistGraph : BaseGraph<V, E> { // 用邻接多重表表示的图
    // TODO
    struct EBox {
        bool mark; // 访问标记
        int ivex; // 该边依附的两个顶点的位置
        int jvex;
        EBox* ilink; // 分别指向依附这两个顶点的下一条边
        EBox* jlink;
        E info; // 边的信息
    };
    struct VBox {
        V data;
        EBox* firstedge; // 指向第一条依附该顶点的边
    };
    std::vector<VBox> list;
    int vexnum; // 顶点数
    int edgenum; // 边数
};

template <Number V, Number E>
int FirstAdjacentVertex(AdjacencyMatrixGraph<V, E>& g, int v) {
    auto n{ g.Size() };
    if (0 <= v && v < n)
        for (int i = 0; i < n; ++i)
            if (i != v && g.arcs[v][i] > 0) return i;
    return -1; // 找不到满足条件的邻接顶点了
}

template <Number V, Number E>
int NextAdjacentVertex(AdjacencyMatrixGraph<V, E>& g, int v, int w) {
    auto n{ g.Size() };
    if (0 <= v && v < n)
        for (int i = w + 1; i < n; ++i)
            if (i != v && g.arcs[v][i] > 0) return i;
    return -1; // 找不到满足条件的邻接顶点了
}

template <Number V, Number E>
int FirstAdjacentVertex(AdjacencyListGraph<V, E>& g, int v) {
    auto* p{ g[v].firstarc };
    if (p) return p->adjvex;
    return -1;
}

template <Number V, Number E>
int NextAdjacentVertex(AdjacencyListGraph<V, E>& g, int v, int w) {
    auto* p{ g[v].firstarc };
    while (p != nullptr && p->adjvex != w)
        p = p->nextarc;
    if (p != nullptr && p->adjvex == w && p->nextarc != nullptr)
        return p->nextarc->adjvex;
    return -1;
}

#endif

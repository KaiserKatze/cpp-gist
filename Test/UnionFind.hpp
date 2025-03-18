// 并查集
#pragma once

#include <vector>
#include <utility> // std::swap

struct UnionFind {
    // 并查集是一种树
    // @see: https://www.bilibili.com/video/BV1W34y1L734/

    UnionFind(int count /* 并查集结点个数 */)
        : fa(count, -1), size(count, 1) {
        // `fa` 初始化，让每个结点指向它自己
        for (int i = 0; i < count; i++) {
            fa[i] = i;
        }
    }

    void Unite(int p, int q) {
        // 合并：让一个集合的根指向另一个集合的根
        // 启发式合并/按秩合并
        p = Find(p);
        q = Find(q);
        if (p == q) return;
        if (size[p] > size[q]) std::swap(p, q);
        fa[p] = q;
        size[q] += size[p];
    }

    int Find(int p) {
        // 查找：确定某个元素所在的集合
        // 带路径压缩的查找的递归实现
        if (fa[p] == p) return p;
        return fa[p] = Find(fa[p]);
    }

    bool IsConnected(int p, int q) {
        return Find(p) == Find(q);
    }

private:
    std::vector<int> fa; // 存放结点 x 的父节点
    std::vector<int> size; // 记录各个集合的大小
};

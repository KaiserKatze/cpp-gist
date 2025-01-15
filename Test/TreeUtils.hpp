#pragma once

template <class NodeType>
int Depth(const NodeType* tree) {
    // 计算二叉树的深度
    if (!tree) return 0; // 空树深度为零
    int ld{ Depth(tree->lChild) };
    int rd{ Depth(tree->rChild) };
    if (ld > rd) return ld + 1; // 左子树更深
    else         return rd + 1; // 右子树更深
}

template <class NodeType>
int Count(const NodeType* tree) {
    // 统计二叉树中结点的个数
    if (!tree) return 0; // 空树的结点个数为零
    // 非空树的结点个数等于左、右子树结点个数之和再加一
    return Count(tree->lChild) + Count(tree->rChild) + 1;
}

template <typename NodeType>
int Count(const NodeType* tree, int degree) {
    // 分类统计二叉树中不同度的结点的个数（该算法本质上是先序遍历的递归算法的应用）
    if (!tree) return 0; // 空树的结点个数为零
    if (degree < 0 || degree > 2) return -1; // 期望的输入是 0,1,2，除此以外都返回 -1 表示异常
    int d{ 0 }; // 根结点的度
    const NodeType* p1{ tree->lChild }; // 左子树
    const NodeType* p2{ tree->rChild }; // 右子树
    if (p1) ++d;
    if (p2) ++d;
    int cnt = Count(p1, degree) + Count(p2, degree); // 左、右子树中度为 `degree` 的结点总数
    if (d == degree)
        return cnt + 1;
    return cnt;
}

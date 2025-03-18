#pragma once
#include <iostream>
#include <queue>
#include <list>
#include <stack>

template <typename NodeType, typename Callable>
void TraverseInOrderR(NodeType* tree, Callable callback) {
    // 中序遍历的递归算法
    if (tree) {                                   // 若二叉树非空
        TraverseInOrderR(tree->lChild, callback); // 中序遍历左子树
        callback(tree);                           // 访问根结点
        TraverseInOrderR(tree->rChild, callback); // 中序遍历右子树
    }
}

template <typename NodeType, typename Callable>
void TraversePrOrderR(NodeType* tree, Callable callback) {
    // 先序遍历的递归算法
    if (tree) {                                   // 若二叉树非空
        callback(tree);                           // 访问根结点
        TraversePrOrderR(tree->lChild, callback); // 先序遍历左子树
        TraversePrOrderR(tree->rChild, callback); // 先序遍历右子树
    }
}

template <typename NodeType, typename Callable>
void TraversePsOrderR(NodeType* tree, Callable callback) {
    // 后序遍历的递归算法
    if (tree) {                                   // 若二叉树非空
        TraversePsOrderR(tree->lChild, callback); // 后序遍历左子树
        TraversePsOrderR(tree->rChild, callback); // 后序遍历右子树
        callback(tree);                           // 访问根结点
    }
}

template <typename NodeType, typename Callable>
void TraverseInOrderNR(const NodeType* tree, Callable callback) {
    // 中序遍历的非递归算法
    if (!tree) return; // 若二叉树为空，立即返回
    std::stack<NodeType*> s; // 设置一个栈，存放根指针
    NodeType* p{ const_cast<NodeType*>(tree) }; // 取得根指针
    while (p || !s.IsEmpty()) {
        if (p) {                    // 如果 p 非空
            s.Push(p);              // 根指针入栈
            p = p->lChild;          // 遍历左子树
        }
        else {                      // 如果 p 为空
            NodeType* q{ s.Pop() }; // 出栈
            callback(q);            // 访问根结点
            p = q->rChild;          // 遍历右子树
        }
    }
}

template <typename NodeType, typename Callable>
void TraversePrOrderNR(NodeType* tree, Callable callback) {
    // 先序遍历的非递归算法
    if (!tree) return; // 若二叉树为空，立即返回
    std::stack<NodeType*> s; // 设置一个栈，存放根指针
    NodeType* p{ tree }; // 取得根指针
    while (p || !s.IsEmpty()) {
        if (p) {                    // 如果 p 非空
            callback(p);            // 访问根结点
            s.Push(p);              // 根指针入栈
            p = p->lChild;          // 遍历左子树
        }
        else {                      // 如果 p 为空
            NodeType* q{ s.Pop() }; // 出栈
            p = q->rChild;          // 遍历右子树
        }
    }
}

template <typename NodeType, typename Callable>
void TraversePsOrderNR(NodeType* tree, Callable callback) {
    // 后序遍历的非递归算法
    if (!tree) return; // 若二叉树为空，立即返回
    std::stack<NodeType*> s; // 设置一个栈，存放根指针
    NodeType* p{ tree }; // 取得根指针
    NodeType* q;
    NodeType* r{ nullptr };
    while (p || !s.IsEmpty()) {
        if (p) {                    // 如果 p 非空
            s.Push(p);              // 根指针入栈
            p = p->lChild;          // 遍历左子树
        }
        else {                      // 如果 p 为空
            p = s.Top();
            q = p->rChild;          // 获取右子树
            if (q && q != r) {      // 右子树非空且未访问
                p = q;              // 遍历右子树的左子树
                s.Push(p);          //      根指针入栈
                p = p->lChild;      //      遍历左子树
            }
            else {                  // 右子树为空，或已访问过
                r = s.Pop();        // 根指针出栈，并设为已访问状态
                callback(r);        // 访问根结点
                p = nullptr;
            }
        }
    }
}

template <typename NodeType, typename Callable>
void TraverseTLOrderNR(NodeType* tree, Callable callback) {
    // 层次遍历的非递归算法
    if (!tree) return; // 若二叉树为空，立即返回
    NodeType* p{ tree }; // 取得根指针
    std::queue<NodeType*, std::list<NodeType*>> q; // 设置一个队列，存放根指针
    q.push(p); // 根指针插入队尾
    while (!q.empty()) { // 若队列非空
        p = q.front(); q.pop(); // 出队
        callback(p); // 访问结点
        NodeType* u{ p->lChild }; // 取出左孩子
        NodeType* v{ p->rChild }; // 取出右孩子
        if (u) q.push(u); // 左孩子非空则入队
        if (v) q.push(v); // 右孩子非空则入队
    }
}

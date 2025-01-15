#pragma once
#include "LinkedQueue.hpp"
#include "TreeUtils.hpp"

template <typename E, int C = 0 /* 结点类型 */>
struct Node { // 结点（二叉树）
    E data; // 数据域
    Node* lChild; // 左指针域（指向左孩子）
    Node* rChild; // 右指针域（指向右孩子）

    Node(E data) : data{ data } {
        lChild = rChild = nullptr;
    }
    ~Node() {
        if (lChild != nullptr) delete lChild;
        if (rChild != nullptr) delete rChild;
        lChild = rChild = nullptr;
    }
};

template <typename E>
struct Node<E, 1> { // 结点（二叉树，带有亲指针域）
    E data; // 数据域
    Node* lChild; // 左指针域（指向左孩子）
    Node* rChild; // 右指针域（指向右孩子）
    Node* parent; // 亲指针域

    Node(E data) : data{ data } {
        parent = lChild = rChild = nullptr;
    }
    ~Node() {
        if (lChild != nullptr) delete lChild;
        if (rChild != nullptr) delete rChild;
        parent = lChild = rChild = nullptr;
    }
};

template <typename ElemType>
struct LinkedBinaryTree { // 二叉树（链式存储，二叉链表）
    struct Node { // 结点
        Node* lChild; // 左指针域（指向左孩子）
        Node* rChild; // 右指针域（指向右孩子）
        ElemType data; // 数据域

        Node(ElemType data);
        ~Node();
        bool InsertChild(bool lr, Node* child);
        bool DeleteChild(bool lr);
    };
    Node* root; // 头指针（指向根）

    LinkedBinaryTree();
    ~LinkedBinaryTree();
    bool IsEmpty() const; // 若为空树，返回 true；否则返回 false。
};

template <typename ElemType>
LinkedBinaryTree<ElemType>::LinkedBinaryTree() : root{ nullptr } { }

template <typename ElemType>
LinkedBinaryTree<ElemType>::~LinkedBinaryTree() {
    delete root;
    root = nullptr;
}

template <typename ElemType>
bool LinkedBinaryTree<ElemType>::IsEmpty() const { return root == nullptr; }

template <typename ElemType>
LinkedBinaryTree<ElemType>::Node::Node(ElemType data) : data{ data } {
    lChild = rChild = nullptr;
}

template <typename ElemType>
LinkedBinaryTree<ElemType>::Node::~Node() {
    if (lChild) {
        delete lChild;
        lChild = nullptr;
    }
    if (rChild) {
        delete rChild;
        rChild = nullptr;
    }
}

template <typename ElemType>
bool LinkedBinaryTree<ElemType>::Node::InsertChild(bool lr, Node* child) {
    if (lr) {
        if (lChild) return false;
        lChild = child;
    }
    else {
        if (rChild) return false;
        rChild = child;
    }
    return true;
}

template <typename ElemType>
bool LinkedBinaryTree<ElemType>::Node::DeleteChild(bool lr) {
    if (lr && lChild) {
        delete lChild;
        lChild = nullptr;
        return true;
    }
    else if (!lr && rChild) {
        delete rChild;
        rChild = nullptr;
        return true;
    }
    return false;
}

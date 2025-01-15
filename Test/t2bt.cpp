#if 0
#include <iostream>
#include <queue>

#include "BinaryTree.hpp"
#include "TraverseTree.hpp"

template <typename ElemType>
struct NodeCS {
    ElemType data;
    NodeCS* firstChild;
    NodeCS* nextSibling;

    NodeCS(ElemType&& data) : data{ data } {
        firstChild = nextSibling = nullptr;
    }
};

template <typename ElemType>
Node<ElemType>* ConvertTree2BinaryTree(NodeCS<ElemType>* tree) {
    if (!tree) return nullptr;
    Node<ElemType>* lChild{ ConvertTree2BinaryTree(tree->firstChild) }; // 第一个孩子变成左孩子
    Node<ElemType>* rChild{ ConvertTree2BinaryTree(tree->nextSibling) }; // 下一个兄弟变成右孩子
    Node<ElemType>* node{ new Node<ElemType> { tree->data, lChild, rChild } };
    return node;
}

template <typename ElemType>
Node<ElemType>* ConvertForest2BinaryTree(NodeCS<ElemType>** forest, int size) {
    Node<ElemType>* tree{ nullptr };
    for (int i = size - 1; i >= 0; i--) {
        Node<ElemType>* node{ ConvertTree2BinaryTree(forest[i]) };
        if (tree == nullptr) {
            tree = node;
        }
        else {
            node->rChild = tree;
            tree = node;
        }
    }
    return tree;
}

int main() {
    using NodeA = NodeCS<char>;
    NodeA n1{ 'A' };
    NodeA n2{ 'B' };
    NodeA n3{ 'C' };
    NodeA n4{ 'D' };
    n1.firstChild = &n2;
    n2.nextSibling = &n3;
    n3.nextSibling = &n4;
    using NodeB = Node<char>;
    NodeB* result{ ConvertTree2BinaryTree(&n1) };

    TEST_TRAVERSE("先序遍历", result, TraversePrOrderR);
    TEST_TRAVERSE("中序遍历", result, TraverseInOrderR);
    TEST_TRAVERSE("后序遍历", result, TraversePsOrderR);

    delete result;
}

#endif

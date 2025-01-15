#pragma once
#include "BinaryTree.hpp"

template <typename ElemType>
Node<ElemType>* ConstructLinkedBinaryTreeFromStandardSequence(const ElemType* list, int len, ElemType placebo) {
    if (list == nullptr || len == 0) return nullptr;
    if (list[0] == placebo) return nullptr;
    Node<ElemType>** nodeList = new Node<ElemType>*[len];
    for (int i = 0; i < len; i++) {
        const ElemType& e{ list[i] };
        Node<ElemType>* node;
        if (e == placebo) {
            node = nullptr;
        }
        else {
            node = new Node<ElemType>;
            node->data = e;
        }
        nodeList[i] = node;
    }
    for (int i = 0; i < len; i++) {
        Node<ElemType>* node{ nodeList[i] };
        if (node == nullptr) continue;
        int j{ 2 * (i + 1) - 1 };
        if (j < len)
            node->lChild = nodeList[j];
        else
            node->lChild = nullptr;
        int k{ j + 1 };
        if (k < len)
            node->rChild = nodeList[k];
        else
            node->rChild = nullptr;
    }
    Node<ElemType>* root{ nodeList[0] };
    delete[] nodeList;
    return root;
}

template <typename ElemType, typename NodeType = typename LinkedBinaryTree<ElemType>::Node>
NodeType* ConstructLinkedBinaryTreeFromTraverseSequencePsIn(
    const ElemType* seqPs /*后序序列*/,
    const ElemType* seqIn /*中序序列*/,
    int len /*序列长度*/
) {
    // 根据后序序列和中序序列构造二叉树，非递归算法
    if (!(seqPs && seqIn && len > 0)) return nullptr; // 空序列，直接返回
    const ElemType* pData;
    NodeType* root{ nullptr };
    struct Relation {
        NodeType* parent; // 双亲指针
        bool isLeftChild; // 左孩子选 true，右孩子选 false
        int loPs; // 后序序列指标起点
        int hiPs; // 后序序列指标终点
        int loIn; // 中序序列指标起点
        int hiIn; // 中序序列指标终点
    };
    Queue<Relation> lq;
    lq.Enqueue(Relation{ nullptr, 1, 0, len - 1, 0, len - 1 });
    while (!lq.IsEmpty()) {
        const Relation& relation{ lq.Dequeue() };
        if (len == 0) continue; // 对空序列不作任何处理
        pData = &seqPs[relation.hiPs];
        NodeType* node{ new NodeType{ *pData } }; // 构造根结点
        if (root == nullptr) root = node;
        else {
            assert(relation.parent != nullptr);
            if (relation.isLeftChild) // 当前结点是它的双亲的左孩子
                relation.parent->lChild = node;
            else // 当前结点是它的双亲的右孩子
                relation.parent->rChild = node;
        }
        int mdIn;
        for (mdIn = relation.loIn; mdIn <= relation.hiIn; mdIn++)
            if (seqIn[mdIn] == *pData)
                break; // 在中序序列中定位根结点
        // 在中序序列中，指标区间 [loIn,mdIn-1] 对应的是左子序列，指标区间 [mdIn+1,hiIn] 对应的是右子序列
        int lenL{ mdIn - relation.loIn }; // 左子序列的长度
        int loPs{ relation.loPs };
        int hiPs{ loPs + lenL - 1 };
        int loIn{ relation.loIn };
        int hiIn{ mdIn - 1 };
        if (lenL)
            lq.Enqueue(Relation{ node, 1, loPs, hiPs, loIn, hiIn }); // 将左子序列插入队列
        int lenR{ relation.hiIn - mdIn }; // 右子序列的长度
        loPs = hiPs + 1;
        hiPs = loPs + lenR - 1;
        loIn = mdIn + 1;
        hiIn = relation.hiIn;
        if (lenR)
            lq.Enqueue(Relation{ node, 0, loPs, hiPs, loIn, hiIn }); // 将右子序列插入队列
    }
    return root; // TODO
}

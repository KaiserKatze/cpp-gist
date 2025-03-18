#pragma once

#include <initializer_list>
#include <stdexcept>
#include <iostream>
#include <concepts>
#include <functional>
#include <stack>
#include <forward_list>
#include <list>
#include <cassert>
#include <type_traits>
#include <cstddef>

#include "BinaryTree.hpp"
#include "TraverseTree.hpp"
//==================================
// BST（基础款）

template <class E, std::strict_weak_order<E, E> Compare = std::less<E>>
struct BinarySearchTree { // 二叉搜索树
    using NodeType = Node<E>; // 使用带有亲指针域的二叉链表
    using Ancestry = std::stack<std::reference_wrapper<NodeType*>>;
    NodeType* root{ nullptr }; // 根指针
    Compare compare{};

    BinarySearchTree() {}

    BinarySearchTree(std::initializer_list<E> list) {
        for (const E& e : list) Insert(e);
    }

    ~BinarySearchTree() {
        if (root) delete root;
    }

    virtual NodeType* Insert(E data /* 新数据 */) {
        // 返回新插入的数据对应的结点
        NodeType** pp{ &root };
        NodeType* p;
        while (true) {
            p = *pp;
            if (p == nullptr) break;
            else if (data == p->data)
                switch (policy)
                {
                case ERROR:
                    throw std::runtime_error{ "插入重复数据" };
                case IGNORE:
                    return nullptr;
                case INSERT:
                    NodeType* node = new NodeType{ data };
                    NodeType* lChild{ p->lChild };
                    p->lChild = node;
                    node->lChild = lChild;
                    return node;
                }
            else if (compare(data, p->data))
                pp = &(p->lChild);
            else
                pp = &(p->rChild);
        }
        NodeType* node = new NodeType{ data };
        *pp = node;
        return node;
    }

    NodeType*& Search(E data /* 待检索数据 */) {
        // 查找指定数据，返回结点指针
        NodeType** pp{ &root };
        NodeType* p;
        while (true) {
            p = *pp;
            if (p == nullptr) break; // 没有找到指定数据
            if (data == p->data) break; // 成功找到指定数据
            else if (compare(data, p->data))
                pp = &(p->lChild);
            else
                pp = &(p->rChild);
        }
        return *pp;
    }

    template <class Callable>
    void GetAncestry(NodeType* node, Callable filter, Ancestry& ancestry) {
        // 获取指定结点的祖先结点
        if (node == nullptr) return; // 参数错误
        NodeType** pp{ &root };
        NodeType* p;
        while ((p = *pp) != nullptr) {
            if (node == p) break; // 成功找到指定结点
            if constexpr (std::is_function_v<Callable>) {
                if (filter(p)) ancestry.push(*pp); // 祖先结点入栈
            }
            else ancestry.push(*pp); // 祖先结点入栈
            if (compare(node->data, p->data))
                pp = &(p->lChild);
            else
                pp = &(p->rChild);
        }
    }

    NodeType*& MaxChild(NodeType* node /* 出发点 */ = nullptr) {
        NodeType*& lLeaf{ LeftmostLeaf(node) };
        NodeType*& rLeaf{ RightmostLeaf(node) };
        if (compare(lLeaf->data, rLeaf->data)) return rLeaf;
        else                                   return lLeaf;
    }

    E Max(NodeType* node /* 出发点 */ = nullptr) {
        return MaxChild(node)->data;
    }

    NodeType*& MinChild(NodeType* node /* 出发点 */ = nullptr) {
        NodeType*& lLeaf{ LeftmostLeaf(node) };
        NodeType*& rLeaf{ RightmostLeaf(node) };
        if (compare(lLeaf->data, rLeaf->data)) return lLeaf;
        else                                   return rLeaf;
    }

    E Min(NodeType* node /* 出发点 */ = nullptr) {
        return MinChild(node)->data;
    }

    NodeType*& LeftmostLeaf(NodeType* node /* 出发点 */ = nullptr) {
        NodeType** pp{ &(node == nullptr ? root : node) };
        NodeType* p{ *pp };
        if (p == nullptr) return root;
        while (p->lChild != nullptr) {
            pp = &(p->lChild);
            p = *pp;
        }
        return *pp;
    }

    NodeType*& RightmostLeaf(NodeType* node /* 出发点 */ = nullptr) {
        NodeType** pp{ &(node == nullptr ? root : node) };
        NodeType* p{ *pp };
        if (p == nullptr) return root;
        while (p->rChild != nullptr) {
            pp = &(p->rChild);
            p = *pp;
        }
        return *pp;
    }

    virtual void Delete(NodeType*& node /* 待删除的结点 */) {
        if (node == nullptr) return;
        if (node->lChild != nullptr && node->rChild != nullptr) {
            // `node` 既有左子树，又有右子树
            NodeType*& chosen{ RightmostLeaf(node->lChild) };
            node->data = chosen->data;
            NodeType* lChild{ chosen->lChild }; // 暂存 `chosen` 的左子树的副本
            chosen->lChild = nullptr; delete chosen; // 断开链接，并释放内存
            chosen = lChild; // 把 `chosen` 的亲结点的右子树（或左子树）置为 `chosen` 的左子树
            return;
        }
        NodeType* nodeCopy{ node }; // 暂存 `node` 的副本，用于释放内存
        if (node->lChild != nullptr)
            node = node->lChild;
        else if (node->rChild != nullptr)
            node = node->rChild;
        else
            node = nullptr;
        nodeCopy->lChild = nodeCopy->rChild = nullptr;
        delete nodeCopy;
    }

    virtual void Delete(E data /* 待删除的数据 */) {
        Delete(Search(data));
    }

protected:

private:
    enum {
        ERROR = 0, // 插入重复数值会报错
        IGNORE = 1, // 插入重复数值会被忽略
        INSERT = 2 // 重复数值插入到左子树
    } policy{ IGNORE };
};

//==================================
// AVL 树

template <class NodeType>
int GetBalanceFactor(const NodeType* node) {
    if (node == nullptr) return 0;
    return Depth(node->lChild) - Depth(node->rChild);
}

template <class E, std::strict_weak_order<E, E> Compare = std::less<E>>
struct AdelsonVelskyLandis : public BinarySearchTree<E, Compare> { // AVL 树
    using BST = BinarySearchTree<E, Compare>;
    using NodeType = typename BST::NodeType;
    using Ancestry = typename BST::Ancestry;

    AdelsonVelskyLandis() {}
    AdelsonVelskyLandis(std::initializer_list<E> list) {
        for (const E& e : list) Insert(e);
    }

#define TRAVERSE_TREE\
    TraverseInOrderR(this->BST::root, [&](auto* p) {\
        std::cout << "\t{ "\
            << "addr: " << p << ", "\
            << "data: " << p->data << ", "\
            << "lchd: " << p->lChild << '(' << (p->lChild ? p->lChild->data : '\0') << "), "\
            << "rchd: " << p->rChild << '(' << (p->rChild ? p->rChild->data : '\0') << "), "\
            << "bf: " << GetBalanceFactor(p)\
             << " } ...\n";\
    });\
    std::cout << "\t树中结点个数 = " << Count(this->BST::root) << '\n';

    virtual NodeType* Insert(E data /* 新数据 */) override {
        // 返回新插入的数据对应的结点
        auto node = this->BST::Insert(data);
        std::cout << "插入新数据 { data: " << data << " } ...\n";
        TRAVERSE_TREE;

        Ancestry ancestry{};
        if (node != nullptr && GetNearestUnbalancedAncestor(node, ancestry)) {
            AutoBalance(ancestry.top().get()); // 最近的失衡祖先结点
        }

        return node;
    }
    virtual void Delete(NodeType*& node /* 待删除的结点 */) override {
        std::cout << "\t待删除的结点 = { "
            << "addr: " << node << ", "
            << "data: " << node->data
            << " } ...\n";

        std::cout << "\t删除结点前，各个结点的平衡因子 = \n";
        TRAVERSE_TREE;

        Ancestry ancestry{};
        this->BST::GetAncestry(node, nullptr, ancestry);
        this->BST::Delete(node);

        std::cout << "\t删除结点后，根结点的平衡因子 = "
            << GetBalanceFactor(this->BST::root) << '\n';
        std::cout << "\t删除结点后，各个结点的平衡因子 = \n";
        TRAVERSE_TREE;

        while (!ancestry.empty()) {
            NodeType*& parentOfNode{ ancestry.top().get() };
            ancestry.pop();
            int bf{ GetBalanceFactor(parentOfNode) };

            std::cout << "自动平衡结点 { "
                << "addr: " << parentOfNode << ", "
                << "data: " << parentOfNode->data << ", "
                << "lchd: " << parentOfNode->lChild << ", "
                << "rchd: " << parentOfNode->rChild << ", "
                << "bf: " << bf
                << " } ...\n";

            if (bf >= 2 || bf <= -2) // 结点失衡，需要旋转
                AutoBalance(parentOfNode);
        }
    }

private:
    bool IsBalanced(NodeType* node = nullptr) {
        if (node == nullptr) node = this->BST::root;
        int bf{ GetBalanceFactor(node) };
        return -1 <= bf && bf <= 1;
    }
    bool GetNearestUnbalancedAncestor(NodeType* newlyInsertedNode, Ancestry& ancestry) {
        // 寻找距离新插入结点最近的祖先结点，得到最小不平衡子树
        this->BST::GetAncestry(newlyInsertedNode, [&](auto p) { return !this->IsBalanced(p); }, ancestry);
        std::cout << "\t失衡祖先栈大小 = " << ancestry.size() << '\n';
        return !ancestry.empty();
    }
    void RotateLeft(NodeType*& node) {
        // 将 `node` 左旋，变成它的右子结点 `p` 的左子结点
        // `node` 的左子结点不变
        // 如果 `p` 已经有一个非空左子结点 `q` 了，就让 `q` 变成 `node` 的右子结点
        // 让 `p` 代替它连接到它的亲结点上
        std::cout << "左旋结点 { "
            << "addr: " << node << ", "
            << "data: " << node->data << ", "
            << "lchd: " << node->lChild << ", "
            << "rchd: " << node->rChild
            << " } ...\n";
        NodeType* p{ node->rChild };
        NodeType* q{ p->lChild };
        p->lChild = node;
        node->rChild = q;
        if (node == this->BST::root) this->BST::root = p;
        else                         node = p;

        TRAVERSE_TREE;
    }
    void RotateRight(NodeType*& node) {
        // 将 `node` 右旋，变成它的左子结点 `p` 的右子结点
        // `node` 的右子结点不变
        // 如果 `p` 已经有一个非空右子结点 `q` 了，就让 `q` 变成 `node` 的左子结点
        // 让 `p` 代替它连接到它的亲结点上
        std::cout << "右旋结点 { "
            << "addr: " << node << ", "
            << "data: " << node->data << ", "
            << "lchd: " << node->lChild << ", "
            << "rchd: " << node->rChild
            << " } ...\n";
        NodeType* p{ node->lChild };
        NodeType* q{ p->rChild };
        p->rChild = node;
        node->lChild = q;
        if (node == this->BST::root) this->BST::root = p;
        else                         node = p;

        TRAVERSE_TREE;
    }
    void AutoBalance(NodeType*& node) {
        int bf = GetBalanceFactor(node);
        int bfL = GetBalanceFactor(node->lChild);
        int bfR = GetBalanceFactor(node->rChild);

        switch (bf)
        {
        case 2:
            switch (bfL)
            {
            case -1: // LR 型（先左旋失衡结点的左孩子，再右旋失衡结点本身）
                RotateLeft(node->lChild);
                [[fallthrough]];
            case 1: // LL 型（直接右旋失衡结点）
                RotateRight(node);
                break;
            }
            break;
        case -2:
            switch (bfR)
            {
            case 1: // RL 型（先右旋失衡结点的右孩子，再左旋失衡结点本身）
                RotateRight(node->rChild);
                [[fallthrough]];
            case -1: // RR 型（直接左旋失衡结点）
                RotateLeft(node);
                break;
            }
            break;
        }

        std::cout << "\t自动平衡后，各个结点的平衡因子 = \n";
        TRAVERSE_TREE;
    }
};

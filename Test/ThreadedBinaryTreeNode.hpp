// 中序线索化二叉树、遍历中序线索二叉树
#pragma once
#include <stack>

template <typename ElemType>
struct ThreadedBinaryTreeNode { // 线索二叉树（二叉线索链表）结点
    ThreadedBinaryTreeNode* lChild; // 左指针域（指向左孩子或前驱）
    ThreadedBinaryTreeNode* rChild; // 右指针域（指向右孩子或后继）
    ElemType data; // 数据域
    int lTag : 1; // 为0时，左指针域指向左孩子；否则指向前驱（只占用1位）
    int rTag : 1; // 为0时，右指针域指向右孩子；否则指向后继（只占用1位）
    ThreadedBinaryTreeNode() {
        lChild = rChild = nullptr;
        lTag = rTag = 0;
    }

    ThreadedBinaryTreeNode(const ElemType& data) : ThreadedBinaryTreeNode() {
        this->data = data;
    }

    ~ThreadedBinaryTreeNode() {
        if (lChild) {
            delete lChild;
            lChild = nullptr;
        }
        if (rChild) {
            delete rChild;
            rChild = nullptr;
        }
    }

    bool InsertChild(bool lr, ThreadedBinaryTreeNode* child) {
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

    bool DeleteChild(bool lr) {
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

};

template <typename NodeType>
NodeType* ThreadingIn(
    NodeType* node /* 子树根指针 */,
    NodeType* prev /* 前驱指针 */
) {
    // 以结点 node 为根的子树中序线索化
    if (node) {
        prev = ThreadingIn(node->lChild, prev); // 左子树递归中序线索化
        if (node->lChild == nullptr) { // node 的左孩子为空
            node->lTag = 1; // 给 node 加上左线索
            node->lChild = prev; // prev 是 node 的前驱
        }
        else node->lTag = 0; // node 的左孩子非空，没有左线索
        if (prev->rChild == nullptr) { // prev 的右孩子为空
            prev->rTag = 1; // 给 prev 加上右线索
            prev->rChild = node; // node 是 prev 的后继
        }
        else prev->rTag = 0; // prev 的右孩子非空，没有右线索
        prev = node; // node 是已经访问过的结点
        prev = ThreadingIn(node->rChild, prev); // 右子树递归中序线索化
    }
    return prev; // 返回刚刚访问过的结点
}

template <typename NodeType>
NodeType* ThreadingIn(NodeType* t /* 不带头结点的二叉树的根指针 */) {
    // 中序遍历二叉树，把它中序线索化
    NodeType* head{ new NodeType{} }; // 建立头结点，确保首元结点和尾结点可以线索化
    head->lTag = 0;
    head->lChild = head;
    head->rTag = 1;
    head->rChild = nullptr;
    if (t) {
        head->lChild = t; // 头结点的左孩子指向根，前驱 prev 初值指向头结点
        NodeType* prev{ ThreadingIn(t, head) }; // 对以 t 为根的二叉树中序线索化
        head->rChild = prev; // 头结点的右线索指向前驱 prev
        prev->rTag = 1;
        prev->rChild = head; // 线索化后，前驱 prev 变成最右结点，它的右线索指向头结点
    }
    return head; // 返回头结点指针
}

template <typename NodeType, typename Callable>
void TraverseInOrderT(NodeType* tree /* 线索二叉树的头结点 */, Callable callback) {
    // 中序遍历中序线索二叉树
    assert(tree != nullptr);
    NodeType* p{ tree->lChild }; // p 指向树根
    while (p != tree) { // 空树或遍历结束时，必有 `p == tree`！
        while (p && !p->lTag) p = p->lChild; // 沿左孩子向下
        if (p) callback(p); // 访问左子树为空的结点
        while (p && p->rTag && p->rChild != tree) {
            p = p->rChild;
            callback(p); // 沿右线索访问后继
        }
        if (p) p = p->rChild; // 转向 p 的右子树
    }
}

template <typename NodeType, typename Callable>
void TraverseNInOrderNR(NodeType* tree, Callable callback) {
    // 中序遍历（结点）的非递归算法
    if (!tree) return; // 若二叉树为空，立即返回
    std::stack<NodeType*> s; // 设置一个栈，存放根指针
    NodeType* p{ tree }; // 取得根指针
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

#pragma once

#include <functional>
#include <algorithm>
#include <utility>
#include <initializer_list>
#include <vector>
#include <cmath>

//==================================
// B-树

template <class E, size_t m /* 阶数 */>
struct BTree {
    // m 阶 B 树的特点：
    //  1. 每个结点至多有 m 棵子树，至多有 m-1 个关键字
    //  2. 当根结点不是叶节点时，它至少要有 2 棵子树，1 个关键字
    //  3. 除根结点以外的非叶结点至少有 ceil(m/2) 棵子树，ceil(m/2)-1 个关键字
    //  4. 非叶结点的结构，由 1 个表示长度的字段以及至多 m-1 个指针域和关键字构成
    //  5. 所有叶节点都出现在同一层次上，并且不含任何信息

    static_assert(m >= 2, "阶数 `m` 应当不小于 2.");

    struct TreeNode;

    struct KeyNode {
        TreeNode* next; // 指针（指向恰好比关键字 keyword 小的记录所在的结点）
        E keyword;      // 关键字

        KeyNode(const E& e) : keyword{ e }, next{ nullptr } {}

        bool operator==(const E& e) const {
            return keyword == e;
        }
        bool operator<(const E& e) const {
            return keyword < e;
        }
        bool operator>(const E& e) const {
            return keyword > e;
        }
    };

    using container = std::vector<KeyNode>;
    using iterator = typename container::iterator;
    using const_iterator = typename container::const_iterator;
    // 非根结点的关键字个数取值范围 [min_size, max_size]
    static constexpr size_t max_size{ m - 1 }; // 关键字数上限 m-1
    static constexpr size_t min_size{ (m + 1) / 2 - 1 }; // 关键字数下限 ceil(m/2)-1

    struct TreeNode : public container { // B树的结点
        TreeNode* parent; // 指针（指向父结点）
        TreeNode* next; // 指针（指向比该结点中所有关键字都要大的记录所在的结点）

        TreeNode(TreeNode* parent) : parent{ parent }, next { nullptr } {}
        TreeNode(TreeNode* parent, const_iterator begin, const_iterator end) : container(begin, end), parent{ parent }, next{ nullptr } {}

        iterator find_keyword_in_tree_node(const E& e) {
            iterator it{ this->begin() };
            for (; *it > e && it != this->end(); ++it); // 寻找与 e 相等或恰好比 e 大的关键字
            return it;
        }
    };

    TreeNode* root;

    BTree() : root{ nullptr /* 默认建立一个空的 B 树 */ } {}
    ~BTree() {
        delete root;
    }

    std::pair<bool, TreeNode*> find(const E& e) { // B树的查找
        // 将给定关键字 e 与根结点的各个关键字进行比较：
        // 如果 e 与根结点中的某个关键字相等，则查找成功；
        // 如果 e 小于第一个关键字，则顺着该关键字对应的左子树继续向下查找；
        // 如果 e 介于某两个关键字，则顺着这两个关键字之间的子树继续向下查找；
        // 如果 e 大于最后一个关键字，则顺着该关键字对应的右子树继续向下查找。
        // 如果直到叶结点还未找到，则查找失败。
        TreeNode* tree_node{ root }; // 从根结点出发查找关键字
        TreeNode* prev_tree_node{ nullptr }; // 上一个检查过的结点
        while (tree_node != nullptr) {
            const_iterator it{ tree_node->find_keyword_in_tree_node(e) }; // find_keyword_in_tree_node 返回的关键字小于或等于 e
            if (*it == e) { // 查找成功，返回关键字对应的结点
                return { true, tree_node };
            }
            prev_tree_node = tree_node;
            tree_node = it->next;
        }
        return { false, prev_tree_node }; // 直到最后也匹配不到完全一致的关键字，返回最后一个非叶结点
    }

private:
    std::tuple<TreeNode*, TreeNode*, iterator> split_tree_node(TreeNode* tree_node) {
        const size_t pos_mid{ (tree_node->size() + 1) / 2 }; // 中间位置的序号
        iterator it_begin{ tree_node->begin() };
        iterator it_end{ tree_node->end() };
        iterator it_mid{ it_begin + pos_mid }; // 获取位于中间位置的关键字
        TreeNode* tree_node_right_sibling{ new TreeNode(tree_node->parent, it_mid + 1, it_end) }; // 利用右段全体关键字的副本，新建一个结点
        return { tree_node, tree_node_right_sibling, it_mid };

        it_mid = put_keyword_into_tree_node(tree_node->parent, it_mid->keyword); // 将位于中间位置的关键字插入父结点
    }

    iterator put_keyword_into_tree_node(TreeNode* tree_node, const E& e) { // 在该结点中插入关键字
        bool finished{ false };
        iterator it_insert{ tree_node->find_keyword_in_tree_node(e) };
        while (tree_node != nullptr && !finished) {
            if (it_insert == tree_node->end()) {
                tree_node->push_back(e); // 把关键字 e 置于线性表末尾
            }
            else {
                tree_node->insert(it_insert, e); // 把关键字 e 插入到线性表中
            }
            if (tree_node->size() < max_size) { // 结点不满，可以直接插入关键字，不需要后处理
                finished = true; // 插入完成
                break;
            }
            // 当前结点中的关键字数不小于 max_size(=m-1)，
            // 不可以直接插入关键字，必须分裂结点
            // 分裂的具体方法是：
            // 先插入关键字 e，从中间位置 ceil(m/2) 将结点分为三段，
            // 左段的关键字保留在原结点中，
            // 右段的关键字放到一个新结点中，
            // 中间位置的关键字放到原结点的父结点，
            // 接下来检查父结点中关键字的个数是否超出关键字数上限
            auto [left_tree_node, right_tree_node, it_mid] { split_tree_node(tree_node) };
            tree_node = tree_node->parent;
            if (tree_node != nullptr) {
                const E& e1{ it_mid->keyword };
                it_insert = tree_node->find_keyword_in_tree_node(e1);
                if (it_insert == tree_node->end()) {
                    tree_node->push_back(e1); // 把关键字 e1 置于线性表末尾
                }
                else {
                    tree_node->insert(it_insert, e1); // 把关键字 e1 插入到线性表中
                }

                TreeNode*& parent_left{ it_insert->next }; // 取得父结点左子树指针
                TreeNode*& parent_right{
                    (++it_insert != tree_node->end())
                    ? (it_insert->next)
                    : (tree_node->next)
                }; // 取得父结点右子树指针
                parent_left = left_tree_node;
                parent_right = right_tree_node;
            }
        }
    }

public:
    void put(const E& e) { // 在 B 树中插入关键字
        auto [status, tree_node] { find(e) }; // 根据关键字 e，在 B 树中查找，定位关键字 e 应该插入的位置
        if (tree_node == nullptr) { // 空树
            root = new TreeNode(nullptr); // 创建根结点
            tree_node = root;
        }
        put_keyword_into_tree_node(tree_node, e); // 在正确的结点插入关键字（在这个过程中可能会新建子树）
    }

    void put_all(std::initializer_list<E> list) {
        for (auto&& e : list) {
            put(e);
        }
    }
};

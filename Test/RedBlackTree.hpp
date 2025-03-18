#pragma once

#include <type_traits>

template <class ElemType>
struct RedBlackTree {
    enum class Color {
        red = 0,
        black = 1
    };

    struct Node {
        ElemType data;
        Node* left;
        Node* right;
        Color color;

        Node(ElemType&& data, Node*&& left, Node*&& right, Color color) : data{ data }, left { left }, right{ right }, color{ color } {}
    };

    const Node sentinelNil({}, nullptr, nullptr, Color::black);

    RedBlackTree() {
    }

    Node CreateNode(ElemType&& data) {
        return { data, sentinelNil, sentinelNil };
    }

    void Insert(ElemType&& data) {

    }

    // TODO
};


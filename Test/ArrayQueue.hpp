#pragma once

#include <vector>

template <typename ElemType>
class Queue { // 队列（顺序存储）
    std::vector<ElemType> v;
    size_t head; // 队首指针（只可删除）
    size_t rear; // 队尾指针（只可插入）

    Queue(size_t capacity) : v(capacity), head{ 0 }, rear{ 0 } {
    }


};

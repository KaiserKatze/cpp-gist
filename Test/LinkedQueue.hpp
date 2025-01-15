#pragma once

template <typename ElemType>
class Queue { // 队列（链式存储）
    struct Node {
        ElemType data; // 数据域
        Node* next;

        Node(const ElemType& data) : data{ data }, next{ nullptr } {}
    };
    int size;
    Node* head; // 队首指针（只可删除）
    Node* rear; // 队尾指针（只可插入）

public:
    Queue() : size{ 0 } {
        head = rear = nullptr;
    }
    ~Queue() {
        while (head) {
            Node* next{ head->next };
            delete head;
            head = next;
        }
    }
    int Size() const { return size; }
    int IsEmpty() const { return Size() == 0; }
    void Enqueue(const ElemType& value) {
        Node* node{ new Node{ value } };
        if (rear) {
            rear->next = node;
            rear = node;
        }
        else {
            head = rear = node;
        }
        ++size;
    }
    ElemType Dequeue() {
        if (head != nullptr && size > 0) {
            Node* next{ head->next };
            ElemType data{ head->data };
            delete head;
            head = next;
            --size;
            if (size == 0) rear = nullptr;
            return data;
        }
        throw - 1;
    }
};
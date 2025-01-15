#pragma once 

template <typename ElemType>
class Stack { // Õ»
    struct Node {
        ElemType data; // Êý¾ÝÓò
        Node* next;

        Node(const ElemType& data, Node* next) : data{ data }, next{ next } {}
    };
    int size;
    Node* head;

public:
    Stack() : size{ 0 }, head{ nullptr } {}
    ~Stack() {
        while (head) {
            Node* next{ head->next };
            delete head;
            head = next;
        }
    }
    int Size() const { return size; }
    int IsEmpty() const { return Size() == 0; }
    void Push(const ElemType& value) {
        head = new Node{ value, head };
        ++size;
    }
    ElemType Pop() {
        if (head != nullptr && size > 0) {
            Node* next{ head->next };
            ElemType data{ head->data };
            delete head;
            head = next;
            --size;
            return data;
        }
        throw - 1;
    }
    ElemType Top() {
        if (head != nullptr && size > 0) {
            return head->data;
        }
        throw - 1;
    }
};
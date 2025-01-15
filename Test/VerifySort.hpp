#pragma once

#include <iostream>

template <class ElemType>
void Verify(ElemType* array, size_t start, size_t end) {
    std::cout << "输出序列：(";
    for (ElemType* p{ array + start }; p < array + end; ++p) {
        std::cout << *p << ",";
    }
    std::cout << ").\n";
    std::cout << "是否有序：";
    bool order{ true };
    for (ElemType* p{ array + start + 1 }; p < array + end; ++p) {
        if (*p < *(p - 1)) { // 后一个记录小于前一个记录
            order = false;
            break;
        }
    }
    std::cout << std::boolalpha << order << ".\n";
}

template <class RandomIt>
void Verify(RandomIt begin, RandomIt end) {
    std::cout << "输出序列：(";
    for (RandomIt p{ begin }; p != end; ++p) {
        std::cout << *p << ",";
    }
    std::cout << ").\n";
    std::cout << "是否有序：";
    bool order{ true };
    for (RandomIt p{ begin }; p != end; ++p) {
        if (*p < *(p - 1)) { // 后一个记录小于前一个记录
            order = false;
            break;
        }
    }
    std::cout << std::boolalpha << order << ".\n";
}


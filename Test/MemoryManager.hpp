#pragma once

#include <vector>
#include <queue>
#include <set>
#include <iostream>
#include <iomanip>
#include <initializer_list>

struct MemoryManager {
    struct Page {
        size_t page_id; // 页号
        bool valid; // 有效位
        size_t access; // 访问位

        Page() : page_id{ std::numeric_limits<size_t>::max() }, valid{ false }, access{ 0 } {
        }

        Page(size_t page_id) : page_id{ page_id }, valid{ true } {
            ResetAccess();
        }

        void ResetAccess() {
            access = ~0;
        }

        void UpdateAccess() {
            access >>= 1;
        }
    };
    std::vector<Page> working_set; // 工作集
    size_t capacity; // 工作集窗口大小

    MemoryManager(size_t capacity /* 工作集窗口大小 */) : capacity{ capacity } {
        working_set.reserve(capacity);
    }

    auto size() const { // 返回已装入的页面数
        return working_set.size();
    }

    void Access(std::initializer_list<size_t> page_id /* 页号 */) {
        for (auto& page : working_set) {
            if (page.valid && page.page_id == page.page_id) {

            }
        }
    }
};

void LRU(const size_t capacity_of_resident_set /* 驻留集窗口大小 */,
    const size_t capacity_of_working_set /* 工作集窗口大小 */,
    std::initializer_list<size_t> list_of_page_ids /* 页缓冲队列 */) {
    struct Page {
        size_t page_id{ 0 };
        bool valid{ false };
        size_t access{ 0 };

        Page& operator=(size_t page_id) {
            this->page_id = page_id;
            valid = true;
            access = 0;
            return *this;
        }

        bool operator==(size_t page_id) const {
            return valid && this->page_id == page_id;
        }

        operator size_t() const {
            return page_id;
        }

        operator char() const {
            if (!valid) {
                return ' ';
            }
            return '0' + static_cast<char>(page_id & 0xff);
        }
    };

    std::vector<Page> resident_set(capacity_of_resident_set); // 驻留集
    std::vector<char> working_set(capacity_of_working_set, ' '); // 工作集
    size_t working_set_pointer{ 0 };
    size_t working_set_size{ 0 };
    size_t counter{ 0 };

    for (size_t page_id : list_of_page_ids) {
        for (Page& page : resident_set) {
            ++page.access;
        }

        std::cout << "[" << std::setw(2) << std::setfill('0') << counter << "] ";
        ++counter;

        working_set[working_set_pointer] = '0' + static_cast<char>(page_id & 0xff); // 将页号加入工作集
        working_set_pointer = (working_set_pointer + 1) % capacity_of_working_set;
        ++working_set_size;
        if (working_set_size > capacity_of_working_set) {
            working_set_size = capacity_of_working_set;
        }

        bool isPageFound{ false };
        for (Page& page : resident_set) {
            if (page == page_id) {
                std::cout << "页面 " << page_id << " 已在驻留集中. ";
                {
                    std::set real_working_set(working_set.cbegin(), working_set.cend());
                    std::cout << "当前工作集={";
                    std::copy(real_working_set.cbegin(), real_working_set.cend(), std::ostream_iterator<char>(std::cout, ","));
                    std::cout << "}.\n";
                }
                isPageFound = true;
                page.access = 0;
                break;
            }
        }
        if (isPageFound) {
            continue;
        }
        std::cout << "页面 " << page_id << " 缺页! ";
        bool doReplace{ true };
        for (Page& page : resident_set) {
            if (!page.valid) {
                page = page_id; // 调入页面
                doReplace = false;
                break;
            }
        }
        if (!doReplace) {
            std::cout << "已成功将页面 " << page_id << " 调入主存. ";
            std::cout << "当前驻留集={";
            std::copy(resident_set.cbegin(), resident_set.cend(), std::ostream_iterator<char>(std::cout, ","));
            std::cout << "}. ";
            {
                std::set real_working_set(working_set.cbegin(), working_set.cend());
                std::cout << "当前工作集={";
                std::copy(real_working_set.cbegin(), real_working_set.cend(), std::ostream_iterator<char>(std::cout, ","));
                std::cout << "}.\n";
            }
            continue;
        }
        // 页面置换
        size_t page_id_to_replace{ 0 };
        size_t max_access{ 0 };
        size_t index_of_page_to_replace{ 0 };
        for (size_t i{ 0 }; Page & page : resident_set) {
            if (page.access > max_access) {
                max_access = page.access;
                page_id_to_replace = page;
                index_of_page_to_replace = i;
            }
            ++i;
        }
        resident_set[index_of_page_to_replace] = page_id;
        std::cout << "没有空闲页框，置换页面 " << page_id_to_replace << ". ";
        std::cout << "当前驻留集={";
        std::copy(resident_set.cbegin(), resident_set.cend(), std::ostream_iterator<char>(std::cout, ","));
        std::cout << "}. ";
        {
            std::set real_working_set(working_set.cbegin(), working_set.cend());
            std::cout << "当前工作集={";
            std::copy(real_working_set.cbegin(), real_working_set.cend(), std::ostream_iterator<char>(std::cout, ","));
            std::cout << "}.\n";
        }
    }
}


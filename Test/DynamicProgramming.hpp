#pragma once

#include <vector>
#include <map>

// # 设计动态规划算法的基本步骤：
// 1. 刻画一个最优解的结构特征；
// 2. 递归地定义最优解的值；
// 3. 计算最优解的值，通常采用自底向上的方法；
// 4. 利用计算出的信息构造一个最优解。

// # 钢条切割问题
// 给定一段长度为 n 英寸的钢条和一个价格表 p_i (i=1,2,...,n)，
// 求切割方案，使得销售收益 r_n 最大。
// 应该注意到，如果长度为 n 英寸的钢条的价格 p_n 足够大，那么最优解可能是完全不需要切割。
// 长度为 n 英寸的钢条共有 2^(n-1) 种不同的切割方案。

struct BestPartitionProblem_brute_force { // 自顶向下递归实现
    std::vector<size_t> _partition;
    float _best_price;

    static float partition(const size_t length, const std::vector<float>& price) {
        if (length == 0) {
            return .0f;
        }
        float max_price_so_far{ -std::numeric_limits<float>::infinity() };
        for (size_t i{ 1 }; i <= length; ++i) {
            float new_solution{ price[i] + partition(length - i, price) };
            if (max_price_so_far < new_solution) {
                max_price_so_far = new_solution;
            }
        }
        return max_price_so_far;
    }

    BestPartitionProblem_brute_force(const size_t length, const std::vector<float>& price)
        : _best_price{ partition(length, price) } {
    }

    BestPartitionProblem_brute_force(const size_t length, std::vector<float>&& price)
        : _best_price{ partition(length, price) } {
    }
};

struct BestPartitionProblem_top_down { // 带备忘的自顶向下法
    std::vector<float> _memo;
    float _best_price;

    static float partition(const size_t length, const std::vector<float>& price, std::vector<float>& memo) {
        float& rn{ memo[length] };
        float q;
        if (rn >= 0) {
            return rn;
        }
        if (length == 0) {
            q = 0;
        }
        else {
            q = -std::numeric_limits<float>::infinity();
            for (size_t i{ 1 }; i <= length; ++i) {
                q = std::max(q, price[i] + partition(length - i, price, memo));
            }
        }
        rn = q;
        return q;
    }

    BestPartitionProblem_top_down(const size_t length, const std::vector<float>& price)
        : _memo{ std::vector<float>(length + 1, -std::numeric_limits<float>::infinity()) } {
        this->_best_price = partition(length, price, this->_memo);
    }
};

struct BestPartitionProblem_bottom_up { // 自底向上法
    float _best_price;

    static float partition(const size_t length, const std::vector<float>& price) {
        if (length == 0) {
            return 0;
        }
        float q{ -std::numeric_limits<float>::infinity() };
        for (size_t i{ 1 }; i <= length; ++i) {
            q = std::max(q, price[i] + partition(length - i, price));
        }
        return q;
    }

    BestPartitionProblem_bottom_up(const size_t length, const std::vector<float>& price)
        : _best_price{ partition(length, price) } {
    }
};


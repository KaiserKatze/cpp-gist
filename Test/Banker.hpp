// @see: https://www.geeksforgeeks.org/bankers-algorithm-in-operating-system-2/

#include <cassert>
#include <vector>
#include <queue>
#include <iostream>
#include <initializer_list>
#include "Matrix.hpp"

struct BankersAlgorithm { // Dijkstra 的银行家算法
    std::vector<size_t> _available; // 可利用资源向量
    const size_t _concurrency; // 并发度 n（系统中进程数量）
    const size_t _size; // 资源的种数 m
    Matrix<size_t> _max; // 最大需求矩阵（定义每一个进程对各类资源的最大需求）
    Matrix<size_t> _allocation; // 分配矩阵（定义系统重每一类资源当前已分配给每一进程的资源数）
    Matrix<size_t> _need; // 需求矩阵（表示每一个进程尚需的各类资源数）
    // 上述三个矩阵之间存在下述关系：
    //      _need[i,j] == _max[i,j] - _allocation[i,j]
    std::queue<size_t, std::vector<size_t>> _blocked; // 进程阻塞队列

    BankersAlgorithm(const size_t nMaxProcess /* 并发度 */,
        const std::initializer_list<size_t>&& resources /* 初始全部可利用资源 */)
        : _available{ resources },
        _concurrency{ nMaxProcess },
        _size{ _available.size() },
        _max(_concurrency, _size, 0),
        _allocation(_concurrency, _size, 0),
        _need(_concurrency, _size, 0) {

    }

    BankersAlgorithm(const BankersAlgorithm&) = delete;
    BankersAlgorithm(BankersAlgorithm&&) = delete;

    bool Check() {
        // 安全性检查

        // 第 (1) 步，设置两个向量
        std::vector<size_t> _work(_available); // 工作向量（表示系统可提供给进程继续运行所需的各类资源数目）
        std::vector<bool> _finish(_concurrency, false); // 表示系统是否有足够的资源分配给进程，使之运行完成
#ifdef DEBUG
        std::vector<size_t> _safe_seq; // 安全序列
        _safe_seq.reserve(_concurrency);
#endif

        size_t _safe_process{ std::numeric_limits<size_t>::max() }; // 可以安全执行的进程的 id
        bool _retry{ true }; // 最多有一次机会
        while (_retry) {
            _retry = false;
            // 第 (2) 步，在进程集合中找到一个尚未拿到足够资源的进程
            for (size_t i = 0; i < _concurrency; ++i) {
                if (!_retry && _safe_process == i) {
                    // 最近一轮查找得到的可以安全执行的进程的 id 是 _safe_process，
                    // 本轮查找再次扫描到这个进程，并且 _retry 为 false（没有下一轮查找了），
                    // 说明没有其他安全执行的进程了，在这里结束查找，可以节省一些常数时间
                    break; // 跳出 for 循环，准备跳出 while 循环
                }
                if (_finish[i]) { // 系统有足够的资源可以分配给第 i 个进程
                    continue; // 继续检查下一个进程是否安全
                }
                size_t j = 0;
                const auto& _need_row{ _need[i] };
                for (; j < _size; ++j) {
                    if (_need_row[j] > _work[j]) { // 系统可供分配的资源不足以支持第 i 个进程继续执行
                        break;
                    }
                }
                if (j < _size) { // 检查下一个进程是否满足条件
#ifdef DEBUG
                    std::cout << "第 " << i << " 个进程尚不满足条件.";
                    std::cout << "\twork=(";
                    for (const auto& x : _work) {
                        std::cout << x << ",";
                    }
                    std::cout << ") < need=(";
                    for (const auto& x : _need_row) {
                        std::cout << x << ",";
                    }
                    std::cout << ")\n";
#endif
                    continue;
                }
#ifdef DEBUG
                std::cout << "第 " << i << " 个进程满足条件.";
                std::cout << "\t\twork=(";
                for (const auto& x : _work) {
                    std::cout << x << ",";
                }
                std::cout << ") > need=(";
                for (const auto& x : _need_row) {
                    std::cout << x << ",";
                }
                std::cout << ")\n";
#endif
                _retry = true; // 成功找到一个满足条件的进程，可以进行下一轮查找
                _safe_process = i; // 满足条件的进程的 id

                // 第 (3) 步，如果第 i 个进程满足上述条件，那么它可以顺利执行，直至完成，并释放出分配给它的资源
                const auto& _allocation_row{ _allocation[i] };
                for (size_t j = 0; j < _size; ++j) {
                    _work[j] += _allocation_row[j]; // 释放资源，就是让可分配资源增加第 i 个进程之前分配得到的资源
                }
                _finish[i] = true; // 第 i 个进程进入终止态，不再参与进程调度
#ifdef DEBUG
                _safe_seq.push_back(i);
                std::cout << "\t进程释放资源：\t\tallocation[" << i << "]=";
                std::copy(_allocation_row.begin(), _allocation_row.end(), std::ostream_iterator<size_t>(std::cout, ", "));
                std::cout << '\n';
                std::cout << "\t工作向量（更新）：\t";
                std::copy(_work.begin(), _work.end(), std::ostream_iterator<size_t>(std::cout, ", "));
                std::cout << '\n';
#endif
                // 回到第 (2) 步
            } // <end-for>
        } // <end-while>

        // 第 (4) 步，检查是否所有进程都满足 _finish[i] == true
        for (size_t i = 0; i < _concurrency; ++i) {
            if (!_finish[i]) {
#ifdef DEBUG
                std::cout << "没有安全序列!\n";
#endif
                return false; // 系统处于不安全状态
            }
        }

#ifdef DEBUG
        std::cout << "安全序列：";
        std::copy(_safe_seq.begin(), _safe_seq.end(),
            std::ostream_iterator<size_t>(std::cout, " -> "));
        std::cout << '\n';
#endif

        return true; // 系统处于安全状态
    }

    bool Require(const size_t i /* 申请资源的进程 id */,
        const std::vector<size_t>& request /* 请求向量 */) {
        // 处理各个进程的资源请求向量
        for (size_t j = 0; j < _size; ++j) {
            if (request[j] > _need[i][j]) {
                return false; // 所需要的资源大于所宣布的最大值，出错
            }
        }
        for (size_t j = 0; j < _size; ++j) {
            if (request[j] > _available[j]) {
                _blocked.push(i); // 尚无足够的资源，须等待，把该进程压入阻塞队列
            }
        }
        // 系统试探着把资源分配给进程，修改一下数据结构
        std::vector<bool> _allocTest(_size, false);
        for (size_t j = 0; j < _size; ++j) {
            if (request[j] > _available[j]) {
                _available[j] -= request[j];
                _allocation[i][j] += request[j];
                _need[i][j] -= request[j];

                _allocTest[j] = true;
            }
        }
        // 执行安全性算法，检查此次资源分配后系统是否处于安全状态
        if (Check()) {
            // 若安全，才正式把资源分配给该进程，完成本次分配；
            return true;
        }
        else {
            // 否则，将本次试探分配作废，恢复原来的资源分配状态，让该进程等待
            for (size_t j = 0; j < _size; ++j) {
                if (_allocTest[j]) {
                    _available[j] += request[j];
                    _allocation[i][j] -= request[j];
                    _need[i][j] += request[j];
                }
            }
            return false;
        }
    }


};

void TestBanker() {
    BankersAlgorithm banker(5, { 10, 5, 7 });

    banker._max = {
        { 7,5,3 },
        { 3,2,2 },
        { 9,0,2 },
        { 2,2,2 },
        { 4,3,3 }
    };

    banker._allocation = {
        {0,1,0},
        {2,0,0},
        {3,0,2},
        {2,1,1},
        {0,0,2}
    };

    banker._need = banker._max - banker._allocation;

    auto _allocation_sum{ banker._allocation.sum_column() };
    //banker._available = { 3,3,2 };
    banker._available -= _allocation_sum;

    std::cout << "最大需求矩阵：\n" << static_cast<std::string>(banker._max) << "\n";
    std::cout << "分配矩阵：\n" << static_cast<std::string>(banker._allocation) << "\n";
    std::cout << "分配矩阵（合计）：\n" << static_cast<std::string>(_allocation_sum) << '\n';
    std::cout << "需求矩阵：\n" << static_cast<std::string>(banker._need) << "\n";
    std::cout << "可用向量：\n\t"; std::copy(banker._available.begin(), banker._available.end(), std::ostream_iterator<size_t>(std::cout, ", ")); std::cout << '\n';
    bool safety{ banker.Check() };
    std::cout << "系统安全性：" << std::boolalpha << safety << "\n";
}

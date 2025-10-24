#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <string>
#include "BinaryTree.hpp"

struct HonoiTower { // 定义汉诺塔（HonoiTower）数学模型
    size_t countDisks; // 磁盘的总数量
    size_t countPoles; // 柱子的总数量
    using PoleStack = std::vector<size_t>; // 为每个柱子定义栈类型（使用vector模拟，存储磁盘大小）
    std::vector<PoleStack> poles; // 存储所有柱子的栈容器

    HonoiTower(size_t countDisks, size_t countPoles) : countDisks{ countDisks }, countPoles{ countPoles }, poles(countPoles, PoleStack{}) { // 构造函数：初始化磁盘数、柱子数，并为所有柱子创建空栈
        for (size_t i{ countDisks }; i > 0; --i) { // 循环从最大磁盘号开始递减初始化
            poles[0].push_back(i); // 将磁盘按从大到小顺序放入第一个柱子（柱子0）
        }
    }
    PoleStack& GetPole(size_t pole) { // 获取指定柱子的栈（非常量版本，允许修改）
        return poles.at(pole); // 返回指定柱子的引用，使用at进行边界检查
    }
    const PoleStack& GetPole(size_t pole) const { // 获取指定柱子的栈（常量版本，用于只读访问）
        return poles.at(pole); // 返回指定柱子的常量引用
    }
    std::string PrintPole(size_t poleIndex) const { // 将指定柱子的磁盘状态格式化为字符串
        std::stringstream ss; // 创建字符串流对象用于构建字符串
        const PoleStack& stack{ poles.at(poleIndex) }; // 获取指定柱子的栈
        bool first{ true }; // 标记是否为第一个磁盘，用于控制逗号输出
        for (auto bitr{ std::cbegin(stack) }; bitr != std::cend(stack); ++bitr) { // 遍历柱子栈上的所有磁盘
            const auto plate{ *bitr }; // 获取当前磁盘的大小
            if (!first) { // 如果不是第一个磁盘，需要在前面加逗号分隔
                ss << ", "; // 向字符串流添加逗号和空格
            }
            ss << plate; // 将磁盘大小添加到字符串流
            first = false; // 将first标记设为false，后续磁盘需要分隔符
        }
        return ss.str(); // 返回构建好的字符串
    }
    std::string PrintAll() const { // 打印所有柱子的当前状态
        std::stringstream ss; // 创建字符串流对象
        for (size_t i{ 0 }; i < countPoles; ++i) { // 遍历每一个柱子
            ss << '#' << i << '\t' << PrintPole(i) << '\n'; // 格式化输出柱子编号和其磁盘状态
        }
        return ss.str(); // 返回整个汉诺塔状态的字符串
    }
    void HonoiMove(size_t fromPole, size_t toPole) { // 执行移动操作：将一个磁盘从源柱子移动到目标柱子
        std::cout << "Move from #" << fromPole << " to #" << toPole << ".\n"; // 打印移动动作信息
        PoleStack& fromPoleStack{ GetPole(fromPole) }; // 获取源柱子的栈引用
        PoleStack& toPoleStack{ GetPole(toPole) }; // 获取目标柱子的栈引用
        if (fromPoleStack.empty()) { // 检查源柱子是否为空（没有磁盘可移动）
            throw std::runtime_error{ "There is no plate on 'from' pole" }; // 抛出运行时异常
        }
        const auto plate{ fromPoleStack.back() }; // 获取源柱子顶部的磁盘（最大的磁盘）
        fromPoleStack.pop_back(); // 从源柱子移除顶部磁盘
        toPoleStack.push_back(plate); // 将磁盘添加到目标柱子顶部
        std::cout << "Current State:\n" << PrintAll() << '\n'; // 打印移动后的汉诺塔状态
    }
    void Honoi(size_t countDisks, size_t poleStart, size_t poleTransit, size_t poleDestination) { // 递归解决汉诺塔问题：移动n个磁盘从起点经中转到终点
        if (countDisks == 1) { // 基准情况：如果只有一个磁盘，直接移动
            HonoiMove(poleStart, poleDestination); // 直接将磁盘从起点移到终点
        }
        else { // 递归情况：多个磁盘时
            Honoi(countDisks - 1, poleStart, poleDestination, poleTransit); // 先将上面n-1个磁盘从起点经终点移到中转柱
            HonoiMove(poleStart, poleDestination); // 将最大的磁盘从起点直接移到终点
            Honoi(countDisks - 1, poleTransit, poleStart, poleDestination); // 最后将n-1个磁盘从中转柱经起点移到终点
        }
    }
    void Solve() { // 解决汉诺塔问题的入口函数
        std::cout << "Current State:\n" << PrintAll() << std::endl; // 打印初始状态
        Honoi(countDisks, 0, 1, 2); // 调用递归函数，从柱子0经柱子1移动到柱子2
    }
};

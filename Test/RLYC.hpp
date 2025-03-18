#pragma once

// “人狼羊菜”问题
// 问题抽象为：
// 从状态 `0b0000` 出发，寻找全部由安全状态构成的状态序列，直到最终目标 `0b1111`
// 状态序列中，每个状态都可以由它的前驱通过“人带一样东西划船过河”的动作到达
// 序列中不能出现重复状态

#include <iostream>
#include <vector>
#include <queue>
#include <cassert>
#include <bitset>
#include <sstream>

enum Role : std::uint32_t {
    FARMER      /* 人 */ = 0x08,
    WOLF        /* 狼 */ = 0x04,
    CABBAGE     /* 菜 */ = 0x02,
    GOAT        /* 羊 */ = 0x01
};

struct State {
    std::uint32_t v;

    State(std::uint32_t v) : v{ v } {}

    bool IsPresent(Role role) const { // 确定每个角色位置
        return (v & static_cast<std::uint32_t>(role)) != 0;
    }

    bool IsValid() const { // 检查状态是否合法，合法则返回 true，否则返回 false
        return !(
            IsPresent(Role::GOAT) == IsPresent(Role::CABBAGE)
            && IsPresent(Role::GOAT) != IsPresent(Role::FARMER) /* 羊和菜不能共存，羊会吃掉菜 */
            || IsPresent(Role::GOAT) == IsPresent(Role::WOLF)
            && IsPresent(Role::GOAT) != IsPresent(Role::FARMER) /* 狼和羊不能共存，狼会吃掉羊 */
        );
    }
    operator std::uint32_t() const {
        return this->v;
    }
    operator std::string() const {
        std::stringstream ss;
        if (!IsPresent(Role::FARMER))
            ss << R"( 人 )";
        if (!IsPresent(Role::WOLF))
            ss << R"( 狼 )";
        if (!IsPresent(Role::CABBAGE))
            ss << R"( 菜 )";
        if (!IsPresent(Role::GOAT))
            ss << R"( 羊 )";
        if (ss.str().size() == 0)
            ss << R"( 空 )";
        return ss.str();
    }
    State operator&(std::uint32_t v) const {
        return v & this->v;
    }
    State operator~() const {
        return ~v;
    }
};

inline void Solve() {
    // 记录已访问过的状态，以及已被发现的能够到达当前状态的路径
    // 它的第 i 个元素表示状态 i 是否已被访问过
    // 若 route[i] 已被访问过，记入前驱状态值
    // 用 (~0) 表示未被访问过
    static const std::uint32_t initState{ static_cast<std::uint32_t>(~0) }; // 初始状态
    std::vector<State> route(4 * 4 /* 可能的状态的个数 */, initState);

    // 记录可以安全到达的中间状态
    std::queue<State> moveTo;

    // 初始化
    moveTo.push(0x00);

    while (!moveTo.empty() /* 队列非空 */
        && route[15] == initState /* 还没到达最终状态 */) {
        State status{ moveTo.front() }; moveTo.pop(); // 取出当前状态
        for (std::uint32_t movers /* 待移动的东西 */ = 1;
            movers <= 8; movers <<= 1) {
            // 依次尝试携带不同东西（羊、狼、菜）
            if (static_cast<bool>(status & movers)
                == status.IsPresent(Role::FARMER)) { // 判断农夫和待移动的东西是否在同侧
                // 人总是在移动，随着人移动的，只能是在人同侧的东西
                State newStatus{ status ^ (movers | 0x08) };
                if (newStatus.IsValid() && route[newStatus] == initState) {
                    // 安全的，并且未考虑过的走法
                    route[newStatus] = status;
                    moveTo.push(newStatus);
                }
            }
        }
    }

    // 反向打印出路径
    if (route[15] == initState) { // 无解
        std::cout << "No solution!\n";
        return;
    }
    std::cout << "The reverse path is:\n";
    for (State status = 15; status != 0; status = route[status]) {
        std::cout << "\t" << std::bitset<4>(status)
            << '(' << R"( 源岸剩下： )" << static_cast<std::string>(status) << ')'
            << '(' << R"( 终岸剩下： )" << static_cast<std::string>(~status) << ')'
            << '\t' << std::boolalpha << status.IsValid()
            << '\n';
    }
}


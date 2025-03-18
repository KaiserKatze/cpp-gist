#pragma once
// 五人提灯过独木桥
// 有一盏灯能使用30秒，要在灯熄灭前过这座桥
// 一家五口人每个人过桥的速度不同：
//      哥哥 1 秒
//      弟弟 3 秒
//      爸爸 6 秒
//      妈妈 8 秒
//      奶奶 12 秒
// 每次只能过两个人
// 每次有人过去后，要有一个人再把灯送回来

#include "Graph.hpp"
#include <iostream>
#include <ranges>
#include <bitset>
#include <cassert>
#include <vector>
#include <queue>

void Solve() {
    static constexpr int ppl{ 5 }; // 人数
    static constexpr int n{ ppl * ppl }; // 状态数（人数的平方）
    const std::string names[ppl]{ // 每个人的称谓
        "哥哥", "弟弟", "爸爸", "妈妈", "奶奶",
    };
    const int deeps[ppl]{ // 每个人的慢度
        1, 3, 6, 8, 12,
    };
    const int availableTime = 30; // 灯的初始可用时间

    std::vector<int> route(n, -1);
    std::vector<int> weight(n, INT_MAX);
    std::queue<int> moveTo;

    moveTo.push(0x00); // 一开始每个人都不在对岸
    while (!moveTo.empty() && route[n - 1] == -1) {
        int status{ moveTo.front() }; moveTo.pop();
        std::cout << "当前状态是" << std::bitset<ppl>(status) << '\n';
        for (int i = 0; i < ppl; ++i) {
            if ((moveTo.size() & 1) == 0) // 正在前往对岸
                for (int j = i + 1; j <= ppl; ++j) {
                    int mover1{ 1 << i };
                    int mover2{ 1 << j };
                    int deeps1{ deeps[i] };
                    int deeps2{};
                    if (j < ppl) {
                        deeps2 = deeps[j];
                        std::cout << "尝试让" << names[i] << "和" << names[j] << "一起过河 ...\n";
                    }
                    else {
                        mover2 = 0;
                        deeps2 = 0;
                        std::cout << "尝试让" << names[i] << "独自过河 ...\n";
                    }
                    int movers{ mover1 | mover2 };
                    int newStatus{ status ^ movers };
                    route[newStatus] = status;
                    weight[newStatus] = std::max(deeps1, deeps2);
                    moveTo.push(newStatus);
                    if (newStatus == n - 1) {
                        std::cout << "发现新路径!\n";
                    }
                }
            else { // 正在返回

            }
        }
    }
    std::cout << "求最短路径 ...\n";
}

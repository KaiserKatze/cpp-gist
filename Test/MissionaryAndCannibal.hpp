#pragma once

// “传教士和食人族”问题

#include <iostream>
#include <vector>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <algorithm>

struct MissionaryAndCannibal {
    struct State {
        const int countMax;     // 传教士、食人族的个数
        int nMissionaries;      // 当前状态下，在左岸的传教士个数
        int nCannibals;         // 当前状态下，在左岸的食人族个数
        bool direction;          // 确定船只航向方向

        State(int countMax, int nMissionaries, int nCannibals, bool direction) :
            countMax{ countMax },
            nMissionaries{ nMissionaries },
            nCannibals{ nCannibals },
            direction{ direction } {
            if (nMissionaries > countMax || nMissionaries < 0) {
                throw std::invalid_argument{ "Invalid `nMissionaries`!" };
            }
            if (nCannibals > countMax || nCannibals < 0) {
                throw std::invalid_argument{ "Invalid `nCannibals`!" };
            }
        }

        bool IsValid() const {  // 检查状态是否合法，合法则返回 true，否则返回 false
            const auto ml{ GetMissionaryLeft() };
            const auto mr{ GetMissionaryRight() };
            const auto cl{ GetCannibalLeft() };
            const auto cr{ GetCannibalRight() };
            return (ml == 0 || ml >= cl) && (mr == 0 || mr >= cr);
        }

        int GetMissionaryLeft() const { return nMissionaries; }

        int GetCannibalLeft() const { return nCannibals; }

        int GetMissionaryRight() const { return countMax - nMissionaries; }

        int GetCannibalRight() const { return countMax - nCannibals; }

        int GetMissionary() const {
            return direction ? GetMissionaryLeft() : GetMissionaryRight();
        }

        int GetCannibal() const {
            return direction ? GetCannibalLeft() : GetCannibalRight();
        }

        int GetDirectionAsInt() const {  // 返回船只朝向（朝右返回1，朝左返回-1）
            return direction ? 1 : -1;
        }

        size_t GetDirectionAsIndex() const {  // 返回船只朝向（朝右返回1，朝左返回0）
            return direction ? 1 : 0;
        }

        State Move(int deltaMissionaries, int deltaCannibals) const {
            return State{
                countMax,
                nMissionaries - GetDirectionAsInt() * deltaMissionaries,
                nCannibals - GetDirectionAsInt() * deltaCannibals,
                !direction,  // 调转船头
            };
        }

        explicit operator std::string() const {
            std::stringstream ss;
            ss << "状态( "
                << "左岸传教士=" << GetMissionaryLeft() << ", "
                << "左岸食人族=" << GetCannibalLeft() << ", "
                << "右岸传教士=" << GetMissionaryRight() << ", "
                << "右岸食人族=" << GetCannibalRight() << ", "
                << "船头朝向=" << (direction ? "朝右" : "朝左") << ", "
                << ")";
            return ss.str();
        }

        bool operator==(const State& other) const {
            return countMax == other.countMax
                && nMissionaries == other.nMissionaries
                && nCannibals == other.nCannibals
                && direction == other.direction;
        }

        bool operator!=(const State& other) const {
            return countMax != other.countMax
                || nMissionaries != other.nMissionaries
                || nCannibals != other.nCannibals
                || direction != other.direction;
        }
    };

static void Solve(const int countMax /* 传教士、食人族的个数 */,
const int shipCapacity /* 船只最大可以容纳的乘客个数 */) {
        // 初始状态
        const State initialState{ countMax, countMax, countMax, true /* 船头朝右 */ };
        // 目标状态
        const State finalState{ countMax, 0, 0, false /* 船头朝左 */ };

        // 记录访问标记
        const size_t dimension{ static_cast<size_t>(countMax) + 1 };
        std::vector<std::vector<std::vector<bool>>> visited(dimension, std::vector<std::vector<bool>>(dimension, std::vector<bool>(2, false)));
        // 记录合法状态
        std::vector<State> validStates;
        // 记录路径（在 `validStates` 中的序号）
        std::vector<int> route;
        // 缓存需要探索的状态（在 `validStates` 中的序号）
        std::queue<int, std::deque<int>> moveTo;
        // 是否抵达目标状态
        bool reachFinalState{ false };

        const auto IsVisited = [&](const State& state) {
            return visited[state.nMissionaries][state.nCannibals][state.GetDirectionAsIndex()];
        };
        const auto Visit = [&](const State& state) {
            visited[state.nMissionaries][state.nCannibals][state.GetDirectionAsIndex()] = true;
        };
        const auto AddValidState = [&](int prec, const State& next) {
            Visit(next);
            route.push_back(prec);              // `next` 的前趋状态在 `validStates` 中的序号
            moveTo.push(validStates.size());    // 记录 `next` 在 `validStates` 中的序号
            validStates.push_back(next);        // 存储合法状态
        };
        const auto GetState = [&](int indexInvalidState) {
            return validStates.at(static_cast<size_t>(indexInvalidState));
        };

        AddValidState(-1, initialState);  // 初始状态是合法状态，初始状态没有前趋状态

        while (!moveTo.empty()  // 还有需要检查的中间状态
            && !reachFinalState) {  // 尚未出现连接初始状态和目标状态的合法路径
            // 取出当前状态（先取出一个序号，再到 `validStates` 中查表）
            const int indexCurrentState{ moveTo.front() }; moveTo.pop();
            const State& currentState{ GetState(indexCurrentState) };
            // 依次尝试不同的乘船安排
            const int supMissionary{ min(shipCapacity, currentState.GetMissionary()) };  // 可以乘船的传教士人数
            for (int x{ 0 }; x <= supMissionary; ++x) {
                const int supCannibal{ min(shipCapacity - x, currentState.GetCannibal()) };  // 可以乘船的食人族人数
                for (int y{ 0 }; y <= supCannibal; ++y) {
                    if (x + y == 0) {  // 传教士和食人族都不动
                        continue;  // 抛弃这种没用的乘船安排
                    }
                    const State newState{ currentState.Move(x, y) };
                    const bool isNewStateValid{ newState.IsValid() };
                    const bool isNewStateVisited{ IsVisited(newState) };
                    const bool isFinalState{ newState == finalState };
                    if (!isNewStateValid) {  // 状态非法（食人族把传教士吃掉了）
                        continue;
                    }
                    if (isNewStateVisited) {  // 已经访问过这个中间状态了
                        continue;
                    }
                    reachFinalState = isFinalState;
                    AddValidState(indexCurrentState, newState);  // 更新合法状态表，更新需要探索的状态
                }
            }
        }

        if (validStates.back() != finalState) {
            std::cout << "没有找到通往目标状态的合法路径!\n";
        } else {
            std::cout << "成功找到通往目标状态的合法路径：\n";
            std::stack<State, std::vector<State>> backtrace;
            int prec{ *std::crbegin(route) };
            backtrace.push(validStates.back());
            while (prec != -1) {
                backtrace.push(validStates.at(prec));
                prec = route.at(prec);
            }
            size_t index{ 0 };
            // std::stringstream ss;
            // ss << '[';
            while (!backtrace.empty()) {
                State& node{ backtrace.top() };
                std::cout << "#" << ++index
                    << '\t'
                    << static_cast<std::string>(node)
                    << '\n';
                // ss << '[' << node.GetMissionaryLeft() << ", " << node.GetCannibalLeft() << ", " << node.GetDirectionAsIndex() << "], ";
                backtrace.pop();
            }
            // ss << ']';
            // std::cout << ss.str() << '\n';
        }
    }
};

// @see: https://www.dmlett.com/archive/v11/DML23_v11_pp84-90.pdf

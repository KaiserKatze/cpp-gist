#pragma once

// @see: https://www.bilibili.com/video/BV1ZK411K7A8/

#include <initializer_list>
#include <stack>
#include <queue>
#include <vector>
#include <sstream>
#include <string>
#include <iostream>

template <int N>
struct EightQueens {
    template <class E> using TL = E[N][N];

    TL<bool> attack;
    TL<char> pieces;
    int row;

    EightQueens() : row{ 0 } {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                attack[i][j] = false;
                pieces[i][j] = '.';
            }
        }
    }

    static constexpr bool IsValid(size_t x, size_t y) {
        return 0 <= x && x < N && 0 <= y && y < N;
    }

    bool PutQueen(size_t x, size_t y) {
        // 尝试在第 x 行、第 y 列放入一个皇后
        //std::cerr << "PutQueen(x=" << x << ", y=" << y << ").\n";
        if (!IsValid(x, y)) {
            // 行数、列数非法
            return false;
        }
        if (attack[x][y]) {
            // 指定坐标已被皇后攻击，不可放入
            return false;
        }
        for (size_t i = 0; i < N; i++) {
            // 攻击第 x 行
            attack[x][i] = true;
            // 攻击第 y 列
            attack[i][y] = true;
        }

        // 攻击对角线、反对角线
        for (int dx : {1, -1}) {
            for (int dy : {1, -1}) {
                size_t x1{ x + dx };
                size_t y1{ y + dy };
                //std::cerr << "diag(x=" << x
                //    << ", dx=" << dx
                //    << ", y=" << y
                //    << ", dy=" << dy
                //    << ", x1=" << x1
                //    << ", y1=" << y1
                //    << ").\n";
                while (IsValid(x1, y1)) {
                    //std::cerr << "attack[" << x1 << "][" << y1 << "]!\n";
                    attack[x1][y1] = true;
                    x1 += dx;
                    y1 += dy;
                }
                //std::cerr << "diag(x=" << x
                //    << ", dx=" << dx
                //    << ", y=" << y
                //    << ", dy=" << dy << "). done\n";
            }
        }

        // 在第 x 行、第 y 列放入皇后
        pieces[x][y] = 'Q';
        return true;
    }

    std::string ToString() const {
        std::stringstream ss;
        ss << "Solution [\n";
        for (size_t i = 0; i < N; i++) {
            ss << '\t';
            for (size_t j = 0; j < N; j++) {
                ss << pieces[i][j];
            }
            ss << '\t';
            for (size_t j = 0; j < N; j++) {
                ss << (attack[i][j] ? 1 : 0);
            }
            ss << '\n';
        }
        ss << "].";
        return ss.str();
    }
};

template <int N>
static auto SolveEightQueensProblem() {
    using P = EightQueens<N>;
    std::queue<P> problem;
    std::vector<P> solutionSpace;

    problem.push({});
    while (!problem.empty()) {
        P p = problem.front();
        problem.pop();
        if (p.row == N) { // 找到一个解
            solutionSpace.push_back(p);
            continue; // 继续找下一个解
        }
        for (size_t j = 0; j < N; j++) {
            P q = p; // 备份
            if (q.PutQueen(q.row, j)) {
                ++q.row;
                problem.push(q);
            }
        }
    }

    return solutionSpace;
}

void TestEightQueens() {
    auto sol = SolveEightQueensProblem<8>();
    std::cout << "Result count: " << sol.size() << '\n';
    for (const auto& s : sol) {
        std::cout << s.ToString() << '\n';
    }
}

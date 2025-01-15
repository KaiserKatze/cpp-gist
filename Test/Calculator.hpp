#pragma once
// https://www.geeksforgeeks.org/infix-postfix-prefix-notation/
// https://www.calcont.in/Conversion/infix_to_postfix

#include <vector>
#include <stack>
#include <sstream>
#include <string>

struct Calculator { // 利用栈，把中缀表达式转化为后缀表达式
    std::stack<char, std::vector<char>> opStack;
    std::stringstream output;

    void clear() { // 清空栈与输出，恢复为初始状态
        while (!opStack.empty()) {
            opStack.pop();
        }
        output.str("");
        output.clear();
    }

    static constexpr uint32_t priority(char op) {
        switch (op) {
        case '+':
        case '-':
            return 0x1;
        case '*':
        case '/':
            return 0x10;
        case '(':
        case ')':
            return 0x1000;
        default:
            return 0;
        }
    }

    void handle_operator(char c) {
        char c1;
        if (!opStack.empty()) { // 栈非空
            c1 = opStack.top(); // 栈顶运算符
            while (c1 != '(' && priority(c1) >= priority(c)) { // 栈顶运算符优先级不低于当前扫描的运算符
                opStack.pop(); // 栈顶运算符出栈
                output << c1; // 栈顶运算符加入后缀表达式
                if (opStack.empty()) { // 栈空
                    break; // 退出循环，不再检查栈顶
                }
                c1 = opStack.top(); // 获取新的栈顶
            }
        }
        opStack.push(c);
    }

    void handle_right_brace() {
        // 遇到右界限符，将栈中运算符依次出栈，直到遇到左界限符为止
        char c1;
        do {
            c1 = opStack.top();
            opStack.pop();
            if (c1 == '(') { // 左界限符直接删除，不加入后缀表达式
                break;
            }
            output << c1;
        } while (!opStack.empty());
    }

    void operator<<(char c) {
        switch (c) {
        case ' ': // 忽略空格
            return;
        case '+':
        case '-':
        case '*':
        case '/':
            handle_operator(c); // 处理运算法
            break;
        case '(':
            opStack.push(c); // 界限符直接入栈
            break;
        case ')':
            handle_right_brace();
            break;
        default:
            if ('a' <= c && c <= 'z'
                || 'A' <= c && c <= 'Z'
                || '0' <= c && c <= '9')
                output << c; // 操作数直接加入后缀表达式
            break;
        }
    }

    void operator<<(const char* s) {
        for (; *s != '\0'; ++s) {
            (*this) << *s;
        }
        // 扫描完所有字符后，将栈中剩余运算符依次出栈，并加入后缀表达式
        while (!opStack.empty()) {
            output << opStack.top();
            opStack.pop();
        }
    }

    explicit operator std::string() const {
        return output.str();
    }
};

void Test() {
    Calculator cc;

    cc << "(A+(B))*((C-D*G/H+I-J*K)-E)/F";
    // expect: AB+CDG*H/-I+JK*-E-*F/
    std::cout << static_cast<std::string>(cc) << '\n';

    cc.clear();
    std::cout << static_cast<std::string>(cc) << '\n';
}
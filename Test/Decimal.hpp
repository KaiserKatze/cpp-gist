#pragma once

#include <array>
#include <vector> // std::vector
#include <string> // std::string
#include <compare> // sign_type (since C++20)
#include <stdexcept> // std::invalid_argument
#include <cassert> // assert
#include <sstream> // std::stringstream
#include <functional> // std::cref
#include <numeric> // std::accumulate
#include <iostream>
#include <type_traits>

struct Decimal { // 高精度十进制数字（支持任意精度的小数）
    using sign_type = std::strong_ordering;
    using int_type = int;
    static_assert(std::is_signed_v<int_type>);
    using data_type = std::vector<int_type>;
    using const_iterator = typename data_type::const_iterator;
    using iterator = typename data_type::iterator;
    using const_pair_type = std::pair<int_type, int_type>;

    sign_type sign; // 符号位
    data_type integral_part; // 整数部分
    data_type fractional_part; // 小数部分

    Decimal() : sign{ sign_type::equal } { // 构造零
    }

    Decimal(const std::string& input) {
        auto begin{ input.cbegin() };
        auto end{ input.cend() };
        if (begin == end) { // 空字符串
            throw std::invalid_argument{ "Invalid argument: empty string!" };
        }
        for (; begin != end && *begin == ' '; ++begin); // 跳过空格
        if (begin == end) { // 字符串中只有空格
            throw std::invalid_argument{ "Invalid argument: only spaces!" };
        }
        switch (*begin) { // 判断符号位
        case '-':
            sign = sign_type::less;
            ++begin;
            break;
        case '+':
            ++begin;
            [[fallthrough]];
        default:
            sign = sign_type::greater;
            break;
        }
        if (begin == end) { // 字符串中只有符号，没有数字
            throw std::invalid_argument{ "Invalid argument: any sign should be followed by digits!" };
        }
        for (; begin != end && *begin == '0'; ++begin); // 跳过整数部分头部的零
        if (begin == end) { // 数值部分全是零
            sign = sign_type::equal;
            return;
        }
        for (; begin != end; ++begin) { // 逐字复制整数部分
            if (*begin == '.') { // 遇到小数点
                ++begin;
                break;
            }
            if (*begin < '0' || *begin > '9') {
                throw std::invalid_argument{ "Invalid argument: any sign should be followed by digits!" };
            }
            integral_part.push_back(*begin - '0');
        }
        if (begin == end) { // 没有小数部分
            if (integral_part.empty()) { // 整数部分全是零
                sign = sign_type::equal;
            }
            return;
        }
        while (end != begin) { // 跳过小数部分尾部的零
            --end;
            if (*end == '0') { // 最后一个字符是零
                continue;
            }
            ++end; // 最后一个字符不是零
            break;
        }
        if (begin == end && *begin == '0') { // 小数部分全是零
            if (integral_part.empty()) { // 整数部分、小数部分全是零
                sign = sign_type::equal;
            }
            return;
        }
        for (; begin != end; ++begin) { // 逐字复制小数部分
            if (*begin < '0' || *begin > '9') {
                throw std::invalid_argument{ "Invalid argument: any sign should be followed by digits!" };
            }
            fractional_part.push_back(*begin - '0');
        }
        assert(begin == end);
    };

    explicit operator std::string() const {
        if (sign == sign_type::equal) {
            return "0.0";
        }
        std::stringstream out;
        if (sign == sign_type::less) {
            out << '-';
        }
        for (int_type digit : integral_part) {
            //digit += '0';
            out << digit;
        }
        if (fractional_part.empty()) {
            out << ".0";
        }
        else {
            out << '.';
            for (int_type digit : fractional_part) {
                //digit += '0';
                out << digit;
            }
        }
        return out.str();
    }

    int_type at(size_t index) const {
        if (0 > index) {
            throw std::invalid_argument{ "Invalid argument: index too small!" };
        }
        const size_t size_integral{ this->integral_part.size() };
        const size_t size_fractional{ this->fractional_part.size() };
        const size_t size_total{ size_integral + size_fractional }; //
        if (index >= size_total) {
            return 0;
        }
        if (index < size_integral) {
            return this->integral_part[index];
        }
        index -= size_integral;
        return this->fractional_part[index];
    }

    void _clear() {
        this->sign = sign_type::equal;
        this->integral_part.clear();
        this->fractional_part.clear();
    }

    void _shrink_integral() {
        const_iterator begin{ integral_part.cbegin() };
        const_iterator end{ integral_part.cend() };
        while (begin != end) {
            if (*begin++ != 0) {
                --begin;
                break;
            }
        }
        integral_part.erase(integral_part.cbegin(), begin);
    }

    void _shrink_fractional() {
        const_iterator begin{ fractional_part.cbegin() };
        const_iterator end{ fractional_part.cend() };
        while (begin != end) {
            if (*--end != 0) {
                ++end;
                break;
            }
        }
        fractional_part.erase(end, fractional_part.cend());
    }

    void _shrink() {
        _shrink_fractional();
        _shrink_integral();
    }

    size_t size() const {
        return this->integral_part.size() + this->fractional_part.size();
    }

    static sign_type _compare_abs(const Decimal& lhs, const Decimal& rhs) {
        {
            size_t size_lhs_integral{ lhs.integral_part.size() };
            size_t size_rhs_integral{ rhs.integral_part.size() };
            sign_type cmp_result_integral{ size_lhs_integral <=> size_rhs_integral };
            if (cmp_result_integral != 0) { // 两者整数部分不等长
                return cmp_result_integral; // 整数部分越长的，绝对值越大
            }
            const_iterator begin_lhs_integral{ lhs.integral_part.cbegin() };
            const_iterator end_lhs_integral{ lhs.integral_part.cend() };
            const_iterator begin_rhs_integral{ rhs.integral_part.cbegin() };
            while (begin_lhs_integral != end_lhs_integral) { // 两者整数部分等长，从高到低逐位比较
                if (*begin_lhs_integral != *begin_rhs_integral) {
                    return *begin_lhs_integral <=> *begin_rhs_integral;
                }
                ++begin_lhs_integral;
                ++begin_rhs_integral;
            }
        }
        {
            const_iterator begin_lhs_fractional{ lhs.fractional_part.cbegin() };
            const_iterator end_lhs_fractional{ lhs.fractional_part.cend() };
            const_iterator begin_rhs_fractional{ rhs.fractional_part.cbegin() };
            const_iterator end_rhs_fractional{ rhs.fractional_part.cend() };
            while (begin_lhs_fractional != end_lhs_fractional && begin_rhs_fractional != end_rhs_fractional) { // 从高到低逐位比较两者的小数部分
                if (*begin_lhs_fractional != *begin_rhs_fractional) {
                    return *begin_lhs_fractional <=> *begin_rhs_fractional;
                }
                ++begin_lhs_fractional;
                ++begin_rhs_fractional;
            }
            if (begin_lhs_fractional == end_lhs_fractional && begin_rhs_fractional == end_rhs_fractional) { // 两者完全一致
                return sign_type::equal;
            }
            if (begin_lhs_fractional != end_lhs_fractional) { // 前者小数部分更长，故绝对值更大
                return sign_type::greater;
            }
            return sign_type::less; // 后者小数部分更长，绝对值更大
        }
    }

    static sign_type _compare(const Decimal& lhs, const Decimal& rhs) {
        if (lhs.sign == 0) {
            return 0 <=> rhs.sign;
        }
        return (lhs.sign == rhs.sign)
            ? _compare_abs(lhs, rhs) // 符号相同，根据绝对值判断大小
            : lhs.sign; // 符号不相同时，直接根据符号判断大小
    }

    static Decimal& _add(Decimal& dest, const Decimal& lhs, const Decimal& rhs) { // 两个符号相同的数字相加
        assert(lhs.sign != 0);
        assert(lhs.fractional_part.size() > 0 || lhs.integral_part.size() > 0);
        assert(rhs.sign != 0);
        assert(rhs.fractional_part.size() > 0 || rhs.integral_part.size() > 0);
        int_type carry{ 0 };
        { // 首先计算小数部分
            const data_type* data_lhs{ &lhs.fractional_part };
            const data_type* data_rhs{ &rhs.fractional_part };
            data_type& data_dest{ dest.fractional_part };
            size_t size_lhs{ data_lhs->size() };
            size_t size_rhs{ data_rhs->size() };
            if (size_lhs < size_rhs) { // 小数部分的位数不一致
                std::swap(data_lhs, data_rhs); // 交换顺序，使得 data_lhs 的位数不小于 data_rhs 的位数
                std::swap(size_lhs, size_rhs);
            }
            const_iterator begin_lhs{ data_lhs->cbegin() };
            const_iterator end_lhs{ data_lhs->cend() };
            if (size_lhs != size_rhs) { // 小数部分的位数不一致
                if (&data_dest != data_lhs) { // 避免自赋值
                    assert(&data_dest == data_rhs);
                    std::advance(begin_lhs, size_rhs); // 调整 begin_lhs 的位置
                    data_dest.insert(data_dest.cend(), begin_lhs, end_lhs); // 复制小数部分
                    assert(data_dest.size() == size_lhs);
                    end_lhs = begin_lhs; // 调整 end_lhs 的位置
                    begin_lhs = data_lhs->cbegin(); // 调整 begin_lhs 的位置
                }
                else { // 只修改 end_lhs 的位置
                    end_lhs = begin_lhs; // 调整 end_lhs 的位置
                    std::advance(end_lhs, size_rhs); // 这里不能改为 `std::advance(end_lhs, size_rhs - size_lhs);`
                }
            }
            const_iterator end_rhs{ data_rhs->cbegin() };
            std::advance(end_rhs, size_rhs);
            iterator end_dest{ data_dest.begin() };
            std::advance(end_dest, size_rhs);
            while (begin_lhs != end_lhs) { // 处理等长的部分
                const int_type& num_lhs{ *--end_lhs };
                const int_type& num_rhs{ *--end_rhs };
                const int_type sum{ num_lhs + num_rhs + carry };
                carry = sum / 10;
                *--end_dest = sum % 10;
            }
        }
        { // 然后计算整数部分
            const data_type* data_lhs{ &lhs.integral_part };
            const data_type* data_rhs{ &rhs.integral_part };
            data_type& data_dest{ dest.integral_part };
            size_t size_lhs{ data_lhs->size() };
            size_t size_rhs{ data_rhs->size() };
            if (size_lhs < size_rhs) { // 整数部分的位数不一致
                std::swap(data_lhs, data_rhs); // 交换顺序，使得 data_lhs 的位数不小于 data_rhs 的位数
                std::swap(size_lhs, size_rhs);
            }
            const_iterator begin_lhs{ data_lhs->cbegin() };
            const_iterator end_lhs{ data_lhs->cend() };
            if (size_lhs != size_rhs) { // 整数部分的位数不一致
                if (&data_dest != data_lhs) { // 避免自赋值
                    assert(&data_dest == data_rhs);
                    end_lhs = begin_lhs;
                    std::advance(end_lhs, size_lhs - size_rhs); // 调整 begin_lhs 的位置
                    data_dest.insert(data_dest.cbegin(), begin_lhs, end_lhs); // 复制整数部分
                    assert(data_dest.size() == size_lhs);
                    begin_lhs = end_lhs; // 调整 begin_lhs 的位置
                    end_lhs = data_lhs->cend(); // 调整 end_lhs 的位置
                }
                else { // 只修改 begin_lhs 的位置
                    std::advance(begin_lhs, size_lhs - size_rhs);
                }
            }
            const_iterator end_rhs{ data_rhs->cend() };
            iterator end_dest{ data_dest.end() };
            while (begin_lhs != end_lhs) { // 处理等长的部分
                const int_type& num_lhs{ *--end_lhs };
                const int_type& num_rhs{ *--end_rhs };
                const int_type sum{ num_lhs + num_rhs + carry };
                carry = sum / 10;
                *--end_dest = sum % 10;
            }
            if (carry != 0) {
                data_dest.insert(data_dest.cbegin(), carry);
            }
        }
        return dest;
    }

    static Decimal& _sub(Decimal& dest, const Decimal& lhs, const Decimal& rhs) { // 两个符号相同的数字相减
        assert(lhs.sign != 0);
        assert(lhs.fractional_part.size() > 0 || lhs.integral_part.size() > 0);
        assert(rhs.sign != 0);
        assert(rhs.fractional_part.size() > 0 || rhs.integral_part.size() > 0);
        const sign_type cmp_result{ _compare_abs(lhs, rhs) };
        if (cmp_result == 0) { // 两者绝对值相同
            dest._clear(); // 绝对值相同的两个数相减得零
            return dest;
        }
        const Decimal* plhs{ &lhs };
        const Decimal* prhs{ &rhs };
        if (cmp_result < 0) { // 前者绝对值更小
            std::swap(plhs, prhs); // 交换顺序，保证前者绝对值更大
            if (lhs.sign < 0) {
                dest.sign = sign_type::greater;
            }
            else {
                dest.sign = sign_type::less;
            }
        }
        else { // if (cmp_result > 0) // 前者绝对值更大
            if (lhs.sign < 0) {
                dest.sign = sign_type::less;
            }
            else {
                dest.sign = sign_type::greater;
            }
        }
        int_type borrow{ 0 };
        { // 先计算小数部分
            const data_type& data_lhs{ plhs->fractional_part };
            const data_type& data_rhs{ prhs->fractional_part };
            const size_t size_lhs{ data_lhs.size() };
            const size_t size_rhs{ data_rhs.size() };
            size_t size_max{ size_lhs };
            size_t size_min{ size_rhs };
            if (size_max < size_min) {
                std::swap(size_max, size_min);
            }
            size_t size_diff{ size_max - size_min };
            const_iterator begin_lhs{ data_lhs.cend() };
            const_iterator begin_rhs{ data_rhs.cend() };
            data_type data_dest(size_max, 0); // 暂存计算结果
            iterator begin_dest{ data_dest.end() };
            for (size_t i{ 0 }; i < size_max; ++i) {
                int_type num_lhs;
                int_type num_rhs;
                assert(size_diff < static_cast<size_t>(std::numeric_limits<ptrdiff_t>::max()));
                ptrdiff_t ptrdiff{ -1 };
                ptrdiff += size_diff - i;
                if (size_lhs != size_max) {
                    if (i < size_diff) {
                        num_lhs = 0;
                    }
                    else {
                        num_lhs = *(begin_lhs + ptrdiff);
                    }
                    num_rhs = *(begin_rhs - 1 - i);
                }
                else { // (size_rhs != size_max)
                    num_lhs = *(begin_lhs - 1 - i);
                    if (i < size_diff) {
                        num_rhs = 0;
                    }
                    else {
                        num_rhs = *(begin_rhs + ptrdiff);
                    }
                }
                int_type result{ num_lhs - num_rhs - borrow };
                if (result < 0) {
                    borrow = 1;
                    result += 10;
                }
                else {
                    borrow = 0;
                }
                *(begin_dest - 1 - i) = result;
            }
            dest.fractional_part = data_dest;
            if (size_diff != 0) {
                dest._shrink_fractional();
            }
        }
        { // 再计算整数部分
            const data_type& data_lhs{ plhs->integral_part };
            const data_type& data_rhs{ prhs->integral_part };
            const size_t size_lhs{ data_lhs.size() };
            const size_t size_rhs{ data_rhs.size() };
            assert(size_lhs >= size_rhs); // 前者绝对值更大，那么前者位数不小于后者
            const_iterator begin_lhs{ data_lhs.cend() };
            const_iterator begin_rhs{ data_rhs.cend() };
            data_type data_dest(size_lhs, 0); // 暂存计算结果
            iterator begin_dest{ data_dest.end() };
            for (size_t i{ 0 }; i < size_lhs; ++i) {
                int_type num_lhs{ *(begin_lhs - 1 - i) };
                int_type num_rhs;
                if (size_rhs != size_lhs && i < size_rhs) {
                    num_rhs = *(begin_rhs - 1 - i);
                }
                else {
                    num_rhs = 0;
                }
                int_type result{ num_lhs - num_rhs - borrow };
                if (result < 0) {
                    borrow = 1;
                    result += 10;
                }
                else {
                    borrow = 0;
                }
                *(begin_dest - 1 - i) = result;
            }
            //assert(borrow == 0); // 绝对值大的数减绝对值小的数，计算到最后，借位一定是零
            dest.integral_part = data_dest;
            if (size_lhs != size_rhs) {
                dest._shrink_integral();
            }
        }
        return dest;
    }

    Decimal& operator+=(const Decimal& other) {
        if (this->sign == sign_type::equal) { // 当前数字是零
            return *this = other;
        }
        if (other.sign == sign_type::equal) { // 另一个数字是零
            return *this;
        }
        if (this->sign == other.sign) { // 两个数字的符号相同
            return this->_add(*this, *this, other);
        }
        if (this->sign > 0) { // 当前数字是正数
            return this->_sub(*this, *this, other);
        }
        else { // 当前数字是负数
            return this->_sub(*this, other, *this);
        }
    }

    Decimal operator+(const Decimal& other) const {
        return Decimal{ *this } += other;
    }

    Decimal& _negate() {
        this->sign = 0 <=> this->sign;
        return *this;
    }

    Decimal operator-() const {
        return Decimal{ *this }._negate();
    }

    Decimal& operator-=(const Decimal& other) {
        if (this->sign == sign_type::equal) { // 当前数字是零
            return *this = -other;
        }
        if (other.sign == sign_type::equal) { // 另一个数字是零
            return *this;
        }
        if (this->sign == other.sign) { // 两个数字的符号相同
            return _sub(*this, *this, other);
        }
        if (this->sign > 0) { // 当前数字是正数
            return _add(*this, *this, other);
        }
        else { // 当前数字是负数
            return _add(*this, other, *this);
        }
    }

    Decimal operator-(const Decimal& other) const {
        return Decimal{ *this } -= other;
    }

    static Decimal& _mul(Decimal& dest, const Decimal& lhs, const Decimal& rhs) {
        const size_t size_fractional_lhs{ lhs.fractional_part.size() };
        const size_t size_fractional_rhs{ rhs.fractional_part.size() };
        const size_t size_integral_lhs{ lhs.integral_part.size() };
        const size_t size_integral_rhs{ rhs.integral_part.size() };
        data_type data_buffer(size_fractional_lhs + size_integral_lhs + size_fractional_rhs + size_integral_rhs, 0);
        {
            const_iterator begin_fractional_lhs{ lhs.fractional_part.cbegin() };
            const_iterator end_fractional_lhs{ lhs.fractional_part.cend() };
            const_iterator begin_integral_lhs{ lhs.integral_part.cbegin() };
            const_iterator end_integral_lhs{ lhs.integral_part.cend() };
            const_iterator begin_fractional_rhs{ rhs.fractional_part.cbegin() };
            const_iterator end_fractional_rhs{ rhs.fractional_part.cend() };
            const_iterator begin_integral_rhs{ rhs.integral_part.cbegin() };
            const_iterator end_integral_rhs{ rhs.integral_part.cend() };
            iterator end_dest{ data_buffer.end() };

            const_iterator itr_lhs_high;
            const_iterator itr_lhs_low;
            const_iterator itr_rhs_high;
            const_iterator itr_rhs_low;
            iterator itr_dest;
            int_type num_lhs;
            int_type num_rhs;
            int_type product;

            using range_type = std::pair<const_iterator, const_iterator>[];
            const range_type range_outer{
                { begin_fractional_lhs, end_fractional_lhs },
                { begin_integral_lhs, end_integral_lhs },
            };
            const range_type range_inner{
                { begin_fractional_rhs, end_fractional_rhs },
                { begin_integral_rhs, end_integral_rhs },
            };

            for (auto&& [begin_lhs, end_lhs] : range_outer) {
                itr_lhs_high = itr_lhs_low = end_lhs;
                while (itr_lhs_high != begin_lhs) {
                    itr_dest = end_dest;
                    num_lhs = *--itr_lhs_high;
                    for (auto&& [begin_rhs, end_rhs] : range_inner) {
                        itr_rhs_high = itr_rhs_low = end_rhs;
                        while (itr_rhs_high != begin_rhs) {
                            num_rhs = *--itr_rhs_high;
                            product = num_lhs * num_rhs;
                            *--itr_dest += product;
                        }
                    }
                    --end_dest;
                }
            }
        }
        {
            int_type carry{ 0 };
            iterator begin_dest{ data_buffer.begin() };
            iterator end_dest{ data_buffer.end() };
            while (end_dest != begin_dest) {
                --end_dest;
                int_type digit{ *end_dest + carry };
                *end_dest = digit % 10;
                carry = digit / 10;
            }
        }
        const size_t size_fractional{ size_fractional_lhs + size_fractional_rhs };
        const size_t size_integral{ size_integral_lhs + size_integral_rhs };
        if (size_fractional == 0) {
            dest.fractional_part.clear();
        }
        else {
            dest.fractional_part.resize(size_fractional);
        }
        if (size_integral == 0) {
            dest.integral_part.clear();
        }
        else {
            dest.integral_part.resize(size_integral);
        }
        {
            const_iterator end_buffer{ data_buffer.cbegin() };
            const_iterator begin_buffer;
            begin_buffer = end_buffer;
            end_buffer += size_integral;
            std::copy(begin_buffer, end_buffer, dest.integral_part.begin());
            begin_buffer = end_buffer;
            end_buffer += size_fractional;
            std::copy(begin_buffer, end_buffer, dest.fractional_part.begin());
        }
        return dest;
    }

    Decimal& operator*=(const Decimal& other) {
        if (this->sign == sign_type::equal) { // 当前数字是零，或另一个数字是零
            return *this;
        }
        if (other.sign == sign_type::equal) {
            this->_clear();
            return *this;
        }
        return _mul(*this, *this, other);
    }

    Decimal operator*(const Decimal& other) const {
        return Decimal{ *this } *= other;
    }

    static sign_type _compare_abs(const_iterator begin_lhs, const_iterator end_lhs,
        const_iterator begin_rhs, const_iterator end_rhs) { // 把两个数都看成整数
        ptrdiff_t size_lhs{ end_lhs - begin_lhs };
        ptrdiff_t size_rhs{ end_rhs - begin_rhs };
        assert(size_lhs >= 0);
        assert(size_rhs >= 0);
        sign_type cmp_result_integral{ size_lhs <=> size_rhs };
        if (cmp_result_integral != sign_type::equal) { // 两者整数部分不等长
            return cmp_result_integral; // 整数部分越长的，绝对值越大
        }
        while (begin_lhs != end_lhs) { // 两者整数部分等长，从高到低逐位比较
            if (*begin_lhs != *begin_rhs) {
                return *begin_lhs <=> *begin_rhs;
            }
            ++begin_lhs;
            ++begin_rhs;
        }
        return sign_type::equal;
    }

    static Decimal& _div(Decimal& dest, const Decimal& lhs, const Decimal& rhs, size_t precision = 28) {
        size_t size_lhs{ lhs.size() };
        size_t size_rhs{ rhs.size() };
        assert(size_lhs != 0);
        assert(size_rhs != 0);
        data_type data_lhs(size_lhs);
        data_type data_rhs(size_rhs);
        data_type data_buffer(precision);
        {
            iterator begin_lhs{ data_lhs.begin() };
            begin_lhs = std::copy(lhs.integral_part.cbegin(), lhs.integral_part.cend(), begin_lhs);
            std::copy(lhs.fractional_part.cbegin(), lhs.fractional_part.cend(), begin_lhs);
        }
        {
            iterator begin_rhs{ data_rhs.begin() };
            begin_rhs = std::copy(rhs.integral_part.cbegin(), rhs.integral_part.cend(), begin_rhs);
            std::copy(rhs.fractional_part.cbegin(), rhs.fractional_part.cend(), begin_rhs);
        }
        iterator begin_lhs{ data_lhs.begin() };
        iterator end_lhs{ data_lhs.end() };
        iterator mid_lhs{ begin_lhs + size_rhs };
        const_iterator begin_rhs{ data_rhs.cbegin() };
        const_iterator end_rhs{ data_rhs.cend() };
        const_iterator mid_rhs{ end_rhs };
        iterator begin_dest{ data_buffer.begin() };
        while (begin_lhs != end_lhs) {
            while (_compare_abs(begin_lhs, mid_lhs, begin_rhs, end_rhs) < 0) {
                if (mid_lhs >= end_lhs) {
                    data_lhs.resize(data_lhs.size() + (mid_lhs - end_lhs) + 1, 0);
                    end_lhs = mid_lhs = data_lhs.end();
                    begin_lhs = data_lhs.begin();
                }
                else {
                    ++mid_lhs;
                }
                *begin_dest++ = 0;
                if (begin_dest == data_buffer.end()) {
                    break;
                }
            }
            if (begin_dest == data_buffer.end()) {
                break;
            }
            int_type num_rhs{ *begin_rhs };
            int_type num_lhs{
                (std::distance(begin_lhs, mid_lhs) == std::distance(begin_rhs, end_rhs))
                ? (*begin_lhs)
                : (*begin_lhs * 10 + *++begin_lhs)
            };
            int_type quotient{ num_lhs / num_rhs }; // 商
            *begin_dest++ = quotient;
            if (begin_dest == data_buffer.end()) {
                break;
            }
            if (begin_lhs != end_lhs) {
                iterator itr_lhs{ begin_lhs };
                const_iterator itr_rhs{ begin_rhs };
                *itr_lhs++ = num_lhs % num_rhs;
                while (itr_rhs != end_rhs) {
                    if (itr_lhs == end_lhs) {
                        ptrdiff_t diff_itr{ std::distance(begin_lhs, itr_lhs) };
                        ptrdiff_t diff_mid{ std::distance(begin_lhs, mid_lhs) };
                        data_lhs.resize(data_lhs.size() + std::distance(itr_rhs, end_rhs), 0);
                        begin_lhs = data_lhs.begin();
                        end_lhs = data_lhs.end();
                        itr_lhs = begin_lhs + diff_itr;
                        mid_lhs = begin_lhs + diff_mid;
                    }
                    *itr_lhs -= quotient * *itr_rhs;
                    ++itr_lhs;
                    ++itr_rhs;
                }
                while (itr_rhs != begin_rhs) {
                    --itr_lhs;
                    --itr_rhs;
                    if (*itr_lhs >= 0) {
                        continue;
                    }
                    *itr_lhs += 10;
                    --*(itr_lhs - 1);
                }
            }
            ++begin_lhs;
        }
        dest.integral_part = data_buffer;
        dest.fractional_part.clear();
        return dest;
    }

    Decimal& operator/=(const Decimal& other) {
        if (other.sign == sign_type::equal) {
            throw std::invalid_argument{ "Invalid argument: zero divisor" };
        }
        if (this->sign == sign_type::equal) {
            return *this;
        }
        return _div(*this, *this, other);
    }

    Decimal operator/(const Decimal& other) const {
        return Decimal{ *this } /= other;
    }

    sign_type operator<=>(const Decimal& other) const {
        return _compare(*this, other);
    }

    bool operator==(const Decimal& other) const {
        return _compare(*this, other) == 0;
    }

    bool operator!=(const Decimal& other) const {
        return _compare(*this, other) != 0;
    }

    bool operator<(const Decimal& other) const {
        return _compare(*this, other) < 0;
    }

    bool operator>(const Decimal& other) const {
        return _compare(*this, other) > 0;
    }

    bool operator<=(const Decimal& other) const {
        return _compare(*this, other) <= 0;
    }

    bool operator>=(const Decimal& other) const {
        return _compare(*this, other) >= 0;
    }

    friend std::ostream& operator<<(std::ostream& out, const Decimal& decimal) {
        out << static_cast<std::string>(decimal);
        return out;
    }

};


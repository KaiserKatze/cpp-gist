#pragma once

#include <algorithm> // std::max
#include <cassert> // assert
#include <sstream> // std::stringstream
#include <string> // std::string
#include <vector> // std::vector
#include <stdexcept> // std::invalid_argument
#include <compare> // std::strong_ordering (since C++20)
#include <type_traits> // std::is_integral_v
#include <iomanip> // std::setw, std::setfill

static const std::exception NotImplemented{ "NotImplemented" };

struct BigInteger;

template <class T>
constexpr bool is_big_integer_v = std::is_base_of_v<BigInteger, T>;

template <class T>
concept IntegralEx = std::is_integral_v<T> || is_big_integer_v<T>;

class DivideByZero : public std::invalid_argument {
public:
    DivideByZero() : std::invalid_argument{ "Divide by zero!" } {}
};

struct BigInteger {
    using sign_type = std::strong_ordering;
    using int_type = std::uint64_t;
    static_assert(std::is_unsigned_v<int_type>);
    using data_type = std::vector<int_type>;
    using const_reverse_iterator = typename data_type::const_reverse_iterator;
    using iterator = typename data_type::iterator;
    using const_iterator = typename data_type::const_iterator;

    sign_type sign; // 大整数的符号位单独处理
    data_type data; // 大整数的数值位表示为若干个64位整数的顺序表

    BigInteger(std::integral auto data)
        : BigInteger(data > 0 ? sign_type::greater : data == 0 ? sign_type::equal : sign_type::less, { static_cast<int_type>(std::abs(data)) }) {
    }

    BigInteger(sign_type sign, const data_type& data) : sign{ sign }, data{ data } {
    }

    BigInteger(sign_type sign, std::initializer_list<int_type> data)
        : sign{ sign }, data{ std::move(data) } {
    }

    BigInteger(const data_type& data) : BigInteger(sign_type::greater, data) {}

    static bool is_numeric(char c) {
        return '0' <= c && c <= '9';
    }

    template <size_t N>
    BigInteger(const char(&number)[N]) { // 读取十进制数字
        throw NotImplemented;
        auto begin{ std::begin(number) };
        if (*begin == '\0') { // 空字符串
            throw std::invalid_argument{ "Invalid argument: empty string!" };
        }
        else if (*begin == '-') { // 第一个字符是负号
            this->sign = sign_type::less; // 暂且判定大整数是负数
            ++begin;
        }
        else {
            if (*begin == '+') { // 第一个字符串是正号
                ++begin;
            }
            else if (!is_numeric(*begin)) { // 第一个字符串不是符号也不是数字
                throw_exception_becoz_invalid_char_in_number:
                std::stringstream ss;
                ss << "Invalid argument: char '"
                    << *begin
                    << "' can't be a part of a number!";
                throw std::invalid_argument{ ss.str() };
            }
            this->sign = sign_type::greater; // 暂且判定大整数是正数
        }
        if (*begin == '\0') { // 字符串中只有符号，没有数字
            throw std::invalid_argument{ "Invalid argument: any sign should be followed by digits!" };
        }
        while (*begin++ == '0'); // 当字符是 '0' 时略过
        if (*begin == '\0') { // 数值部分全是零
            this->sign = sign_type::equal;
            this->data = { 0 };
            return;
        }
        std::vector<char> digits(begin, std::end(number) - 1); // 逐个复制剩余字符（去掉字符串终止符 '\0'）
        for (char& c : digits) {
            c -= '0';
        }
        size_t digits_size{ digits.size() };
        char q;
        char r;
        // TODO
        while (digits_size > 0) {
            for (char& c : digits) {
                q = c / 2;
                r = c % 2;
            }
        }
    }

    static size_t get_size_between_iterators_back2front(const_reverse_iterator& begin, const const_reverse_iterator& end) {
        for (; begin != end && *begin == 0; ++begin); // 如果序列中的整数全为零，得到的返回值就是零
        const ptrdiff_t size{ std::distance(begin, end) };
        assert(size >= 0);
        return static_cast<size_t>((size >= 0) ? size : 0);
    }

    static size_t get_size_between_iterators_front2back(const const_iterator& begin, const_iterator& end) {
        for (; end != begin && *--end == 0;); // 如果序列中的整数全为零，得到的返回值就是零
        const ptrdiff_t size{ std::distance(begin, ++end) };
        assert(size >= 0);
        return static_cast<size_t>((size >= 0) ? size : 0);
    }

    /// <summary>
    /// 计算大整数实际占用的存储空间大小
    /// </summary>
    /// <param name="target">大整数的数值</param>
    /// <returns>大整数实际占用的存储空间大小</returns>
    static size_t get_size_data_back2front(const data_type& target) {
        auto begin{ target.crbegin() };
        const auto end{ target.crend() };
        size_t target_size{ get_size_between_iterators_back2front(begin, end) };
        return target_size;
    }

    static size_t get_size_data_front2back(const data_type& target) {
        const auto begin{ target.cbegin() };
        auto end{ target.cend() };
        size_t target_size{ get_size_between_iterators_front2back(begin, end) };
        return target_size;
    }

    size_t size() const { // 计算大整数实际占用的存储空间大小
        return get_size_data_back2front(this->data);
    }

    size_t capacity() const { // 计算大整数宣称占用的存储空间大小
        return this->data.size();
    }

    /// <summary>
    /// 将大整数头部的零元素清理掉
    /// </summary>
    void shrink_to_fit() {
        data_type& data{ this->data };
        while (data.back() == 0) {
            data.pop_back();
        }
    }

    /// <summary>
    /// 逻辑扩展
    /// </summary>
    static void unsigned_extend(data_type& target, size_t count = 1) {
        while (count-- > 0) {
            target.push_back(0);
        }
    }

    /// <summary>
    /// 算术扩展
    /// </summary>
    static void signed_extend(data_type& target, size_t count = 1) {
        static constexpr int_type num[2] = { 0, std::numeric_limits<int_type>::max() };
        const int_type ext{ num[target.back() >> (std::numeric_limits<int_type>::digits - 1)] };
        while (count-- > 0) {
            target.push_back(ext);
        }
    }

    static void bitwise_not(data_type& target) { // 按位取反
        for (int_type& x : target) {
            x = ~x;
        }
    }

    /// <summary>
    /// 两个 64 位整数相加
    /// </summary>
    /// <param name="lhs">第一个加数</param>
    /// <param name="rhs">第二个加数</param>
    /// <param name="carry">初始进位</param>
    /// <returns>加法计算结果，包括进位与和</returns>
    static std::pair<int_type, int_type> add(const int_type lhs, const int_type rhs, const int_type carry = 0) {
        static constexpr int_type half_digits{ std::numeric_limits<int_type>::digits / 2 }; // 常量=32
        static constexpr int_type full_bits{ std::numeric_limits<int_type>::max() }; // 常量 0xffff_ffff_ffff_ffff
        static constexpr int_type lower_mask{ full_bits >> half_digits }; // 常量 0xffff_ffff
        int_type result_lower{ (lhs & lower_mask) + (rhs & lower_mask) + carry }; // 计算低32位之和
        const int_type carry_lower{ result_lower >> half_digits }; // 取得低32位相加的进位
        result_lower &= lower_mask; // 只取低32位，抛弃进位
        int_type result_higher{ (lhs >> half_digits) + (rhs >> half_digits) + carry_lower }; // 计算高32位之和（带上进位）
        const int_type carry_higher{ result_higher >> half_digits }; // 取得高32位相加的进位
        result_higher &= lower_mask; // 只取低32位，抛弃进位
        const int_type result{ (result_higher << half_digits) | result_lower }; 
        return { carry_higher, result }; // 返回全部64位加法的进位、和
    }

    /// <summary>
    /// 一个整数序列与一个整数相加
    /// </summary>
    static void add(data_type& target, int_type other) {
        if (other == 0) {
            return;
        }
        auto begin_lhs{ target.begin() };
        const auto end_lhs{ target.end() };
        if (begin_lhs != end_lhs) {
            int_type& lhs{ *begin_lhs };
            std::tie(other, lhs) = add(lhs, other, 0);
            ++begin_lhs;
        }
        while (other != 0 && begin_lhs != end_lhs) {
            int_type& lhs{ *begin_lhs };
            std::tie(other, lhs) = add(lhs, 0, other);
            ++begin_lhs;
        }
        if (other != 0) {
            target.push_back(other);
        }
    }

    /// <summary>
    /// 两个整数序列相加
    /// </summary>
    /// <param name="target">第一个加数，也用于保存计算结果</param>
    /// <param name="other">第二个加数</param>
    static void add(data_type& target, const data_type& other) {
        auto begin_lhs{ target.begin() };
        const auto end_lhs{ target.end() };
        auto begin_rhs{ other.cbegin() };
        const auto end_rhs{ other.cend() };
        int_type carry{ 0 };
        while (begin_lhs != end_lhs && begin_rhs != end_rhs) { // 处理两个加数等长的部分
            int_type& lhs{ *begin_lhs };
            const int_type& rhs{ *begin_rhs };
            std::tie(carry, lhs) = add(lhs, rhs, carry);
            ++begin_lhs;
            ++begin_rhs;
        }
        for (int_type result; begin_rhs != end_rhs; ++begin_rhs) { // 处理第二个加数剩余部分
            const int_type& rhs{ *begin_rhs };
            std::tie(carry, result) = add(0, rhs, carry);
            target.push_back(result);
        }
        if (carry != 0) {
            target.push_back(carry);
        }
    }

    /// <summary>
    /// 一个整数序列与一个整数相减
    /// </summary>
    static void sub(data_type& target, int_type other) {
        if (other == 0) {
            return;
        }
        auto begin_lhs{ target.begin() };
        const auto end_lhs{ target.end() };
        if (begin_lhs != end_lhs) {
            int_type& lhs{ *begin_lhs };
            std::tie(other, lhs) = add(lhs, ~other, 1);
            ++begin_lhs;
        }
        while (other != 0 && begin_lhs != end_lhs) {
            int_type& lhs{ *begin_lhs };
            std::tie(other, lhs) = add(lhs, 0, other);
            ++begin_lhs;
        }
        //if (other != 0) {
        //    target.push_back(other);
        //}
    }

    /// <summary>
    /// 两个整数序列相减
    /// </summary>
    static void sub(data_type& data_dest, const data_type& data_lhs, const data_type& data_rhs) {
        //const size_t size_lhs{ get_size_data_back2front(data_lhs) };
        //const size_t size_rhs{ get_size_data_back2front(data_rhs) };
        auto begin_dest{ data_dest.begin() };
        auto begin_lhs{ data_lhs.cbegin() };
        const auto end_lhs{ data_lhs.cend() };
        auto begin_rhs{ data_rhs.cbegin() };
        const auto end_rhs{ data_rhs.cend() };
        int_type carry_sub{ 1 }; // 减法相当于补码加法
        while (begin_lhs != end_lhs && begin_rhs != end_rhs) { // 处理被减数与减数等长的部分
            int_type& dest{ *begin_dest };
            const int_type& lhs{ *begin_lhs };
            const int_type& rhs{ *begin_rhs };
            std::tie(carry_sub, dest) = add(lhs, ~rhs, carry_sub);
            ++begin_lhs;
            ++begin_rhs;
            ++begin_dest;
        }
        for (int_type result_sub; begin_rhs != end_rhs; ++begin_rhs) { // 处理减数剩余部分
            const int_type& rhs{ *begin_rhs };
            std::tie(carry_sub, result_sub) = add(0, ~rhs, carry_sub);
            data_dest.push_back(result_sub);
        }
        //const size_t size_dest{ get_size_data_back2front(data_dest) };
        //assert(size_dest == 0 || size_dest == std::max(size_lhs, size_rhs));
    }

    static std::pair<int_type, int_type> mul_brute_force(const int_type lhs, const int_type rhs) { // 两个64位整数相乘
        static constexpr int_type half_digits{ std::numeric_limits<int_type>::digits / 2 }; // 常量=32
        static constexpr int_type full_bits{ std::numeric_limits<int_type>::max() }; // 常量 0xffff_ffff_ffff_ffff
        static constexpr int_type lower_mask{ full_bits >> half_digits }; // 常量 0xffff_ffff
        const int_type a{ lhs >> half_digits }; // 第一个加数的高32位
        const int_type b{ lhs & lower_mask }; // 第一个加数的低32位
        const int_type c{ rhs >> half_digits }; // 第二个加数的高32位
        const int_type d{ rhs & lower_mask }; // 第二个加数的低32位
        const int_type ac{ a * c };
        const int_type bd{ b * d };
        const int_type bc{ b * c };
        const int_type ad{ a * d };
        const int_type ad_bc{ ad + bc };
        const int_type result{ (ad_bc << half_digits) + bd };
        const int_type carry{ ac + (ad_bc >> half_digits) };
        return { carry, result };
    }

    static void mul_brute_force(data_type& target, const data_type& other) {
        const size_t target_size{ target.size() };
        const size_t other_size{ other.size() };
        const size_t new_size{ target_size + other_size }; // 积的最大长度=两个乘数的最大长度之和
        data_type result(new_size, 0); // 开辟了一块辅助存储空间
        const auto begin_lhs{ target.cbegin() }; // 从第一个乘数的最低位开始
        const auto end_lhs{ target.cend() };
        const auto begin_rhs{ other.cbegin() }; // 从第二个乘数的最低位开始
        const auto end_rhs{ other.cend() };
        auto result_iterator{ result.begin() }; // 从结果的最低位开始
        for (auto it_lhs{ begin_lhs }; it_lhs != end_lhs; ++it_lhs) {
            const auto save_result_iterator{ result_iterator }; // 暂存结果迭代器
            for (auto it_rhs{ begin_rhs }; it_rhs != end_rhs; ++it_rhs) {
                const auto [carry, partial_result] = mul_brute_force(*it_lhs, *it_rhs); // 按块计算位积
                *result_iterator += partial_result; // 按块存入位积
                ++result_iterator; // 结果迭代器向前移动
                *result_iterator += carry; // 按块存入进位
            }
            result_iterator = save_result_iterator; // 读取之前暂存的结果迭代器
            ++result_iterator; // 结果迭代器向前移动
        }
        target = result; // 把辅助存储移动复制给目标对象
    }

    static std::pair<int_type, int_type> mul_karatsuba(const int_type lhs, const int_type rhs) { // 两个64位整数相乘
        static constexpr int_type half_digits{ std::numeric_limits<int_type>::digits / 2 }; // 常量=32
        static constexpr int_type full_bits{ std::numeric_limits<int_type>::max() }; // 常量 0xffff_ffff_ffff_ffff
        static constexpr int_type lower_mask{ full_bits >> half_digits }; // 常量 0xffff_ffff
        const int_type a{ lhs >> half_digits }; // 第一个加数的高32位
        const int_type b{ lhs & lower_mask }; // 第一个加数的低32位
        const int_type c{ rhs >> half_digits }; // 第二个加数的高32位
        const int_type d{ rhs & lower_mask }; // 第二个加数的低32位
        const int_type ac{ a * c };
        const int_type bd{ b * d };
        const int_type ad_bc{ (a + b) * (c + d) - bd - ac }; // 精妙之处就在于此，Karatsuba算法减少了一次乘法，代价是增加了3次加法
        const int_type result{ (ad_bc << half_digits) + bd };
        const int_type carry{ ac + (ad_bc >> half_digits) };
        return { carry, result };
    }

    static int_type mul_karatsuba(iterator& begin_lhs, iterator& end_lhs, const_iterator& begin_rhs, const_iterator& end_rhs) {
        throw NotImplemented;
        if (begin_lhs == end_lhs - 1 && begin_rhs == end_rhs - 1) { // 两个整数序列中都只有 1 个元素
            const int_type& lhs{ *begin_lhs };
            const int_type& rhs{ *begin_rhs };
            int_type carry;
            std::tie(carry, *begin_lhs) = mul_karatsuba(lhs, rhs);
            return carry;
        }
        iterator mid_lhs{ begin_lhs + (end_lhs - begin_lhs) / 2 };
        const_iterator mid_rhs{ begin_rhs + (end_rhs - begin_rhs) / 2 };
        int_type carry_lower{ mul_karatsuba(begin_lhs, mid_lhs, begin_rhs, mid_rhs) };
        int_type carry_upper{ mul_karatsuba(mid_lhs, end_lhs, mid_rhs, end_rhs) };
    }

    static void mul_karatsuba(data_type& target, const data_type& other) {
        throw NotImplemented;
        // 用分治法实现两个整数序列的乘法
        // 1. 如果两个整数序列的长度都小于某个阈值，就用暴力法计算乘法
        // 2. 否则，将两个整数序列分成两半，分别计算高半部分的乘法、低半部分的乘法、中间部分的乘法
        // 3. 递归计算高半部分的乘法、低半部分的乘法
        // 4. 计算中间部分的乘法
        // 5. 将高半部分的乘法、低半部分的乘法、中间部分的乘法相加
        // 6. 返回结果
        iterator begin_lhs{ target.begin() };
        iterator end_lhs{ target.end() };
        const_iterator begin_rhs{ other.cbegin() };
        const_iterator end_rhs{ other.cend() };
        mul_karatsuba(begin_lhs, end_lhs, begin_rhs, end_rhs);
    }

    static std::pair<BigInteger, BigInteger> div(const BigInteger& dividend, const BigInteger& divisor) {
        if (divisor == 0) {
            throw DivideByZero{};
        }
        if (dividend == 0) {
            return { 0, 0 };
        }

        const sign_type cmp_result{ compare(dividend.data, divisor.data) };
        if (cmp_result < 0) {
            return { 0, dividend };
        }

        if (divisor.size() == 1) {
        }

        sign_type q_sign{ dividend.sign == divisor.sign ? sign_type::greater : sign_type::less };
        sign_type r_sign{ dividend.sign };
    }

    /// <summary>
    /// 比较一个整数序列与一个整数的相对大小
    /// </summary>
    static sign_type compare(const data_type& target, int_type other) {
        const auto begin{ target.cbegin() };
        return (get_size_data_back2front(target) == 1) // 整数序列中只有 1 个元素
            ? (*begin <=> other) // 直接将唯一一个元素与 other 进行比较
            : sign_type::greater; // 整数序列有 2 个以上元素，必定比任意一个 64 位整数更大
    }

    /// <summary>
    /// 比较一个大整数与一个整数的相对大小
    /// </summary>
    sign_type operator<=>(int_type other) const {
        if (this->sign == 0) { // 大整数是零
            return 0 <=> other;
        }
        return (this->sign == other <=> 0) // 检查大整数的符号与整数的符号是否相同
            ? compare(this->data, other) // 符号相同，根据绝对值判断大小
            : this->sign; // 符号不相同时，直接根据符号判断大小
    }

    /// <summary>
    /// 比较两个整数序列的相对大小
    /// </summary>
    static sign_type compare(const data_type& target, const data_type& other) {
        auto begin_target{ target.crbegin() };
        const auto end_target{ target.crend() };
        auto begin_other{ other.crbegin() };
        const auto end_other{ other.crend() };
        const size_t size_target{ get_size_between_iterators_back2front(begin_target, end_target) };
        const size_t size_other{ get_size_between_iterators_back2front(begin_other, end_other) };
        const sign_type cmp_result{ size_target <=> size_other };
        if (cmp_result != 0) { // 两个序列实际占用存储空间不一致
            return cmp_result; // 哪个序列占用的存储空间越多，哪个大整数的绝对值就越大
        }
        while (begin_target != end_target) { // 从最高位开始逐个检查整数相对大小
            if (*begin_target != *begin_other) {
                return *begin_target <=> *begin_other;
            }
            ++begin_target;
            ++begin_other;
        }
        return sign_type::equal;
    }

    /// <summary>
    /// 比较两个大整数的相对大小
    /// </summary>
    sign_type operator<=>(const BigInteger& other) const {
        if (this->sign == 0) {
            return 0 <=> other.sign;
        }
        return (this->sign == other <=> 0)
            ? compare(this->data, other.data) // 符号相同，根据绝对值判断大小
            : this->sign; // 符号不相同时，直接根据符号判断大小
    }

    bool operator<(const IntegralEx auto& other) const {
        return *this <=> other < 0;
    }

    bool operator<=(const IntegralEx auto& other) const {
        return *this <=> other <= 0;
    }

    bool operator>(const IntegralEx auto& other) const {
        return *this <=> other > 0;
    }

    bool operator>=(const IntegralEx auto& other) const {
        return *this <=> other >= 0;
    }

    bool operator==(const IntegralEx auto& other) const {
        return *this <=> other == 0;
    }

    bool operator!=(const IntegralEx auto& other) const {
        return *this <=> other != 0;
    }

    /// <summary>
    /// 取相反数
    /// </summary>
    BigInteger& negate() {
        this->sign = 0 <=> this->sign;
        return *this;
    }

    /// <summary>
    /// 取相反数
    /// </summary>
    BigInteger operator-() const {
        return BigInteger{ *this }.negate();
    }

    static BigInteger& add(BigInteger& lhs, const BigInteger& rhs) {
        add(lhs.data, rhs.data);
        return lhs;
    }

    /// <summary>
    /// 两个大整数相减
    /// </summary>
    /// <param name="dest"></param>
    /// <param name="lhs"></param>
    /// <param name="rhs"></param>
    /// <returns></returns>
    static BigInteger& sub(BigInteger& dest, const BigInteger& lhs, const BigInteger& rhs) {
        // 判断 lhs 与 rhs 的绝对值的相对大小，以确定结果 dest 的符号
        const sign_type cmp_result{ compare(lhs.data, rhs.data) };
        if (cmp_result == 0) { // lhs 与 rhs 的绝对值相等
            dest = { sign_type::equal, { 0 } };
        }
        if (cmp_result > 0) { // lhs 的绝对值更大
            sub(dest.data, lhs.data, rhs.data);
            dest.sign = sign_type::greater;
        }
        else { // if (cmp_result < 0) // rhs 的绝对值更大
            sub(dest.data, rhs.data, lhs.data);
            dest.sign = sign_type::less;
        }
        return dest;
    }

    BigInteger& operator+=(const BigInteger& other) {
        if (this->sign == 0) { // 第一个加数是零
            return *this = other; // 复制第二个加数的符号和数值
        }
        if (other.sign == 0) { // 第一个加数不是零，第二个加数是零
            return *this; // 没有任何变化，直接返回
        } // 下面几种情况下，两个加数都不是零
        if (this->sign == other.sign) { // 两个加数符号相同（两个加数都是正数，或者两个加数都是负数）
            return add(*this, other); // 两个加数符号相同，相当于做加法
        }
        return (this->sign > 0) // 两个加数符号不同（一个是正数，一个是负数），相当于做减法
            ? sub(*this, *this, other) // 第一个加数是正数，第二个加数是负数
            : sub(*this, other, *this); // 第一个加数是负数，第二个加数是正数
    }

    BigInteger& operator-=(const BigInteger& other) {
        if (this->sign == 0) { // 被减数是零
            return *this = -other; // 复制减数的数值，但是符号取反
        }
        if (other.sign == 0) { // 被减数不是零，减数是零
            return *this; // 没有任何变化，直接返回
        } // 下面几种情况下，被减数和减数都不是零
        if (this->sign == other.sign) { // 被减数和减数符号相同（都是正数，或者都是负数）
            return (this->sign > 0)
                ? sub(*this, *this, other) // 被减数、减数都是正数，让被减数减去减数
                : sub(*this, other, *this); // 被减数、减数都是负数，让减数的绝对值减去被减数的绝对值
        }
        return (this->sign > 0)
            ? add(*this, other) // 被减数是正数，减数是负数，相当于做加法
            : sub(*this, other, *this); // 被减数是负数，减数是正数，相当于做减法
    }

    BigInteger& operator+=(const std::integral auto& other) {
        // FIXME 无法处理有符号数（特别是负数）
        add(this->data, other);
        return *this;
    }

    BigInteger& operator-=(const std::integral auto& other) {
        // FIXME 无法处理有符号数（特别是负数）
        sub(this->data, other);
        return *this;
    }

    BigInteger operator+(const IntegralEx auto& other) {
        return BigInteger{ *this } += other;
    }

    BigInteger operator-(const IntegralEx auto& other) {
        return BigInteger{ *this } -= other;
    }

    BigInteger& operator*=(const BigInteger& other) {
        mul_brute_force(this->data, other.data);
        return *this;
        //throw NotImplemented;
    }

    BigInteger operator*(const BigInteger& other) const {
        return BigInteger{ *this } *= other;
    }

    BigInteger& operator/=(const BigInteger& other) {
        throw NotImplemented;
    }

    BigInteger operator/(const BigInteger& other) const {
        return BigInteger{ *this } /= other;
    }

    std::string to_string(const char seperator) const {
        static constexpr size_t chunk_size_in_quadruple_bits{ 4 };
        static constexpr size_t chunk_size_in_bits{ 4 * chunk_size_in_quadruple_bits }; // 0xffff 占用 16 位
        static constexpr int_type chunk_mask{ (1 << chunk_size_in_bits) - 1 }; // 常量=0xffff
        static constexpr int_type chunk_count{ std::numeric_limits<int_type>::digits / chunk_size_in_bits }; // 常量=16
        using chunk_type = std::uint16_t;
        auto begin{ this->data.crbegin() };
        const auto end{ this->data.crend() };
        size_t size{ get_size_between_iterators_back2front(begin, end) };
        if (size == 0) { // 整数序列中全是零
            return "0x0";
        }
        std::stringstream out;
        auto get_chunk = [](int_type x, size_t i) {
            return static_cast<chunk_type>(x >> i * chunk_size_in_bits & chunk_mask);
        };
        auto print_int = [&](int_type x, bool first) {
            // 第一个非零整数的高位零也需要跳过，并且从第二个整数开始需要前接下划线
            size_t i{ chunk_count };
            if (first) { // 正在打印的整数是整数序列中的第一个非零数字（从高到低）
                // 把一个 64 位整数分成 4 块，每一块都有 16 位
                // 如果这一块中每一位都是零，那么跳过，不打印这一块
                while (i != 0) {
                    chunk_type chunk{ get_chunk(x, --i) };
                    if (chunk != 0) {
                        ++i;
                        break;
                    }
                }
            }
            while (i != 0) {
                if (!first) {
                    out << seperator // 在最高 4 位数字以后，每 4 位数字之前，加上下划线
                        << std::setfill('0')
                        << std::setw(chunk_size_in_quadruple_bits);
                }
                chunk_type chunk{ get_chunk(x, --i) };
                out << chunk;
                first = false;
            }
        }; // 打印整数不需要返回值
        if (this->sign < 0) {
            out << '-';
        }
        out << "0x";
        out << std::hex;
        print_int(*begin++, true); // 打印最高的 64 位非零数字
        while (begin != end) {
            print_int(*begin++, false);
        }
        return out.str();
    }

    explicit operator std::string() const {
        return to_string('_');
    }

    friend std::ostream& operator<<(std::ostream& out, const BigInteger& big_integer) {
        out << static_cast<std::string>(big_integer);
        return out;
    }
};


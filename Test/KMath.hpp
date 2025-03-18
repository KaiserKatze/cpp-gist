#pragma once

#include <type_traits>
#include <concepts>

template <std::integral T>
struct CommonFactor {
private:
    constexpr T _calc_gcd(T a, T b) {
        while (b != 0) {
            T temp{ b };
            b = a % b;
            a = temp;
        }
        return a;
    }
    constexpr T _calc_lcm(T a, T b, T c) {
        return a * b / c;
    }
public:
    const T gcd;
    const T lcm;
    constexpr CommonFactor(T&& a, T&& b)
        : gcd{ _calc_gcd(a, b) },
        lcm{ _calc_lcm(a, b, gcd) } {
    }
};

template <std::integral T>
struct GCD {
private:
    constexpr T _calc_gcd(T a, T b) {
        while (b != 0) {
            T temp{ b };
            b = a % b;
            a = temp;
        }
        return a;
    }
protected:
    const T _gcd;
public:
    constexpr GCD(T&& a, T&& b) : _gcd{ _calc_gcd(a,b) } {
    }
    constexpr T gcd() const {
        return _gcd;
    }
};

template <std::integral T>
struct LCM : public GCD<T> {
private:
    constexpr T _calc_lcm(T a, T b, T c) {
        return a * b / c;
    }
    const T _lcm;
public:
    constexpr LCM(T&& a, T&& b) : GCD<T>(std::move(a), std::move(b)), _lcm{ _calc_lcm(a, b, this->_gcd) } {
    }
    constexpr T lcm() const {
        return _lcm;
    }
};


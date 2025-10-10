#pragma once

// 质因数分解
#include <vector>

struct PrimeFactor {
    PrimeFactor(size_t limit) {
        generate_primes(limit);
    }

    void generate_primes(size_t limit) {
        // 使用埃拉托斯特尼筛法预先生成素数表
        sieve.resize(limit + 1, true);
        size_t i;
        for (size_t p{ 2 }; (i = p * p) <= limit; ++p) {
            if (sieve[p]) {  // 如果 p 是素数
                for (; i <= limit; i += p) {
                    sieve[i] = false;
                }
            }
        }
        // 将从 2 开始的所有素数存入素数表
        for (size_t p{ 2 }; p <= limit; ++p) {
            if (sieve[p]) {
                primes.push_back(p);
            }
        }
    }

    std::vector<size_t> factorize(size_t n) {
        // 质因数分解
        std::vector<size_t> factors;

        for (size_t p : primes) {
            if (p * p > n) {
                break;
            }
            while (n % p == 0) {  // 当 p 可以整除 n 时
                factors.push_back(p);
                n /= p;
            }
        }
        if (n > 1) {
            factors.push_back(n);  // n 是它本身的因子
        }
        return factors;
    }

private:
    std::vector<size_t> primes;
    std::vector<bool> sieve;
};

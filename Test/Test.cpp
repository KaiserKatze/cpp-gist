#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include <cassert>
#include <stack>
#include <queue>
#include <concepts>
#include <type_traits>
#include <iomanip>
#include <string>
#include <vector>
#include <compare> // std::strong_ordering (since C++20)

#define DEBUG
#include "BigInteger.hpp"
#include "Decimal.hpp"

struct BigInteger;

using int_type = std::uint64_t;

void test() {
    BigInteger a{ std::strong_ordering::greater, { 0xff, 0xfe, 0x1a, 0x99, 0x18 } };
    BigInteger b{ std::strong_ordering::greater, { 0x9c, 0x7e, 0x27, 0x81, 0x12 } };

    std::cout
        << "a =\n\t"
        << a
        << ' '
        << std::boolalpha
        << (static_cast<std::string>(a) == "0x18_0000_0000_0000_0099_0000_0000_0000_001a_0000_0000_0000_00fe_0000_0000_0000_00ff")
        << '\n'

        << "a.size() ->\n\t"
        << BigInteger::get_size_data_back2front(a.data)
        << ' '
        << BigInteger::get_size_data_front2back(a.data)
        << '\n'

        //<< "a + 0 ->\n\t"
        //<< (a + 0)
        //<< ' '
        //<< std::boolalpha
        //<< ((a + 0) == a)
        //<< '\n'

        << "b =\n\t"
        << b
        << ' '
        << std::boolalpha
        << (static_cast<std::string>(b) == "0x12_0000_0000_0000_0081_0000_0000_0000_0027_0000_0000_0000_007e_0000_0000_0000_009c")
        << '\n'

        << "(a - a) =\n\t"
        << (a - a)
        << ' '
        << std::boolalpha
        << (static_cast<std::string>(a - a) == "0x0")
        << '\n'

        //<< "a + b ->\n\t"
        //<< (a + b)
        //<< ' '
        //<< std::boolalpha
        //<< (static_cast<std::string>(a + b) == "0x2a_0000_0000_0000_011a_0000_0000_0000_0041_0000_0000_0000_017c_0000_0000_0000_019b")
        //<< '\n'

        << "b - a ->\n\t"
        << (b - a)
        << ' '
        << std::boolalpha
        << (static_cast<std::string>(b - a) == "-0x6_0000_0000_0000_0017_ffff_ffff_ffff_fff3_0000_0000_0000_0080_0000_0000_0000_0063")
        << '\n'

        << "a * b ->\n\t"
        << (a * b)
        << ' '
        << std::boolalpha
        << (static_cast<std::string>(a * b) == "0x1b0_0000_0000_0000_16da_0000_0000_0000_5295_0000_0000_0000_4215_0000_0000_0000_efd0_0000_0000_0001_1139_0000_0000_0000_b3b5_0000_0000_0001_184a_0000_0000_0000_9b64")
        << '\n'

        << '\n';

}

int main(int argc, char** argv) {
    Decimal d1{ "+999." };
    Decimal d2{ "99." };
    std::cout
        << "d1 =\n\t"
        << d1
        << '\n'
        << "d2 =\n\t"
        << d2
        << '\n'
        << "d1 * d2 =\n\t"
        << (d1 * d2)
        << '\n';
    return 0;
}


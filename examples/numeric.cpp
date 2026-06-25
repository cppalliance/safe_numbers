// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers/numeric.hpp>
#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/iostream.hpp>
#include <iostream>

int main()
{
    using namespace boost::safe_numbers;

    // gcd: greatest common divisor (Euclidean algorithm)
    std::cout << "gcd(12, 8)      = " << gcd(u32{12}, u32{8}) << '\n';
    std::cout << "gcd(54, 24)     = " << gcd(u32{54}, u32{24}) << '\n';
    std::cout << "gcd(7, 11)      = " << gcd(u32{7}, u32{11}) << '\n';
    std::cout << "gcd(0, 42)      = " << gcd(u32{0}, u32{42}) << '\n';

    std::cout << '\n';

    // lcm: least common multiple
    std::cout << "lcm(4, 6)       = " << lcm(u32{4}, u32{6}) << '\n';
    std::cout << "lcm(12, 18)     = " << lcm(u32{12}, u32{18}) << '\n';
    std::cout << "lcm(7, 11)      = " << lcm(u32{7}, u32{11}) << '\n';

    std::cout << '\n';

    // midpoint: average without overflow, rounded toward the first argument
    std::cout << "midpoint(0, 10) = " << midpoint(u32{0}, u32{10}) << '\n';
    std::cout << "midpoint(1, 4)  = " << midpoint(u32{1}, u32{4}) << '\n';
    std::cout << "midpoint(4, 1)  = " << midpoint(u32{4}, u32{1}) << '\n';

    return 0;
}

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers/integer_utilities.hpp>
#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/iostream.hpp>
#include <iostream>

int main()
{
    using namespace boost::safe_numbers;

    std::cout << std::boolalpha;

    // Integer square root (floor of sqrt)
    std::cout << "isqrt(100)              = " << isqrt(u32{100}) << '\n';
    std::cout << "isqrt(200)              = " << isqrt(u32{200}) << '\n';

    std::cout << '\n';

    // Remove trailing decimal zeros
    const auto rtz = remove_trailing_zeros(u32{12300});
    std::cout << "remove_trailing_zeros(12300) = " << rtz.trimmed_number
              << " (removed " << rtz.number_of_removed_zeros << " zeros)\n";

    std::cout << '\n';

    // Power-of-N predicates
    std::cout << "is_power_10(1000)       = " << is_power_10(u32{1000}) << '\n';
    std::cout << "is_power_10(1234)       = " << is_power_10(u32{1234}) << '\n';
    std::cout << "is_power_2(1024)        = " << is_power_2(u32{1024}) << '\n';
    std::cout << "is_power_2(1000)        = " << is_power_2(u32{1000}) << '\n';

    std::cout << '\n';

    // Integer logarithms
    std::cout << "ilog2(1024)             = " << ilog2(u32{1024}) << '\n';
    std::cout << "ilog10(1000)            = " << ilog10(u32{1000}) << '\n';
    std::cout << "ilog(80, 3)             = " << ilog(u32{80}, u32{3}) << '\n';

    std::cout << '\n';

    // Exponentiation and arithmetic helpers
    std::cout << "ipow(2, 10)             = " << ipow(u32{2}, u32{10}) << '\n';
    std::cout << "abs_diff(3, 10)         = " << abs_diff(u32{3}, u32{10}) << '\n';
    std::cout << "div_ceil(10, 3)         = " << div_ceil(u32{10}, u32{3}) << '\n';
    std::cout << "next_multiple_of(10, 3) = " << next_multiple_of(u32{10}, u32{3}) << '\n';

    return 0;
}

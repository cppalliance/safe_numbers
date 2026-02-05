// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// This example demonstrates the use of overflowing arithmetic operations.
// These functions return a std::pair containing the result and a boolean
// flag indicating whether overflow/underflow occurred. The result is the
// wrapped value (as if using normal unsigned arithmetic).

#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/iostream.hpp>
#include <iostream>
#include <cstdint>
#include <limits>
#include <utility>

int main()
{
    using boost::safe_numbers::u32;
    using boost::safe_numbers::overflowing_add;
    using boost::safe_numbers::overflowing_sub;
    using boost::safe_numbers::overflowing_mul;

    // Overflowing addition: returns (wrapped_result, did_overflow)
    {
        const u32 a {std::numeric_limits<std::uint32_t>::max()};
        const u32 b {100U};
        const auto [result, overflowed] {overflowing_add(a, b)};

        std::cout << "overflowing_add(max, 100):" << std::endl;
        std::cout << "  result = " << result << std::endl;
        std::cout << "  overflowed = " << std::boolalpha << overflowed << std::endl;
        // Output: result = 99 (wrapped), overflowed = true
    }

    // Overflowing subtraction: returns (wrapped_result, did_underflow)
    {
        const u32 a {10U};
        const u32 b {100U};
        const auto [result, underflowed] {overflowing_sub(a, b)};

        std::cout << "overflowing_sub(10, 100):" << std::endl;
        std::cout << "  result = " << result << std::endl;
        std::cout << "  underflowed = " << std::boolalpha << underflowed << std::endl;
        // Output: result = 4294967206 (wrapped), underflowed = true
    }

    // Overflowing multiplication: returns (wrapped_result, did_overflow)
    {
        const u32 a {std::numeric_limits<std::uint32_t>::max()};
        const u32 b {2U};
        const auto [result, overflowed] {overflowing_mul(a, b)};

        std::cout << "overflowing_mul(max, 2):" << std::endl;
        std::cout << "  result = " << result << std::endl;
        std::cout << "  overflowed = " << std::boolalpha << overflowed << std::endl;
        // Output: result = 4294967294 (wrapped), overflowed = true
    }

    // Normal operations that don't overflow
    {
        const u32 a {100U};
        const u32 b {50U};
        const auto [add_result, add_overflow] {overflowing_add(a, b)};
        const auto [sub_result, sub_overflow] {overflowing_sub(a, b)};
        const auto [mul_result, mul_overflow] {overflowing_mul(a, b)};

        std::cout << "overflowing_add(100, 50) = " << add_result
                  << " (overflow: " << add_overflow << ")" << std::endl;
        std::cout << "overflowing_sub(100, 50) = " << sub_result
                  << " (overflow: " << sub_overflow << ")" << std::endl;
        std::cout << "overflowing_mul(100, 50) = " << mul_result
                  << " (overflow: " << mul_overflow << ")" << std::endl;
        // Output: 150/false, 50/false, 5000/false
    }

    // Using the overflow flag for conditional logic
    {
        const u32 a {1000000000U};
        const u32 b {5U};

        if (const auto [result, overflowed] {overflowing_mul(a, b)}; !overflowed)
        {
            std::cout << "Safe multiplication: " << a << " * " << b << " = " << result << std::endl;
        }
        else
        {
            std::cout << "Multiplication would overflow!" << std::endl;
        }
    }

    return 0;
}


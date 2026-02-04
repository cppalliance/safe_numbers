// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

//[wrapping_arithmetic_example
//` This example demonstrates the use of wrapping arithmetic operations.
//` These functions perform standard C unsigned integer wrapping behavior -
//` when overflow or underflow occurs, the result wraps around modulo 2^N.
//` This matches the behavior of built-in unsigned integers in C/C++.

#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/iostream.hpp>
#include <iostream>
#include <cstdint>
#include <limits>

int main()
{
    using boost::safe_numbers::u8;
    using boost::safe_numbers::u32;
    using boost::safe_numbers::wrapping_add;
    using boost::safe_numbers::wrapping_sub;
    using boost::safe_numbers::wrapping_mul;

    // Wrapping addition: wraps around on overflow
    {
        const u8 a {255U};  // UINT8_MAX
        const u8 b {2U};
        const u8 result {wrapping_add(a, b)};

        std::cout << "wrapping_add(255, 2) = " << result << std::endl;
        // Output: 1 (255 + 2 = 257, wraps to 257 % 256 = 1)
    }

    // Wrapping subtraction: wraps around on underflow
    {
        const u8 a {0U};
        const u8 b {1U};
        const u8 result {wrapping_sub(a, b)};

        std::cout << "wrapping_sub(0, 1) = " << result << std::endl;
        // Output: 255 (wraps to UINT8_MAX)
    }

    // Wrapping multiplication: wraps around on overflow
    {
        const u8 a {200U};
        const u8 b {2U};
        const u8 result {wrapping_mul(a, b)};

        std::cout << "wrapping_mul(200, 2) = " << result << std::endl;
        // Output: 144 (200 * 2 = 400, wraps to 400 % 256 = 144)
    }

    // Demonstration with u32
    {
        const u32 max_val {std::numeric_limits<std::uint32_t>::max()};
        const u32 one {1U};

        std::cout << "wrapping_add(UINT32_MAX, 1) = "
                  << wrapping_add(max_val, one) << std::endl;
        // Output: 0 (wraps around)

        const u32 zero {0U};
        std::cout << "wrapping_sub(0, 1) = "
                  << wrapping_sub(zero, one) << std::endl;
        // Output: 4294967295 (UINT32_MAX)
    }

    // Normal operations that don't overflow work as expected
    {
        const u32 a {100U};
        const u32 b {50U};

        std::cout << "wrapping_add(100, 50) = " << wrapping_add(a, b) << std::endl;
        std::cout << "wrapping_sub(100, 50) = " << wrapping_sub(a, b) << std::endl;
        std::cout << "wrapping_mul(100, 50) = " << wrapping_mul(a, b) << std::endl;
        // Output: 150, 50, 5000
    }

    // Useful for implementing counters that wrap
    {
        u8 counter {254U};
        std::cout << "Counter sequence: ";
        for (int i = 0; i < 5; ++i)
        {
            std::cout << counter << " ";
            counter = wrapping_add(counter, u8{1U});
        }
        std::cout << std::endl;
        // Output: 254 255 0 1 2
    }

    return 0;
}
//]

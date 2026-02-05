// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

//[strict_arithmetic_example
//` This example demonstrates the use of strict arithmetic operations.
//` These functions call std::exit(EXIT_FAILURE) on overflow, underflow,
//` or division by zero. They are designed for safety-critical applications
//` where exceptions cannot be used but silent wrapping is unacceptable.

#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/iostream.hpp>
#include <iostream>
#include <cstdint>

int main()
{
    using boost::safe_numbers::u32;
    using boost::safe_numbers::strict_add;
    using boost::safe_numbers::strict_sub;
    using boost::safe_numbers::strict_mul;
    using boost::safe_numbers::strict_div;
    using boost::safe_numbers::strict_mod;

    // Normal operations that don't overflow work as expected
    {
        const u32 a {100U};
        const u32 b {50U};

        std::cout << "strict_add(100, 50) = " << strict_add(a, b) << std::endl;
        std::cout << "strict_sub(100, 50) = " << strict_sub(a, b) << std::endl;
        std::cout << "strict_mul(100, 50) = " << strict_mul(a, b) << std::endl;
        std::cout << "strict_div(100, 50) = " << strict_div(a, b) << std::endl;
        std::cout << "strict_mod(100, 50) = " << strict_mod(a, b) << std::endl;
    }

    // Strict arithmetic is noexcept - safe for -fno-exceptions environments
    {
        const u32 a {1000000U};
        const u32 b {3U};

        // These are guaranteed to never throw
        static_assert(noexcept(strict_add(a, b)));
        static_assert(noexcept(strict_sub(a, b)));
        static_assert(noexcept(strict_mul(a, b)));
        static_assert(noexcept(strict_div(a, b)));
        static_assert(noexcept(strict_mod(a, b)));

        std::cout << "strict_div(1000000, 3) = " << strict_div(a, b) << std::endl;
        std::cout << "strict_mod(1000000, 3) = " << strict_mod(a, b) << std::endl;
    }

    // NOTE: If any of these operations would overflow, underflow, or
    // divide by zero, the program would immediately terminate via
    // std::exit(EXIT_FAILURE). For example:
    //
    //   u32 max_val {std::numeric_limits<std::uint32_t>::max()};
    //   strict_add(max_val, u32{1U});  // Terminates the program
    //
    //   strict_sub(u32{0U}, u32{1U});  // Terminates the program
    //
    //   strict_div(u32{1U}, u32{0U});  // Terminates the program

    return 0;
}
//]

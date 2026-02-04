// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

//[saturating_arithmetic_example
//` This example demonstrates the use of saturating arithmetic operations.
//` When overflow or underflow would occur, the result saturates at the
//` type's maximum or minimum value instead of wrapping or throwing.

#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/iostream.hpp>
#include <iostream>
#include <cstdint>
#include <limits>

int main()
{
    using boost::safe_numbers::u32;
    using boost::safe_numbers::saturating_add;
    using boost::safe_numbers::saturating_sub;
    using boost::safe_numbers::saturating_mul;

    // Saturating addition: clamps to max on overflow
    {
        const u32 a {std::numeric_limits<std::uint32_t>::max()};
        const u32 b {100U};
        const u32 result {saturating_add(a, b)};

        std::cout << "saturating_add(max, 100) = " << result << std::endl;
        // Output: 4294967295 (UINT32_MAX, saturated)
    }

    // Saturating subtraction: clamps to min (0) on underflow
    {
        const u32 a {10U};
        const u32 b {100U};
        const u32 result {saturating_sub(a, b)};

        std::cout << "saturating_sub(10, 100) = " << result << std::endl;
        // Output: 0 (saturated at minimum)
    }

    // Saturating multiplication: clamps to max on overflow
    {
        const u32 a {std::numeric_limits<std::uint32_t>::max()};
        const u32 b {2U};
        const u32 result {saturating_mul(a, b)};

        std::cout << "saturating_mul(max, 2) = " << result << std::endl;
        // Output: 4294967295 (UINT32_MAX, saturated)
    }

    // Normal operations that don't overflow work as expected
    {
        const u32 a {100U};
        const u32 b {50U};

        std::cout << "saturating_add(100, 50) = " << saturating_add(a, b) << std::endl;
        std::cout << "saturating_sub(100, 50) = " << saturating_sub(a, b) << std::endl;
        std::cout << "saturating_mul(100, 50) = " << saturating_mul(a, b) << std::endl;
        // Output: 150, 50, 5000
    }

    return 0;
}
//]

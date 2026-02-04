// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

//[checked_arithmetic_example
//` This example demonstrates the use of checked arithmetic operations.
//` These functions return std::optional - containing the result if the
//` operation succeeded, or std::nullopt if overflow/underflow occurred.
//` This provides a clean, exception-free way to handle arithmetic errors.

#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/iostream.hpp>
#include <iostream>
#include <cstdint>
#include <limits>
#include <optional>

int main()
{
    using boost::safe_numbers::u32;
    using boost::safe_numbers::checked_add;
    using boost::safe_numbers::checked_sub;
    using boost::safe_numbers::checked_mul;
    using boost::safe_numbers::checked_div;

    // Checked addition: returns nullopt on overflow
    {
        const u32 a {std::numeric_limits<std::uint32_t>::max()};
        const u32 b {100U};
        const auto result {checked_add(a, b)};

        if (result.has_value())
        {
            std::cout << "checked_add(max, 100) = " << *result << std::endl;
        }
        else
        {
            std::cout << "checked_add(max, 100) = overflow detected!" << std::endl;
        }
        // Output: overflow detected!
    }

    // Checked subtraction: returns nullopt on underflow
    {
        const u32 a {10U};
        const u32 b {100U};
        const auto result {checked_sub(a, b)};

        if (result.has_value())
        {
            std::cout << "checked_sub(10, 100) = " << *result << std::endl;
        }
        else
        {
            std::cout << "checked_sub(10, 100) = underflow detected!" << std::endl;
        }
        // Output: underflow detected!
    }

    // Checked division: returns nullopt on divide by zero
    {
        const u32 a {100U};
        const u32 b {0U};
        const auto result {checked_div(a, b)};

        if (result.has_value())
        {
            std::cout << "checked_div(100, 0) = " << *result << std::endl;
        }
        else
        {
            std::cout << "checked_div(100, 0) = division by zero!" << std::endl;
        }
        // Output: division by zero!
    }

    // Successful operations return the value wrapped in optional
    {
        const u32 a {100U};
        const u32 b {50U};

        // Using value_or for safe access with default
        std::cout << "checked_add(100, 50) = "
                  << checked_add(a, b).value_or(u32{0U}) << std::endl;
        std::cout << "checked_sub(100, 50) = "
                  << checked_sub(a, b).value_or(u32{0U}) << std::endl;
        std::cout << "checked_mul(100, 50) = "
                  << checked_mul(a, b).value_or(u32{0U}) << std::endl;
        // Output: 150, 50, 5000
    }

    // Chaining checked operations with value_or
    {
        const u32 a {1000000000U};
        const u32 b {5U};

        // Only proceed if multiplication doesn't overflow
        const auto product {checked_mul(a, b)};
        if (product)
        {
            std::cout << "Safe: " << a << " * " << b << " = " << *product << std::endl;
        }
        else
        {
            std::cout << "Operation would overflow, using fallback" << std::endl;
        }
    }

    return 0;
}
//]

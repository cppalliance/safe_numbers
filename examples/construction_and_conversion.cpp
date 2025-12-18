// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// This example demonstrates how to construct a safe unsigned integer,
// and then convert it back to built-in types

#include <boost/safe_numbers/unsigned_integers.hpp> // For the safe unsigned integers
#include <boost/safe_numbers/iostream.hpp>          // For safe numbers support to <iostream>
#include <iostream>
#include <cstdint>

int main()
{
    using boost::safe_numbers::u32; // 32-bit unsigned safe integer

    // Construct a safe u32 from a builtin unsigned value explicitly
    constexpr u32 safe_value {42U};

    // Safe values support <iostream> in the same way that the builtins do
    std::cout << "Safe Value: " << safe_value << '\n';

    // To convert the safe value back to a builtin value
    // use the known basis, or generically via the basis_type typedef
    constexpr std::uint32_t unsafe_value {static_cast<std::uint32_t>(safe_value)};
    constexpr std::uint32_t unsafe_value_from_typedef {static_cast<u32::basis_type>(safe_value)};

    // The conversions are all completely constexpr
    // Invalid conversions such as narrowing will result in compile errors
    // instead of run-time error
    if constexpr (unsafe_value == unsafe_value_from_typedef)
    {
       std::cout << "Builtin Value: " << unsafe_value << '\n';
    }

    return 0;
}

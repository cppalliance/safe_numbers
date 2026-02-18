// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// This example shows what you can do with verified types at runtime.
// While construction and arithmetic are compile-time only, verified types
// are designed to be useful runtime constants: they support output streaming,
// to_chars, std::format/fmt::format, comparisons, and bit queries.

#include <boost/safe_numbers/verified_integers.hpp>
#include <boost/safe_numbers/iostream.hpp>
#include <boost/safe_numbers/charconv.hpp>
#include <boost/safe_numbers/bit.hpp>
#include <boost/safe_numbers/limits.hpp>
#include <iostream>
#include <limits>

int main()
{
    using namespace boost::safe_numbers;

    constexpr verified_u32 val {1024U};

    // --- Output streaming (runtime) ---
    std::cout << "Value: " << val << '\n';

    // --- Conversions (runtime) ---
    // Explicit cast back to the fundamental type
    const auto raw = static_cast<std::uint32_t>(val);
    std::cout << "As uint32_t: " << raw << '\n';

    // Explicit cast to the safe basis type
    const auto basis = static_cast<u32>(val);
    std::cout << "As u32: " << basis << '\n';

    std::cout << '\n';

    // --- Comparisons (runtime) ---
    constexpr verified_u32 small {10U};
    constexpr verified_u32 large {50000U};

    std::cout << std::boolalpha;
    std::cout << "10 == 10:    " << (small == small) << '\n';
    std::cout << "10 != 50000: " << (small != large) << '\n';
    std::cout << "10 <  50000: " << (small < large)  << '\n';
    std::cout << "10 >= 50000: " << (small >= large) << '\n';

    std::cout << '\n';

    // --- to_chars (runtime) ---
    // Writing to a character buffer works at runtime since it only
    // reads the value through the constexpr conversion operator
    char buffer[32];
    auto result = boost::charconv::to_chars(buffer, buffer + sizeof(buffer), val);
    if (result)
    {
        *result.ptr = '\0';
        std::cout << "to_chars (base 10): " << buffer << '\n';
    }

    result = boost::charconv::to_chars(buffer, buffer + sizeof(buffer), val, 16);
    if (result)
    {
        *result.ptr = '\0';
        std::cout << "to_chars (base 16): " << buffer << '\n';
    }

    std::cout << '\n';

    // --- Bit queries (runtime) ---
    // Functions returning int or bool work at runtime
    std::cout << "has_single_bit(1024) = " << has_single_bit(val) << '\n';
    std::cout << "bit_width(1024)      = " << bit_width(val) << '\n';
    std::cout << "countl_zero(1024)    = " << countl_zero(val) << '\n';
    std::cout << "popcount(1024)       = " << popcount(val) << '\n';

    // Functions returning the verified type are consteval.
    // They work as compile-time constants:
    constexpr auto ceiled  = bit_ceil(verified_u32{48U});
    constexpr auto floored = bit_floor(verified_u32{48U});
    std::cout << "bit_ceil(48)         = " << ceiled << '\n';
    std::cout << "bit_floor(48)        = " << floored << '\n';

    std::cout << '\n';

    // --- std::numeric_limits (runtime) ---
    std::cout << "numeric_limits<verified_u8>::min()  = "
              << std::numeric_limits<verified_u8>::min() << '\n';
    std::cout << "numeric_limits<verified_u8>::max()  = "
              << std::numeric_limits<verified_u8>::max() << '\n';
    std::cout << "numeric_limits<verified_u32>::digits = "
              << std::numeric_limits<verified_u32>::digits << '\n';

    // --- What does NOT work at runtime ---
    // The following operations require compile-time evaluation:
    //
    //   verified_u32 x {some_runtime_variable};  // ERROR: consteval constructor
    //   auto sum = a + b;  // Only at compile time, e.g. constexpr auto sum = a + b;
    //   from_chars(str, str + len, verified_val);  // consteval only
    //   bit_ceil(verified_val);  // consteval only (returns verified type)
    //
    // This is by design: verified types are compile-time validated constants
    // that can be safely passed around and inspected at runtime.

    return 0;
}

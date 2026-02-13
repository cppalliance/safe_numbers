// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// Verified types are compile-time constants, but they can participate in
// runtime operations when mixed with their basis types. The result is
// always the runtime (basis) type, effectively treating the verified value
// as a read-only constant operand.

#include <boost/safe_numbers/verified_integers.hpp>
#include <boost/safe_numbers/iostream.hpp>
#include <iostream>

int main()
{
    using namespace boost::safe_numbers;

    // A compile-time constant and a runtime value
    constexpr auto tax_rate = verified_u32{u32{20}};
    auto price = u32{500};

    // --- Arithmetic: verified op basis -> basis ---
    const auto tax = tax_rate * price / u32{100};
    std::cout << "Tax on " << price << " at " << tax_rate << "%: " << tax << '\n';

    // --- Arithmetic: basis op verified -> basis ---
    constexpr auto discount = verified_u32{u32{50}};
    const auto discounted = price - discount;
    std::cout << price << " - " << discount << " discount = " << discounted << '\n';

    std::cout << '\n';

    // --- Comparisons: verified vs basis ---
    constexpr auto threshold = verified_u32{u32{1000}};
    std::cout << std::boolalpha;
    std::cout << price << " < " << threshold << ": " << (price < threshold) << '\n';
    std::cout << threshold << " > " << price << ": " << (threshold > price) << '\n';
    std::cout << price << " == " << threshold << ": " << (price == threshold) << '\n';

    std::cout << '\n';

    // --- Bitwise operations: verified vs basis ---
    constexpr auto mask = verified_u32{u32{0xFF}};
    auto value = u32{0xABCD};
    const auto masked = value & mask;
    std::cout << "0xABCD & 0xFF = " << masked << '\n';

    const auto combined = mask | value;
    std::cout << "0xFF | 0xABCD = " << combined << '\n';

    // Shift operations
    constexpr auto shift = verified_u32{u32{4}};
    const auto shifted = value << shift;
    std::cout << "0xABCD << 4 = " << shifted << '\n';

    std::cout << '\n';

    // --- Bounded types work too ---
    constexpr auto bounded_offset = verified_bounded_integer<0u, 100u>{10u};
    auto bounded_val = bounded_uint<0u, 100u>{50u};
    const auto bounded_sum = bounded_val + bounded_offset;
    std::cout << "bounded 50 + 10 = " << bounded_sum << '\n';

    const auto bounded_diff = bounded_val - bounded_offset;
    std::cout << "bounded 50 - 10 = " << bounded_diff << '\n';

    // Bounded comparisons
    std::cout << "bounded 50 > 10: " << (bounded_val > bounded_offset) << '\n';

    return 0;
}

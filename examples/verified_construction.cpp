// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// Verified types guarantee that all values are validated at compile time.
// Construction and arithmetic are consteval (compile-time only),
// while conversions and comparisons are constexpr (usable at runtime).

#include <boost/safe_numbers/verified_integers.hpp>
#include <boost/safe_numbers/iostream.hpp>
#include <iostream>
#include <cstdint>

int main()
{
    using namespace boost::safe_numbers;

    // Construction must happen at compile time.
    // All of these are evaluated by the compiler:
    constexpr verified_u8  a {u8{42}};
    constexpr verified_u16 b {u16{1000}};
    constexpr verified_u32 c {u32{100000}};
    constexpr verified_u64 d {u64{9999999999ULL}};

    // You can also construct from the underlying fundamental type directly
    constexpr verified_u32 e {42U};

    // At runtime, verified types are read-only values.
    // Output works because operator<< uses the constexpr conversion operator:
    std::cout << "verified_u8:  " << a << '\n';
    std::cout << "verified_u16: " << b << '\n';
    std::cout << "verified_u32: " << c << '\n';
    std::cout << "verified_u64: " << d << '\n';
    std::cout << "verified_u32: " << e << '\n';

    std::cout << '\n';

    // Explicit conversion back to fundamental types works at runtime
    const auto raw = static_cast<std::uint32_t>(c);
    std::cout << "Converted to uint32_t: " << raw << '\n';

    // Comparisons work at runtime
    if (a < b)
    {
        std::cout << "42 < 1000: true\n";
    }

    // The three-way comparison operator is also available
    if ((a <=> a) == std::strong_ordering::equal)
    {
        std::cout << "42 == 42: true\n";
    }

    std::cout << '\n';

    // Bounded verified types work the same way.
    // The bounded range is enforced at compile time:
    constexpr verified_bounded_integer<0u, 100u> percent {50u};
    std::cout << "percent: " << percent << '\n';

    // This would be a compile error - value out of bounds:
    // constexpr verified_bounded_integer<0u, 100u> bad {101u};

    return 0;
}

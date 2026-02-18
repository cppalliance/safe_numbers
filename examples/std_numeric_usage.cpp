// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

//
// This example demonstrates that standard library algorithms requiring
// only comparison operators work seamlessly with safe integer types.
//
// Note: std::gcd, std::lcm, and std::midpoint do NOT work because they
// static_assert on std::is_integral<T>, which is false for class types.
//

#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/iostream.hpp>
#include <algorithm>
#include <array>
#include <iostream>

int main()
{
    using namespace boost::safe_numbers;

    // ---- std::min / std::max ----
    std::cout << "=== std::min / std::max ===\n";
    {
        const auto a = u32{42U};
        const auto b = u32{100U};
        std::cout << "std::min(42, 100) = " << std::min(a, b) << '\n';
        std::cout << "std::max(42, 100) = " << std::max(a, b) << '\n';

        // Also works with initializer lists
        std::cout << "std::min({30, 10, 50}) = "
                  << std::min({u32{30U}, u32{10U}, u32{50U}}) << '\n';
        std::cout << "std::max({30, 10, 50}) = "
                  << std::max({u32{30U}, u32{10U}, u32{50U}}) << '\n';
    }

    // ---- std::clamp ----
    std::cout << "\n=== std::clamp ===\n";
    {
        const auto lo = u32{10U};
        const auto hi = u32{200U};
        std::cout << "std::clamp(5, 10, 200)   = " << std::clamp(u32{5U}, lo, hi) << '\n';
        std::cout << "std::clamp(50, 10, 200)  = " << std::clamp(u32{50U}, lo, hi) << '\n';
        std::cout << "std::clamp(999, 10, 200) = " << std::clamp(u32{999U}, lo, hi) << '\n';
    }

    // ---- std::sort ----
    std::cout << "\n=== std::sort ===\n";
    {
        std::array<u32, 5> values {u32{50U}, u32{10U}, u32{40U}, u32{20U}, u32{30U}};
        std::sort(values.begin(), values.end());
        std::cout << "sorted: ";
        for (const auto& v : values)
        {
            std::cout << v << ' ';
        }
        std::cout << '\n';
    }

    // ---- std::minmax_element ----
    std::cout << "\n=== std::minmax_element ===\n";
    {
        const std::array<u32, 5> values {u32{50U}, u32{10U}, u32{40U}, u32{20U}, u32{30U}};
        const auto [min_it, max_it] = std::minmax_element(values.begin(), values.end());
        std::cout << "min element = " << *min_it << '\n';
        std::cout << "max element = " << *max_it << '\n';
    }

    // ---- Works with different widths ----
    std::cout << "\n=== Other widths ===\n";
    {
        std::cout << "std::min(u8(10), u8(20))     = "
                  << static_cast<unsigned>(std::min(u8{10}, u8{20})) << '\n';
        std::cout << "std::max(u16(100), u16(200)) = "
                  << std::max(u16{static_cast<std::uint16_t>(100)},
                              u16{static_cast<std::uint16_t>(200)}) << '\n';
        std::cout << "std::clamp(u64(500), u64(0), u64(100)) = "
                  << std::clamp(u64{500ULL}, u64{0ULL}, u64{100ULL}) << '\n';
    }

    return 0;
}

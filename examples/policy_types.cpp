// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// This example demonstrates the type-level error policies.
// Instead of selecting a policy at each call site (saturating_add and friends),
// the policy can be part of the type itself: every operator on sat_u8 saturates
// and every operator on strict_u32 terminates on error.

#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/floats.hpp>
#include <boost/safe_numbers/iostream.hpp>
#include <iostream>
#include <cstdint>
#include <limits>

int main()
{
    using boost::safe_numbers::u8;
    using boost::safe_numbers::sat_u8;
    using boost::safe_numbers::sat_f32;
    using boost::safe_numbers::saturating_add;

    // A saturating accumulator: no exception handling needed, the value
    // simply pins at the maximum once the sum no longer fits
    {
        sat_u8 total {0U};
        for (int i = 0; i < 100; ++i)
        {
            total += sat_u8{10U};
        }

        std::cout << "sum of 100 tens in a sat_u8 = " << total << std::endl;
        // Output: 255 (saturated at UINT8_MAX)
    }

    // The operators are exactly the named free functions of the same policy
    {
        const sat_u8 a {200U};
        const sat_u8 b {100U};

        std::cout << "sat_u8{200} + sat_u8{100}          = " << a + b << std::endl;
        std::cout << "saturating_add(u8{200}, u8{100})   = "
                  << saturating_add(u8{200U}, u8{100U}) << std::endl;
        // Both output: 255
    }

    // Saturating overflow is a defined value, so it works in constant expressions
    {
        constexpr sat_u8 clamped {sat_u8{255U} + sat_u8{1U}};
        static_assert(static_cast<std::uint8_t>(clamped) == 255U);

        std::cout << "constexpr sat_u8{255} + sat_u8{1}  = " << clamped << std::endl;
        // Output: 255 (under the default throwing policy this would not compile)
    }

    // Saturating floats are raw IEEE 754 arithmetic: overflow goes to infinity
    {
        const sat_f32 big {std::numeric_limits<float>::max()};
        const sat_f32 doubled {big + big};

        std::cout << "sat_f32{FLT_MAX} + sat_f32{FLT_MAX} = " << doubled << std::endl;
        // Output: inf
    }

    // Types with different policies do not mix, exactly like different widths.
    // Uncommenting the next line produces a static_assert telling you to
    // convert explicitly through basis_type first:
    //
    // auto bad = sat_u8{1U} + u8{1U};

    // strict_u8, strict_i32, and the other strict_ aliases call
    // std::exit(EXIT_FAILURE) on any error instead of throwing, for code that
    // must never unwind. Their success paths behave identically to the
    // throwing types, so they are not exercised here.

    return 0;
}

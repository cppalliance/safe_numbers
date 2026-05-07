// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// This example demonstrates the use of user-defined literals for
// constructing safe numeric types. The literals provide a concise
// syntax and perform compile-time range checking when possible.

#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/signed_integers.hpp>
#include <boost/safe_numbers/floats.hpp>
#include <boost/safe_numbers/literals.hpp>
#include <boost/safe_numbers/iostream.hpp>
#include <iostream>

int main()
{
    using namespace boost::safe_numbers;
    using namespace boost::safe_numbers::literals;

    // Construct safe unsigned integers using literal suffixes
    {
        constexpr auto a {42_u8};
        constexpr auto b {1000_u16};
        constexpr auto c {100000_u32};
        constexpr auto d {9999999999_u64};
        constexpr auto e {340282366920938463463374607431768211455_u128};

        std::cout << "42_u8   = " << a << std::endl;
        std::cout << "1000_u16 = " << b << std::endl;
        std::cout << "100000_u32 = " << c << std::endl;
        std::cout << "9999999999_u64 = " << d << std::endl;
        std::cout << "max_u128 = " << e << std::endl;
    }

    // Signed integer literals. Negative values are formed by applying unary
    // minus to a positive literal: -42_i32 parses as -(42_i32).
    {
        constexpr auto a {-42_i8};
        constexpr auto b {32767_i16};
        constexpr auto c {-2147483647_i32};
        constexpr auto d {9223372036854775807_i64};
        constexpr auto e {-170141183460469231731687303715884105727_i128};

        std::cout << "-42_i8  = " << a << std::endl;
        std::cout << "max_i16 = " << b << std::endl;
        std::cout << "min+1_i32 = " << c << std::endl;
        std::cout << "max_i64 = " << d << std::endl;
        std::cout << "min+1_i128 = " << e << std::endl;
    }

    // Floating-point literals. The literal accepts a long double and is
    // range-checked against the target type's maximum.
    {
        constexpr auto pi {3.14_f32};
        constexpr auto e {2.718281828459045_f64};

        std::cout << "pi_f32 = " << pi << std::endl;
        std::cout << "e_f64  = " << e << std::endl;
    }

    // Literals work naturally in expressions
    {
        const auto sum {100_u32 + 50_u32};
        const auto product {6_u32 * 7_u32};

        std::cout << "100_u32 + 50_u32 = " << sum << std::endl;
        std::cout << "6_u32 * 7_u32    = " << product << std::endl;
    }

    // Literals are constexpr - can be used in compile-time contexts
    {
        constexpr auto compile_time {255_u8};
        static_assert(compile_time == u8{255U});

        constexpr auto zero {0_u32};
        static_assert(zero == u32{0U});

        std::cout << "constexpr 255_u8 = " << compile_time << std::endl;
    }

    // NOTE: Out-of-range literals throw std::overflow_error at runtime,
    // or produce a compile error when used in constexpr context:
    //
    //   auto bad = 256_u8;    // throws std::overflow_error (> UINT8_MAX)
    //   auto bad = 70000_u16; // throws std::overflow_error (> UINT16_MAX)
    //   auto bad = 128_i8;    // throws std::overflow_error (> INT8_MAX)
    //   auto bad = 1.0e40_f32; // throws std::overflow_error (> FLT_MAX)

    return 0;
}


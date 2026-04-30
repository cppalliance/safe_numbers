// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/lightweight_test.hpp>

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers/floats.hpp>

#endif

#include <cstdint>
#include <cstring>
#include <limits>
#include <type_traits>

using namespace boost::safe_numbers;

// Default construction.
static_assert(std::is_default_constructible_v<f32>);
static_assert(std::is_default_constructible_v<f64>);

// Construction from the matching basis type.
static_assert(std::is_constructible_v<f32, float>);
static_assert(std::is_constructible_v<f64, double>);

// Copy and move construction.
static_assert(std::is_copy_constructible_v<f32>);
static_assert(std::is_copy_constructible_v<f64>);
static_assert(std::is_move_constructible_v<f32>);
static_assert(std::is_move_constructible_v<f64>);

// Floating-point narrowing is rejected: f32 cannot be built from a wider
// floating-point type because the value may not be representable exactly.
static_assert(!std::is_constructible_v<f32, double>);
static_assert(!std::is_constructible_v<f32, long double>);
static_assert(!std::is_constructible_v<f64, long double>);

// Cross-precision construction is also rejected: f64 from float is widening
// but the user must opt in explicitly via the basis type.
static_assert(!std::is_constructible_v<f64, float>);

// Integer construction is rejected for both f32 and f64. Even when the
// value fits exactly in the mantissa, the conversion from integer to
// floating-point must be deliberate.
static_assert(!std::is_constructible_v<f32, char>);
static_assert(!std::is_constructible_v<f32, signed char>);
static_assert(!std::is_constructible_v<f32, unsigned char>);
static_assert(!std::is_constructible_v<f32, short>);
static_assert(!std::is_constructible_v<f32, unsigned short>);
static_assert(!std::is_constructible_v<f32, int>);
static_assert(!std::is_constructible_v<f32, unsigned int>);
static_assert(!std::is_constructible_v<f32, long>);
static_assert(!std::is_constructible_v<f32, unsigned long>);
static_assert(!std::is_constructible_v<f32, long long>);
static_assert(!std::is_constructible_v<f32, unsigned long long>);
static_assert(!std::is_constructible_v<f32, std::int8_t>);
static_assert(!std::is_constructible_v<f32, std::int16_t>);
static_assert(!std::is_constructible_v<f32, std::int32_t>);
static_assert(!std::is_constructible_v<f32, std::int64_t>);
static_assert(!std::is_constructible_v<f32, std::uint8_t>);
static_assert(!std::is_constructible_v<f32, std::uint16_t>);
static_assert(!std::is_constructible_v<f32, std::uint32_t>);
static_assert(!std::is_constructible_v<f32, std::uint64_t>);

static_assert(!std::is_constructible_v<f64, char>);
static_assert(!std::is_constructible_v<f64, signed char>);
static_assert(!std::is_constructible_v<f64, unsigned char>);
static_assert(!std::is_constructible_v<f64, short>);
static_assert(!std::is_constructible_v<f64, unsigned short>);
static_assert(!std::is_constructible_v<f64, int>);
static_assert(!std::is_constructible_v<f64, unsigned int>);
static_assert(!std::is_constructible_v<f64, long>);
static_assert(!std::is_constructible_v<f64, unsigned long>);
static_assert(!std::is_constructible_v<f64, long long>);
static_assert(!std::is_constructible_v<f64, unsigned long long>);
static_assert(!std::is_constructible_v<f64, std::int8_t>);
static_assert(!std::is_constructible_v<f64, std::int16_t>);
static_assert(!std::is_constructible_v<f64, std::int32_t>);
static_assert(!std::is_constructible_v<f64, std::int64_t>);
static_assert(!std::is_constructible_v<f64, std::uint8_t>);
static_assert(!std::is_constructible_v<f64, std::uint16_t>);
static_assert(!std::is_constructible_v<f64, std::uint32_t>);
static_assert(!std::is_constructible_v<f64, std::uint64_t>);

// Bool is strictly disallowed, mirroring the integer types.
static_assert(!std::is_constructible_v<f32, bool>);
static_assert(!std::is_constructible_v<f64, bool>);

// The basis_type alias resolves back to the built-in floating-point type.
static_assert(std::is_same_v<f32::basis_type, float>);
static_assert(std::is_same_v<f64::basis_type, double>);

void test_f32_round_trip()
{
    const f32 zero {};
    std::uint32_t zero_bits;
    std::memcpy(&zero_bits, &zero, sizeof(f32));
    BOOST_TEST_EQ(zero_bits, std::uint32_t{0});

    const float values[] {
        0.0f,
        1.0f,
        -1.0f,
        3.14159265f,
        -3.14159265f,
        std::numeric_limits<float>::min(),
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::lowest(),
        std::numeric_limits<float>::epsilon(),
        std::numeric_limits<float>::infinity(),
        -std::numeric_limits<float>::infinity(),
        std::numeric_limits<float>::denorm_min()
    };

    for (const auto val : values)
    {
        const f32 v {val};
        std::uint32_t actual_bits;
        std::uint32_t expected_bits;
        std::memcpy(&actual_bits, &v, sizeof(f32));
        std::memcpy(&expected_bits, &val, sizeof(float));
        BOOST_TEST_EQ(actual_bits, expected_bits);
    }
}

void test_f64_round_trip()
{
    const f64 zero {};
    std::uint64_t zero_bits;
    std::memcpy(&zero_bits, &zero, sizeof(f64));
    BOOST_TEST_EQ(zero_bits, std::uint64_t{0});

    const double values[] {
        0.0,
        1.0,
        -1.0,
        3.141592653589793,
        -3.141592653589793,
        std::numeric_limits<double>::min(),
        std::numeric_limits<double>::max(),
        std::numeric_limits<double>::lowest(),
        std::numeric_limits<double>::epsilon(),
        std::numeric_limits<double>::infinity(),
        -std::numeric_limits<double>::infinity(),
        std::numeric_limits<double>::denorm_min()
    };

    for (const auto val : values)
    {
        const f64 v {val};
        std::uint64_t actual_bits;
        std::uint64_t expected_bits;
        std::memcpy(&actual_bits, &v, sizeof(f64));
        std::memcpy(&expected_bits, &val, sizeof(double));
        BOOST_TEST_EQ(actual_bits, expected_bits);
    }
}

int main()
{
    test_f32_round_trip();
    test_f64_round_trip();

    return boost::report_errors();
}

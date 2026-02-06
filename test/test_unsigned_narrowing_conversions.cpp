// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/lightweight_test.hpp>

// Ignore [[nodiscard]] on the tests that we know are going to throw
#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunused-result"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wunused-result"
#elif defined(_MSC_VER)
#  pragma warning (push)
#  pragma warning (disable: 4834)
#endif

#include <boost/safe_numbers.hpp>

#include <stdexcept>
#include <limits>
#include <cstdint>

using namespace boost::safe_numbers;

// The conversion operator OtherBasis() converts to raw unsigned integral types
// (std::uint8_t, std::uint16_t, etc.), not to other library types.

// -----------------------------------------------
// Widening conversions (should always succeed)
// -----------------------------------------------

void test_widening_u8_to_larger()
{
    const u8 a {200};
    const auto b {static_cast<std::uint16_t>(a)};
    BOOST_TEST_EQ(b, static_cast<std::uint16_t>(200));

    const auto c {static_cast<std::uint32_t>(a)};
    BOOST_TEST_EQ(c, static_cast<std::uint32_t>(200));

    const auto d {static_cast<std::uint64_t>(a)};
    BOOST_TEST_EQ(d, static_cast<std::uint64_t>(200));

    const u8 zero {0};
    BOOST_TEST_EQ(static_cast<std::uint16_t>(zero), static_cast<std::uint16_t>(0));
    BOOST_TEST_EQ(static_cast<std::uint32_t>(zero), static_cast<std::uint32_t>(0));
    BOOST_TEST_EQ(static_cast<std::uint64_t>(zero), static_cast<std::uint64_t>(0));

    const u8 max8 {255};
    BOOST_TEST_EQ(static_cast<std::uint16_t>(max8), static_cast<std::uint16_t>(255));
    BOOST_TEST_EQ(static_cast<std::uint32_t>(max8), static_cast<std::uint32_t>(255));
    BOOST_TEST_EQ(static_cast<std::uint64_t>(max8), static_cast<std::uint64_t>(255));
}

void test_widening_u16_to_larger()
{
    const u16 a {65535};
    const auto b {static_cast<std::uint32_t>(a)};
    BOOST_TEST_EQ(b, static_cast<std::uint32_t>(65535));

    const auto c {static_cast<std::uint64_t>(a)};
    BOOST_TEST_EQ(c, static_cast<std::uint64_t>(65535));

    const u16 zero {0};
    BOOST_TEST_EQ(static_cast<std::uint32_t>(zero), static_cast<std::uint32_t>(0));
    BOOST_TEST_EQ(static_cast<std::uint64_t>(zero), static_cast<std::uint64_t>(0));
}

void test_widening_u32_to_u64()
{
    const u32 a {4294967295u};
    const auto b {static_cast<std::uint64_t>(a)};
    BOOST_TEST_EQ(b, static_cast<std::uint64_t>(4294967295u));

    const u32 zero {0};
    BOOST_TEST_EQ(static_cast<std::uint64_t>(zero), static_cast<std::uint64_t>(0));
}

// -----------------------------------------------
// Same-width conversions (identity, always succeed)
// -----------------------------------------------

void test_same_width()
{
    const u8 a {42};
    BOOST_TEST_EQ(static_cast<std::uint8_t>(a), static_cast<std::uint8_t>(42));

    const u16 b {1000};
    BOOST_TEST_EQ(static_cast<std::uint16_t>(b), static_cast<std::uint16_t>(1000));

    const u32 c {100000};
    BOOST_TEST_EQ(static_cast<std::uint32_t>(c), static_cast<std::uint32_t>(100000));

    const u64 d {1'000'000'000ULL};
    BOOST_TEST_EQ(static_cast<std::uint64_t>(d), static_cast<std::uint64_t>(1'000'000'000ULL));
}

// -----------------------------------------------
// Narrowing conversions that fit (should succeed)
// -----------------------------------------------

void test_narrowing_u16_to_u8_fits()
{
    const u16 a {0};
    BOOST_TEST_EQ(static_cast<std::uint8_t>(a), static_cast<std::uint8_t>(0));

    const u16 b {255};
    BOOST_TEST_EQ(static_cast<std::uint8_t>(b), static_cast<std::uint8_t>(255));

    const u16 c {42};
    BOOST_TEST_EQ(static_cast<std::uint8_t>(c), static_cast<std::uint8_t>(42));
}

void test_narrowing_u32_to_u8_fits()
{
    const u32 a {0};
    BOOST_TEST_EQ(static_cast<std::uint8_t>(a), static_cast<std::uint8_t>(0));

    const u32 b {255};
    BOOST_TEST_EQ(static_cast<std::uint8_t>(b), static_cast<std::uint8_t>(255));
}

void test_narrowing_u32_to_u16_fits()
{
    const u32 a {0};
    BOOST_TEST_EQ(static_cast<std::uint16_t>(a), static_cast<std::uint16_t>(0));

    const u32 b {65535};
    BOOST_TEST_EQ(static_cast<std::uint16_t>(b), static_cast<std::uint16_t>(65535));
}

void test_narrowing_u64_to_u8_fits()
{
    const u64 a {0};
    BOOST_TEST_EQ(static_cast<std::uint8_t>(a), static_cast<std::uint8_t>(0));

    const u64 b {255};
    BOOST_TEST_EQ(static_cast<std::uint8_t>(b), static_cast<std::uint8_t>(255));
}

void test_narrowing_u64_to_u16_fits()
{
    const u64 a {65535};
    BOOST_TEST_EQ(static_cast<std::uint16_t>(a), static_cast<std::uint16_t>(65535));
}

void test_narrowing_u64_to_u32_fits()
{
    const u64 a {4294967295ULL};
    BOOST_TEST_EQ(static_cast<std::uint32_t>(a), static_cast<std::uint32_t>(4294967295u));

    const u64 b {0};
    BOOST_TEST_EQ(static_cast<std::uint32_t>(b), static_cast<std::uint32_t>(0));
}

// -----------------------------------------------
// Narrowing conversions that overflow (should throw)
// -----------------------------------------------

void test_narrowing_u16_to_u8_throws()
{
    // 256 > 255 (u8 max)
    const u16 a {256};
    BOOST_TEST_THROWS((static_cast<std::uint8_t>(a)), std::domain_error);

    // Max u16 value
    const u16 b {65535};
    BOOST_TEST_THROWS((static_cast<std::uint8_t>(b)), std::domain_error);

    // Just above u8 max
    const u16 c {300};
    BOOST_TEST_THROWS((static_cast<std::uint8_t>(c)), std::domain_error);
}

void test_narrowing_u32_to_u8_throws()
{
    const u32 a {256};
    BOOST_TEST_THROWS((static_cast<std::uint8_t>(a)), std::domain_error);

    const u32 b {100000};
    BOOST_TEST_THROWS((static_cast<std::uint8_t>(b)), std::domain_error);
}

void test_narrowing_u32_to_u16_throws()
{
    // 65536 > 65535 (u16 max)
    const u32 a {65536};
    BOOST_TEST_THROWS((static_cast<std::uint16_t>(a)), std::domain_error);

    const u32 b {4294967295u};
    BOOST_TEST_THROWS((static_cast<std::uint16_t>(b)), std::domain_error);
}

void test_narrowing_u64_to_u8_throws()
{
    const u64 a {256};
    BOOST_TEST_THROWS((static_cast<std::uint8_t>(a)), std::domain_error);

    const u64 b {1'000'000};
    BOOST_TEST_THROWS((static_cast<std::uint8_t>(b)), std::domain_error);
}

void test_narrowing_u64_to_u16_throws()
{
    const u64 a {65536};
    BOOST_TEST_THROWS((static_cast<std::uint16_t>(a)), std::domain_error);

    const u64 b {UINT64_MAX};
    BOOST_TEST_THROWS((static_cast<std::uint16_t>(b)), std::domain_error);
}

void test_narrowing_u64_to_u32_throws()
{
    // 4294967296 > UINT32_MAX
    const u64 a {4294967296ULL};
    BOOST_TEST_THROWS((static_cast<std::uint32_t>(a)), std::domain_error);

    const u64 b {UINT64_MAX};
    BOOST_TEST_THROWS((static_cast<std::uint32_t>(b)), std::domain_error);
}

// -----------------------------------------------
// Boundary values (exact max fits, max+1 throws)
// -----------------------------------------------

void test_narrowing_boundaries()
{
    // u16 -> u8: 255 fits, 256 does not
    const u16 fits {255};
    BOOST_TEST_EQ(static_cast<std::uint8_t>(fits), static_cast<std::uint8_t>(255));

    const u16 no_fit {256};
    BOOST_TEST_THROWS((static_cast<std::uint8_t>(no_fit)), std::domain_error);

    // u32 -> u16: 65535 fits, 65536 does not
    const u32 fits32 {65535};
    BOOST_TEST_EQ(static_cast<std::uint16_t>(fits32), static_cast<std::uint16_t>(65535));

    const u32 no_fit32 {65536};
    BOOST_TEST_THROWS((static_cast<std::uint16_t>(no_fit32)), std::domain_error);

    // u64 -> u32: UINT32_MAX fits, UINT32_MAX+1 does not
    const u64 fits64 {4294967295ULL};
    BOOST_TEST_EQ(static_cast<std::uint32_t>(fits64), static_cast<std::uint32_t>(4294967295u));

    const u64 no_fit64 {4294967296ULL};
    BOOST_TEST_THROWS((static_cast<std::uint32_t>(no_fit64)), std::domain_error);
}

int main()
{
    // Widening (always succeed)
    test_widening_u8_to_larger();
    test_widening_u16_to_larger();
    test_widening_u32_to_u64();

    // Same-width
    test_same_width();

    // Narrowing that fits (succeed)
    test_narrowing_u16_to_u8_fits();
    test_narrowing_u32_to_u8_fits();
    test_narrowing_u32_to_u16_fits();
    test_narrowing_u64_to_u8_fits();
    test_narrowing_u64_to_u16_fits();
    test_narrowing_u64_to_u32_fits();

    // Narrowing that overflows (throw)
    test_narrowing_u16_to_u8_throws();
    test_narrowing_u32_to_u8_throws();
    test_narrowing_u32_to_u16_throws();
    test_narrowing_u64_to_u8_throws();
    test_narrowing_u64_to_u16_throws();
    test_narrowing_u64_to_u32_throws();

    // Boundary values
    test_narrowing_boundaries();

    return boost::report_errors();
}

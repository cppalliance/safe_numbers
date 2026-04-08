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

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>
#include <limits>
#include <type_traits>

#endif

#include <stdexcept>
#include <cstdint>

using namespace boost::safe_numbers;

// -----------------------------------------------
// Conversion to basis_type (noexcept)
// -----------------------------------------------

void test_conversion_to_basis_type()
{
    constexpr bounded_int<-100, 100> a {i8{42}};
    const auto b {static_cast<i8>(a)};
    const i8 expected {42};
    BOOST_TEST(b == expected);

    constexpr bounded_int<-100, 100> c {i8{-73}};
    const auto d {static_cast<i8>(c)};
    const i8 expected_d {-73};
    BOOST_TEST(d == expected_d);
}

// -----------------------------------------------
// Conversion to underlying_type (noexcept)
// -----------------------------------------------

void test_conversion_to_underlying_type()
{
    constexpr bounded_int<-100, 100> a {i8{42}};
    const auto b {static_cast<std::int8_t>(a)};
    BOOST_TEST(b == static_cast<std::int8_t>(42));

    constexpr bounded_int<-100, 100> c {i8{-73}};
    const auto d {static_cast<std::int8_t>(c)};
    BOOST_TEST(d == static_cast<std::int8_t>(-73));
}

// -----------------------------------------------
// Widening conversion (basis to larger signed type)
// -----------------------------------------------

void test_widening_conversion()
{
    constexpr bounded_int<-100, 100> a {i8{42}};
    const auto b {static_cast<i16>(a)};
    const i16 expected {42};
    BOOST_TEST(b == expected);

    const auto c {static_cast<i32>(a)};
    const i32 expected_c {42};
    BOOST_TEST(c == expected_c);

    const auto d {static_cast<i64>(a)};
    const i64 expected_d {42};
    BOOST_TEST(d == expected_d);
}

void test_widening_negative()
{
    constexpr bounded_int<-100, 100> a {i8{-100}};
    const auto b {static_cast<i16>(a)};
    const i16 expected {-100};
    BOOST_TEST(b == expected);
}

// -----------------------------------------------
// Narrowing conversion (larger basis to smaller type)
// -----------------------------------------------

void test_narrowing_conversion_fits()
{
    // bounded_int<-1000, 1000> has i16 basis, convert to i8 with value 42 (fits)
    constexpr bounded_int<-1000, 1000> a {i16{42}};
    const auto b {static_cast<i8>(a)};
    const i8 expected {42};
    BOOST_TEST(b == expected);

    // Negative value that fits
    constexpr bounded_int<-1000, 1000> c {i16{-100}};
    const auto d {static_cast<i8>(c)};
    const i8 expected_d {-100};
    BOOST_TEST(d == expected_d);
}

void test_narrowing_conversion_throws()
{
    // Value 500 exceeds int8_t max (127)
    constexpr bounded_int<-1000, 1000> a {i16{500}};
    BOOST_TEST_THROWS((static_cast<i8>(a)), std::domain_error);

    // Value -500 is below int8_t min (-128)
    constexpr bounded_int<-1000, 1000> b {i16{-500}};
    BOOST_TEST_THROWS((static_cast<i8>(b)), std::domain_error);
}

// -----------------------------------------------
// Cross-bounded conversion (different bounds, same or different basis)
// -----------------------------------------------

void test_cross_bounded_fits()
{
    constexpr bounded_int<-100, 100> a {i8{42}};
    const auto b {static_cast<bounded_int<-50, 50>>(a)};
    const bounded_int<-50, 50> expected {i8{42}};
    BOOST_TEST(b == expected);
}

void test_cross_bounded_throws()
{
    constexpr bounded_int<-100, 100> a {i8{75}};
    BOOST_TEST_THROWS((static_cast<bounded_int<-50, 50>>(a)), std::domain_error);

    constexpr bounded_int<-100, 100> b {i8{-75}};
    BOOST_TEST_THROWS((static_cast<bounded_int<-50, 50>>(b)), std::domain_error);
}

void test_cross_bounded_wider_to_narrower()
{
    // [−200, 200] (i16 basis) → [−50, 50] (i8 basis): value 25 fits
    constexpr bounded_int<-200, 200> a {i16{25}};
    const auto b {static_cast<bounded_int<-50, 50>>(a)};
    const bounded_int<-50, 50> expected {i8{25}};
    BOOST_TEST(b == expected);
}

void test_cross_bounded_wider_to_narrower_throws()
{
    // [−200, 200] (i16 basis) → [−50, 50] (i8 basis): value 100 exceeds target max
    constexpr bounded_int<-200, 200> a {i16{100}};
    BOOST_TEST_THROWS((static_cast<bounded_int<-50, 50>>(a)), std::domain_error);
}

// -----------------------------------------------
// Identity conversion (same bounds)
// -----------------------------------------------

void test_identity_conversion()
{
    constexpr bounded_int<-100, 100> a {i8{42}};
    const auto b {static_cast<bounded_int<-100, 100>>(a)};
    BOOST_TEST(a == b);

    constexpr bounded_int<-100, 100> c {i8{-100}};
    const auto d {static_cast<bounded_int<-100, 100>>(c)};
    BOOST_TEST(c == d);
}

// -----------------------------------------------
// Constexpr conversions
// -----------------------------------------------

void test_constexpr_conversions()
{
    // basis_type conversion
    constexpr bounded_int<-100, 100> a {i8{42}};
    constexpr auto b {static_cast<i8>(a)};
    static_assert(b == i8{42});

    // widening
    constexpr auto c {static_cast<i16>(a)};
    static_assert(c == i16{42});

    // cross-bounded
    constexpr auto d {static_cast<bounded_int<-50, 50>>(a)};
    constexpr bounded_int<-50, 50> expected_d {i8{42}};
    static_assert(d == expected_d);
}

int main()
{
    test_conversion_to_basis_type();
    test_conversion_to_underlying_type();

    test_widening_conversion();
    test_widening_negative();

    test_narrowing_conversion_fits();
    test_narrowing_conversion_throws();

    test_cross_bounded_fits();
    test_cross_bounded_throws();
    test_cross_bounded_wider_to_narrower();
    test_cross_bounded_wider_to_narrower_throws();

    test_identity_conversion();
    test_constexpr_conversions();

    return boost::report_errors();
}

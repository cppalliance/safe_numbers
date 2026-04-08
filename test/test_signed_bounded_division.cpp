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

void test_valid_division()
{
    constexpr bounded_int<-100, 100> a {i8{50}};
    constexpr bounded_int<-100, 100> b {i8{5}};
    const auto c {a / b};
    const bounded_int<-100, 100> expected {i8{10}};
    BOOST_TEST(c == expected);
}

void test_division_by_zero()
{
    constexpr bounded_int<-100, 100> a {i8{10}};
    constexpr bounded_int<-100, 100> zero {i8{0}};
    BOOST_TEST_THROWS(a / zero, std::domain_error);
}

void test_min_div_neg_one_overflow()
{
    // -128 / -1 = 128, which overflows int8_t max (127)
    constexpr bounded_int<-128, 127> a {i8{-128}};
    constexpr bounded_int<-128, 127> neg_one {i8{-1}};
    BOOST_TEST_THROWS(a / neg_one, std::overflow_error);
}

void test_zero_divided_by_one()
{
    constexpr bounded_int<-100, 100> a {i8{0}};
    constexpr bounded_int<-100, 100> one {i8{1}};
    const auto c {a / one};
    const bounded_int<-100, 100> expected {i8{0}};
    BOOST_TEST(c == expected);
}

void test_negative_division()
{
    constexpr bounded_int<-100, 100> a {i8{-50}};
    constexpr bounded_int<-100, 100> b {i8{5}};
    const auto c {a / b};
    const bounded_int<-100, 100> expected {i8{-10}};
    BOOST_TEST(c == expected);
}

void test_compound_assignment_division()
{
    bounded_int<-100, 100> a {i8{50}};
    const bounded_int<-100, 100> b {i8{5}};
    a /= b;
    const bounded_int<-100, 100> expected {i8{10}};
    BOOST_TEST(a == expected);
}

void test_self_division()
{
    constexpr bounded_int<-100, 100> a {i8{42}};
    const auto c {a / a};
    const bounded_int<-100, 100> expected {i8{1}};
    BOOST_TEST(c == expected);
}

void test_divide_by_one_identity()
{
    constexpr bounded_int<-100, 100> a {i8{-73}};
    constexpr bounded_int<-100, 100> one {i8{1}};
    const auto c {a / one};
    BOOST_TEST(c == a);
}

void test_truncation()
{
    // Integer division truncates toward zero: 7 / 2 = 3
    constexpr bounded_int<-100, 100> a {i8{7}};
    constexpr bounded_int<-100, 100> b {i8{2}};
    const auto c {a / b};
    const bounded_int<-100, 100> expected {i8{3}};
    BOOST_TEST(c == expected);

    // -7 / 2 = -3 (truncation toward zero)
    constexpr bounded_int<-100, 100> d {i8{-7}};
    const auto e {d / b};
    const bounded_int<-100, 100> expected_e {i8{-3}};
    BOOST_TEST(e == expected_e);
}

int main()
{
    test_valid_division();
    test_division_by_zero();
    test_min_div_neg_one_overflow();
    test_zero_divided_by_one();
    test_negative_division();
    test_compound_assignment_division();
    test_self_division();
    test_divide_by_one_identity();
    test_truncation();

    return boost::report_errors();
}

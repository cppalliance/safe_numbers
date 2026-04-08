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

void test_valid_modulo()
{
    constexpr bounded_int<-100, 100> a {i8{53}};
    constexpr bounded_int<-100, 100> b {i8{10}};
    const auto c {a % b};
    const bounded_int<-100, 100> expected {i8{3}};
    BOOST_TEST(c == expected);
}

void test_mod_by_zero()
{
    constexpr bounded_int<-100, 100> a {i8{10}};
    constexpr bounded_int<-100, 100> zero {i8{0}};
    BOOST_TEST_THROWS(a % zero, std::domain_error);
}

void test_min_mod_neg_one_overflow()
{
    // -128 % -1 is undefined behavior for int8_t; the library throws overflow
    constexpr bounded_int<-128, 127> a {i8{-128}};
    constexpr bounded_int<-128, 127> neg_one {i8{-1}};
    BOOST_TEST_THROWS(a % neg_one, std::overflow_error);
}

void test_negative_modulo()
{
    // C++ truncation toward zero: -53 % 10 = -3
    constexpr bounded_int<-100, 100> a {i8{-53}};
    constexpr bounded_int<-100, 100> b {i8{10}};
    const auto c {a % b};
    const bounded_int<-100, 100> expected {i8{-3}};
    BOOST_TEST(c == expected);
}

void test_zero_mod()
{
    // 0 % x = 0 (fast path)
    constexpr bounded_int<-100, 100> a {i8{0}};
    constexpr bounded_int<-100, 100> b {i8{7}};
    const auto c {a % b};
    const bounded_int<-100, 100> expected {i8{0}};
    BOOST_TEST(c == expected);
}

void test_mod_by_one()
{
    // Any value % 1 = 0
    constexpr bounded_int<-100, 100> a {i8{42}};
    constexpr bounded_int<-100, 100> one {i8{1}};
    const auto c {a % one};
    const bounded_int<-100, 100> expected {i8{0}};
    BOOST_TEST(c == expected);
}

void test_mod_exact_division()
{
    // 50 % 10 = 0 (exact division)
    constexpr bounded_int<-100, 100> a {i8{50}};
    constexpr bounded_int<-100, 100> b {i8{10}};
    const auto c {a % b};
    const bounded_int<-100, 100> expected {i8{0}};
    BOOST_TEST(c == expected);
}

void test_negative_divisor()
{
    // 53 % -10 = 3 (sign follows dividend in C++)
    constexpr bounded_int<-100, 100> a {i8{53}};
    constexpr bounded_int<-100, 100> b {i8{-10}};
    const auto c {a % b};
    const bounded_int<-100, 100> expected {i8{3}};
    BOOST_TEST(c == expected);

    // -53 % -10 = -3
    constexpr bounded_int<-100, 100> d {i8{-53}};
    const auto e {d % b};
    const bounded_int<-100, 100> expected_e {i8{-3}};
    BOOST_TEST(e == expected_e);
}

int main()
{
    test_valid_modulo();
    test_mod_by_zero();
    test_min_mod_neg_one_overflow();
    test_negative_modulo();
    test_zero_mod();
    test_mod_by_one();
    test_mod_exact_division();
    test_negative_divisor();

    return boost::report_errors();
}

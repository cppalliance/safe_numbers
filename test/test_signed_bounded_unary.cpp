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
// Unary plus
// -----------------------------------------------

void test_unary_plus()
{
    constexpr bounded_int<-100, 100> a {i8{42}};
    const auto b {+a};
    const bounded_int<-100, 100> expected {i8{42}};
    BOOST_TEST(b == expected);
}

// -----------------------------------------------
// Unary negation
// -----------------------------------------------

void test_negation_positive()
{
    constexpr bounded_int<-100, 100> a {i8{42}};
    const auto b {-a};
    const bounded_int<-100, 100> expected {i8{-42}};
    BOOST_TEST(b == expected);
}

void test_negation_negative()
{
    constexpr bounded_int<-100, 100> a {i8{-42}};
    const auto b {-a};
    const bounded_int<-100, 100> expected {i8{42}};
    BOOST_TEST(b == expected);
}

void test_negation_zero()
{
    constexpr bounded_int<-100, 100> a {i8{0}};
    const auto b {-a};
    const bounded_int<-100, 100> expected {i8{0}};
    BOOST_TEST(b == expected);
}

void test_negation_out_of_range()
{
    // -(-10) = 10, but max is 5, so the constructor rejects it
    constexpr bounded_int<-10, 5> a {i8{-10}};
    BOOST_TEST_THROWS(-a, std::domain_error);
}

void test_negation_type_min_overflow()
{
    // -(-128) overflows int8_t (result 128 > INT8_MAX)
    constexpr bounded_int<-128, 127> a {i8{-128}};
    BOOST_TEST_THROWS(-a, std::overflow_error);
}

// -----------------------------------------------
// Pre-increment
// -----------------------------------------------

void test_pre_increment_valid()
{
    bounded_int<-100, 100> a {i8{99}};
    ++a;
    const bounded_int<-100, 100> expected {i8{100}};
    BOOST_TEST(a == expected);
}

void test_pre_increment_at_max()
{
    bounded_int<-100, 100> a {i8{100}};
    BOOST_TEST_THROWS(++a, std::domain_error);
}

// -----------------------------------------------
// Pre-decrement
// -----------------------------------------------

void test_pre_decrement_valid()
{
    bounded_int<-100, 100> a {i8{-99}};
    --a;
    const bounded_int<-100, 100> expected {i8{-100}};
    BOOST_TEST(a == expected);
}

void test_pre_decrement_at_min()
{
    bounded_int<-100, 100> a {i8{-100}};
    BOOST_TEST_THROWS(--a, std::domain_error);
}

// -----------------------------------------------
// Post-increment
// -----------------------------------------------

void test_post_increment_valid()
{
    bounded_int<-100, 100> a {i8{50}};
    const auto old {a++};
    const bounded_int<-100, 100> expected_old {i8{50}};
    const bounded_int<-100, 100> expected_new {i8{51}};
    BOOST_TEST(old == expected_old);
    BOOST_TEST(a == expected_new);
}

void test_post_increment_at_max()
{
    bounded_int<-100, 100> a {i8{100}};
    BOOST_TEST_THROWS(a++, std::domain_error);
}

// -----------------------------------------------
// Post-decrement
// -----------------------------------------------

void test_post_decrement_valid()
{
    bounded_int<-100, 100> a {i8{-50}};
    const auto old {a--};
    const bounded_int<-100, 100> expected_old {i8{-50}};
    const bounded_int<-100, 100> expected_new {i8{-51}};
    BOOST_TEST(old == expected_old);
    BOOST_TEST(a == expected_new);
}

void test_post_decrement_at_min()
{
    bounded_int<-100, 100> a {i8{-100}};
    BOOST_TEST_THROWS(a--, std::domain_error);
}

int main()
{
    test_unary_plus();
    test_negation_positive();
    test_negation_negative();
    test_negation_zero();
    test_negation_out_of_range();
    test_negation_type_min_overflow();

    test_pre_increment_valid();
    test_pre_increment_at_max();
    test_pre_decrement_valid();
    test_pre_decrement_at_min();

    test_post_increment_valid();
    test_post_increment_at_max();
    test_post_decrement_valid();
    test_post_decrement_at_min();

    return boost::report_errors();
}

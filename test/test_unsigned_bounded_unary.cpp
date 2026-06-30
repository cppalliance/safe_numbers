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

// bounded_uint has no unary plus or negation (those are blocked for unsigned
// types), so the only unary operations to exercise are increment and decrement.
// Each throws in two distinct ways: an underlying-type overflow/underflow when a
// bound sits at the underlying type's limit, and a result-out-of-range domain
// error otherwise.

// -----------------------------------------------
// Pre-increment
// -----------------------------------------------

void test_pre_increment_valid()
{
    bounded_uint<0u, 100u> a {u8{5}};
    ++a;
    const bounded_uint<0u, 100u> expected {u8{6}};
    BOOST_TEST(a == expected);
}

void test_pre_increment_at_max()
{
    // Max < UINT8_MAX, so 100 + 1 = 101 fits the underlying type but exceeds the
    // bound (result out of range, not a type overflow).
    bounded_uint<0u, 100u> a {u8{100}};
    BOOST_TEST_THROWS(++a, std::domain_error);
}

void test_pre_increment_type_max_overflow()
{
    // Max equals UINT8_MAX, so 255 + 1 wraps the underlying type itself and throws
    // before the result-range check (overflow, not domain).
    bounded_uint<0u, 255u> a {u8{255}};
    BOOST_TEST_THROWS(++a, std::overflow_error);
}

// -----------------------------------------------
// Pre-decrement
// -----------------------------------------------

void test_pre_decrement_valid()
{
    bounded_uint<0u, 100u> a {u8{5}};
    --a;
    const bounded_uint<0u, 100u> expected {u8{4}};
    BOOST_TEST(a == expected);
}

void test_pre_decrement_at_min()
{
    // Min > 0, so 10 - 1 = 9 fits the underlying type but falls below the bound
    // (result out of range, not a type underflow).
    bounded_uint<10u, 100u> a {u8{10}};
    BOOST_TEST_THROWS(--a, std::domain_error);
}

void test_pre_decrement_type_min_underflow()
{
    // Min equals 0 (the underlying type minimum), so 0 - 1 wraps the underlying
    // type itself and throws before the result-range check (underflow, not domain).
    bounded_uint<0u, 255u> a {u8{0}};
    BOOST_TEST_THROWS(--a, std::underflow_error);
}

// -----------------------------------------------
// Post-increment
// -----------------------------------------------

void test_post_increment_valid()
{
    bounded_uint<0u, 100u> a {u8{50}};
    const auto old {a++};
    const bounded_uint<0u, 100u> expected_old {u8{50}};
    const bounded_uint<0u, 100u> expected_new {u8{51}};
    BOOST_TEST(old == expected_old);
    BOOST_TEST(a == expected_new);
}

void test_post_increment_at_max()
{
    bounded_uint<0u, 100u> a {u8{100}};
    BOOST_TEST_THROWS(a++, std::domain_error);
}

// -----------------------------------------------
// Post-decrement
// -----------------------------------------------

void test_post_decrement_valid()
{
    bounded_uint<0u, 100u> a {u8{50}};
    const auto old {a--};
    const bounded_uint<0u, 100u> expected_old {u8{50}};
    const bounded_uint<0u, 100u> expected_new {u8{49}};
    BOOST_TEST(old == expected_old);
    BOOST_TEST(a == expected_new);
}

void test_post_decrement_at_min()
{
    bounded_uint<10u, 100u> a {u8{10}};
    BOOST_TEST_THROWS(a--, std::domain_error);
}

int main()
{
    test_pre_increment_valid();
    test_pre_increment_at_max();
    test_pre_increment_type_max_overflow();
    test_pre_decrement_valid();
    test_pre_decrement_at_min();
    test_pre_decrement_type_min_underflow();

    test_post_increment_valid();
    test_post_increment_at_max();
    test_post_decrement_valid();
    test_post_decrement_at_min();

    return boost::report_errors();
}

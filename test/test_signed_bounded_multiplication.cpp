// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/lightweight_test.hpp>

#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunused-result"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wunused-result"
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable: 4834)
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
// i8 basis: bounded_int<-100, 100>
// -----------------------------------------------

void test_i8_valid_multiplication()
{
    // Basic: 5 * 10 = 50
    constexpr bounded_int<-100, 100> a {i8{5}};
    constexpr bounded_int<-100, 100> b {i8{10}};
    const auto c {a * b};
    const bounded_int<-100, 100> expected {i8{50}};
    BOOST_TEST(c == expected);

    // Multiply by zero
    constexpr bounded_int<-100, 100> zero {i8{0}};
    const auto d {a * zero};
    const bounded_int<-100, 100> expected_d {i8{0}};
    BOOST_TEST(d == expected_d);

    // Multiply by one
    constexpr bounded_int<-100, 100> one {i8{1}};
    const auto e {a * one};
    BOOST_TEST(e == a);

    // Negative result: -5 * 10 = -50
    constexpr bounded_int<-100, 100> f {i8{-5}};
    constexpr bounded_int<-100, 100> g {i8{10}};
    const auto h {f * g};
    const bounded_int<-100, 100> expected_h {i8{-50}};
    BOOST_TEST(h == expected_h);

    // Negative * negative = positive: -5 * -10 = 50
    constexpr bounded_int<-100, 100> i {i8{-5}};
    constexpr bounded_int<-100, 100> j {i8{-10}};
    const auto k {i * j};
    const bounded_int<-100, 100> expected_k {i8{50}};
    BOOST_TEST(k == expected_k);

    // Max boundary: 10 * 10 = 100
    constexpr bounded_int<-100, 100> l {i8{10}};
    constexpr bounded_int<-100, 100> m {i8{10}};
    const auto n {l * m};
    const bounded_int<-100, 100> expected_n {i8{100}};
    BOOST_TEST(n == expected_n);

    // Min boundary: -10 * 10 = -100
    constexpr bounded_int<-100, 100> o {i8{-10}};
    constexpr bounded_int<-100, 100> p {i8{10}};
    const auto q {o * p};
    const bounded_int<-100, 100> expected_q {i8{-100}};
    BOOST_TEST(q == expected_q);
}

void test_i8_out_of_range_positive()
{
    // 50 * 3 = 150 > 127 (i8 max) -> overflow_error (underlying overflow fires first)
    constexpr bounded_int<-100, 100> a {i8{50}};
    constexpr bounded_int<-100, 100> b {i8{3}};
    BOOST_TEST_THROWS(a * b, std::overflow_error);

    // Use wider type to test domain_error (result fits in type but exceeds bounds)
    // bounded_int<-100, 100> with i8 basis: 11 * 10 = 110 > 100, but 110 < 127 so no underlying overflow
    constexpr bounded_int<-100, 100> c {i8{11}};
    constexpr bounded_int<-100, 100> d {i8{10}};
    BOOST_TEST_THROWS(c * d, std::domain_error);
}

void test_i8_out_of_range_negative()
{
    // -50 * 3 = -150 < -128 (i8 min) -> underflow_error (underlying underflow fires first)
    constexpr bounded_int<-100, 100> a {i8{-50}};
    constexpr bounded_int<-100, 100> b {i8{3}};
    BOOST_TEST_THROWS(a * b, std::underflow_error);

    // Just one below: -34 * 3 = -102 < -100
    constexpr bounded_int<-100, 100> c {i8{-34}};
    constexpr bounded_int<-100, 100> d {i8{3}};
    BOOST_TEST_THROWS(c * d, std::domain_error);
}

void test_i8_underlying_overflow()
{
    // 100 * 100 = 10000 overflows i8 range [-128, 127]
    constexpr bounded_int<-128, 127> a {i8{100}};
    constexpr bounded_int<-128, 127> b {i8{100}};
    BOOST_TEST_THROWS(a * b, std::overflow_error);

    // -100 * 100 = -10000 underflows i8 range
    constexpr bounded_int<-128, 127> c {i8{-100}};
    constexpr bounded_int<-128, 127> d {i8{100}};
    BOOST_TEST_THROWS(c * d, std::underflow_error);
}

// -----------------------------------------------
// i16 basis: bounded_int<-1000, 1000>
// -----------------------------------------------

void test_i16_valid_multiplication()
{
    constexpr bounded_int<-1000, 1000> a {i16{25}};
    constexpr bounded_int<-1000, 1000> b {i16{30}};
    const auto c {a * b};
    const bounded_int<-1000, 1000> expected {i16{750}};
    BOOST_TEST(c == expected);

    // Negative: -20 * 40 = -800
    constexpr bounded_int<-1000, 1000> d {i16{-20}};
    constexpr bounded_int<-1000, 1000> e {i16{40}};
    const auto f {d * e};
    const bounded_int<-1000, 1000> expected_f {i16{-800}};
    BOOST_TEST(f == expected_f);

    // Max boundary: 100 * 10 = 1000
    constexpr bounded_int<-1000, 1000> g {i16{100}};
    constexpr bounded_int<-1000, 1000> h {i16{10}};
    const auto i {g * h};
    const bounded_int<-1000, 1000> expected_i {i16{1000}};
    BOOST_TEST(i == expected_i);
}

void test_i16_out_of_range_multiplication()
{
    // 500 * 3 = 1500 > 1000 -> domain_error
    constexpr bounded_int<-1000, 1000> a {i16{500}};
    constexpr bounded_int<-1000, 1000> b {i16{3}};
    BOOST_TEST_THROWS(a * b, std::domain_error);

    // -500 * 3 = -1500 < -1000 -> domain_error
    constexpr bounded_int<-1000, 1000> c {i16{-500}};
    constexpr bounded_int<-1000, 1000> d {i16{3}};
    BOOST_TEST_THROWS(c * d, std::domain_error);
}

// -----------------------------------------------
// i32 basis: bounded_int<-100000, 100000>
// -----------------------------------------------

void test_i32_valid_multiplication()
{
    constexpr bounded_int<-100000, 100000> a {i32{200}};
    constexpr bounded_int<-100000, 100000> b {i32{300}};
    const auto c {a * b};
    const bounded_int<-100000, 100000> expected {i32{60000}};
    BOOST_TEST(c == expected);

    // Negative: -200 * 300 = -60000
    constexpr bounded_int<-100000, 100000> d {i32{-200}};
    constexpr bounded_int<-100000, 100000> e {i32{300}};
    const auto f {d * e};
    const bounded_int<-100000, 100000> expected_f {i32{-60000}};
    BOOST_TEST(f == expected_f);
}

void test_i32_out_of_range_multiplication()
{
    // 500 * 300 = 150000 > 100000 -> domain_error
    constexpr bounded_int<-100000, 100000> a {i32{500}};
    constexpr bounded_int<-100000, 100000> b {i32{300}};
    BOOST_TEST_THROWS(a * b, std::domain_error);

    // -500 * 300 = -150000 < -100000 -> domain_error
    constexpr bounded_int<-100000, 100000> c {i32{-500}};
    constexpr bounded_int<-100000, 100000> d {i32{300}};
    BOOST_TEST_THROWS(c * d, std::domain_error);
}

// -----------------------------------------------
// i64 basis: bounded_int<-3000000000LL, 3000000000LL>
// -----------------------------------------------

void test_i64_valid_multiplication()
{
    constexpr bounded_int<-3000000000LL, 3000000000LL> a {i64{50000LL}};
    constexpr bounded_int<-3000000000LL, 3000000000LL> b {i64{40000LL}};
    const auto c {a * b};
    const bounded_int<-3000000000LL, 3000000000LL> expected {i64{2000000000LL}};
    BOOST_TEST(c == expected);

    // Negative: -50000 * 40000 = -2000000000
    constexpr bounded_int<-3000000000LL, 3000000000LL> d {i64{-50000LL}};
    constexpr bounded_int<-3000000000LL, 3000000000LL> e {i64{40000LL}};
    const auto f {d * e};
    const bounded_int<-3000000000LL, 3000000000LL> expected_f {i64{-2000000000LL}};
    BOOST_TEST(f == expected_f);
}

void test_i64_out_of_range_multiplication()
{
    // 100000 * 40000 = 4000000000 > 3000000000 -> domain_error
    constexpr bounded_int<-3000000000LL, 3000000000LL> a {i64{100000LL}};
    constexpr bounded_int<-3000000000LL, 3000000000LL> b {i64{40000LL}};
    BOOST_TEST_THROWS(a * b, std::domain_error);

    // -100000 * 40000 = -4000000000 < -3000000000 -> domain_error
    constexpr bounded_int<-3000000000LL, 3000000000LL> c {i64{-100000LL}};
    constexpr bounded_int<-3000000000LL, 3000000000LL> d {i64{40000LL}};
    BOOST_TEST_THROWS(c * d, std::domain_error);
}

// -----------------------------------------------
// Compound assignment *=
// -----------------------------------------------

void test_compound_multiplication()
{
    // i8 basis
    bounded_int<-100, 100> a {i8{5}};
    a *= bounded_int<-100, 100>{i8{10}};
    const bounded_int<-100, 100> expected_a {i8{50}};
    BOOST_TEST(a == expected_a);

    // i16 basis
    bounded_int<-1000, 1000> b {i16{25}};
    b *= bounded_int<-1000, 1000>{i16{30}};
    const bounded_int<-1000, 1000> expected_b {i16{750}};
    BOOST_TEST(b == expected_b);

    // i32 basis
    bounded_int<-100000, 100000> c {i32{200}};
    c *= bounded_int<-100000, 100000>{i32{300}};
    const bounded_int<-100000, 100000> expected_c {i32{60000}};
    BOOST_TEST(c == expected_c);

    // i64 basis
    bounded_int<-3000000000LL, 3000000000LL> d {i64{50000LL}};
    d *= bounded_int<-3000000000LL, 3000000000LL>{i64{40000LL}};
    const bounded_int<-3000000000LL, 3000000000LL> expected_d {i64{2000000000LL}};
    BOOST_TEST(d == expected_d);

    // Compound assignment that throws (11 * 10 = 110 > 100, fits in i8 but exceeds bounds)
    bounded_int<-100, 100> e {i8{11}};
    using bi100 = bounded_int<-100, 100>;
    BOOST_TEST_THROWS(e *= bi100{i8{10}}, std::domain_error);
}

// -----------------------------------------------
// Constexpr multiplication
// -----------------------------------------------

void test_constexpr_multiplication()
{
    constexpr bounded_int<-100, 100> a {i8{5}};
    constexpr bounded_int<-100, 100> b {i8{10}};
    constexpr auto c {a * b};
    constexpr bounded_int<-100, 100> expected {i8{50}};
    static_assert(c == expected);

    constexpr bounded_int<-1000, 1000> d {i16{-25}};
    constexpr bounded_int<-1000, 1000> e {i16{30}};
    constexpr auto f {d * e};
    constexpr bounded_int<-1000, 1000> expected_f {i16{-750}};
    static_assert(f == expected_f);

    constexpr bounded_int<-100000, 100000> g {i32{200}};
    constexpr bounded_int<-100000, 100000> h {i32{-300}};
    constexpr auto i {g * h};
    constexpr bounded_int<-100000, 100000> expected_i {i32{-60000}};
    static_assert(i == expected_i);
}

int main()
{
    test_i8_valid_multiplication();
    test_i8_out_of_range_positive();
    test_i8_out_of_range_negative();
    test_i8_underlying_overflow();

    test_i16_valid_multiplication();
    test_i16_out_of_range_multiplication();

    test_i32_valid_multiplication();
    test_i32_out_of_range_multiplication();

    test_i64_valid_multiplication();
    test_i64_out_of_range_multiplication();

    test_compound_multiplication();
    test_constexpr_multiplication();

    return boost::report_errors();
}

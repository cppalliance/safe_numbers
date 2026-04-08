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

void test_i8_valid_addition()
{
    // Basic positive addition: 10 + 20 = 30
    constexpr bounded_int<-100, 100> a {i8{10}};
    constexpr bounded_int<-100, 100> b {i8{20}};
    const auto c {a + b};
    const bounded_int<-100, 100> expected {i8{30}};
    BOOST_TEST(c == expected);

    // Adding zero
    constexpr bounded_int<-100, 100> zero {i8{0}};
    const auto d {a + zero};
    BOOST_TEST(d == a);

    // Negative + positive: -30 + 50 = 20
    constexpr bounded_int<-100, 100> e {i8{-30}};
    constexpr bounded_int<-100, 100> f {i8{50}};
    const auto g {e + f};
    const bounded_int<-100, 100> expected_g {i8{20}};
    BOOST_TEST(g == expected_g);

    // Negative + negative: -30 + -20 = -50
    constexpr bounded_int<-100, 100> h {i8{-30}};
    constexpr bounded_int<-100, 100> i {i8{-20}};
    const auto j {h + i};
    const bounded_int<-100, 100> expected_j {i8{-50}};
    BOOST_TEST(j == expected_j);

    // Max boundary: 50 + 50 = 100
    constexpr bounded_int<-100, 100> k {i8{50}};
    constexpr bounded_int<-100, 100> l {i8{50}};
    const auto m {k + l};
    const bounded_int<-100, 100> expected_m {i8{100}};
    BOOST_TEST(m == expected_m);

    // Min boundary: -50 + -50 = -100
    constexpr bounded_int<-100, 100> n {i8{-50}};
    constexpr bounded_int<-100, 100> o {i8{-50}};
    const auto p {n + o};
    const bounded_int<-100, 100> expected_p {i8{-100}};
    BOOST_TEST(p == expected_p);
}

void test_i8_out_of_range_addition()
{
    // Positive out of range: 90 + 20 = 110 > 100 -> domain_error
    constexpr bounded_int<-100, 100> a {i8{90}};
    constexpr bounded_int<-100, 100> b {i8{20}};
    BOOST_TEST_THROWS(a + b, std::domain_error);

    // Negative out of range: -90 + -20 = -110 < -100 -> domain_error
    constexpr bounded_int<-100, 100> c {i8{-90}};
    constexpr bounded_int<-100, 100> d {i8{-20}};
    BOOST_TEST_THROWS(c + d, std::domain_error);

    // Just one over max: 51 + 50 = 101 > 100
    constexpr bounded_int<-100, 100> e {i8{51}};
    constexpr bounded_int<-100, 100> f {i8{50}};
    BOOST_TEST_THROWS(e + f, std::domain_error);

    // Just one below min: -51 + -50 = -101 < -100
    constexpr bounded_int<-100, 100> g {i8{-51}};
    constexpr bounded_int<-100, 100> h {i8{-50}};
    BOOST_TEST_THROWS(g + h, std::domain_error);
}

void test_i8_overflow_addition()
{
    // Overflow of underlying i8: 100 + 100 = 200 > 127 (i8 max) -> overflow_error
    constexpr bounded_int<-128, 127> a {i8{100}};
    constexpr bounded_int<-128, 127> b {i8{100}};
    BOOST_TEST_THROWS(a + b, std::overflow_error);

    // Underflow of underlying i8: -100 + -100 = -200 < -128 (i8 min) -> underflow_error
    constexpr bounded_int<-128, 127> c {i8{-100}};
    constexpr bounded_int<-128, 127> d {i8{-100}};
    BOOST_TEST_THROWS(c + d, std::underflow_error);
}

// -----------------------------------------------
// i16 basis: bounded_int<-1000, 1000>
// -----------------------------------------------

void test_i16_valid_addition()
{
    constexpr bounded_int<-1000, 1000> a {i16{300}};
    constexpr bounded_int<-1000, 1000> b {i16{400}};
    const auto c {a + b};
    const bounded_int<-1000, 1000> expected {i16{700}};
    BOOST_TEST(c == expected);

    // Negative result: -600 + 200 = -400
    constexpr bounded_int<-1000, 1000> d {i16{-600}};
    constexpr bounded_int<-1000, 1000> e {i16{200}};
    const auto f {d + e};
    const bounded_int<-1000, 1000> expected_f {i16{-400}};
    BOOST_TEST(f == expected_f);

    // Max boundary: 500 + 500 = 1000
    constexpr bounded_int<-1000, 1000> g {i16{500}};
    constexpr bounded_int<-1000, 1000> h {i16{500}};
    const auto i {g + h};
    const bounded_int<-1000, 1000> expected_i {i16{1000}};
    BOOST_TEST(i == expected_i);
}

void test_i16_out_of_range_addition()
{
    // 900 + 200 = 1100 > 1000 -> domain_error
    constexpr bounded_int<-1000, 1000> a {i16{900}};
    constexpr bounded_int<-1000, 1000> b {i16{200}};
    BOOST_TEST_THROWS(a + b, std::domain_error);

    // -900 + -200 = -1100 < -1000 -> domain_error
    constexpr bounded_int<-1000, 1000> c {i16{-900}};
    constexpr bounded_int<-1000, 1000> d {i16{-200}};
    BOOST_TEST_THROWS(c + d, std::domain_error);
}

// -----------------------------------------------
// i32 basis: bounded_int<-100000, 100000>
// -----------------------------------------------

void test_i32_valid_addition()
{
    constexpr bounded_int<-100000, 100000> a {i32{30000}};
    constexpr bounded_int<-100000, 100000> b {i32{40000}};
    const auto c {a + b};
    const bounded_int<-100000, 100000> expected {i32{70000}};
    BOOST_TEST(c == expected);

    // Negative: -50000 + 20000 = -30000
    constexpr bounded_int<-100000, 100000> d {i32{-50000}};
    constexpr bounded_int<-100000, 100000> e {i32{20000}};
    const auto f {d + e};
    const bounded_int<-100000, 100000> expected_f {i32{-30000}};
    BOOST_TEST(f == expected_f);
}

void test_i32_out_of_range_addition()
{
    // 60000 + 60000 = 120000 > 100000 -> domain_error
    constexpr bounded_int<-100000, 100000> a {i32{60000}};
    constexpr bounded_int<-100000, 100000> b {i32{60000}};
    BOOST_TEST_THROWS(a + b, std::domain_error);

    // -60000 + -60000 = -120000 < -100000 -> domain_error
    constexpr bounded_int<-100000, 100000> c {i32{-60000}};
    constexpr bounded_int<-100000, 100000> d {i32{-60000}};
    BOOST_TEST_THROWS(c + d, std::domain_error);
}

// -----------------------------------------------
// i64 basis: bounded_int<-3000000000LL, 3000000000LL>
// -----------------------------------------------

void test_i64_valid_addition()
{
    constexpr bounded_int<-3000000000LL, 3000000000LL> a {i64{1000000000LL}};
    constexpr bounded_int<-3000000000LL, 3000000000LL> b {i64{1500000000LL}};
    const auto c {a + b};
    const bounded_int<-3000000000LL, 3000000000LL> expected {i64{2500000000LL}};
    BOOST_TEST(c == expected);
}

void test_i64_out_of_range_addition()
{
    // 2000000000 + 2000000000 = 4000000000 > 3000000000 -> domain_error
    constexpr bounded_int<-3000000000LL, 3000000000LL> a {i64{2000000000LL}};
    constexpr bounded_int<-3000000000LL, 3000000000LL> b {i64{2000000000LL}};
    BOOST_TEST_THROWS(a + b, std::domain_error);

    // -2000000000 + -2000000000 = -4000000000 < -3000000000 -> domain_error
    constexpr bounded_int<-3000000000LL, 3000000000LL> c {i64{-2000000000LL}};
    constexpr bounded_int<-3000000000LL, 3000000000LL> d {i64{-2000000000LL}};
    BOOST_TEST_THROWS(c + d, std::domain_error);
}

// -----------------------------------------------
// Compound assignment +=
// -----------------------------------------------

void test_compound_addition()
{
    // i8 basis
    bounded_int<-100, 100> a {i8{10}};
    a += bounded_int<-100, 100>{i8{20}};
    const bounded_int<-100, 100> expected_a {i8{30}};
    BOOST_TEST(a == expected_a);

    // i16 basis
    bounded_int<-1000, 1000> b {i16{300}};
    b += bounded_int<-1000, 1000>{i16{400}};
    const bounded_int<-1000, 1000> expected_b {i16{700}};
    BOOST_TEST(b == expected_b);

    // i32 basis
    bounded_int<-100000, 100000> c {i32{30000}};
    c += bounded_int<-100000, 100000>{i32{40000}};
    const bounded_int<-100000, 100000> expected_c {i32{70000}};
    BOOST_TEST(c == expected_c);

    // i64 basis
    bounded_int<-3000000000LL, 3000000000LL> d {i64{1000000000LL}};
    d += bounded_int<-3000000000LL, 3000000000LL>{i64{1500000000LL}};
    const bounded_int<-3000000000LL, 3000000000LL> expected_d {i64{2500000000LL}};
    BOOST_TEST(d == expected_d);

    // Compound assignment that throws
    bounded_int<-100, 100> e {i8{90}};
    using bi100 = bounded_int<-100, 100>;
    BOOST_TEST_THROWS(e += bi100{i8{20}}, std::domain_error);
}

// -----------------------------------------------
// Constexpr addition
// -----------------------------------------------

void test_constexpr_addition()
{
    constexpr bounded_int<-100, 100> a {i8{10}};
    constexpr bounded_int<-100, 100> b {i8{20}};
    constexpr auto c {a + b};
    constexpr bounded_int<-100, 100> expected {i8{30}};
    static_assert(c == expected);

    constexpr bounded_int<-1000, 1000> d {i16{300}};
    constexpr bounded_int<-1000, 1000> e {i16{400}};
    constexpr auto f {d + e};
    constexpr bounded_int<-1000, 1000> expected_f {i16{700}};
    static_assert(f == expected_f);

    constexpr bounded_int<-100000, 100000> g {i32{-25000}};
    constexpr bounded_int<-100000, 100000> h {i32{25000}};
    constexpr auto i {g + h};
    constexpr bounded_int<-100000, 100000> expected_i {i32{0}};
    static_assert(i == expected_i);
}

int main()
{
    test_i8_valid_addition();
    test_i8_out_of_range_addition();
    test_i8_overflow_addition();

    test_i16_valid_addition();
    test_i16_out_of_range_addition();

    test_i32_valid_addition();
    test_i32_out_of_range_addition();

    test_i64_valid_addition();
    test_i64_out_of_range_addition();

    test_compound_addition();
    test_constexpr_addition();

    return boost::report_errors();
}

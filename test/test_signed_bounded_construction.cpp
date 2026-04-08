// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/lightweight_test.hpp>

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
// Static assertions on basis_type selection
// -----------------------------------------------

// Both bounds fit in [-128, 127] -> basis_type should be i8
static_assert(std::is_same_v<bounded_int<-100, 100>::basis_type, i8>);
static_assert(std::is_same_v<bounded_int<-128, 127>::basis_type, i8>);
static_assert(std::is_same_v<bounded_int<0, 127>::basis_type, i8>);

// Bounds exceed int8_t but fit in [-32768, 32767] -> basis_type should be i16
static_assert(std::is_same_v<bounded_int<-200, 200>::basis_type, i16>);
static_assert(std::is_same_v<bounded_int<-32768, 32767>::basis_type, i16>);

// Bounds exceed int16_t but fit in int32_t -> basis_type should be i32
static_assert(std::is_same_v<bounded_int<-100000, 100000>::basis_type, i32>);

// Bounds exceed int32_t but fit in int64_t -> basis_type should be i64
static_assert(std::is_same_v<bounded_int<-3000000000LL, 3000000000LL>::basis_type, i64>);

// -----------------------------------------------
// sizeof checks - bounded_int should be the same
// size as the underlying safe type
// -----------------------------------------------

static_assert(sizeof(bounded_int<-100, 100>) == sizeof(i8));
static_assert(sizeof(bounded_int<-200, 200>) == sizeof(i16));
static_assert(sizeof(bounded_int<-100000, 100000>) == sizeof(i32));
static_assert(sizeof(bounded_int<-3000000000LL, 3000000000LL>) == sizeof(i64));

// -----------------------------------------------
// Valid construction at Min and Max boundaries
// -----------------------------------------------

void test_i8_boundary_construction()
{
    // Construct at exact Min boundary
    constexpr bounded_int<-100, 100> a {i8{-100}};
    (void)a;

    // Construct at exact Max boundary
    constexpr bounded_int<-100, 100> b {i8{100}};
    (void)b;

    // Construct at zero
    constexpr bounded_int<-100, 100> c {i8{0}};
    (void)c;

    // Full int8_t range boundaries
    constexpr bounded_int<-128, 127> d {i8{-128}};
    constexpr bounded_int<-128, 127> e {i8{127}};
    (void)d;
    (void)e;

    // Non-negative range
    constexpr bounded_int<0, 127> f {i8{0}};
    constexpr bounded_int<0, 127> g {i8{127}};
    (void)f;
    (void)g;
}

void test_i16_boundary_construction()
{
    constexpr bounded_int<-200, 200> a {i16{-200}};
    constexpr bounded_int<-200, 200> b {i16{200}};
    (void)a;
    (void)b;

    constexpr bounded_int<-32768, 32767> c {i16{-32768}};
    constexpr bounded_int<-32768, 32767> d {i16{32767}};
    (void)c;
    (void)d;
}

void test_i32_boundary_construction()
{
    constexpr bounded_int<-100000, 100000> a {i32{-100000}};
    constexpr bounded_int<-100000, 100000> b {i32{100000}};
    (void)a;
    (void)b;
}

void test_i64_boundary_construction()
{
    constexpr bounded_int<-3000000000LL, 3000000000LL> a {i64{-3000000000LL}};
    constexpr bounded_int<-3000000000LL, 3000000000LL> b {i64{3000000000LL}};
    (void)a;
    (void)b;
}

// -----------------------------------------------
// Out-of-range construction throws std::domain_error
// -----------------------------------------------

void test_i8_out_of_range()
{
    // Below Min
    BOOST_TEST_THROWS((bounded_int<-100, 100>{i8{-101}}), std::domain_error);
    BOOST_TEST_THROWS((bounded_int<-100, 100>{i8{-128}}), std::domain_error);

    // Above Max
    BOOST_TEST_THROWS((bounded_int<-100, 100>{i8{101}}), std::domain_error);
    BOOST_TEST_THROWS((bounded_int<-100, 100>{i8{127}}), std::domain_error);

    // Non-negative range: below Min
    BOOST_TEST_THROWS((bounded_int<0, 127>{i8{-1}}), std::domain_error);
    BOOST_TEST_THROWS((bounded_int<0, 127>{i8{-128}}), std::domain_error);
}

void test_i16_out_of_range()
{
    // Below Min
    BOOST_TEST_THROWS((bounded_int<-200, 200>{i16{-201}}), std::domain_error);
    BOOST_TEST_THROWS((bounded_int<-200, 200>{i16{-32768}}), std::domain_error);

    // Above Max
    BOOST_TEST_THROWS((bounded_int<-200, 200>{i16{201}}), std::domain_error);
    BOOST_TEST_THROWS((bounded_int<-200, 200>{i16{32767}}), std::domain_error);
}

void test_i32_out_of_range()
{
    // Below Min
    BOOST_TEST_THROWS((bounded_int<-100000, 100000>{i32{-100001}}), std::domain_error);

    // Above Max
    BOOST_TEST_THROWS((bounded_int<-100000, 100000>{i32{100001}}), std::domain_error);
}

void test_i64_out_of_range()
{
    // Below Min
    BOOST_TEST_THROWS((bounded_int<-3000000000LL, 3000000000LL>{i64{-3000000001LL}}), std::domain_error);

    // Above Max
    BOOST_TEST_THROWS((bounded_int<-3000000000LL, 3000000000LL>{i64{3000000001LL}}), std::domain_error);
}

// -----------------------------------------------
// Construction from underlying_type
// -----------------------------------------------

void test_construction_from_underlying()
{
    // i8 basis: underlying is std::int8_t
    bounded_int<-100, 100> a {static_cast<std::int8_t>(42)};
    const bounded_int<-100, 100> expected_a {i8{42}};
    BOOST_TEST(a == expected_a);

    // i16 basis: underlying is std::int16_t
    bounded_int<-200, 200> b {static_cast<std::int16_t>(-150)};
    const bounded_int<-200, 200> expected_b {i16{-150}};
    BOOST_TEST(b == expected_b);

    // i32 basis: underlying is std::int32_t
    bounded_int<-100000, 100000> c {static_cast<std::int32_t>(50000)};
    const bounded_int<-100000, 100000> expected_c {i32{50000}};
    BOOST_TEST(c == expected_c);

    // i64 basis: underlying is std::int64_t
    bounded_int<-3000000000LL, 3000000000LL> d {static_cast<std::int64_t>(1000000000LL)};
    const bounded_int<-3000000000LL, 3000000000LL> expected_d {i64{1000000000LL}};
    BOOST_TEST(d == expected_d);
}

// -----------------------------------------------
// Constexpr construction and comparisons
// -----------------------------------------------

void test_constexpr_construction()
{
    constexpr bounded_int<-100, 100> a {i8{-50}};
    constexpr bounded_int<-100, 100> b {i8{50}};
    static_assert(a < b);
    static_assert(a == a);
    static_assert(b == b);
    static_assert(a != b);

    constexpr bounded_int<-200, 200> c {i16{-200}};
    constexpr bounded_int<-200, 200> d {i16{200}};
    static_assert(c < d);

    constexpr bounded_int<-100000, 100000> e {i32{-100000}};
    constexpr bounded_int<-100000, 100000> f {i32{100000}};
    static_assert(e < f);

    constexpr bounded_int<-3000000000LL, 3000000000LL> g {i64{-3000000000LL}};
    constexpr bounded_int<-3000000000LL, 3000000000LL> h {i64{3000000000LL}};
    static_assert(g < h);
}

// -----------------------------------------------
// Comparison operators
// -----------------------------------------------

void test_comparisons()
{
    constexpr bounded_int<-100, 100> a {i8{-50}};
    constexpr bounded_int<-100, 100> b {i8{0}};
    constexpr bounded_int<-100, 100> c {i8{50}};
    constexpr bounded_int<-100, 100> d {i8{50}};

    BOOST_TEST(a < b);
    BOOST_TEST(b < c);
    BOOST_TEST(a < c);
    BOOST_TEST(c == d);
    BOOST_TEST(c != a);
    BOOST_TEST(c > a);
    BOOST_TEST(a <= b);
    BOOST_TEST(c >= b);
    BOOST_TEST(c <= d);
    BOOST_TEST(c >= d);
}

int main()
{
    test_i8_boundary_construction();
    test_i16_boundary_construction();
    test_i32_boundary_construction();
    test_i64_boundary_construction();

    test_i8_out_of_range();
    test_i16_out_of_range();
    test_i32_out_of_range();
    test_i64_out_of_range();

    test_construction_from_underlying();
    test_constexpr_construction();
    test_comparisons();

    return boost::report_errors();
}

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/lightweight_test.hpp>

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers/bounded_floats.hpp>
#include <boost/safe_numbers/floats.hpp>

#endif

#include <cmath>
#include <limits>
#include <stdexcept>
#include <type_traits>

using namespace boost::safe_numbers;

// -----------------------------------------------
// basis_type selection follows the bound's literal type
// -----------------------------------------------

static_assert(std::is_same_v<bounded_float<-1.0f, 1.0f>::basis_type, f32>);
static_assert(std::is_same_v<bounded_float<0.0f, 100.0f>::basis_type, f32>);
static_assert(std::is_same_v<bounded_float<-1.0, 1.0>::basis_type, f64>);
static_assert(std::is_same_v<bounded_float<0.0, 1000.0>::basis_type, f64>);
static_assert(std::is_same_v<bounded_float<-1.0e100, 1.0e100>::basis_type, f64>);

// -----------------------------------------------
// sizeof: bounded_float should match its basis_type
// -----------------------------------------------

static_assert(sizeof(bounded_float<-1.0f, 1.0f>) == sizeof(f32));
static_assert(sizeof(bounded_float<-1.0, 1.0>) == sizeof(f64));
static_assert(sizeof(bounded_float<-1.0e100, 1.0e100>) == sizeof(f64));

// -----------------------------------------------
// No default constructor (mirrors bounded_int)
// -----------------------------------------------

static_assert(!std::is_default_constructible_v<bounded_float<-1.0f, 1.0f>>);
static_assert(!std::is_default_constructible_v<bounded_float<-1.0, 1.0>>);

// -----------------------------------------------
// Valid construction at boundaries
// -----------------------------------------------

void test_f32_boundary_construction()
{
    constexpr bounded_float<-1.0f, 1.0f> a {f32{-1.0f}};
    constexpr bounded_float<-1.0f, 1.0f> b {f32{1.0f}};
    constexpr bounded_float<-1.0f, 1.0f> c {f32{0.0f}};
    (void)a;
    (void)b;
    (void)c;

    constexpr bounded_float<0.0f, 100.0f> d {f32{0.0f}};
    constexpr bounded_float<0.0f, 100.0f> e {f32{100.0f}};
    constexpr bounded_float<0.0f, 100.0f> f {f32{50.5f}};
    (void)d;
    (void)e;
    (void)f;
}

void test_f64_boundary_construction()
{
    constexpr bounded_float<-1.0e100, 1.0e100> a {f64{-1.0e100}};
    constexpr bounded_float<-1.0e100, 1.0e100> b {f64{1.0e100}};
    (void)a;
    (void)b;
}

// -----------------------------------------------
// Out-of-range construction throws std::domain_error
// -----------------------------------------------

void test_out_of_range()
{
    BOOST_TEST_THROWS((bounded_float<-1.0f, 1.0f>{f32{-1.5f}}), std::domain_error);
    BOOST_TEST_THROWS((bounded_float<-1.0f, 1.0f>{f32{1.5f}}), std::domain_error);
    BOOST_TEST_THROWS((bounded_float<-1.0f, 1.0f>{f32{2.0f}}), std::domain_error);
    BOOST_TEST_THROWS((bounded_float<-1.0f, 1.0f>{f32{-2.0f}}), std::domain_error);

    BOOST_TEST_THROWS((bounded_float<0.0f, 100.0f>{f32{-0.1f}}), std::domain_error);
    BOOST_TEST_THROWS((bounded_float<0.0f, 100.0f>{f32{100.5f}}), std::domain_error);
}

// -----------------------------------------------
// NaN construction throws std::domain_error
// -----------------------------------------------

void test_nan_rejected()
{
    const auto qnan_f {std::numeric_limits<float>::quiet_NaN()};
    const auto qnan_d {std::numeric_limits<double>::quiet_NaN()};
    const auto snan_f {std::numeric_limits<float>::signaling_NaN()};
    const auto snan_d {std::numeric_limits<double>::signaling_NaN()};

    BOOST_TEST_THROWS((bounded_float<-1.0f, 1.0f>{f32{qnan_f}}), std::domain_error);
    BOOST_TEST_THROWS((bounded_float<-1.0f, 1.0f>{f32{snan_f}}), std::domain_error);
    BOOST_TEST_THROWS((bounded_float<-1.0e100, 1.0e100>{f64{qnan_d}}), std::domain_error);
    BOOST_TEST_THROWS((bounded_float<-1.0e100, 1.0e100>{f64{snan_d}}), std::domain_error);

    BOOST_TEST_THROWS((bounded_float<-1.0f, 1.0f>{qnan_f}), std::domain_error);
    BOOST_TEST_THROWS((bounded_float<-1.0e100, 1.0e100>{qnan_d}), std::domain_error);
}

// -----------------------------------------------
// Infinity outside finite bounds throws
// -----------------------------------------------

void test_infinity_outside_finite_bounds()
{
    const auto pinf_f {std::numeric_limits<float>::infinity()};
    const auto ninf_f {-std::numeric_limits<float>::infinity()};

    BOOST_TEST_THROWS((bounded_float<-1.0f, 1.0f>{f32{pinf_f}}), std::domain_error);
    BOOST_TEST_THROWS((bounded_float<-1.0f, 1.0f>{f32{ninf_f}}), std::domain_error);
}

// -----------------------------------------------
// Construction from underlying float / double
// -----------------------------------------------

void test_construction_from_underlying()
{
    bounded_float<-1.0f, 1.0f> a {0.5f};
    const bounded_float<-1.0f, 1.0f> expected_a {f32{0.5f}};
    BOOST_TEST(a == expected_a);

    bounded_float<-1.0e100, 1.0e100> b {1.0e50};
    const bounded_float<-1.0e100, 1.0e100> expected_b {f64{1.0e50}};
    BOOST_TEST(b == expected_b);
}

// -----------------------------------------------
// Constexpr construction and comparisons
// -----------------------------------------------

void test_constexpr_construction()
{
    constexpr bounded_float<-1.0f, 1.0f> a {f32{-0.5f}};
    constexpr bounded_float<-1.0f, 1.0f> b {f32{0.5f}};
    static_assert(a < b);
    static_assert(a == a);
    static_assert(a != b);

    constexpr bounded_float<-1.0e100, 1.0e100> c {f64{-1.0e50}};
    constexpr bounded_float<-1.0e100, 1.0e100> d {f64{1.0e50}};
    static_assert(c < d);
}

// -----------------------------------------------
// Comparison operators
// -----------------------------------------------

void test_comparisons()
{
    constexpr bounded_float<-1.0f, 1.0f> a {f32{-0.5f}};
    constexpr bounded_float<-1.0f, 1.0f> b {f32{0.0f}};
    constexpr bounded_float<-1.0f, 1.0f> c {f32{0.5f}};
    constexpr bounded_float<-1.0f, 1.0f> d {f32{0.5f}};

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
    test_f32_boundary_construction();
    test_f64_boundary_construction();
    test_out_of_range();
    test_nan_rejected();
    test_infinity_outside_finite_bounds();
    test_construction_from_underlying();
    test_constexpr_construction();
    test_comparisons();

    return boost::report_errors();
}

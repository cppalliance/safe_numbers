// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE
import boost.safe_numbers;
#else
#include <boost/safe_numbers.hpp>
#include <limits>
#include <type_traits>
#endif

#include <boost/core/lightweight_test.hpp>

using namespace boost::safe_numbers;

// =============================================================================
// Test is_specialized
// =============================================================================

void test_is_specialized()
{
    BOOST_TEST(std::numeric_limits<f32>::is_specialized);
    BOOST_TEST(std::numeric_limits<f64>::is_specialized);
}

// =============================================================================
// Test is_signed (floating-point types are always signed)
// =============================================================================

void test_is_signed()
{
    BOOST_TEST(std::numeric_limits<f32>::is_signed);
    BOOST_TEST(std::numeric_limits<f64>::is_signed);
}

// =============================================================================
// Test is_integer (false for floating-point)
// =============================================================================

void test_is_integer()
{
    BOOST_TEST(!std::numeric_limits<f32>::is_integer);
    BOOST_TEST(!std::numeric_limits<f64>::is_integer);
}

// =============================================================================
// Test is_exact (false for floating-point)
// =============================================================================

void test_is_exact()
{
    BOOST_TEST(!std::numeric_limits<f32>::is_exact);
    BOOST_TEST(!std::numeric_limits<f64>::is_exact);
}

// =============================================================================
// Test has_infinity / has_quiet_NaN / has_signaling_NaN
// =============================================================================

void test_has_special_values()
{
    BOOST_TEST(std::numeric_limits<f32>::has_infinity);
    BOOST_TEST(std::numeric_limits<f32>::has_quiet_NaN);
    BOOST_TEST(std::numeric_limits<f32>::has_signaling_NaN);

    BOOST_TEST(std::numeric_limits<f64>::has_infinity);
    BOOST_TEST(std::numeric_limits<f64>::has_quiet_NaN);
    BOOST_TEST(std::numeric_limits<f64>::has_signaling_NaN);
}

// =============================================================================
// Test is_iec559 / is_bounded / is_modulo
// =============================================================================

void test_iec559_bounded_modulo()
{
    BOOST_TEST(std::numeric_limits<f32>::is_iec559);
    BOOST_TEST(std::numeric_limits<f32>::is_bounded);
    BOOST_TEST(!std::numeric_limits<f32>::is_modulo);

    BOOST_TEST(std::numeric_limits<f64>::is_iec559);
    BOOST_TEST(std::numeric_limits<f64>::is_bounded);
    BOOST_TEST(!std::numeric_limits<f64>::is_modulo);
}

// =============================================================================
// Test radix (always 2 for IEC 559 binary floating-point)
// =============================================================================

void test_radix()
{
    BOOST_TEST_EQ(std::numeric_limits<f32>::radix, 2);
    BOOST_TEST_EQ(std::numeric_limits<f64>::radix, 2);
}

// =============================================================================
// Test digits and digits10
// =============================================================================

void test_digits()
{
    BOOST_TEST_EQ(std::numeric_limits<f32>::digits, std::numeric_limits<float>::digits);
    BOOST_TEST_EQ(std::numeric_limits<f64>::digits, std::numeric_limits<double>::digits);

    BOOST_TEST_EQ(std::numeric_limits<f32>::digits10, std::numeric_limits<float>::digits10);
    BOOST_TEST_EQ(std::numeric_limits<f64>::digits10, std::numeric_limits<double>::digits10);

    BOOST_TEST_EQ(std::numeric_limits<f32>::max_digits10, std::numeric_limits<float>::max_digits10);
    BOOST_TEST_EQ(std::numeric_limits<f64>::max_digits10, std::numeric_limits<double>::max_digits10);
}

// =============================================================================
// Test exponent ranges
// =============================================================================

void test_exponents()
{
    BOOST_TEST_EQ(std::numeric_limits<f32>::min_exponent, std::numeric_limits<float>::min_exponent);
    BOOST_TEST_EQ(std::numeric_limits<f64>::min_exponent, std::numeric_limits<double>::min_exponent);

    BOOST_TEST_EQ(std::numeric_limits<f32>::max_exponent, std::numeric_limits<float>::max_exponent);
    BOOST_TEST_EQ(std::numeric_limits<f64>::max_exponent, std::numeric_limits<double>::max_exponent);

    BOOST_TEST_EQ(std::numeric_limits<f32>::min_exponent10, std::numeric_limits<float>::min_exponent10);
    BOOST_TEST_EQ(std::numeric_limits<f64>::min_exponent10, std::numeric_limits<double>::min_exponent10);

    BOOST_TEST_EQ(std::numeric_limits<f32>::max_exponent10, std::numeric_limits<float>::max_exponent10);
    BOOST_TEST_EQ(std::numeric_limits<f64>::max_exponent10, std::numeric_limits<double>::max_exponent10);
}

// =============================================================================
// Test min() and max() values
// =============================================================================

void test_min_max()
{
    BOOST_TEST(std::numeric_limits<f32>::min() == f32{std::numeric_limits<float>::min()});
    BOOST_TEST(std::numeric_limits<f32>::max() == f32{std::numeric_limits<float>::max()});

    BOOST_TEST(std::numeric_limits<f64>::min() == f64{std::numeric_limits<double>::min()});
    BOOST_TEST(std::numeric_limits<f64>::max() == f64{std::numeric_limits<double>::max()});
}

// =============================================================================
// Test lowest() (for floating-point, lowest != min)
// =============================================================================

void test_lowest()
{
    BOOST_TEST(std::numeric_limits<f32>::lowest() == f32{std::numeric_limits<float>::lowest()});
    BOOST_TEST(std::numeric_limits<f64>::lowest() == f64{std::numeric_limits<double>::lowest()});

    // For floating-point, lowest() is the most-negative finite value, not the smallest positive.
    BOOST_TEST(std::numeric_limits<f32>::lowest() < f32{0.0f});
    BOOST_TEST(std::numeric_limits<f32>::min() > f32{0.0f});
    BOOST_TEST(std::numeric_limits<f64>::lowest() < f64{0.0});
    BOOST_TEST(std::numeric_limits<f64>::min() > f64{0.0});
}

// =============================================================================
// Test epsilon and round_error
// =============================================================================

void test_epsilon_round_error()
{
    BOOST_TEST(std::numeric_limits<f32>::epsilon() == f32{std::numeric_limits<float>::epsilon()});
    BOOST_TEST(std::numeric_limits<f64>::epsilon() == f64{std::numeric_limits<double>::epsilon()});

    BOOST_TEST(std::numeric_limits<f32>::round_error() == f32{std::numeric_limits<float>::round_error()});
    BOOST_TEST(std::numeric_limits<f64>::round_error() == f64{std::numeric_limits<double>::round_error()});
}

// =============================================================================
// Test infinity
// =============================================================================

void test_infinity()
{
    BOOST_TEST(std::numeric_limits<f32>::infinity() == f32{std::numeric_limits<float>::infinity()});
    BOOST_TEST(std::numeric_limits<f64>::infinity() == f64{std::numeric_limits<double>::infinity()});

    BOOST_TEST(std::numeric_limits<f32>::infinity() > std::numeric_limits<f32>::max());
    BOOST_TEST(std::numeric_limits<f64>::infinity() > std::numeric_limits<f64>::max());
}

// =============================================================================
// Test NaN (NaN is unordered, NaN != NaN by definition)
// =============================================================================

void test_nan()
{
    const auto qnan_f32 {std::numeric_limits<f32>::quiet_NaN()};
    const auto qnan_f64 {std::numeric_limits<f64>::quiet_NaN()};

    BOOST_TEST(qnan_f32 != qnan_f32);
    BOOST_TEST(qnan_f64 != qnan_f64);

    const auto snan_f32 {std::numeric_limits<f32>::signaling_NaN()};
    const auto snan_f64 {std::numeric_limits<f64>::signaling_NaN()};

    BOOST_TEST(snan_f32 != snan_f32);
    BOOST_TEST(snan_f64 != snan_f64);
}

// =============================================================================
// Test denorm_min
// =============================================================================

void test_denorm_min()
{
    BOOST_TEST(std::numeric_limits<f32>::denorm_min() == f32{std::numeric_limits<float>::denorm_min()});
    BOOST_TEST(std::numeric_limits<f64>::denorm_min() == f64{std::numeric_limits<double>::denorm_min()});

    BOOST_TEST(std::numeric_limits<f32>::denorm_min() > f32{0.0f});
    BOOST_TEST(std::numeric_limits<f64>::denorm_min() > f64{0.0});
}

// =============================================================================
// Test that lowest() == -max() for IEC 559 floating-point
// =============================================================================

void test_lowest_equals_neg_max()
{
    using basis_f32 = std::numeric_limits<float>;
    using basis_f64 = std::numeric_limits<double>;

    BOOST_TEST(std::numeric_limits<f32>::lowest() == f32{-basis_f32::max()});
    BOOST_TEST(std::numeric_limits<f64>::lowest() == f64{-basis_f64::max()});
}

// =============================================================================
// Constexpr tests
// =============================================================================

void test_constexpr()
{
    static_assert(std::numeric_limits<f32>::is_specialized);
    static_assert(std::numeric_limits<f32>::is_signed);
    static_assert(!std::numeric_limits<f32>::is_integer);
    static_assert(!std::numeric_limits<f32>::is_exact);
    static_assert(std::numeric_limits<f32>::has_infinity);
    static_assert(std::numeric_limits<f32>::has_quiet_NaN);
    static_assert(std::numeric_limits<f32>::has_signaling_NaN);
    static_assert(std::numeric_limits<f32>::is_iec559);
    static_assert(std::numeric_limits<f32>::is_bounded);
    static_assert(!std::numeric_limits<f32>::is_modulo);
    static_assert(std::numeric_limits<f32>::radix == 2);
    static_assert(std::numeric_limits<f32>::digits == std::numeric_limits<float>::digits);
    static_assert(std::numeric_limits<f32>::max_exponent == std::numeric_limits<float>::max_exponent);

    static_assert(std::numeric_limits<f64>::is_specialized);
    static_assert(std::numeric_limits<f64>::is_signed);
    static_assert(!std::numeric_limits<f64>::is_integer);
    static_assert(!std::numeric_limits<f64>::is_exact);
    static_assert(std::numeric_limits<f64>::has_infinity);
    static_assert(std::numeric_limits<f64>::has_quiet_NaN);
    static_assert(std::numeric_limits<f64>::has_signaling_NaN);
    static_assert(std::numeric_limits<f64>::is_iec559);
    static_assert(std::numeric_limits<f64>::is_bounded);
    static_assert(!std::numeric_limits<f64>::is_modulo);
    static_assert(std::numeric_limits<f64>::radix == 2);
    static_assert(std::numeric_limits<f64>::digits == std::numeric_limits<double>::digits);
    static_assert(std::numeric_limits<f64>::max_exponent == std::numeric_limits<double>::max_exponent);

    static_assert(std::numeric_limits<f32>::min() == f32{std::numeric_limits<float>::min()});
    static_assert(std::numeric_limits<f32>::max() == f32{std::numeric_limits<float>::max()});
    static_assert(std::numeric_limits<f64>::min() == f64{std::numeric_limits<double>::min()});
    static_assert(std::numeric_limits<f64>::max() == f64{std::numeric_limits<double>::max()});
}

int main()
{
    test_is_specialized();
    test_is_signed();
    test_is_integer();
    test_is_exact();
    test_has_special_values();
    test_iec559_bounded_modulo();
    test_radix();
    test_digits();
    test_exponents();
    test_min_max();
    test_lowest();
    test_epsilon_round_error();
    test_infinity();
    test_nan();
    test_denorm_min();
    test_lowest_equals_neg_max();
    test_constexpr();

    return boost::report_errors();
}

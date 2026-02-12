// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>

#endif

#include <boost/core/lightweight_test.hpp>
#include <limits>

using namespace boost::safe_numbers;

// =============================================================================
// Compile-time tests via static_assert
// =============================================================================

template <typename VerifiedT, typename BasisT>
consteval auto test_limits_properties_impl() -> bool
{
    using vlimits = std::numeric_limits<VerifiedT>;
    using blimits = std::numeric_limits<BasisT>;

    return vlimits::is_specialized == blimits::is_specialized &&
           vlimits::is_signed == blimits::is_signed &&
           vlimits::is_integer == blimits::is_integer &&
           vlimits::is_exact == blimits::is_exact &&
           vlimits::has_infinity == blimits::has_infinity &&
           vlimits::has_quiet_NaN == blimits::has_quiet_NaN &&
           vlimits::has_signaling_NaN == blimits::has_signaling_NaN &&
           vlimits::is_iec559 == blimits::is_iec559 &&
           vlimits::is_bounded == blimits::is_bounded &&
           vlimits::is_modulo == blimits::is_modulo &&
           vlimits::digits == blimits::digits &&
           vlimits::digits10 == blimits::digits10 &&
           vlimits::max_digits10 == blimits::max_digits10 &&
           vlimits::radix == blimits::radix &&
           vlimits::min_exponent == blimits::min_exponent &&
           vlimits::min_exponent10 == blimits::min_exponent10 &&
           vlimits::max_exponent == blimits::max_exponent &&
           vlimits::max_exponent10 == blimits::max_exponent10 &&
           vlimits::traps == blimits::traps &&
           vlimits::tinyness_before == blimits::tinyness_before;
}

template <typename VerifiedT, typename BasisT>
consteval auto test_limits_functions_impl() -> bool
{
    using vlimits = std::numeric_limits<VerifiedT>;
    using blimits = std::numeric_limits<BasisT>;

    return vlimits::min() == VerifiedT{blimits::min()} &&
           vlimits::max() == VerifiedT{blimits::max()} &&
           vlimits::lowest() == VerifiedT{blimits::lowest()} &&
           vlimits::epsilon() == VerifiedT{blimits::epsilon()} &&
           vlimits::round_error() == VerifiedT{blimits::round_error()} &&
           vlimits::infinity() == VerifiedT{blimits::infinity()} &&
           vlimits::quiet_NaN() == VerifiedT{blimits::quiet_NaN()} &&
           vlimits::signaling_NaN() == VerifiedT{blimits::signaling_NaN()} &&
           vlimits::denorm_min() == VerifiedT{blimits::denorm_min()};
}

// --- Properties ---
static_assert(test_limits_properties_impl<verified_u8, u8>());
static_assert(test_limits_properties_impl<verified_u16, u16>());
static_assert(test_limits_properties_impl<verified_u32, u32>());
static_assert(test_limits_properties_impl<verified_u64, u64>());
static_assert(test_limits_properties_impl<verified_u128, u128>());

// --- Functions ---
static_assert(test_limits_functions_impl<verified_u8, u8>());
static_assert(test_limits_functions_impl<verified_u16, u16>());
static_assert(test_limits_functions_impl<verified_u32, u32>());
static_assert(test_limits_functions_impl<verified_u64, u64>());
static_assert(test_limits_functions_impl<verified_u128, u128>());

// --- Bounded verified types ---
using test_bounded = bounded_uint<0u, 255u>;
using test_verified_bounded = verified_bounded_integer<0u, 255u>;

static_assert(test_limits_properties_impl<test_verified_bounded, test_bounded>());
static_assert(test_limits_functions_impl<test_verified_bounded, test_bounded>());

using test_bounded_narrow = bounded_uint<10u, 200u>;
using test_verified_bounded_narrow = verified_bounded_integer<10u, 200u>;

static_assert(test_limits_properties_impl<test_verified_bounded_narrow, test_bounded_narrow>());
static_assert(test_limits_functions_impl<test_verified_bounded_narrow, test_bounded_narrow>());

// =============================================================================
// Runtime tests
// =============================================================================

template <typename VerifiedT, typename BasisT>
void test_properties()
{
    using vlimits = std::numeric_limits<VerifiedT>;
    using blimits = std::numeric_limits<BasisT>;

    BOOST_TEST_EQ(vlimits::is_specialized, blimits::is_specialized);
    BOOST_TEST_EQ(vlimits::is_signed, blimits::is_signed);
    BOOST_TEST_EQ(vlimits::is_integer, blimits::is_integer);
    BOOST_TEST_EQ(vlimits::is_exact, blimits::is_exact);
    BOOST_TEST_EQ(vlimits::has_infinity, blimits::has_infinity);
    BOOST_TEST_EQ(vlimits::has_quiet_NaN, blimits::has_quiet_NaN);
    BOOST_TEST_EQ(vlimits::has_signaling_NaN, blimits::has_signaling_NaN);

    #if ((!defined(_MSC_VER) && (__cplusplus <= 202002L)) || (defined(_MSC_VER) && (_MSVC_LANG <= 202002L)))
    BOOST_TEST(vlimits::has_denorm == blimits::has_denorm);
    BOOST_TEST(vlimits::has_denorm_loss == blimits::has_denorm_loss);
    #endif

    BOOST_TEST(vlimits::round_style == blimits::round_style);

    BOOST_TEST_EQ(vlimits::is_iec559, blimits::is_iec559);
    BOOST_TEST_EQ(vlimits::is_bounded, blimits::is_bounded);
    BOOST_TEST_EQ(vlimits::is_modulo, blimits::is_modulo);
    BOOST_TEST_EQ(vlimits::digits, blimits::digits);
    BOOST_TEST_EQ(vlimits::digits10, blimits::digits10);
    BOOST_TEST_EQ(vlimits::max_digits10, blimits::max_digits10);
    BOOST_TEST_EQ(vlimits::radix, blimits::radix);
    BOOST_TEST_EQ(vlimits::min_exponent, blimits::min_exponent);
    BOOST_TEST_EQ(vlimits::min_exponent10, blimits::min_exponent10);
    BOOST_TEST_EQ(vlimits::max_exponent, blimits::max_exponent);
    BOOST_TEST_EQ(vlimits::max_exponent10, blimits::max_exponent10);
    BOOST_TEST_EQ(vlimits::traps, blimits::traps);
    BOOST_TEST_EQ(vlimits::tinyness_before, blimits::tinyness_before);
}

template <typename VerifiedT, typename BasisT>
void test_functions()
{
    using vlimits = std::numeric_limits<VerifiedT>;
    using blimits = std::numeric_limits<BasisT>;

    BOOST_TEST(vlimits::min() == VerifiedT{blimits::min()});
    BOOST_TEST(vlimits::max() == VerifiedT{blimits::max()});
    BOOST_TEST(vlimits::lowest() == VerifiedT{blimits::lowest()});
    BOOST_TEST(vlimits::epsilon() == VerifiedT{blimits::epsilon()});
    BOOST_TEST(vlimits::round_error() == VerifiedT{blimits::round_error()});
    BOOST_TEST(vlimits::infinity() == VerifiedT{blimits::infinity()});
    BOOST_TEST(vlimits::quiet_NaN() == VerifiedT{blimits::quiet_NaN()});
    BOOST_TEST(vlimits::signaling_NaN() == VerifiedT{blimits::signaling_NaN()});
    BOOST_TEST(vlimits::denorm_min() == VerifiedT{blimits::denorm_min()});
}

int main()
{
    test_properties<verified_u8, u8>();
    test_properties<verified_u16, u16>();
    test_properties<verified_u32, u32>();
    test_properties<verified_u64, u64>();
    test_properties<verified_u128, u128>();

    test_functions<verified_u8, u8>();
    test_functions<verified_u16, u16>();
    test_functions<verified_u32, u32>();
    test_functions<verified_u64, u64>();
    test_functions<verified_u128, u128>();

    // Bounded verified types
    test_properties<test_verified_bounded, test_bounded>();
    test_functions<test_verified_bounded, test_bounded>();

    test_properties<test_verified_bounded_narrow, test_bounded_narrow>();
    test_functions<test_verified_bounded_narrow, test_bounded_narrow>();

    return boost::report_errors();
}

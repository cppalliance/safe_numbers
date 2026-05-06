// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>

#endif

#include <boost/core/lightweight_test.hpp>

#include <cfloat>
#include <limits>
#include <stdexcept>

using namespace boost::safe_numbers;
using namespace boost::safe_numbers::literals;

template <typename T>
void test();

template <>
void test<f32>()
{
    constexpr auto good_value {1.5_f32};
    BOOST_TEST(good_value == f32{1.5f});

    constexpr auto zero_value {0.0_f32};
    BOOST_TEST(zero_value == f32{0.0f});

    // Scientific-notation literal still routes through the long double overload.
    constexpr auto sci_value {1.0e10_f32};
    BOOST_TEST(sci_value == f32{1.0e10f});

    // A subnormal-adjacent magnitude does not trip the overflow check.
    constexpr auto small_value {1.0e-30_f32};
    BOOST_TEST(small_value == f32{1.0e-30f});

    // Inside float range but in the upper region.
    constexpr auto large_value {3.0e38_f32};
    BOOST_TEST(large_value == f32{3.0e38f});

    // The literal parameter is long double, so a finite long double value that
    // exceeds float max must throw before the narrowing cast.
    BOOST_TEST_THROWS(1.0e40_f32, std::overflow_error);

    // Just past float max (3.4028e38) also throws.
    BOOST_TEST_THROWS(3.5e38_f32, std::overflow_error);

    // A value far past float max throws.
    BOOST_TEST_THROWS(1.0e300_f32, std::overflow_error);
}

template <>
void test<f64>()
{
    constexpr auto good_value {1.5_f64};
    BOOST_TEST(good_value == f64{1.5});

    constexpr auto zero_value {0.0_f64};
    BOOST_TEST(zero_value == f64{0.0});

    constexpr auto sci_value {1.0e100_f64};
    BOOST_TEST(sci_value == f64{1.0e100});

    constexpr auto small_value {1.0e-300_f64};
    BOOST_TEST(small_value == f64{1.0e-300});

    // Inside double range but in the upper region.
    constexpr auto large_value {1.0e308_f64};
    BOOST_TEST(large_value == f64{1.0e308});

    // Overflow can only be expressed as a source-level literal when long double
    // has a wider exponent range than double; otherwise the literal token would
    // be ill-formed on the host platform (e.g. Apple Silicon, where long double
    // matches double). Gate the test on that property so it runs everywhere it
    // is meaningful.
#if LDBL_MAX_EXP > DBL_MAX_EXP
    BOOST_TEST_THROWS(1.0e310_f64, std::overflow_error);
    BOOST_TEST_THROWS(1.0e1000_f64, std::overflow_error);
#endif
}

int main()
{
    test<f32>();
    test<f64>();

    return boost::report_errors();
}

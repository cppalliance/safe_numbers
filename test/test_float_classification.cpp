// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/lightweight_test.hpp>

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers/cmath.hpp>

#endif

#include <bit>
#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>

using namespace boost::safe_numbers;

// The classification predicates never throw and must agree with the standard
// library for every category of value, including the special ones.
template <typename T>
void test_predicates()
{
    using basis_type = typename T::basis_type;

    const T zero {static_cast<basis_type>(0.0)};
    const T neg_zero {static_cast<basis_type>(-0.0)};
    const T one {static_cast<basis_type>(1.0)};
    const T neg_one {static_cast<basis_type>(-1.0)};
    const T pos_inf {std::numeric_limits<basis_type>::infinity()};
    const T neg_inf {-std::numeric_limits<basis_type>::infinity()};
    const T qnan {std::numeric_limits<basis_type>::quiet_NaN()};
    const T tiny {std::numeric_limits<basis_type>::denorm_min()};

    // isnan
    BOOST_TEST(isnan(qnan));
    BOOST_TEST(!isnan(one));
    BOOST_TEST(!isnan(pos_inf));

    // isinf
    BOOST_TEST(isinf(pos_inf));
    BOOST_TEST(isinf(neg_inf));
    BOOST_TEST(!isinf(one));
    BOOST_TEST(!isinf(qnan));

    // isfinite
    BOOST_TEST(isfinite(one));
    BOOST_TEST(isfinite(zero));
    BOOST_TEST(!isfinite(pos_inf));
    BOOST_TEST(!isfinite(qnan));

    // isnormal
    BOOST_TEST(isnormal(one));
    BOOST_TEST(!isnormal(zero));
    BOOST_TEST(!isnormal(pos_inf));
    BOOST_TEST(!isnormal(qnan));
    BOOST_TEST(!isnormal(tiny));

    // signbit
    BOOST_TEST(signbit(neg_zero));
    BOOST_TEST(!signbit(zero));
    BOOST_TEST(signbit(neg_one));
    BOOST_TEST(!signbit(one));
    BOOST_TEST(signbit(neg_inf));
    BOOST_TEST(!signbit(pos_inf));

    // fpclassify
    BOOST_TEST(fpclassify(zero) == FP_ZERO);
    BOOST_TEST(fpclassify(neg_zero) == FP_ZERO);
    BOOST_TEST(fpclassify(one) == FP_NORMAL);
    BOOST_TEST(fpclassify(pos_inf) == FP_INFINITE);
    BOOST_TEST(fpclassify(qnan) == FP_NAN);
    BOOST_TEST(fpclassify(tiny) == FP_SUBNORMAL);
}

// Signaling-NaN bit pattern built via bit_cast, so it reaches the predicate
// without being quieted by a floating-point operation first.
template <typename T>
void test_signaling_nan()
{
    using basis_type = typename T::basis_type;

    if constexpr (std::numeric_limits<basis_type>::has_signaling_NaN)
    {
        using bit_type = std::conditional_t<std::is_same_v<basis_type, float>, std::uint32_t, std::uint64_t>;

        constexpr auto snan_bits {std::bit_cast<bit_type>(std::numeric_limits<basis_type>::signaling_NaN())};
        const T snan {std::bit_cast<basis_type>(snan_bits)};

        BOOST_TEST(isnan(snan));
        BOOST_TEST(!isinf(snan));
        BOOST_TEST(!isfinite(snan));
        BOOST_TEST(!isnormal(snan));
        BOOST_TEST(fpclassify(snan) == FP_NAN);
    }
}

// Non-signaling ordered comparisons: false (not a throw) when an operand is NAN.
template <typename T>
void test_comparisons()
{
    using basis_type = typename T::basis_type;

    const T one {static_cast<basis_type>(1.0)};
    const T two {static_cast<basis_type>(2.0)};
    const T qnan {std::numeric_limits<basis_type>::quiet_NaN()};

    BOOST_TEST(isgreater(two, one));
    BOOST_TEST(!isgreater(one, two));
    BOOST_TEST(!isgreater(qnan, one));
    BOOST_TEST(!isgreater(one, qnan));

    BOOST_TEST(isgreaterequal(two, one));
    BOOST_TEST(isgreaterequal(one, one));
    BOOST_TEST(!isgreaterequal(qnan, one));

    BOOST_TEST(isless(one, two));
    BOOST_TEST(!isless(two, one));
    BOOST_TEST(!isless(qnan, one));

    BOOST_TEST(islessequal(one, two));
    BOOST_TEST(islessequal(one, one));
    BOOST_TEST(!islessequal(qnan, one));

    BOOST_TEST(islessgreater(one, two));
    BOOST_TEST(!islessgreater(one, one));
    BOOST_TEST(!islessgreater(qnan, one));

    BOOST_TEST(isunordered(qnan, one));
    BOOST_TEST(isunordered(one, qnan));
    BOOST_TEST(!isunordered(one, two));
}

int main()
{
    test_predicates<f32>();
    test_predicates<f64>();

    test_signaling_nan<f32>();
    test_signaling_nan<f64>();

    test_comparisons<f32>();
    test_comparisons<f64>();

    return boost::report_errors();
}

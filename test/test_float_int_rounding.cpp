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

#include <boost/safe_numbers/cmath.hpp>

#endif

#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>

using namespace boost::safe_numbers;

// Finite inputs round into the matching safe integer type. Comparing against the
// std result keeps the test independent of the active rounding mode for *rint.
template <typename T>
void test_finite()
{
    using basis_type = typename T::basis_type;

    for (const auto x : {static_cast<basis_type>(2.5), static_cast<basis_type>(2.4),
                         static_cast<basis_type>(-2.5), static_cast<basis_type>(3.5),
                         static_cast<basis_type>(0.0)})
    {
        BOOST_TEST(lround(T{x})  == i64{static_cast<std::int64_t>(std::lround(x))});
        BOOST_TEST(llround(T{x}) == i64{static_cast<std::int64_t>(std::llround(x))});
        BOOST_TEST(lrint(T{x})   == i64{static_cast<std::int64_t>(std::lrint(x))});
        BOOST_TEST(llrint(T{x})  == i64{static_cast<std::int64_t>(std::llrint(x))});
    }

    // lround rounds half away from zero
    BOOST_TEST(lround(T{static_cast<basis_type>(2.5)}) == i64{3});
    BOOST_TEST(lround(T{static_cast<basis_type>(-2.5)}) == i64{-3});

    // ilogb is the floor of log2 of the magnitude
    BOOST_TEST(ilogb(T{static_cast<basis_type>(8.0)}) == i32{3});
    BOOST_TEST(ilogb(T{static_cast<basis_type>(1.0)}) == i32{0});
    BOOST_TEST(ilogb(T{static_cast<basis_type>(0.5)}) == i32{-1});
}

// Non-finite input has an unspecified std result, so it is a domain_error.
template <typename T>
void test_non_finite_input()
{
    using basis_type = typename T::basis_type;

    const T qnan {std::numeric_limits<basis_type>::quiet_NaN()};
    const T pos_inf {std::numeric_limits<basis_type>::infinity()};
    const T neg_inf {-std::numeric_limits<basis_type>::infinity()};

    BOOST_TEST_THROWS(lround(qnan), std::domain_error);
    BOOST_TEST_THROWS(lround(pos_inf), std::domain_error);
    BOOST_TEST_THROWS(llround(neg_inf), std::domain_error);
    BOOST_TEST_THROWS(lrint(qnan), std::domain_error);
    BOOST_TEST_THROWS(llrint(pos_inf), std::domain_error);

    // ilogb additionally rejects zero (its std result is FP_ILOGB0)
    BOOST_TEST_THROWS(ilogb(T{static_cast<basis_type>(0.0)}), std::domain_error);
    BOOST_TEST_THROWS(ilogb(qnan), std::domain_error);
    BOOST_TEST_THROWS(ilogb(pos_inf), std::domain_error);
}

int main()
{
    test_finite<f32>();
    test_finite<f64>();

    test_non_finite_input<f32>();
    test_non_finite_input<f64>();

    return boost::report_errors();
}

#ifdef __clang__
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning (pop)
#endif

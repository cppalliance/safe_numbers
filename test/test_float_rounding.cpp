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
#include <limits>
#include <stdexcept>

using namespace boost::safe_numbers;

template <typename T>
void test_finite()
{
    using basis_type = typename T::basis_type;

    for (const auto x : {static_cast<basis_type>(2.7), static_cast<basis_type>(-2.7),
                         static_cast<basis_type>(2.5), static_cast<basis_type>(-2.5),
                         static_cast<basis_type>(0.0), static_cast<basis_type>(3.0)})
    {
        BOOST_TEST(floor(T{x})     == T{std::floor(x)});
        BOOST_TEST(ceil(T{x})      == T{std::ceil(x)});
        BOOST_TEST(trunc(T{x})     == T{std::trunc(x)});
        BOOST_TEST(round(T{x})     == T{std::round(x)});
        BOOST_TEST(nearbyint(T{x}) == T{std::nearbyint(x)});
        BOOST_TEST(rint(T{x})      == T{std::rint(x)});
    }
}

template <typename T>
void test_nan_input()
{
    using basis_type = typename T::basis_type;

    const T qnan {std::numeric_limits<basis_type>::quiet_NaN()};

    BOOST_TEST_THROWS(floor(qnan),     std::domain_error);
    BOOST_TEST_THROWS(ceil(qnan),      std::domain_error);
    BOOST_TEST_THROWS(trunc(qnan),     std::domain_error);
    BOOST_TEST_THROWS(round(qnan),     std::domain_error);
    BOOST_TEST_THROWS(nearbyint(qnan), std::domain_error);
    BOOST_TEST_THROWS(rint(qnan),      std::domain_error);
}

// Rounding an infinity returns that infinity, which violates the invariant.
template <typename T>
void test_infinite_input()
{
    using basis_type = typename T::basis_type;

    const T pos_inf {std::numeric_limits<basis_type>::infinity()};
    const T neg_inf {-std::numeric_limits<basis_type>::infinity()};

    BOOST_TEST_THROWS(floor(pos_inf), std::overflow_error);
    BOOST_TEST_THROWS(floor(neg_inf), std::underflow_error);
    BOOST_TEST_THROWS(ceil(pos_inf),  std::overflow_error);
    BOOST_TEST_THROWS(round(neg_inf), std::underflow_error);
}

int main()
{
    test_finite<f32>();
    test_finite<f64>();

    test_nan_input<f32>();
    test_nan_input<f64>();

    test_infinite_input<f32>();
    test_infinite_input<f64>();

    return boost::report_errors();
}

#ifdef __clang__
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning (pop)
#endif

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
#include <exception>
#include <limits>
#include <stdexcept>

using namespace boost::safe_numbers;

template <typename T>
void test_finite()
{
    using basis_type = typename T::basis_type;

    for (const auto x : {static_cast<basis_type>(0.0), static_cast<basis_type>(0.5),
                         static_cast<basis_type>(-0.5), static_cast<basis_type>(1.0)})
    {
        BOOST_TEST(erf(T{x}) == T{std::erf(x)});
        BOOST_TEST(erfc(T{x}) == T{std::erfc(x)});
    }

    // tgamma(5) = 4! = 24, lgamma of a positive value is finite
    BOOST_TEST(tgamma(T{static_cast<basis_type>(5.0)}) == T{std::tgamma(static_cast<basis_type>(5.0))});
    BOOST_TEST(lgamma(T{static_cast<basis_type>(5.0)}) == T{std::lgamma(static_cast<basis_type>(5.0))});
    BOOST_TEST(tgamma(T{static_cast<basis_type>(0.5)}) == T{std::tgamma(static_cast<basis_type>(0.5))});
}

// tgamma at a negative integer is a pole. Most standard libraries report it as a
// NAN (so it surfaces as std::domain_error), but newlib/Cygwin returns an infinity
// (so it surfaces as std::overflow_error or std::underflow_error). The portable
// invariant is only that the non-finite result is rejected, so accept any of them
// via their common base std::exception.
template <typename T>
void test_domain()
{
    using basis_type = typename T::basis_type;

    BOOST_TEST_THROWS(tgamma(T{static_cast<basis_type>(-1.0)}), std::exception);
    BOOST_TEST_THROWS(tgamma(T{static_cast<basis_type>(-2.0)}), std::exception);
}

// tgamma(0) and lgamma(0) are poles producing +inf -> overflow_error.
// tgamma of a large argument overflows.
template <typename T>
void test_overflow()
{
    using basis_type = typename T::basis_type;

    const T zero {static_cast<basis_type>(0.0)};

    BOOST_TEST_THROWS(tgamma(zero), std::overflow_error);
    BOOST_TEST_THROWS(lgamma(zero), std::overflow_error);
    BOOST_TEST_THROWS(tgamma(T{static_cast<basis_type>(200.0)}), std::overflow_error);
}

template <typename T>
void test_nan_input()
{
    using basis_type = typename T::basis_type;

    const T qnan {std::numeric_limits<basis_type>::quiet_NaN()};

    BOOST_TEST_THROWS(erf(qnan), std::domain_error);
    BOOST_TEST_THROWS(erfc(qnan), std::domain_error);
    BOOST_TEST_THROWS(tgamma(qnan), std::domain_error);
    BOOST_TEST_THROWS(lgamma(qnan), std::domain_error);
}

int main()
{
    test_finite<f32>();
    test_finite<f64>();

    test_domain<f32>();
    test_domain<f64>();

    test_overflow<f32>();
    test_overflow<f64>();

    test_nan_input<f32>();
    test_nan_input<f64>();

    return boost::report_errors();
}

#ifdef __clang__
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning (pop)
#endif

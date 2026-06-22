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

    BOOST_TEST(sqrt(T{static_cast<basis_type>(4.0)}) == T{static_cast<basis_type>(2.0)});
    BOOST_TEST(sqrt(T{static_cast<basis_type>(0.0)}) == T{static_cast<basis_type>(0.0)});
    BOOST_TEST(cbrt(T{static_cast<basis_type>(27.0)}) == T{std::cbrt(static_cast<basis_type>(27.0))});
    // cbrt of a negative value is well defined
    BOOST_TEST(cbrt(T{static_cast<basis_type>(-8.0)}) == T{std::cbrt(static_cast<basis_type>(-8.0))});

    BOOST_TEST(pow(T{static_cast<basis_type>(2.0)}, T{static_cast<basis_type>(10.0)})
             == T{std::pow(static_cast<basis_type>(2.0), static_cast<basis_type>(10.0))});
    BOOST_TEST(pow(T{static_cast<basis_type>(9.0)}, T{static_cast<basis_type>(0.5)})
             == T{std::pow(static_cast<basis_type>(9.0), static_cast<basis_type>(0.5))});

    BOOST_TEST(hypot(T{static_cast<basis_type>(3.0)}, T{static_cast<basis_type>(4.0)})
             == T{static_cast<basis_type>(5.0)});
}

// sqrt of a negative value and pow of a negative base with a non-integer
// exponent both produce NAN -> domain_error
template <typename T>
void test_domain()
{
    using basis_type = typename T::basis_type;

    BOOST_TEST_THROWS(sqrt(T{static_cast<basis_type>(-1.0)}), std::domain_error);
    BOOST_TEST_THROWS(pow(T{static_cast<basis_type>(-1.0)}, T{static_cast<basis_type>(0.5)}), std::domain_error);
}

// pow producing infinity: pole (0 ^ negative) and range overflow
template <typename T>
void test_overflow()
{
    using basis_type = typename T::basis_type;

    // 0 ^ -1 = +inf
    BOOST_TEST_THROWS(pow(T{static_cast<basis_type>(0.0)}, T{static_cast<basis_type>(-1.0)}), std::overflow_error);
    // 2 ^ 2000 overflows for both float and double
    BOOST_TEST_THROWS(pow(T{static_cast<basis_type>(2.0)}, T{static_cast<basis_type>(2000.0)}), std::overflow_error);
    // hypot(max, max) overflows
    const T max_val {std::numeric_limits<basis_type>::max()};
    BOOST_TEST_THROWS(hypot(max_val, max_val), std::overflow_error);
}

template <typename T>
void test_nan_input()
{
    using basis_type = typename T::basis_type;

    const T qnan {std::numeric_limits<basis_type>::quiet_NaN()};
    const T one {static_cast<basis_type>(1.0)};
    const T two {static_cast<basis_type>(2.0)};

    BOOST_TEST_THROWS(sqrt(qnan), std::domain_error);
    BOOST_TEST_THROWS(cbrt(qnan), std::domain_error);
    BOOST_TEST_THROWS(pow(qnan, one), std::domain_error);
    // pow(1, y) == 1 for any y per the C standard, so use a base other than 1
    BOOST_TEST_THROWS(pow(two, qnan), std::domain_error);
    BOOST_TEST_THROWS(hypot(qnan, one), std::domain_error);
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

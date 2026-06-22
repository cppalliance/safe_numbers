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

    const auto a {static_cast<basis_type>(7.0)};
    const auto b {static_cast<basis_type>(3.0)};

    BOOST_TEST(abs(T{static_cast<basis_type>(-2.5)})  == T{static_cast<basis_type>(2.5)});
    BOOST_TEST(fabs(T{static_cast<basis_type>(-2.5)}) == T{static_cast<basis_type>(2.5)});
    BOOST_TEST(copysign(T{b}, T{static_cast<basis_type>(-1.0)}) == T{static_cast<basis_type>(-3.0)});

    BOOST_TEST(fmod(T{a}, T{b})      == T{std::fmod(a, b)});
    BOOST_TEST(remainder(T{a}, T{b}) == T{std::remainder(a, b)});
    BOOST_TEST(fdim(T{a}, T{b})      == T{std::fdim(a, b)});
    BOOST_TEST(fdim(T{b}, T{a})      == T{static_cast<basis_type>(0.0)});
    BOOST_TEST(fmin(T{a}, T{b})      == T{b});
    BOOST_TEST(fmax(T{a}, T{b})      == T{a});

    BOOST_TEST(fma(T{static_cast<basis_type>(2.0)}, T{b}, T{static_cast<basis_type>(4.0)})
             == T{std::fma(static_cast<basis_type>(2.0), b, static_cast<basis_type>(4.0))});

    BOOST_TEST(ldexp(T{static_cast<basis_type>(1.0)}, 3)  == T{static_cast<basis_type>(8.0)});
    BOOST_TEST(scalbn(T{static_cast<basis_type>(1.0)}, 4) == T{static_cast<basis_type>(16.0)});
    BOOST_TEST(scalbln(T{static_cast<basis_type>(1.0)}, 2L) == T{static_cast<basis_type>(4.0)});

    BOOST_TEST(nextafter(T{static_cast<basis_type>(1.0)}, T{static_cast<basis_type>(2.0)})
             == T{std::nextafter(static_cast<basis_type>(1.0), static_cast<basis_type>(2.0))});
}

// fmod/remainder by zero produce NAN -> domain_error
template <typename T>
void test_domain()
{
    using basis_type = typename T::basis_type;

    const T one {static_cast<basis_type>(1.0)};
    const T zero {static_cast<basis_type>(0.0)};

    BOOST_TEST_THROWS(fmod(one, zero), std::domain_error);
    BOOST_TEST_THROWS(remainder(one, zero), std::domain_error);
}

// Operations whose result is infinite violate the invariant.
template <typename T>
void test_overflow()
{
    using basis_type = typename T::basis_type;

    // 2^5000 overflows
    BOOST_TEST_THROWS(ldexp(T{static_cast<basis_type>(1.0)}, 5000), std::overflow_error);
    BOOST_TEST_THROWS(scalbn(T{static_cast<basis_type>(1.0)}, 5000), std::overflow_error);

    // copysign carries the infinity of its first argument through
    const T pos_inf {std::numeric_limits<basis_type>::infinity()};
    BOOST_TEST_THROWS(copysign(pos_inf, T{static_cast<basis_type>(-1.0)}), std::underflow_error);
    BOOST_TEST_THROWS(fmax(pos_inf, T{static_cast<basis_type>(3.0)}), std::overflow_error);
}

template <typename T>
void test_nan_input()
{
    using basis_type = typename T::basis_type;

    const T qnan {std::numeric_limits<basis_type>::quiet_NaN()};
    const T one {static_cast<basis_type>(1.0)};

    BOOST_TEST_THROWS(abs(qnan), std::domain_error);
    BOOST_TEST_THROWS(fabs(qnan), std::domain_error);
    BOOST_TEST_THROWS(fmod(qnan, one), std::domain_error);
    BOOST_TEST_THROWS(fma(qnan, one, one), std::domain_error);
    BOOST_TEST_THROWS(ldexp(qnan, 1), std::domain_error);

    // fmin/fmax ignore a single NAN operand, so the finite result does not throw
    BOOST_TEST(fmin(qnan, one) == T{one});
    BOOST_TEST(fmax(qnan, one) == T{one});
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

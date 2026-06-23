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

#include <bit>
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>

using namespace boost::safe_numbers;

// Route the argument through a volatile so the reference std:: call is evaluated by the
// runtime libm, exactly as the wrapper evaluates it, rather than being constant-folded by
// the compiler to a correctly-rounded value. On 32-bit x86 some libm transcendentals are
// not correctly rounded, so the folded reference would not match the wrapper's faithful
// passthrough of the runtime result.
template <typename U>
auto opaque(const U value) noexcept -> U
{
    volatile U sink {value};
    return sink;
}

// Compare the stored basis values bit-for-bit. Going through bit_cast forces each value
// out of any 80-bit x87 register to its true 32/64-bit storage, so the comparison is not
// corrupted by excess precision (FLT_EVAL_METHOD == 2), where float_basis::operator== can
// otherwise compare 80-bit register values that differ even when the stored values match.
template <typename T>
auto same_bits(const T a, const T b) noexcept -> bool
{
    using basis_type = typename T::basis_type;
    using bits_type = std::conditional_t<std::is_same_v<basis_type, float>, std::uint32_t, std::uint64_t>;
    return std::bit_cast<bits_type>(static_cast<basis_type>(a)) == std::bit_cast<bits_type>(static_cast<basis_type>(b));
}

template <typename T>
void test_finite()
{
    using basis_type = typename T::basis_type;

    for (const auto raw : {static_cast<basis_type>(0.0), static_cast<basis_type>(0.5),
                           static_cast<basis_type>(-0.5), static_cast<basis_type>(1.0)})
    {
        const auto x {opaque(raw)};
        BOOST_TEST(same_bits(sinh(T{x}), T{std::sinh(x)}));
        BOOST_TEST(same_bits(cosh(T{x}), T{std::cosh(x)}));
        BOOST_TEST(same_bits(tanh(T{x}), T{std::tanh(x)}));
        BOOST_TEST(same_bits(asinh(T{x}), T{std::asinh(x)}));
    }

    // atanh domain is the open interval (-1, 1)
    for (const auto raw : {static_cast<basis_type>(0.0), static_cast<basis_type>(0.5),
                           static_cast<basis_type>(-0.5), static_cast<basis_type>(0.9)})
    {
        const auto x {opaque(raw)};
        BOOST_TEST(same_bits(atanh(T{x}), T{std::atanh(x)}));
    }

    // acosh domain is [1, inf)
    for (const auto raw : {static_cast<basis_type>(1.0), static_cast<basis_type>(2.0)})
    {
        const auto x {opaque(raw)};
        BOOST_TEST(same_bits(acosh(T{x}), T{std::acosh(x)}));
    }
}

// acosh(< 1) and atanh(|x| > 1) produce NAN -> domain_error
template <typename T>
void test_domain()
{
    using basis_type = typename T::basis_type;

    BOOST_TEST_THROWS(acosh(T{static_cast<basis_type>(0.0)}), std::domain_error);
    BOOST_TEST_THROWS(acosh(T{static_cast<basis_type>(-1.0)}), std::domain_error);
    BOOST_TEST_THROWS(atanh(T{static_cast<basis_type>(2.0)}), std::domain_error);
    BOOST_TEST_THROWS(atanh(T{static_cast<basis_type>(-2.0)}), std::domain_error);
}

// atanh(+/-1) is a pole: +1 -> +inf (overflow), -1 -> -inf (underflow)
template <typename T>
void test_pole()
{
    using basis_type = typename T::basis_type;

    BOOST_TEST_THROWS(atanh(T{static_cast<basis_type>(1.0)}), std::overflow_error);
    BOOST_TEST_THROWS(atanh(T{static_cast<basis_type>(-1.0)}), std::underflow_error);
}

// sinh/cosh of a large magnitude overflow to +/- infinity
template <typename T>
void test_overflow()
{
    using basis_type = typename T::basis_type;

    const T big {static_cast<basis_type>(1000.0)};
    const T neg_big {static_cast<basis_type>(-1000.0)};

    BOOST_TEST_THROWS(cosh(big), std::overflow_error);
    BOOST_TEST_THROWS(cosh(neg_big), std::overflow_error);
    BOOST_TEST_THROWS(sinh(big), std::overflow_error);
    BOOST_TEST_THROWS(sinh(neg_big), std::underflow_error);
}

template <typename T>
void test_nan_input()
{
    using basis_type = typename T::basis_type;

    const T qnan {std::numeric_limits<basis_type>::quiet_NaN()};

    BOOST_TEST_THROWS(sinh(qnan), std::domain_error);
    BOOST_TEST_THROWS(cosh(qnan), std::domain_error);
    BOOST_TEST_THROWS(tanh(qnan), std::domain_error);
    BOOST_TEST_THROWS(asinh(qnan), std::domain_error);
    BOOST_TEST_THROWS(acosh(qnan), std::domain_error);
    BOOST_TEST_THROWS(atanh(qnan), std::domain_error);
}

int main()
{
    test_finite<f32>();
    test_finite<f64>();

    test_domain<f32>();
    test_domain<f64>();

    test_pole<f32>();
    test_pole<f64>();

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

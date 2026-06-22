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

    // sqrt is correctly rounded everywhere, so an exact integral result is portable
    BOOST_TEST(sqrt(T{static_cast<basis_type>(4.0)}) == T{static_cast<basis_type>(2.0)});
    BOOST_TEST(sqrt(T{static_cast<basis_type>(0.0)}) == T{static_cast<basis_type>(0.0)});

    const auto twenty_seven {opaque(static_cast<basis_type>(27.0))};
    BOOST_TEST(same_bits(cbrt(T{twenty_seven}), T{std::cbrt(twenty_seven)}));
    // cbrt of a negative value is well defined
    const auto neg_eight {opaque(static_cast<basis_type>(-8.0))};
    BOOST_TEST(same_bits(cbrt(T{neg_eight}), T{std::cbrt(neg_eight)}));

    const auto two {opaque(static_cast<basis_type>(2.0))};
    const auto ten {opaque(static_cast<basis_type>(10.0))};
    BOOST_TEST(same_bits(pow(T{two}, T{ten}), T{std::pow(two, ten)}));
    const auto nine {opaque(static_cast<basis_type>(9.0))};
    const auto half {opaque(static_cast<basis_type>(0.5))};
    BOOST_TEST(same_bits(pow(T{nine}, T{half}), T{std::pow(nine, half)}));

    // hypot(3, 4) == 5 exactly
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

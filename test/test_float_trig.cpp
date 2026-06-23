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

// Finite inputs: the wrapper delegates to the same std function on the same basis
// value, so the result is bit-identical and can be compared with exact equality.
template <typename T>
void test_finite()
{
    using basis_type = typename T::basis_type;

    for (const auto raw : {static_cast<basis_type>(0.0), static_cast<basis_type>(0.5),
                           static_cast<basis_type>(-0.5), static_cast<basis_type>(1.0)})
    {
        const auto x {opaque(raw)};
        BOOST_TEST(same_bits(sin(T{x}), T{std::sin(x)}));
        BOOST_TEST(same_bits(cos(T{x}), T{std::cos(x)}));
        BOOST_TEST(same_bits(tan(T{x}), T{std::tan(x)}));
        BOOST_TEST(same_bits(atan(T{x}), T{std::atan(x)}));
        BOOST_TEST(same_bits(asin(T{x}), T{std::asin(x)}));
        BOOST_TEST(same_bits(acos(T{x}), T{std::acos(x)}));
    }

    const auto a {opaque(static_cast<basis_type>(1.0))};
    const auto b {opaque(static_cast<basis_type>(1.0))};
    BOOST_TEST(same_bits(atan2(T{a}, T{b}), T{std::atan2(a, b)}));
    const auto c {opaque(static_cast<basis_type>(-1.0))};
    const auto d {opaque(static_cast<basis_type>(2.0))};
    BOOST_TEST(same_bits(atan2(T{c}, T{d}), T{std::atan2(c, d)}));
}

// asin/acos outside [-1, 1] produce NAN -> domain_error
template <typename T>
void test_domain()
{
    using basis_type = typename T::basis_type;

    const T two {static_cast<basis_type>(2.0)};
    const T neg_two {static_cast<basis_type>(-2.0)};

    BOOST_TEST_THROWS(asin(two), std::domain_error);
    BOOST_TEST_THROWS(asin(neg_two), std::domain_error);
    BOOST_TEST_THROWS(acos(two), std::domain_error);
    BOOST_TEST_THROWS(acos(neg_two), std::domain_error);
}

// Any NAN operand (quiet or signaling) yields a NAN result -> domain_error
template <typename T>
void test_nan_input()
{
    using basis_type = typename T::basis_type;

    const T qnan {std::numeric_limits<basis_type>::quiet_NaN()};

    BOOST_TEST_THROWS(sin(qnan), std::domain_error);
    BOOST_TEST_THROWS(cos(qnan), std::domain_error);
    BOOST_TEST_THROWS(tan(qnan), std::domain_error);
    BOOST_TEST_THROWS(atan(qnan), std::domain_error);
    BOOST_TEST_THROWS(atan2(qnan, T{static_cast<basis_type>(1.0)}), std::domain_error);

    if constexpr (std::numeric_limits<basis_type>::has_signaling_NaN)
    {
        const T snan {std::numeric_limits<basis_type>::signaling_NaN()};
        BOOST_TEST_THROWS(sin(snan), std::domain_error);
        BOOST_TEST_THROWS(atan(snan), std::domain_error);
    }
}

int main()
{
    test_finite<f32>();
    test_finite<f64>();

    test_domain<f32>();
    test_domain<f64>();

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

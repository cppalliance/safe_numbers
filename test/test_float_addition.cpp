// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/lightweight_test.hpp>

// Ignore [[nodiscard]] on the tests that we know are going to throw
#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunused-result"
#  pragma clang diagnostic ignored "-Wfloat-equal"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wunused-result"
#  pragma GCC diagnostic ignored "-Wfloat-equal"
#elif defined(_MSC_VER)
#  pragma warning (push)
#  pragma warning (disable: 4834)
#endif

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers/floats.hpp>

#endif

#include <bit>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>

using namespace boost::safe_numbers;

// -----------------------------------------------
// No error: regular finite addition
// IEEE 754 section 6 hot path
// -----------------------------------------------

template <typename T>
void test_finite_addition()
{
    using basis_type = typename T::basis_type;

    BOOST_TEST(T{static_cast<basis_type>(1.0)} + T{static_cast<basis_type>(2.0)} == T{static_cast<basis_type>(3.0)});
    BOOST_TEST(T{static_cast<basis_type>(0.0)} + T{static_cast<basis_type>(0.0)} == T{static_cast<basis_type>(0.0)});
    BOOST_TEST(T{static_cast<basis_type>(-1.0)} + T{static_cast<basis_type>(1.0)} == T{static_cast<basis_type>(0.0)});
    BOOST_TEST(T{static_cast<basis_type>(0.5)} + T{static_cast<basis_type>(0.5)} == T{static_cast<basis_type>(1.0)});
    BOOST_TEST(T{static_cast<basis_type>(-3.5)} + T{static_cast<basis_type>(-1.5)} == T{static_cast<basis_type>(-5.0)});

    // Adding zero is identity for any finite value
    BOOST_TEST(T{std::numeric_limits<basis_type>::max()} + T{static_cast<basis_type>(0.0)} == T{std::numeric_limits<basis_type>::max()});
    BOOST_TEST(T{std::numeric_limits<basis_type>::lowest()} + T{static_cast<basis_type>(0.0)} == T{std::numeric_limits<basis_type>::lowest()});

    // Adding max + lowest cancels exactly
    BOOST_TEST(T{std::numeric_limits<basis_type>::max()} + T{std::numeric_limits<basis_type>::lowest()} == T{static_cast<basis_type>(0.0)});

    // A subnormal result is finite and should not throw
    const T tiny {std::numeric_limits<basis_type>::denorm_min()};
    BOOST_TEST(tiny + T{static_cast<basis_type>(0.0)} == tiny);
}

// -----------------------------------------------
// Overflow: saturation to +infinity
// IEEE 754 section 6.1
// -----------------------------------------------

template <typename T>
void test_overflow_to_positive_infinity()
{
    using basis_type = typename T::basis_type;

    const T max_val {std::numeric_limits<basis_type>::max()};

    // max + max produces +inf
    BOOST_TEST_THROWS(max_val + max_val, std::overflow_error);

    // Two large finite values that sum past max
    const T big {std::numeric_limits<basis_type>::max() / static_cast<basis_type>(2)};
    const T also_big {std::numeric_limits<basis_type>::max()};
    BOOST_TEST_THROWS(big + also_big, std::overflow_error);

    // +inf + finite is still +inf so it should also be flagged as overflow
    const T pos_inf {std::numeric_limits<basis_type>::infinity()};
    const T finite {static_cast<basis_type>(1.0)};
    BOOST_TEST_THROWS(pos_inf + finite, std::overflow_error);
    BOOST_TEST_THROWS(finite + pos_inf, std::overflow_error);

    // +inf + +inf is +inf (allowed in IEEE 754, but our policy treats it as overflow)
    BOOST_TEST_THROWS(pos_inf + pos_inf, std::overflow_error);
}

// -----------------------------------------------
// Underflow: saturation to -infinity
// IEEE 754 section 6.1
// -----------------------------------------------

template <typename T>
void test_underflow_to_negative_infinity()
{
    using basis_type = typename T::basis_type;

    const T lowest_val {std::numeric_limits<basis_type>::lowest()};

    // lowest + lowest produces -inf
    BOOST_TEST_THROWS(lowest_val + lowest_val, std::underflow_error);

    // Two large negative finite values that sum past lowest
    const T big_neg {std::numeric_limits<basis_type>::lowest() / static_cast<basis_type>(2)};
    const T also_big_neg {std::numeric_limits<basis_type>::lowest()};
    BOOST_TEST_THROWS(big_neg + also_big_neg, std::underflow_error);

    // -inf + finite is still -inf so it should be flagged as underflow
    const T neg_inf {-std::numeric_limits<basis_type>::infinity()};
    const T finite {static_cast<basis_type>(1.0)};
    BOOST_TEST_THROWS(neg_inf + finite, std::underflow_error);
    BOOST_TEST_THROWS(finite + neg_inf, std::underflow_error);

    // -inf + -inf is -inf
    BOOST_TEST_THROWS(neg_inf + neg_inf, std::underflow_error);
}

// -----------------------------------------------
// NaN op: any operand is a quiet NaN
// IEEE 754 section 6.2
// -----------------------------------------------

template <typename T>
void test_qnan_operand()
{
    using basis_type = typename T::basis_type;

    const T qnan {std::numeric_limits<basis_type>::quiet_NaN()};
    const T finite {static_cast<basis_type>(1.0)};
    const T pos_inf {std::numeric_limits<basis_type>::infinity()};
    const T neg_inf {-std::numeric_limits<basis_type>::infinity()};
    const T zero {static_cast<basis_type>(0.0)};

    // QNaN + finite, in either order
    BOOST_TEST_THROWS(qnan + finite, std::domain_error);
    BOOST_TEST_THROWS(finite + qnan, std::domain_error);

    // QNaN + QNaN
    BOOST_TEST_THROWS(qnan + qnan, std::domain_error);

    // QNaN + +inf and QNaN + -inf
    BOOST_TEST_THROWS(qnan + pos_inf, std::domain_error);
    BOOST_TEST_THROWS(pos_inf + qnan, std::domain_error);
    BOOST_TEST_THROWS(qnan + neg_inf, std::domain_error);
    BOOST_TEST_THROWS(neg_inf + qnan, std::domain_error);

    // QNaN + zero
    BOOST_TEST_THROWS(qnan + zero, std::domain_error);
    BOOST_TEST_THROWS(zero + qnan, std::domain_error);
}

// -----------------------------------------------
// Invalid op: addition of infinities of opposite sign
// IEEE 754 section 7.2.d
// -----------------------------------------------

template <typename T>
void test_inf_minus_inf_invalid()
{
    using basis_type = typename T::basis_type;

    const T pos_inf {std::numeric_limits<basis_type>::infinity()};
    const T neg_inf {-std::numeric_limits<basis_type>::infinity()};

    BOOST_TEST_THROWS(pos_inf + neg_inf, std::domain_error);
    BOOST_TEST_THROWS(neg_inf + pos_inf, std::domain_error);
}

// -----------------------------------------------
// Invalid op: any operand is a signaling NaN
// IEEE 754 section 7.2.a
//
// We construct the SNaN via std::bit_cast from the integer bit pattern
// of std::numeric_limits<basis_type>::signaling_NaN(). Going through bit_cast
// avoids any FP ops that could quiet the value before it reaches our checker.
// -----------------------------------------------

template <typename T>
void test_snan_operand()
{
    using basis_type = typename T::basis_type;

    if constexpr (std::numeric_limits<basis_type>::has_signaling_NaN)
    {
        using bit_type = std::conditional_t<std::is_same_v<basis_type, float>, std::uint32_t, std::uint64_t>;

        constexpr auto snan_bits {std::bit_cast<bit_type>(std::numeric_limits<basis_type>::signaling_NaN())};
        constexpr auto qnan_bits {std::bit_cast<bit_type>(std::numeric_limits<basis_type>::quiet_NaN())};

        // On platforms that do not actually distinguish between SNaN and QNaN at
        // the bit-pattern level there is nothing to test here.
        if constexpr (snan_bits != qnan_bits)
        {
            const auto snan_val {std::bit_cast<basis_type>(snan_bits)};
            const T snan {snan_val};
            const T finite {static_cast<basis_type>(1.0)};
            const T pos_inf {std::numeric_limits<basis_type>::infinity()};
            const T neg_inf {-std::numeric_limits<basis_type>::infinity()};
            const T zero {static_cast<basis_type>(0.0)};

            // SNaN + finite, in either order
            BOOST_TEST_THROWS(snan + finite, std::domain_error);
            BOOST_TEST_THROWS(finite + snan, std::domain_error);

            // SNaN + SNaN
            BOOST_TEST_THROWS(snan + snan, std::domain_error);

            // SNaN + +inf and SNaN + -inf both trip the SNaN check before the
            // inf - inf check, which is the order described in the implementation
            BOOST_TEST_THROWS(snan + pos_inf, std::domain_error);
            BOOST_TEST_THROWS(pos_inf + snan, std::domain_error);
            BOOST_TEST_THROWS(snan + neg_inf, std::domain_error);
            BOOST_TEST_THROWS(neg_inf + snan, std::domain_error);

            // SNaN + zero
            BOOST_TEST_THROWS(snan + zero, std::domain_error);
            BOOST_TEST_THROWS(zero + snan, std::domain_error);
        }
    }
}

int main()
{
    test_finite_addition<f32>();
    test_finite_addition<f64>();

    test_overflow_to_positive_infinity<f32>();
    test_overflow_to_positive_infinity<f64>();

    test_underflow_to_negative_infinity<f32>();
    test_underflow_to_negative_infinity<f64>();

    test_qnan_operand<f32>();
    test_qnan_operand<f64>();

    test_inf_minus_inf_invalid<f32>();
    test_inf_minus_inf_invalid<f64>();

    test_snan_operand<f32>();
    test_snan_operand<f64>();

    return boost::report_errors();
}

#ifdef __clang__
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning (pop)
#endif

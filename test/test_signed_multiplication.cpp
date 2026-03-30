// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/lightweight_test.hpp>

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wold-style-cast"
#  pragma clang diagnostic ignored "-Wundef"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wsign-conversion"
#  pragma clang diagnostic ignored "-Wfloat-equal"
#  pragma clang diagnostic ignored "-Wsign-compare"
#  pragma clang diagnostic ignored "-Woverflow"

#  if (__clang_major__ >= 10 && !defined(__APPLE__)) || __clang_major__ >= 13
#    pragma clang diagnostic ignored "-Wdeprecated-copy"
#  endif

#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wundef"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#  pragma GCC diagnostic ignored "-Wsign-compare"
#  pragma GCC diagnostic ignored "-Wfloat-equal"
#  pragma GCC diagnostic ignored "-Woverflow"

#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4389)
#  pragma warning(disable : 4127)
#  pragma warning(disable : 4305)
#  pragma warning(disable : 4309)
#endif

#define BOOST_SAFE_NUMBERS_DETAIL_INT128_ALLOW_SIGN_COMPARE
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_ALLOW_SIGN_CONVERSION

#include <boost/random/uniform_int_distribution.hpp>

#ifdef __clang__
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

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

#include <boost/safe_numbers.hpp>
#include <random>
#include <limits>
#include <type_traits>
#include <iostream>

#endif

using namespace boost::safe_numbers;

inline std::mt19937_64 rng{42};
inline constexpr std::size_t N {1024};

// Helper: returns a safe multiplication bound (approx sqrt of max)
// so that |a| <= bound && |b| <= bound implies a*b fits in T
template <typename T>
constexpr auto safe_mul_bound() -> T
{
    if constexpr (std::is_same_v<T, std::int8_t>)
    {
        return 11; // floor(sqrt(127))
    }
    else if constexpr (std::is_same_v<T, std::int16_t>)
    {
        return 181; // floor(sqrt(32767))
    }
    else if constexpr (std::is_same_v<T, std::int32_t>)
    {
        return 46340; // floor(sqrt(2^31 - 1))
    }
    else if constexpr (std::is_same_v<T, std::int64_t>)
    {
        return static_cast<T>(3037000499LL); // floor(sqrt(2^63 - 1))
    }
    else
    {
        return static_cast<T>(std::numeric_limits<std::int64_t>::max()); // conservative bound for i128
    }
}

// -----------------------------------------------
// Valid multiplication: values in the safe sqrt-range
// -----------------------------------------------

template <typename T>
void test_valid_multiplication()
{
    using basis_type = detail::underlying_type_t<T>;

    const auto bound {safe_mul_bound<basis_type>()};
    boost::random::uniform_int_distribution<basis_type> dist {static_cast<basis_type>(-bound), bound};

    for (std::size_t i {0}; i < N; ++i)
    {
        const auto lhs_value {dist(rng)};
        const auto rhs_value {dist(rng)};

        T ref_value {};
        if constexpr (std::is_same_v<basis_type, std::int8_t> || std::is_same_v<basis_type, std::int16_t>)
        {
            ref_value = T{static_cast<basis_type>(static_cast<std::int32_t>(lhs_value) * static_cast<std::int32_t>(rhs_value))};
        }
        else
        {
            ref_value = T{static_cast<basis_type>(lhs_value * rhs_value)};
        }

        const T lhs {lhs_value};
        const T rhs {rhs_value};
        const auto res {lhs * rhs};

        BOOST_TEST(ref_value == res);
    }
}

// -----------------------------------------------
// Positive overflow: max * [2..max] => overflow
// (same-sign multiplication producing too-large positive)
// -----------------------------------------------

template <typename T>
void test_positive_overflow()
{
    using basis_type = detail::underlying_type_t<T>;
    boost::random::uniform_int_distribution<basis_type> dist {basis_type{2},
                                                              std::numeric_limits<basis_type>::max()};

    for (std::size_t i {0}; i < N; ++i)
    {
        constexpr auto lhs_value {std::numeric_limits<basis_type>::max()};
        const auto rhs_value {dist(rng)};

        const T lhs {lhs_value};
        const T rhs {rhs_value};

        BOOST_TEST_THROWS(lhs * rhs, std::overflow_error);
    }
}

// -----------------------------------------------
// Negative overflow (underflow): max * [min..-2] => underflow
// (mixed-sign multiplication producing too-large negative)
// -----------------------------------------------

template <typename T>
void test_negative_overflow()
{
    using basis_type = detail::underlying_type_t<T>;
    boost::random::uniform_int_distribution<basis_type> dist {std::numeric_limits<basis_type>::min(),
                                                              basis_type{-2}};

    for (std::size_t i {0}; i < N; ++i)
    {
        constexpr auto lhs_value {std::numeric_limits<basis_type>::max()};
        const auto rhs_value {dist(rng)};

        const T lhs {lhs_value};
        const T rhs {rhs_value};

        BOOST_TEST_THROWS(lhs * rhs, std::underflow_error);
    }
}

// -----------------------------------------------
// Multiply by zero: anything * 0 = 0
// -----------------------------------------------

template <typename T>
void test_multiply_by_zero()
{
    using basis_type = detail::underlying_type_t<T>;
    boost::random::uniform_int_distribution<basis_type> dist {std::numeric_limits<basis_type>::min(),
                                                              std::numeric_limits<basis_type>::max()};

    for (std::size_t i {0}; i < N; ++i)
    {
        const auto val {dist(rng)};
        const T x {val};
        const T zero {basis_type{0}};

        BOOST_TEST(x * zero == zero);
        BOOST_TEST(zero * x == zero);
    }
}

// -----------------------------------------------
// Compound assignment: operator*=
// -----------------------------------------------

template <typename T>
void test_multiply_equals()
{
    using basis_type = detail::underlying_type_t<T>;

    const auto bound {safe_mul_bound<basis_type>()};
    boost::random::uniform_int_distribution<basis_type> dist {static_cast<basis_type>(-bound), bound};

    for (std::size_t i {0}; i < N; ++i)
    {
        const auto lhs_value {dist(rng)};
        const auto rhs_value {dist(rng)};

        const T lhs {lhs_value};
        const T rhs {rhs_value};

        // operator* result for reference
        const auto expected {lhs * rhs};

        // operator*= should give the same result
        T result {lhs_value};
        result *= rhs;
        BOOST_TEST(result == expected);
    }

    // *= with overflow should throw
    T near_max {std::numeric_limits<basis_type>::max()};
    BOOST_TEST_THROWS(near_max *= T{basis_type{2}}, std::overflow_error);

    // *= with underflow should throw
    T near_min {std::numeric_limits<basis_type>::min()};
    BOOST_TEST_THROWS(near_min *= T{basis_type{2}}, std::underflow_error);

    // *= with zero gives zero
    T val {basis_type{42}};
    val *= T{basis_type{0}};
    BOOST_TEST(val == T{basis_type{0}});

    // *= with one is identity
    T val2 {basis_type{42}};
    val2 *= T{basis_type{1}};
    BOOST_TEST(val2 == T{basis_type{42}});
}

// -----------------------------------------------
// Specific boundary cases
// -----------------------------------------------

template <typename T>
void test_boundary_cases()
{
    using basis_type = detail::underlying_type_t<T>;

    // 0 * 0 = 0
    BOOST_TEST(T{basis_type{0}} * T{basis_type{0}} == T{basis_type{0}});

    // 1 * x = x
    BOOST_TEST(T{basis_type{1}} * T{basis_type{42}} == T{basis_type{42}});
    BOOST_TEST(T{basis_type{42}} * T{basis_type{1}} == T{basis_type{42}});

    // -1 * x = -x
    BOOST_TEST(T{basis_type{-1}} * T{basis_type{42}} == T{basis_type{-42}});
    BOOST_TEST(T{basis_type{42}} * T{basis_type{-1}} == T{basis_type{-42}});

    // -1 * -1 = 1
    BOOST_TEST(T{basis_type{-1}} * T{basis_type{-1}} == T{basis_type{1}});

    // max * 1 = max
    BOOST_TEST(T{std::numeric_limits<basis_type>::max()} * T{basis_type{1}} == T{std::numeric_limits<basis_type>::max()});

    // min * 1 = min
    BOOST_TEST(T{std::numeric_limits<basis_type>::min()} * T{basis_type{1}} == T{std::numeric_limits<basis_type>::min()});

    // max * 0 = 0
    BOOST_TEST(T{std::numeric_limits<basis_type>::max()} * T{basis_type{0}} == T{basis_type{0}});

    // min * 0 = 0
    BOOST_TEST(T{std::numeric_limits<basis_type>::min()} * T{basis_type{0}} == T{basis_type{0}});

    // max * -1 = -max (valid since -max >= min)
    BOOST_TEST(T{std::numeric_limits<basis_type>::max()} * T{basis_type{-1}} == T{static_cast<basis_type>(-std::numeric_limits<basis_type>::max())});

    // min * -1 overflows (since -min > max for two's complement)
    BOOST_TEST_THROWS(T{std::numeric_limits<basis_type>::min()} * T{basis_type{-1}}, std::overflow_error);

    // max * 2 overflows
    BOOST_TEST_THROWS(T{std::numeric_limits<basis_type>::max()} * T{basis_type{2}}, std::overflow_error);

    // min * 2 underflows
    BOOST_TEST_THROWS(T{std::numeric_limits<basis_type>::min()} * T{basis_type{2}}, std::underflow_error);

    // max * -2 underflows
    BOOST_TEST_THROWS(T{std::numeric_limits<basis_type>::max()} * T{basis_type{-2}}, std::underflow_error);

    // min * -2 overflows
    BOOST_TEST_THROWS(T{std::numeric_limits<basis_type>::min()} * T{basis_type{-2}}, std::overflow_error);
}

int main()
{
    test_valid_multiplication<i8>();
    test_valid_multiplication<i16>();
    test_valid_multiplication<i32>();
    test_valid_multiplication<i64>();
    test_valid_multiplication<i128>();

    test_positive_overflow<i8>();
    test_positive_overflow<i16>();
    test_positive_overflow<i32>();
    test_positive_overflow<i64>();
    test_positive_overflow<i128>();

    test_negative_overflow<i8>();
    test_negative_overflow<i16>();
    test_negative_overflow<i32>();
    test_negative_overflow<i64>();
    test_negative_overflow<i128>();

    test_multiply_by_zero<i8>();
    test_multiply_by_zero<i16>();
    test_multiply_by_zero<i32>();
    test_multiply_by_zero<i64>();
    test_multiply_by_zero<i128>();

    test_multiply_equals<i8>();
    test_multiply_equals<i16>();
    test_multiply_equals<i32>();
    test_multiply_equals<i64>();
    test_multiply_equals<i128>();

    test_boundary_cases<i8>();
    test_boundary_cases<i16>();
    test_boundary_cases<i32>();
    test_boundary_cases<i64>();
    test_boundary_cases<i128>();

    return boost::report_errors();
}

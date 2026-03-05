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

#endif

using namespace boost::safe_numbers;

inline std::mt19937_64 rng{42};
inline constexpr std::size_t N {1024};

// -----------------------------------------------
// Valid addition: values in the safe half-range
// -----------------------------------------------

template <typename T>
void test_valid_addition()
{
    using basis_type = detail::underlying_type_t<T>;

    // Use quarter-range to ensure no overflow when adding two values
    boost::random::uniform_int_distribution<basis_type> dist {static_cast<basis_type>(std::numeric_limits<basis_type>::min() / 2),
                                                              static_cast<basis_type>(std::numeric_limits<basis_type>::max() / 2)};

    for (std::size_t i {0}; i < N; ++i)
    {
        const auto lhs_value {dist(rng)};
        const auto rhs_value {dist(rng)};

        // For int8_t/int16_t, promotion to int means no overflow in the builtin addition
        T ref_value {};
        if constexpr (std::is_same_v<basis_type, std::int8_t> || std::is_same_v<basis_type, std::int16_t>)
        {
            ref_value = T{static_cast<basis_type>(static_cast<std::int32_t>(lhs_value) + static_cast<std::int32_t>(rhs_value))};
        }
        else
        {
            ref_value = T{static_cast<basis_type>(lhs_value + rhs_value)};
        }

        const T lhs {lhs_value};
        const T rhs {rhs_value};
        const auto res {lhs + rhs};

        BOOST_TEST(ref_value == res);
    }
}

// -----------------------------------------------
// Positive overflow: near-max + positive
// -----------------------------------------------

template <typename T>
void test_positive_overflow()
{
    using basis_type = detail::underlying_type_t<T>;
    boost::random::uniform_int_distribution<basis_type> dist {basis_type{2},
                                                              std::numeric_limits<basis_type>::max()};

    for (std::size_t i {0}; i < N; ++i)
    {
        constexpr auto lhs_value {static_cast<basis_type>(std::numeric_limits<basis_type>::max() - 1)};
        const auto rhs_value {dist(rng)};

        const T lhs {lhs_value};
        const T rhs {rhs_value};

        BOOST_TEST_THROWS(lhs + rhs, std::overflow_error);
    }
}

// -----------------------------------------------
// Negative overflow (underflow): near-min + negative
// -----------------------------------------------

template <typename T>
void test_negative_overflow()
{
    using basis_type = detail::underlying_type_t<T>;
    boost::random::uniform_int_distribution<basis_type> dist {std::numeric_limits<basis_type>::min(),
                                                              basis_type{-2}};

    for (std::size_t i {0}; i < N; ++i)
    {
        constexpr auto lhs_value {static_cast<basis_type>(std::numeric_limits<basis_type>::min() + 1)};
        const auto rhs_value {dist(rng)};

        const T lhs {lhs_value};
        const T rhs {rhs_value};

        BOOST_TEST_THROWS(lhs + rhs, std::underflow_error);
    }
}

// -----------------------------------------------
// Mixed-sign addition: should never overflow
// Positive + negative can never exceed the range
// -----------------------------------------------

template <typename T>
void test_mixed_sign_addition()
{
    using basis_type = detail::underlying_type_t<T>;
    boost::random::uniform_int_distribution<basis_type> pos_dist {basis_type{0},
                                                                   std::numeric_limits<basis_type>::max()};
    boost::random::uniform_int_distribution<basis_type> neg_dist {std::numeric_limits<basis_type>::min(),
                                                                   basis_type{-1}};

    for (std::size_t i {0}; i < N; ++i)
    {
        const auto pos_value {pos_dist(rng)};
        const auto neg_value {neg_dist(rng)};

        const T pos {pos_value};
        const T neg {neg_value};

        // This should never throw
        const auto res {pos + neg};

        // Verify the result
        T expected {};
        if constexpr (std::is_same_v<basis_type, std::int8_t> || std::is_same_v<basis_type, std::int16_t>)
        {
            expected = T{static_cast<basis_type>(static_cast<std::int32_t>(pos_value) + static_cast<std::int32_t>(neg_value))};
        }
        else
        {
            expected = T{static_cast<basis_type>(pos_value + neg_value)};
        }

        BOOST_TEST(res == expected);
    }
}

// -----------------------------------------------
// Compound assignment: operator+=
// -----------------------------------------------

template <typename T>
void test_plus_equals()
{
    using basis_type = detail::underlying_type_t<T>;

    // Use quarter-range to ensure no overflow
    boost::random::uniform_int_distribution<basis_type> dist {static_cast<basis_type>(std::numeric_limits<basis_type>::min() / 2),
                                                              static_cast<basis_type>(std::numeric_limits<basis_type>::max() / 2)};

    for (std::size_t i {0}; i < N; ++i)
    {
        const auto lhs_value {dist(rng)};
        const auto rhs_value {dist(rng)};

        const T lhs {lhs_value};
        const T rhs {rhs_value};

        // operator+ result for reference
        const auto expected {lhs + rhs};

        // operator+= should give the same result
        T result {lhs_value};
        result += rhs;
        BOOST_TEST(result == expected);
    }

    // += with overflow should throw
    T near_max {static_cast<basis_type>(std::numeric_limits<basis_type>::max() - 1)};
    BOOST_TEST_THROWS(near_max += T{basis_type{2}}, std::overflow_error);

    // += with underflow should throw
    T near_min {static_cast<basis_type>(std::numeric_limits<basis_type>::min() + 1)};
    BOOST_TEST_THROWS(near_min += T{basis_type{-2}}, std::underflow_error);

    // += with zero is identity
    T val {basis_type{42}};
    val += T{basis_type{0}};
    BOOST_TEST(val == T{basis_type{42}});
}

// -----------------------------------------------
// Specific boundary cases
// -----------------------------------------------

template <typename T>
void test_boundary_cases()
{
    using basis_type = detail::underlying_type_t<T>;

    // 0 + 0 = 0
    BOOST_TEST(T{basis_type{0}} + T{basis_type{0}} == T{basis_type{0}});

    // 1 + (-1) = 0
    BOOST_TEST(T{basis_type{1}} + T{basis_type{-1}} == T{basis_type{0}});

    // max + 0 = max
    BOOST_TEST(T{std::numeric_limits<basis_type>::max()} + T{basis_type{0}} == T{std::numeric_limits<basis_type>::max()});

    // min + 0 = min
    BOOST_TEST(T{std::numeric_limits<basis_type>::min()} + T{basis_type{0}} == T{std::numeric_limits<basis_type>::min()});

    // max + (-max) = 0
    BOOST_TEST(T{std::numeric_limits<basis_type>::max()} + T{static_cast<basis_type>(-std::numeric_limits<basis_type>::max())} == T{basis_type{0}});

    // max + 1 overflows
    BOOST_TEST_THROWS(T{std::numeric_limits<basis_type>::max()} + T{basis_type{1}}, std::overflow_error);

    // min + (-1) underflows
    BOOST_TEST_THROWS(T{std::numeric_limits<basis_type>::min()} + T{basis_type{-1}}, std::underflow_error);

    // max + min = -1 (should succeed)
    BOOST_TEST(T{std::numeric_limits<basis_type>::max()} + T{std::numeric_limits<basis_type>::min()} == T{basis_type{-1}});
}

int main()
{
    test_valid_addition<i8>();
    test_valid_addition<i16>();
    test_valid_addition<i32>();
    test_valid_addition<i64>();
    test_valid_addition<i128>();

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

    test_mixed_sign_addition<i8>();
    test_mixed_sign_addition<i16>();
    test_mixed_sign_addition<i32>();
    test_mixed_sign_addition<i64>();
    test_mixed_sign_addition<i128>();

    test_plus_equals<i8>();
    test_plus_equals<i16>();
    test_plus_equals<i32>();
    test_plus_equals<i64>();
    test_plus_equals<i128>();

    test_boundary_cases<i8>();
    test_boundary_cases<i16>();
    test_boundary_cases<i32>();
    test_boundary_cases<i64>();
    test_boundary_cases<i128>();

    return boost::report_errors();
}

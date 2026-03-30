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
// Valid modulo: random numerator % random non-zero denominator
// -----------------------------------------------

template <typename T>
void test_valid_modulo()
{
    using basis_type = detail::underlying_type_t<T>;

    boost::random::uniform_int_distribution<basis_type> dist {std::numeric_limits<basis_type>::min(),
                                                              std::numeric_limits<basis_type>::max()};
    boost::random::uniform_int_distribution<basis_type> nonzero_dist {basis_type{1},
                                                                       std::numeric_limits<basis_type>::max()};

    for (std::size_t i {0}; i < N; ++i)
    {
        const auto lhs_value {dist(rng)};
        auto rhs_value {nonzero_dist(rng)};

        // Randomly negate for full coverage
        if (dist(rng) < 0)
        {
            rhs_value = static_cast<basis_type>(-rhs_value);
        }

        // Skip min % -1 (tested separately)
        if (lhs_value == std::numeric_limits<basis_type>::min() && rhs_value == static_cast<basis_type>(-1))
        {
            continue;
        }

        // Defensive: skip if rhs is somehow zero (should not happen with nonzero_dist)
        if (rhs_value == basis_type{0})
        {
            continue;
        }

        // Compute reference value using raw modulo
        // For int128 on software platforms, to_words asserts non-zero on both operands,
        // so we handle zero dividend separately
        T ref_value {basis_type{0}};
        if (lhs_value == basis_type{0})
        {
            // 0 % x = 0 for any non-zero x; ref_value already set to 0
        }
        else if constexpr (std::is_same_v<basis_type, std::int8_t> || std::is_same_v<basis_type, std::int16_t>)
        {
            ref_value = T{static_cast<basis_type>(static_cast<std::int32_t>(lhs_value) % static_cast<std::int32_t>(rhs_value))};
        }
        else
        {
            ref_value = T{static_cast<basis_type>(lhs_value % rhs_value)};
        }

        const T lhs {lhs_value};
        const T rhs {rhs_value};
        const auto res {lhs % rhs};

        BOOST_TEST(ref_value == res);
    }
}

// -----------------------------------------------
// Modulo by zero: should throw domain_error
// -----------------------------------------------

template <typename T>
void test_modulo_by_zero()
{
    using basis_type = detail::underlying_type_t<T>;

    boost::random::uniform_int_distribution<basis_type> dist {std::numeric_limits<basis_type>::min(),
                                                              std::numeric_limits<basis_type>::max()};

    for (std::size_t i {0}; i < N; ++i)
    {
        const auto lhs_value {dist(rng)};
        const T lhs {lhs_value};
        const T zero {basis_type{0}};

        BOOST_TEST_THROWS(lhs % zero, std::domain_error);
    }
}

// -----------------------------------------------
// min % -1 overflow
// -----------------------------------------------

template <typename T>
void test_min_mod_neg1_overflow()
{
    using basis_type = detail::underlying_type_t<T>;

    const T min_val {std::numeric_limits<basis_type>::min()};
    const T neg_one {basis_type{-1}};

    BOOST_TEST_THROWS(min_val % neg_one, std::overflow_error);
}

// -----------------------------------------------
// Compound assignment: operator%=
// -----------------------------------------------

template <typename T>
void test_modulo_equals()
{
    using basis_type = detail::underlying_type_t<T>;

    boost::random::uniform_int_distribution<basis_type> dist {std::numeric_limits<basis_type>::min(),
                                                              std::numeric_limits<basis_type>::max()};
    boost::random::uniform_int_distribution<basis_type> nonzero_dist {basis_type{1},
                                                                       std::numeric_limits<basis_type>::max()};

    for (std::size_t i {0}; i < N; ++i)
    {
        const auto lhs_value {dist(rng)};
        auto rhs_value {nonzero_dist(rng)};

        if (dist(rng) < 0)
        {
            rhs_value = static_cast<basis_type>(-rhs_value);
        }

        // Skip min % -1
        if (lhs_value == std::numeric_limits<basis_type>::min() && rhs_value == static_cast<basis_type>(-1))
        {
            continue;
        }

        // Defensive: skip zero rhs
        if (rhs_value == basis_type{0})
        {
            continue;
        }

        const T lhs {lhs_value};
        const T rhs {rhs_value};

        const auto expected {lhs % rhs};

        T result {lhs_value};
        result %= rhs;
        BOOST_TEST(result == expected);
    }

    // %= with zero should throw
    T val {basis_type{42}};
    BOOST_TEST_THROWS(val %= T{basis_type{0}}, std::domain_error);

    // %= with min % -1 should throw
    T min_val {std::numeric_limits<basis_type>::min()};
    BOOST_TEST_THROWS(min_val %= T{basis_type{-1}}, std::overflow_error);

    // %= with one gives zero
    T val2 {basis_type{42}};
    val2 %= T{basis_type{1}};
    BOOST_TEST(val2 == T{basis_type{0}});
}

// -----------------------------------------------
// Specific boundary cases
// -----------------------------------------------

template <typename T>
void test_boundary_cases()
{
    using basis_type = detail::underlying_type_t<T>;

    // 0 % x = 0 (for any non-zero x)
    BOOST_TEST(T{basis_type{0}} % T{basis_type{1}} == T{basis_type{0}});
    BOOST_TEST(T{basis_type{0}} % T{basis_type{-1}} == T{basis_type{0}});
    BOOST_TEST(T{basis_type{0}} % T{std::numeric_limits<basis_type>::max()} == T{basis_type{0}});
    BOOST_TEST(T{basis_type{0}} % T{std::numeric_limits<basis_type>::min()} == T{basis_type{0}});

    // x % 1 = 0
    BOOST_TEST(T{basis_type{42}} % T{basis_type{1}} == T{basis_type{0}});
    BOOST_TEST(T{basis_type{-42}} % T{basis_type{1}} == T{basis_type{0}});
    BOOST_TEST(T{std::numeric_limits<basis_type>::max()} % T{basis_type{1}} == T{basis_type{0}});
    BOOST_TEST(T{std::numeric_limits<basis_type>::min()} % T{basis_type{1}} == T{basis_type{0}});

    // x % x = 0
    BOOST_TEST(T{basis_type{42}} % T{basis_type{42}} == T{basis_type{0}});
    BOOST_TEST(T{basis_type{-42}} % T{basis_type{-42}} == T{basis_type{0}});
    BOOST_TEST(T{std::numeric_limits<basis_type>::max()} % T{std::numeric_limits<basis_type>::max()} == T{basis_type{0}});
    BOOST_TEST(T{std::numeric_limits<basis_type>::min()} % T{std::numeric_limits<basis_type>::min()} == T{basis_type{0}});

    // x % -x = 0
    BOOST_TEST(T{basis_type{42}} % T{basis_type{-42}} == T{basis_type{0}});

    // Truncation toward zero (C++ semantics: result has same sign as dividend)
    BOOST_TEST(T{basis_type{7}} % T{basis_type{3}} == T{basis_type{1}});
    BOOST_TEST(T{basis_type{-7}} % T{basis_type{3}} == T{basis_type{-1}});
    BOOST_TEST(T{basis_type{7}} % T{basis_type{-3}} == T{basis_type{1}});
    BOOST_TEST(T{basis_type{-7}} % T{basis_type{-3}} == T{basis_type{-1}});

    // max % min: max = 2^(N-1)-1, min = -2^(N-1), so max % min = max (since |max| < |min|)
    BOOST_TEST(T{std::numeric_limits<basis_type>::max()} % T{std::numeric_limits<basis_type>::min()} == T{std::numeric_limits<basis_type>::max()});

    // min % max: min = -(max+1), so min % max = -1
    BOOST_TEST(T{std::numeric_limits<basis_type>::min()} % T{std::numeric_limits<basis_type>::max()} == T{basis_type{-1}});

    // 0 % 0 throws
    BOOST_TEST_THROWS(T{basis_type{0}} % T{basis_type{0}}, std::domain_error);

    // min % -1 throws
    BOOST_TEST_THROWS(T{std::numeric_limits<basis_type>::min()} % T{basis_type{-1}}, std::overflow_error);
}

int main()
{
    test_valid_modulo<i8>();
    test_valid_modulo<i16>();
    test_valid_modulo<i32>();
    test_valid_modulo<i64>();
    test_valid_modulo<i128>();

    test_modulo_by_zero<i8>();
    test_modulo_by_zero<i16>();
    test_modulo_by_zero<i32>();
    test_modulo_by_zero<i64>();
    test_modulo_by_zero<i128>();

    test_min_mod_neg1_overflow<i8>();
    test_min_mod_neg1_overflow<i16>();
    test_min_mod_neg1_overflow<i32>();
    test_min_mod_neg1_overflow<i64>();
    test_min_mod_neg1_overflow<i128>();

    test_modulo_equals<i8>();
    test_modulo_equals<i16>();
    test_modulo_equals<i32>();
    test_modulo_equals<i64>();
    test_modulo_equals<i128>();

    test_boundary_cases<i8>();
    test_boundary_cases<i16>();
    test_boundary_cases<i32>();
    test_boundary_cases<i64>();
    test_boundary_cases<i128>();

    return boost::report_errors();
}

// Copyright 2025 Matt Borland
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

// Ignore [[nodiscard]] on the test that we know are going to throw
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
#include <cstring>
#include <limits>
#include <type_traits>
#include <iostream>

#endif

using namespace boost::safe_numbers;

inline std::mt19937_64 rng{42};
inline constexpr std::size_t N {1024};

template <typename T>
void test_valid_addition()
{
    using basis_type = detail::underlying_type_t<T>;
    boost::random::uniform_int_distribution<basis_type> dist {static_cast<basis_type>(std::numeric_limits<basis_type>::min() / 2),
                                                              static_cast<basis_type>(std::numeric_limits<basis_type>::max() / 2)};

    for (std::size_t i {0}; i < N; ++i)
    {
        const auto lhs_value {dist(rng)};
        const auto rhs_value {dist(rng)};

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
        const T res {saturating_add(lhs, rhs)};

        BOOST_TEST_EQ(ref_value, res);
    }
}

template <typename T>
void test_saturated_overflow()
{
    using basis_type = detail::underlying_type_t<T>;
    boost::random::uniform_int_distribution<basis_type> dist {basis_type{2},
                                                              std::numeric_limits<basis_type>::max()};

    for (std::size_t i {0}; i < N; ++i)
    {
        constexpr T max_value {std::numeric_limits<basis_type>::max()};
        constexpr auto lhs_value {static_cast<basis_type>(std::numeric_limits<basis_type>::max() - 1)};
        const auto rhs_value {dist(rng)};

        const T lhs {lhs_value};
        const T rhs {rhs_value};
        const T res {saturating_add(lhs, rhs)};

        BOOST_TEST_EQ(res, max_value);
    }
}

template <typename T>
void test_saturated_underflow()
{
    using basis_type = detail::underlying_type_t<T>;
    boost::random::uniform_int_distribution<basis_type> dist {std::numeric_limits<basis_type>::min(),
                                                              basis_type{-2}};

    for (std::size_t i {0}; i < N; ++i)
    {
        constexpr T min_value {std::numeric_limits<basis_type>::min()};
        constexpr auto lhs_value {static_cast<basis_type>(std::numeric_limits<basis_type>::min() + 1)};
        const auto rhs_value {dist(rng)};

        const T lhs {lhs_value};
        const T rhs {rhs_value};
        const T res {saturating_add(lhs, rhs)};

        BOOST_TEST_EQ(res, min_value);
    }
}

int main()
{
    test_valid_addition<i8>();
    test_saturated_overflow<i8>();
    test_saturated_underflow<i8>();

    test_valid_addition<i16>();
    test_saturated_overflow<i16>();
    test_saturated_underflow<i16>();

    test_valid_addition<i32>();
    test_saturated_overflow<i32>();
    test_saturated_underflow<i32>();

    test_valid_addition<i64>();
    test_saturated_overflow<i64>();
    test_saturated_underflow<i64>();

    test_valid_addition<i128>();
    test_saturated_overflow<i128>();
    test_saturated_underflow<i128>();

    return boost::report_errors();
}

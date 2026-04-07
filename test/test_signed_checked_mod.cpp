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
void test_valid_mod()
{
    using basis_type = detail::underlying_type_t<T>;

    // Distribute over the full range but ensure non-zero divisor and skip min/-1
    boost::random::uniform_int_distribution<basis_type> lhs_dist {std::numeric_limits<basis_type>::min(),
                                                                   std::numeric_limits<basis_type>::max()};
    boost::random::uniform_int_distribution<basis_type> rhs_dist {basis_type{1},
                                                                   std::numeric_limits<basis_type>::max()};

    for (std::size_t i {0}; i < N; ++i)
    {
        const auto lhs_value {lhs_dist(rng)};
        const auto rhs_value {rhs_dist(rng)};

        // rhs_dist is [1, max] so min/-1 case cannot occur

        T ref_value {};
        if constexpr (std::is_same_v<basis_type, std::int8_t> || std::is_same_v<basis_type, std::int16_t>)
        {
            ref_value = T{static_cast<basis_type>(static_cast<std::int32_t>(lhs_value) % static_cast<std::int32_t>(rhs_value))};
        }
        else
        {
            ref_value = T{static_cast<basis_type>(lhs_value % rhs_value)};
        }

        const T lhs {lhs_value};
        const T rhs {rhs_value};
        const auto res {checked_mod(lhs, rhs)};

        BOOST_TEST(res.has_value());
        BOOST_TEST_EQ(ref_value, res.value());
    }
}

template <typename T>
void test_checked_mod_by_zero()
{
    using basis_type = detail::underlying_type_t<T>;
    boost::random::uniform_int_distribution<basis_type> dist {std::numeric_limits<basis_type>::min(),
                                                              std::numeric_limits<basis_type>::max()};

    for (std::size_t i {0}; i < N; ++i)
    {
        const auto lhs_value {dist(rng)};
        constexpr basis_type rhs_value {0};

        const T lhs {lhs_value};
        const T rhs {rhs_value};
        const auto res {checked_mod(lhs, rhs)};

        BOOST_TEST(!res.has_value());
    }
}

template <typename T>
void test_checked_min_mod_neg1()
{
    using basis_type = detail::underlying_type_t<T>;

    constexpr auto lhs_value {std::numeric_limits<basis_type>::min()};
    constexpr basis_type rhs_value {-1};

    const T lhs {lhs_value};
    const T rhs {rhs_value};
    const auto res {checked_mod(lhs, rhs)};

    // min % -1 implies a division that overflows in two's complement, checked returns nullopt
    BOOST_TEST(!res.has_value());
}

int main()
{
    test_valid_mod<i8>();
    test_checked_mod_by_zero<i8>();
    test_checked_min_mod_neg1<i8>();

    test_valid_mod<i16>();
    test_checked_mod_by_zero<i16>();
    test_checked_min_mod_neg1<i16>();

    test_valid_mod<i32>();
    test_checked_mod_by_zero<i32>();
    test_checked_min_mod_neg1<i32>();

    test_valid_mod<i64>();
    test_checked_mod_by_zero<i64>();
    test_checked_min_mod_neg1<i64>();

    test_valid_mod<i128>();
    test_checked_mod_by_zero<i128>();
    test_checked_min_mod_neg1<i128>();

    return boost::report_errors();
}

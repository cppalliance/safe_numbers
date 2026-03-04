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

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>
#include <random>
#include <cstring>
#include <limits>
#include <type_traits>
#include <compare>

#endif

using namespace boost::safe_numbers;

inline std::mt19937_64 rng{42};
inline constexpr std::size_t N {1024};

template <typename T>
void test()
{
    using basis_type = detail::underlying_type_t<T>;

    boost::random::uniform_int_distribution<basis_type> distribution(std::numeric_limits<basis_type>::min(),
                                                                     std::numeric_limits<basis_type>::max());

    for (std::size_t i {0}; i < N; ++i)
    {
        const auto lhs {distribution(rng)};
        const auto rhs {distribution(rng)};

        const T lib_lhs {lhs};
        const T lib_rhs {rhs};

        BOOST_TEST((lhs > rhs) == (lib_lhs > lib_rhs));
        BOOST_TEST((lhs >= rhs) == (lib_lhs >= lib_rhs));
        BOOST_TEST((lhs == rhs) == (lib_lhs == lib_rhs));
        BOOST_TEST((lhs != rhs) == (lib_lhs != lib_rhs));
        BOOST_TEST((lhs <= rhs) == (lib_lhs <= lib_rhs));
        BOOST_TEST((lhs < rhs) == (lib_lhs < lib_rhs));
        BOOST_TEST((lhs <=> rhs) == (lib_lhs <=> lib_rhs));
    }
}

int main()
{
    test<i8>();
    test<i16>();
    test<i32>();
    test<i64>();
    test<i128>();

    return boost::report_errors();
}

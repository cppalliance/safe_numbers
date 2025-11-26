// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers.hpp>
#include <boost/core/lightweight_test.hpp>

// Ignore [[nodiscard]] on the test that we know are going to throw
#ifdef __GNUC__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wunused-result"
#elif defined(_MSC_VER)
#  pragma warning (push)
#  pragma warning (disable: 4834)
#endif

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

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
    std::uniform_int_distribution<basis_type> dist {std::numeric_limits<basis_type>::min() / 2U,
                                                    std::numeric_limits<basis_type>::max() / 2U};

    for (std::size_t i = 0; i < N; ++i)
    {
        const auto lhs_value {dist(rng)};
        const auto rhs_value {dist(rng)};
        const T ref_value {lhs_value + rhs_value};

        const T lhs {lhs_value};
        const T rhs {rhs_value};
        const T res {lhs + rhs};

        BOOST_TEST(ref_value == res);
    }
}

template <typename T>
void test_throwing_addition()
{
    using basis_type = detail::underlying_type_t<T>;
    std::uniform_int_distribution<basis_type> dist {2U, std::numeric_limits<basis_type>::max()};

    for (std::size_t i = 0; i < N; ++i)
    {
        constexpr basis_type lhs_value {std::numeric_limits<basis_type>::max() - 1U};
        const auto rhs_value {dist(rng)};

        const T lhs {lhs_value};
        const T rhs {rhs_value};

        BOOST_TEST_THROWS(lhs + rhs, std::overflow_error);
    }
}

int main()
{
    test_valid_addition<u32>();
    test_throwing_addition<u32>();

    test_valid_addition<u64>();
    test_throwing_addition<u64>();

    return boost::report_errors();
}

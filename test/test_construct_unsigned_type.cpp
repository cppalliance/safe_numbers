// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers/unsigned_integers.hpp>
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

#include <random>
#include <cstring>
#include <limits>
#include <type_traits>

#endif

inline std::mt19937_64 rng{42};
inline constexpr std::size_t N {1024};

template <typename T, typename BasisType>
void test()
{
    boost::random::uniform_int_distribution<BasisType> dist {std::numeric_limits<BasisType>::min(),
                                                             std::numeric_limits<BasisType>::max()};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto basis_value {dist(rng)};
        const T value {basis_value};

        BasisType bits;
        std::memcpy(&bits, &value, sizeof(T));

        BOOST_TEST_EQ(basis_value, bits);
    }
}

int main()
{
    using namespace boost::safe_numbers;

    test<u8, std::uint8_t>();
    test<u16, std::uint16_t>();
    test<u32, std::uint32_t>();
    test<u64, std::uint64_t>();

    return boost::report_errors();
}

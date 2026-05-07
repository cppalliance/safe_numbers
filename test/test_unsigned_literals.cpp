// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>

#endif

#include <boost/core/lightweight_test.hpp>

using namespace boost::safe_numbers;
using namespace boost::safe_numbers::literals;

template <typename T>
void test();

template <>
void test<u8>()
{
    const auto good_value {42_u8};
    BOOST_TEST_EQ(good_value, u8{42u});
    BOOST_TEST_THROWS(18446744073709551615_u8, std::overflow_error);
}

template <>
void test<u16>()
{
    constexpr auto good_value {42_u16};
    BOOST_TEST_EQ(good_value, u16{42u});
    BOOST_TEST_THROWS(18446744073709551615_u16, std::overflow_error);
}

template <>
void test<u32>()
{
    constexpr auto good_value {42_u32};
    BOOST_TEST_EQ(good_value, u32{42u});
    BOOST_TEST_THROWS(18446744073709551615_u32, std::overflow_error);
}

template <>
void test<u64>()
{
    constexpr auto good_value {42_u64};
    BOOST_TEST_EQ(good_value, u64{42u});
    BOOST_TEST_NO_THROW(BOOST_TEST_EQ(18446744073709551615_u64, u64{18446744073709551615ULL}));
}

template <>
void test<u128>()
{
    constexpr auto good_value {42_u128};
    BOOST_TEST_EQ(good_value, u128{42u});

    // UINT128_MAX itself is representable.
    constexpr auto max_value {340282366920938463463374607431768211455_u128};
    static_cast<void>(max_value);

    // UINT128_MAX+1 (2^128) must overflow. Regression test for from_chars
    // overflow detection at the exact boundary.
    BOOST_TEST_THROWS(340282366920938463463374607431768211456_u128, std::overflow_error);
    BOOST_TEST_THROWS(18446744073709551615184467440737095516151844674407370955161518446744073709551615_u128, std::overflow_error);
}

int main()
{
    test<u8>();
    test<u16>();
    test<u32>();
    test<u64>();
    test<u128>();

    return boost::report_errors();
}

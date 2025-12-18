// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers.hpp>
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
    const auto good_value {42_u16};
    BOOST_TEST_EQ(good_value, u16{42u});
    BOOST_TEST_THROWS(18446744073709551615_u16, std::overflow_error);
}

template <>
void test<u32>()
{
    const auto good_value {42_u32};
    BOOST_TEST_EQ(good_value, u32{42u});
    BOOST_TEST_THROWS(18446744073709551615_u32, std::overflow_error);
}

template <>
void test<u64>()
{
    const auto good_value {42_u64};
    BOOST_TEST_EQ(good_value, u64{42u});
    BOOST_TEST_NO_THROW(BOOST_TEST_EQ(18446744073709551615_u64, u64{18446744073709551615ULL}));
}

int main()
{
    test<u8>();
    test<u16>();
    test<u32>();
    test<u64>();

    return boost::report_errors();
}

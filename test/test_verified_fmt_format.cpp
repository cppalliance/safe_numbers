// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wfloat-equal"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wfloat-equal"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#  pragma GCC diagnostic ignored "-Wconversion"
#endif

#define FMT_HEADER_ONLY

#if __has_include(<fmt/format.h>)

#include <boost/core/lightweight_test.hpp>
#include <boost/safe_numbers.hpp>
#include <boost/safe_numbers/fmt_format.hpp>
#include <fmt/format.h>

using namespace boost::safe_numbers;

template <typename VerifiedT, typename BasisT>
void test()
{
    constexpr auto x = VerifiedT{BasisT{42}};

    BOOST_TEST_CSTR_EQ(fmt::format("{}", x).c_str(), "42");
    BOOST_TEST_CSTR_EQ(fmt::format("{:08x}", x).c_str(), "0000002a");
    BOOST_TEST_CSTR_EQ(fmt::format("{:#010b}", x).c_str(), "0b00101010");
}

template <typename VerifiedT, typename BasisT>
void test_zero()
{
    constexpr auto x = VerifiedT{BasisT{0}};

    BOOST_TEST_CSTR_EQ(fmt::format("{}", x).c_str(), "0");
}

template <typename VerifiedT, typename BasisT>
void test_255()
{
    constexpr auto x = VerifiedT{BasisT{255}};

    BOOST_TEST_CSTR_EQ(fmt::format("{}", x).c_str(), "255");
    BOOST_TEST_CSTR_EQ(fmt::format("{:x}", x).c_str(), "ff");
}

int main()
{
    test<verified_u8, u8>();
    test<verified_u16, u16>();
    test<verified_u32, u32>();
    test<verified_u64, u64>();
    test<verified_u128, u128>();

    test_zero<verified_u8, u8>();
    test_zero<verified_u16, u16>();
    test_zero<verified_u32, u32>();
    test_zero<verified_u64, u64>();
    test_zero<verified_u128, u128>();

    test_255<verified_u8, u8>();
    test_255<verified_u16, u16>();
    test_255<verified_u32, u32>();
    test_255<verified_u64, u64>();
    test_255<verified_u128, u128>();

    // Bounded verified type
    using test_bounded = bounded_uint<0u, 255u>;
    using test_verified_bounded = verified_bounded_integer<0u, 255u>;

    test<test_verified_bounded, test_bounded>();
    test_zero<test_verified_bounded, test_bounded>();
    test_255<test_verified_bounded, test_bounded>();

    return boost::report_errors();
}

#else

int main()
{
    return 0;
}

#endif

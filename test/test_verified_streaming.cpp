// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers.hpp>
#include <boost/core/lightweight_test.hpp>
#include <sstream>

using namespace boost::safe_numbers;

// Verified types are consteval for construction, so only output streaming
// (operator<<) is testable. Input streaming (operator>>) would require
// runtime construction which is not possible with consteval constructors.

template <typename VerifiedT, typename BasisT>
void test_output()
{
    constexpr auto val = VerifiedT{BasisT{42}};

    std::stringstream out;
    out << val;

    BOOST_TEST_CSTR_EQ(out.str().c_str(), "42");
}

template <typename VerifiedT, typename BasisT>
void test_output_zero()
{
    constexpr auto val = VerifiedT{BasisT{0}};

    std::stringstream out;
    out << val;

    BOOST_TEST_CSTR_EQ(out.str().c_str(), "0");
}

template <typename VerifiedT, typename BasisT>
void test_output_large()
{
    constexpr auto val = VerifiedT{BasisT{255}};

    std::stringstream out;
    out << val;

    BOOST_TEST_CSTR_EQ(out.str().c_str(), "255");
}

int main()
{
    test_output<verified_u8, u8>();
    test_output<verified_u16, u16>();
    test_output<verified_u32, u32>();
    test_output<verified_u64, u64>();
    test_output<verified_u128, u128>();

    test_output_zero<verified_u8, u8>();
    test_output_zero<verified_u16, u16>();
    test_output_zero<verified_u32, u32>();
    test_output_zero<verified_u64, u64>();
    test_output_zero<verified_u128, u128>();

    test_output_large<verified_u8, u8>();
    test_output_large<verified_u16, u16>();
    test_output_large<verified_u32, u32>();
    test_output_large<verified_u64, u64>();
    test_output_large<verified_u128, u128>();

    // Bounded verified type
    using test_bounded = bounded_uint<0u, 255u>;
    using test_verified_bounded = verified_bounded_integer<0u, 255u>;

    test_output<test_verified_bounded, test_bounded>();
    test_output_zero<test_verified_bounded, test_bounded>();
    test_output_large<test_verified_bounded, test_bounded>();

    return boost::report_errors();
}

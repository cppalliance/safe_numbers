// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>
#include <system_error>
#include <cstddef>

#endif

#include <boost/core/lightweight_test.hpp>

using namespace boost::safe_numbers;

// =============================================================================
// to_chars tests (constexpr - runtime testable with verified types)
// =============================================================================

template <typename VerifiedT, typename BasisT>
void test_to_chars()
{
    constexpr auto val = VerifiedT{BasisT{42}};

    char buffer[256];
    const auto r = boost::safe_numbers::to_chars(buffer, buffer + sizeof(buffer), val);
    BOOST_TEST(r.ec == std::errc{});

    const auto len = static_cast<std::size_t>(r.ptr - buffer);
    BOOST_TEST_EQ(len, std::size_t{2});
    BOOST_TEST_EQ(buffer[0], '4');
    BOOST_TEST_EQ(buffer[1], '2');
}

template <typename VerifiedT, typename BasisT>
void test_to_chars_zero()
{
    constexpr auto val = VerifiedT{BasisT{0}};

    char buffer[256];
    const auto r = boost::safe_numbers::to_chars(buffer, buffer + sizeof(buffer), val);
    BOOST_TEST(r.ec == std::errc{});

    const auto len = static_cast<std::size_t>(r.ptr - buffer);
    BOOST_TEST_EQ(len, std::size_t{1});
    BOOST_TEST_EQ(buffer[0], '0');
}

template <typename VerifiedT, typename BasisT>
void test_to_chars_255()
{
    constexpr auto val = VerifiedT{BasisT{255}};

    char buffer[256];
    const auto r = boost::safe_numbers::to_chars(buffer, buffer + sizeof(buffer), val);
    BOOST_TEST(r.ec == std::errc{});

    const auto len = static_cast<std::size_t>(r.ptr - buffer);
    BOOST_TEST_EQ(len, std::size_t{3});
    BOOST_TEST_EQ(buffer[0], '2');
    BOOST_TEST_EQ(buffer[1], '5');
    BOOST_TEST_EQ(buffer[2], '5');
}

template <typename VerifiedT, typename BasisT>
void test_to_chars_hex()
{
    constexpr auto val = VerifiedT{BasisT{255}};

    char buffer[256];
    const auto r = boost::safe_numbers::to_chars(buffer, buffer + sizeof(buffer), val, 16);
    BOOST_TEST(r.ec == std::errc{});

    const auto len = static_cast<std::size_t>(r.ptr - buffer);
    BOOST_TEST_EQ(len, std::size_t{2});
    BOOST_TEST_EQ(buffer[0], 'f');
    BOOST_TEST_EQ(buffer[1], 'f');
}

// =============================================================================
// from_chars tests (consteval - compile-time only for verified types)
// =============================================================================

template <typename VerifiedT, typename BasisT>
consteval auto test_from_chars_impl() -> bool
{
    const char str[] = "42";
    auto val = VerifiedT{BasisT{0}};
    const auto r = boost::safe_numbers::from_chars(str, str + 2, val);
    return r.ec == std::errc{} && val == VerifiedT{BasisT{42}};
}

template <typename VerifiedT, typename BasisT>
consteval auto test_from_chars_zero_impl() -> bool
{
    const char str[] = "0";
    auto val = VerifiedT{BasisT{1}};
    const auto r = boost::safe_numbers::from_chars(str, str + 1, val);
    return r.ec == std::errc{} && val == VerifiedT{BasisT{0}};
}

template <typename VerifiedT, typename BasisT>
consteval auto test_from_chars_255_impl() -> bool
{
    const char str[] = "255";
    auto val = VerifiedT{BasisT{0}};
    const auto r = boost::safe_numbers::from_chars(str, str + 3, val);
    return r.ec == std::errc{} && val == VerifiedT{BasisT{255}};
}

template <typename VerifiedT, typename BasisT>
consteval auto test_from_chars_hex_impl() -> bool
{
    const char str[] = "ff";
    auto val = VerifiedT{BasisT{0}};
    const auto r = boost::safe_numbers::from_chars(str, str + 2, val, 16);
    return r.ec == std::errc{} && val == VerifiedT{BasisT{255}};
}

// --- from_chars base 10 ---
static_assert(test_from_chars_impl<verified_u8, u8>());
static_assert(test_from_chars_impl<verified_u16, u16>());
static_assert(test_from_chars_impl<verified_u32, u32>());
static_assert(test_from_chars_impl<verified_u64, u64>());
static_assert(test_from_chars_impl<verified_u128, u128>());

// --- from_chars zero ---
static_assert(test_from_chars_zero_impl<verified_u8, u8>());
static_assert(test_from_chars_zero_impl<verified_u16, u16>());
static_assert(test_from_chars_zero_impl<verified_u32, u32>());
static_assert(test_from_chars_zero_impl<verified_u64, u64>());
static_assert(test_from_chars_zero_impl<verified_u128, u128>());

// --- from_chars 255 ---
static_assert(test_from_chars_255_impl<verified_u8, u8>());
static_assert(test_from_chars_255_impl<verified_u16, u16>());
static_assert(test_from_chars_255_impl<verified_u32, u32>());
static_assert(test_from_chars_255_impl<verified_u64, u64>());
static_assert(test_from_chars_255_impl<verified_u128, u128>());

// --- from_chars hex ---
static_assert(test_from_chars_hex_impl<verified_u8, u8>());
static_assert(test_from_chars_hex_impl<verified_u16, u16>());
static_assert(test_from_chars_hex_impl<verified_u32, u32>());
static_assert(test_from_chars_hex_impl<verified_u64, u64>());
static_assert(test_from_chars_hex_impl<verified_u128, u128>());

// --- from_chars with bounded type ---
using test_bounded = bounded_uint<0u, 255u>;
using test_verified_bounded = verified_bounded_integer<0u, 255u>;

static_assert(test_from_chars_impl<test_verified_bounded, test_bounded>());
static_assert(test_from_chars_zero_impl<test_verified_bounded, test_bounded>());
static_assert(test_from_chars_255_impl<test_verified_bounded, test_bounded>());
static_assert(test_from_chars_hex_impl<test_verified_bounded, test_bounded>());

// =============================================================================
// Main - runtime tests for to_chars
// =============================================================================

int main()
{
    test_to_chars<verified_u8, u8>();
    test_to_chars<verified_u16, u16>();
    test_to_chars<verified_u32, u32>();
    test_to_chars<verified_u64, u64>();
    test_to_chars<verified_u128, u128>();

    test_to_chars_zero<verified_u8, u8>();
    test_to_chars_zero<verified_u16, u16>();
    test_to_chars_zero<verified_u32, u32>();
    test_to_chars_zero<verified_u64, u64>();
    test_to_chars_zero<verified_u128, u128>();

    test_to_chars_255<verified_u8, u8>();
    test_to_chars_255<verified_u16, u16>();
    test_to_chars_255<verified_u32, u32>();
    test_to_chars_255<verified_u64, u64>();
    test_to_chars_255<verified_u128, u128>();

    test_to_chars_hex<verified_u8, u8>();
    test_to_chars_hex<verified_u16, u16>();
    test_to_chars_hex<verified_u32, u32>();
    test_to_chars_hex<verified_u64, u64>();
    test_to_chars_hex<verified_u128, u128>();

    // Bounded verified types
    test_to_chars<test_verified_bounded, test_bounded>();
    test_to_chars_zero<test_verified_bounded, test_bounded>();
    test_to_chars_255<test_verified_bounded, test_bounded>();
    test_to_chars_hex<test_verified_bounded, test_bounded>();

    return boost::report_errors();
}

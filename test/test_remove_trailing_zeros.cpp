// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>
#include <cstdint>

#endif

#include <boost/core/lightweight_test.hpp>

using namespace boost::safe_numbers;

// =============================================================================
// Runtime tests for all bit widths
// =============================================================================

template <typename T>
void test_rtz_no_trailing_zeros()
{
    using underlying = typename detail::underlying_type_t<T>;

    {
        auto result = remove_trailing_zeros(T{static_cast<underlying>(1)});
        BOOST_TEST_EQ(result.trimmed_number, static_cast<underlying>(1));
        BOOST_TEST_EQ(result.number_of_removed_zeros, static_cast<std::size_t>(0));
    }
    {
        auto result = remove_trailing_zeros(T{static_cast<underlying>(7)});
        BOOST_TEST_EQ(result.trimmed_number, static_cast<underlying>(7));
        BOOST_TEST_EQ(result.number_of_removed_zeros, static_cast<std::size_t>(0));
    }
    {
        auto result = remove_trailing_zeros(T{static_cast<underlying>(13)});
        BOOST_TEST_EQ(result.trimmed_number, static_cast<underlying>(13));
        BOOST_TEST_EQ(result.number_of_removed_zeros, static_cast<std::size_t>(0));
    }
    {
        auto result = remove_trailing_zeros(T{static_cast<underlying>(99)});
        BOOST_TEST_EQ(result.trimmed_number, static_cast<underlying>(99));
        BOOST_TEST_EQ(result.number_of_removed_zeros, static_cast<std::size_t>(0));
    }
}

template <typename T>
void test_rtz_one_trailing_zero()
{
    using underlying = typename detail::underlying_type_t<T>;

    {
        auto result = remove_trailing_zeros(T{static_cast<underlying>(10)});
        BOOST_TEST_EQ(result.trimmed_number, static_cast<underlying>(1));
        BOOST_TEST_EQ(result.number_of_removed_zeros, static_cast<std::size_t>(1));
    }
    {
        auto result = remove_trailing_zeros(T{static_cast<underlying>(50)});
        BOOST_TEST_EQ(result.trimmed_number, static_cast<underlying>(5));
        BOOST_TEST_EQ(result.number_of_removed_zeros, static_cast<std::size_t>(1));
    }
    {
        auto result = remove_trailing_zeros(T{static_cast<underlying>(90)});
        BOOST_TEST_EQ(result.trimmed_number, static_cast<underlying>(9));
        BOOST_TEST_EQ(result.number_of_removed_zeros, static_cast<std::size_t>(1));
    }
}

template <typename T>
void test_rtz_two_trailing_zeros()
{
    using underlying = typename detail::underlying_type_t<T>;

    {
        auto result = remove_trailing_zeros(T{static_cast<underlying>(100)});
        BOOST_TEST_EQ(result.trimmed_number, static_cast<underlying>(1));
        BOOST_TEST_EQ(result.number_of_removed_zeros, static_cast<std::size_t>(2));
    }
    {
        auto result = remove_trailing_zeros(T{static_cast<underlying>(200)});
        BOOST_TEST_EQ(result.trimmed_number, static_cast<underlying>(2));
        BOOST_TEST_EQ(result.number_of_removed_zeros, static_cast<std::size_t>(2));
    }
}

// =============================================================================
// Type-specific tests for larger trailing zero counts
// =============================================================================

void test_rtz_u16_large()
{
    {
        auto result = remove_trailing_zeros(u16{static_cast<std::uint16_t>(1000)});
        BOOST_TEST_EQ(result.trimmed_number, static_cast<std::uint16_t>(1));
        BOOST_TEST_EQ(result.number_of_removed_zeros, static_cast<std::size_t>(3));
    }
    {
        auto result = remove_trailing_zeros(u16{static_cast<std::uint16_t>(10000)});
        BOOST_TEST_EQ(result.trimmed_number, static_cast<std::uint16_t>(1));
        BOOST_TEST_EQ(result.number_of_removed_zeros, static_cast<std::size_t>(4));
    }
    {
        auto result = remove_trailing_zeros(u16{static_cast<std::uint16_t>(30000)});
        BOOST_TEST_EQ(result.trimmed_number, static_cast<std::uint16_t>(3));
        BOOST_TEST_EQ(result.number_of_removed_zeros, static_cast<std::size_t>(4));
    }
    {
        auto result = remove_trailing_zeros(u16{static_cast<std::uint16_t>(60000)});
        BOOST_TEST_EQ(result.trimmed_number, static_cast<std::uint16_t>(6));
        BOOST_TEST_EQ(result.number_of_removed_zeros, static_cast<std::size_t>(4));
    }
}

void test_rtz_u32_large()
{
    {
        auto result = remove_trailing_zeros(u32{UINT32_C(1000000)});
        BOOST_TEST_EQ(result.trimmed_number, UINT32_C(1));
        BOOST_TEST_EQ(result.number_of_removed_zeros, static_cast<std::size_t>(6));
    }
    {
        auto result = remove_trailing_zeros(u32{UINT32_C(4000000000)});
        BOOST_TEST_EQ(result.trimmed_number, UINT32_C(4));
        BOOST_TEST_EQ(result.number_of_removed_zeros, static_cast<std::size_t>(9));
    }
    {
        auto result = remove_trailing_zeros(u32{UINT32_C(123000)});
        BOOST_TEST_EQ(result.trimmed_number, UINT32_C(123));
        BOOST_TEST_EQ(result.number_of_removed_zeros, static_cast<std::size_t>(3));
    }
}

void test_rtz_u64_large()
{
    {
        auto result = remove_trailing_zeros(u64{UINT64_C(1000000000000)});
        BOOST_TEST_EQ(result.trimmed_number, UINT64_C(1));
        BOOST_TEST_EQ(result.number_of_removed_zeros, static_cast<std::size_t>(12));
    }
    {
        auto result = remove_trailing_zeros(u64{UINT64_C(10000000000000000)});
        BOOST_TEST_EQ(result.trimmed_number, UINT64_C(1));
        BOOST_TEST_EQ(result.number_of_removed_zeros, static_cast<std::size_t>(16));
    }
    {
        auto result = remove_trailing_zeros(u64{UINT64_C(123456789000)});
        BOOST_TEST_EQ(result.trimmed_number, UINT64_C(123456789));
        BOOST_TEST_EQ(result.number_of_removed_zeros, static_cast<std::size_t>(3));
    }
}

void test_rtz_u128_large()
{
    using boost::int128::uint128_t;

    {
        auto result = remove_trailing_zeros(u128{uint128_t{UINT64_C(1000000000000)}});
        BOOST_TEST_EQ(result.trimmed_number, uint128_t{1});
        BOOST_TEST_EQ(result.number_of_removed_zeros, static_cast<std::size_t>(12));
    }
    {
        // 10^20 = 10^10 * 10^10
        const auto ten_pow_20 = uint128_t{UINT64_C(10000000000)} * uint128_t{UINT64_C(10000000000)};
        auto result = remove_trailing_zeros(u128{ten_pow_20});
        BOOST_TEST_EQ(result.trimmed_number, uint128_t{1});
        BOOST_TEST_EQ(result.number_of_removed_zeros, static_cast<std::size_t>(20));
    }
    {
        // 42 * 10^10
        const auto val = uint128_t{42} * uint128_t{UINT64_C(10000000000)};
        auto result = remove_trailing_zeros(u128{val});
        BOOST_TEST_EQ(result.trimmed_number, uint128_t{42});
        BOOST_TEST_EQ(result.number_of_removed_zeros, static_cast<std::size_t>(10));
    }
}

// =============================================================================
// Exhaustive tests for u8 and u16
// =============================================================================

void test_rtz_exhaustive_u8()
{
    for (unsigned i {1}; i <= 255; ++i)
    {
        const auto n = static_cast<std::uint8_t>(i);
        auto result = remove_trailing_zeros(u8{n});

        auto expected {i};
        std::size_t ez {};
        while (expected > 0 && expected % 10 == 0)
        {
            expected /= 10;
            ++ez;
        }

        BOOST_TEST_EQ(result.trimmed_number, static_cast<std::uint8_t>(expected));
        BOOST_TEST_EQ(result.number_of_removed_zeros, ez);
    }
}

void test_rtz_exhaustive_u16()
{
    for (unsigned i {1}; i <= 65535; ++i)
    {
        const auto n = static_cast<std::uint16_t>(i);
        auto result = remove_trailing_zeros(u16{n});

        auto expected {i};
        std::size_t ez {};
        while (expected > 0 && expected % 10 == 0)
        {
            expected /= 10;
            ++ez;
        }

        BOOST_TEST_EQ(result.trimmed_number, static_cast<std::uint16_t>(expected));
        BOOST_TEST_EQ(result.number_of_removed_zeros, ez);
    }
}

// =============================================================================
// Constexpr tests
// =============================================================================

void test_rtz_constexpr()
{
    constexpr auto r8 = remove_trailing_zeros(u8{static_cast<std::uint8_t>(100)});
    BOOST_TEST_EQ(r8.trimmed_number, static_cast<std::uint8_t>(1));
    BOOST_TEST_EQ(r8.number_of_removed_zeros, static_cast<std::size_t>(2));

    constexpr auto r16 = remove_trailing_zeros(u16{static_cast<std::uint16_t>(10000)});
    BOOST_TEST_EQ(r16.trimmed_number, static_cast<std::uint16_t>(1));
    BOOST_TEST_EQ(r16.number_of_removed_zeros, static_cast<std::size_t>(4));

    constexpr auto r32 = remove_trailing_zeros(u32{UINT32_C(1000)});
    BOOST_TEST_EQ(r32.trimmed_number, UINT32_C(1));
    BOOST_TEST_EQ(r32.number_of_removed_zeros, static_cast<std::size_t>(3));

    constexpr auto r64 = remove_trailing_zeros(u64{UINT64_C(5000000)});
    BOOST_TEST_EQ(r64.trimmed_number, UINT64_C(5));
    BOOST_TEST_EQ(r64.number_of_removed_zeros, static_cast<std::size_t>(6));
}

// =============================================================================
// Verified type tests (consteval)
// =============================================================================

void test_rtz_verified()
{
    constexpr auto v8 = remove_trailing_zeros(verified_u8{u8{static_cast<std::uint8_t>(50)}});
    BOOST_TEST_EQ(v8.trimmed_number, static_cast<std::uint8_t>(5));
    BOOST_TEST_EQ(v8.number_of_removed_zeros, static_cast<std::size_t>(1));

    constexpr auto v16 = remove_trailing_zeros(verified_u16{u16{static_cast<std::uint16_t>(3000)}});
    BOOST_TEST_EQ(v16.trimmed_number, static_cast<std::uint16_t>(3));
    BOOST_TEST_EQ(v16.number_of_removed_zeros, static_cast<std::size_t>(3));

    constexpr auto v32 = remove_trailing_zeros(verified_u32{u32{UINT32_C(200)}});
    BOOST_TEST_EQ(v32.trimmed_number, UINT32_C(2));
    BOOST_TEST_EQ(v32.number_of_removed_zeros, static_cast<std::size_t>(2));

    constexpr auto v64 = remove_trailing_zeros(verified_u64{u64{UINT64_C(9000000000)}});
    BOOST_TEST_EQ(v64.trimmed_number, UINT64_C(9));
    BOOST_TEST_EQ(v64.number_of_removed_zeros, static_cast<std::size_t>(9));

    using boost::int128::uint128_t;
    constexpr auto v128 = remove_trailing_zeros(verified_u128{u128{uint128_t{UINT64_C(100000)}}});
    BOOST_TEST_EQ(v128.trimmed_number, uint128_t{1});
    BOOST_TEST_EQ(v128.number_of_removed_zeros, static_cast<std::size_t>(5));
}

// =============================================================================
// Zero input tests
// =============================================================================

template <typename T>
void test_rtz_zero()
{
    using underlying = typename detail::underlying_type_t<T>;

    auto result = remove_trailing_zeros(T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(result.trimmed_number, static_cast<underlying>(0));
    BOOST_TEST_EQ(result.number_of_removed_zeros, static_cast<std::size_t>(0));
}

void test_rtz_zero_constexpr()
{
    constexpr auto r8 = remove_trailing_zeros(u8{static_cast<std::uint8_t>(0)});
    BOOST_TEST_EQ(r8.trimmed_number, static_cast<std::uint8_t>(0));
    BOOST_TEST_EQ(r8.number_of_removed_zeros, static_cast<std::size_t>(0));

    constexpr auto r32 = remove_trailing_zeros(u32{UINT32_C(0)});
    BOOST_TEST_EQ(r32.trimmed_number, UINT32_C(0));
    BOOST_TEST_EQ(r32.number_of_removed_zeros, static_cast<std::size_t>(0));
}

void test_rtz_zero_verified()
{
    constexpr auto v8 = remove_trailing_zeros(verified_u8{u8{static_cast<std::uint8_t>(0)}});
    BOOST_TEST_EQ(v8.trimmed_number, static_cast<std::uint8_t>(0));
    BOOST_TEST_EQ(v8.number_of_removed_zeros, static_cast<std::size_t>(0));

    constexpr auto v32 = remove_trailing_zeros(verified_u32{u32{UINT32_C(0)}});
    BOOST_TEST_EQ(v32.trimmed_number, UINT32_C(0));
    BOOST_TEST_EQ(v32.number_of_removed_zeros, static_cast<std::size_t>(0));
}

int main()
{
    // Zero input - all types
    test_rtz_zero<u8>();
    test_rtz_zero<u16>();
    test_rtz_zero<u32>();
    test_rtz_zero<u64>();
    test_rtz_zero<u128>();
    test_rtz_zero_constexpr();
    test_rtz_zero_verified();

    // No trailing zeros - all types
    test_rtz_no_trailing_zeros<u8>();
    test_rtz_no_trailing_zeros<u16>();
    test_rtz_no_trailing_zeros<u32>();
    test_rtz_no_trailing_zeros<u64>();
    test_rtz_no_trailing_zeros<u128>();

    // One trailing zero - all types
    test_rtz_one_trailing_zero<u8>();
    test_rtz_one_trailing_zero<u16>();
    test_rtz_one_trailing_zero<u32>();
    test_rtz_one_trailing_zero<u64>();
    test_rtz_one_trailing_zero<u128>();

    // Two trailing zeros - all types
    test_rtz_two_trailing_zeros<u8>();
    test_rtz_two_trailing_zeros<u16>();
    test_rtz_two_trailing_zeros<u32>();
    test_rtz_two_trailing_zeros<u64>();
    test_rtz_two_trailing_zeros<u128>();

    // Larger trailing zero counts per type
    test_rtz_u16_large();
    test_rtz_u32_large();
    test_rtz_u64_large();
    test_rtz_u128_large();

    // Exhaustive correctness for u8 and u16
    test_rtz_exhaustive_u8();
    test_rtz_exhaustive_u16();

    // Constexpr evaluation
    test_rtz_constexpr();

    // Verified types (consteval)
    test_rtz_verified();

    return boost::report_errors();
}

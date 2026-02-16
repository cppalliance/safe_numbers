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
// Runtime tests: powers of 10
// =============================================================================

template <typename T>
void test_is_power_10_true()
{
    using underlying = typename detail::underlying_type_t<T>;

    BOOST_TEST(is_power_10(T{static_cast<underlying>(1)}));
    BOOST_TEST(is_power_10(T{static_cast<underlying>(10)}));
    BOOST_TEST(is_power_10(T{static_cast<underlying>(100)}));
}

void test_is_power_10_true_u16()
{
    BOOST_TEST(is_power_10(u16{static_cast<std::uint16_t>(1000)}));
    BOOST_TEST(is_power_10(u16{static_cast<std::uint16_t>(10000)}));
}

void test_is_power_10_true_u32()
{
    BOOST_TEST(is_power_10(u32{UINT32_C(1000)}));
    BOOST_TEST(is_power_10(u32{UINT32_C(10000)}));
    BOOST_TEST(is_power_10(u32{UINT32_C(100000)}));
    BOOST_TEST(is_power_10(u32{UINT32_C(1000000)}));
    BOOST_TEST(is_power_10(u32{UINT32_C(10000000)}));
    BOOST_TEST(is_power_10(u32{UINT32_C(100000000)}));
    BOOST_TEST(is_power_10(u32{UINT32_C(1000000000)}));
}

void test_is_power_10_true_u64()
{
    BOOST_TEST(is_power_10(u64{UINT64_C(1000000000000)}));
    BOOST_TEST(is_power_10(u64{UINT64_C(10000000000000000)}));
    BOOST_TEST(is_power_10(u64{UINT64_C(1000000000000000000)}));
    BOOST_TEST(is_power_10(u64{UINT64_C(10000000000000000000)}));
}

void test_is_power_10_true_u128()
{
    using boost::int128::uint128_t;

    BOOST_TEST(is_power_10(u128{uint128_t{1}}));
    BOOST_TEST(is_power_10(u128{uint128_t{10}}));
    BOOST_TEST(is_power_10(u128{uint128_t{UINT64_C(1000000000000)}}));

    // 10^20
    const auto ten_pow_20 = uint128_t{UINT64_C(10000000000)} * uint128_t{UINT64_C(10000000000)};
    BOOST_TEST(is_power_10(u128{ten_pow_20}));

    // 10^30
    const auto ten_pow_30 = ten_pow_20 * uint128_t{UINT64_C(10000000000)};
    BOOST_TEST(is_power_10(u128{ten_pow_30}));
}

// =============================================================================
// Runtime tests: non-powers of 10
// =============================================================================

template <typename T>
void test_is_power_10_false()
{
    using underlying = typename detail::underlying_type_t<T>;

    BOOST_TEST(!is_power_10(T{static_cast<underlying>(2)}));
    BOOST_TEST(!is_power_10(T{static_cast<underlying>(3)}));
    BOOST_TEST(!is_power_10(T{static_cast<underlying>(5)}));
    BOOST_TEST(!is_power_10(T{static_cast<underlying>(7)}));
    BOOST_TEST(!is_power_10(T{static_cast<underlying>(9)}));
    BOOST_TEST(!is_power_10(T{static_cast<underlying>(11)}));
    BOOST_TEST(!is_power_10(T{static_cast<underlying>(20)}));
    BOOST_TEST(!is_power_10(T{static_cast<underlying>(50)}));
    BOOST_TEST(!is_power_10(T{static_cast<underlying>(99)}));
    BOOST_TEST(!is_power_10(T{static_cast<underlying>(101)}));
    BOOST_TEST(!is_power_10(T{static_cast<underlying>(200)}));
    BOOST_TEST(!is_power_10(T{static_cast<underlying>(255)}));
}

void test_is_power_10_false_u32()
{
    BOOST_TEST(!is_power_10(u32{UINT32_C(999)}));
    BOOST_TEST(!is_power_10(u32{UINT32_C(1001)}));
    BOOST_TEST(!is_power_10(u32{UINT32_C(2000000)}));
    BOOST_TEST(!is_power_10(u32{UINT32_C(4294967295)}));
}

void test_is_power_10_false_u64()
{
    BOOST_TEST(!is_power_10(u64{UINT64_C(9999999999999999)}));
    BOOST_TEST(!is_power_10(u64{UINT64_C(10000000000000001)}));
    BOOST_TEST(!is_power_10(u64{UINT64_MAX}));
}

void test_is_power_10_false_u128()
{
    using boost::int128::uint128_t;

    BOOST_TEST(!is_power_10(u128{uint128_t{2}}));
    BOOST_TEST(!is_power_10(u128{uint128_t{UINT64_C(9999999999999)}}));

    // 10^20 + 1
    const auto ten_pow_20 = uint128_t{UINT64_C(10000000000)} * uint128_t{UINT64_C(10000000000)};
    BOOST_TEST(!is_power_10(u128{ten_pow_20 + uint128_t{1}}));

    // 2 * 10^20
    BOOST_TEST(!is_power_10(u128{ten_pow_20 * uint128_t{2}}));
}

// =============================================================================
// Exhaustive tests for u8 and u16
// =============================================================================

void test_is_power_10_exhaustive_u8()
{
    for (unsigned i {1}; i <= 255; ++i)
    {
        const auto n = static_cast<std::uint8_t>(i);
        auto result = is_power_10(u8{n});

        const auto expected = (i == 1 || i == 10 || i == 100);
        BOOST_TEST_EQ(result, expected);
    }
}

void test_is_power_10_exhaustive_u16()
{
    for (unsigned i {1}; i <= 65535; ++i)
    {
        const auto n = static_cast<std::uint16_t>(i);
        auto result = is_power_10(u16{n});

        const auto expected = (i == 1 || i == 10 || i == 100 || i == 1000 || i == 10000);
        BOOST_TEST_EQ(result, expected);
    }
}

// =============================================================================
// Constexpr tests
// =============================================================================

void test_is_power_10_constexpr()
{
    static_assert(is_power_10(u8{static_cast<std::uint8_t>(1)}));
    static_assert(is_power_10(u8{static_cast<std::uint8_t>(10)}));
    static_assert(is_power_10(u8{static_cast<std::uint8_t>(100)}));
    static_assert(!is_power_10(u8{static_cast<std::uint8_t>(50)}));

    static_assert(is_power_10(u16{static_cast<std::uint16_t>(10000)}));
    static_assert(!is_power_10(u16{static_cast<std::uint16_t>(30000)}));

    static_assert(is_power_10(u32{UINT32_C(1000000000)}));
    static_assert(!is_power_10(u32{UINT32_C(1234)}));

    static_assert(is_power_10(u64{UINT64_C(10000000000000000000)}));
    static_assert(!is_power_10(u64{UINT64_C(7)}));
}

// =============================================================================
// Verified type tests (consteval)
// =============================================================================

void test_is_power_10_verified()
{
    static_assert(is_power_10(verified_u8{u8{static_cast<std::uint8_t>(1)}}));
    static_assert(is_power_10(verified_u8{u8{static_cast<std::uint8_t>(10)}}));
    static_assert(!is_power_10(verified_u8{u8{static_cast<std::uint8_t>(5)}}));

    static_assert(is_power_10(verified_u16{u16{static_cast<std::uint16_t>(1000)}}));
    static_assert(!is_power_10(verified_u16{u16{static_cast<std::uint16_t>(999)}}));

    static_assert(is_power_10(verified_u32{u32{UINT32_C(100)}}));
    static_assert(!is_power_10(verified_u32{u32{UINT32_C(200)}}));

    static_assert(is_power_10(verified_u64{u64{UINT64_C(1000000)}}));
    static_assert(!is_power_10(verified_u64{u64{UINT64_C(999999)}}));

    using boost::int128::uint128_t;
    static_assert(is_power_10(verified_u128{u128{uint128_t{UINT64_C(100000)}}}));
    static_assert(!is_power_10(verified_u128{u128{uint128_t{UINT64_C(100001)}}}));
}

// =============================================================================
// Zero input tests
// =============================================================================

template <typename T>
void test_is_power_10_zero()
{
    using underlying = typename detail::underlying_type_t<T>;
    BOOST_TEST(!is_power_10(T{static_cast<underlying>(0)}));
}

void test_is_power_10_zero_constexpr()
{
    static_assert(!is_power_10(u8{static_cast<std::uint8_t>(0)}));
    static_assert(!is_power_10(u32{UINT32_C(0)}));
    static_assert(!is_power_10(u64{UINT64_C(0)}));
}

void test_is_power_10_zero_verified()
{
    static_assert(!is_power_10(verified_u8{u8{static_cast<std::uint8_t>(0)}}));
    static_assert(!is_power_10(verified_u32{u32{UINT32_C(0)}}));
}

int main()
{
    // Zero input - all types
    test_is_power_10_zero<u8>();
    test_is_power_10_zero<u16>();
    test_is_power_10_zero<u32>();
    test_is_power_10_zero<u64>();
    test_is_power_10_zero<u128>();
    test_is_power_10_zero_constexpr();
    test_is_power_10_zero_verified();

    // Powers of 10 - all types
    test_is_power_10_true<u8>();
    test_is_power_10_true<u16>();
    test_is_power_10_true<u32>();
    test_is_power_10_true<u64>();
    test_is_power_10_true<u128>();
    test_is_power_10_true_u16();
    test_is_power_10_true_u32();
    test_is_power_10_true_u64();
    test_is_power_10_true_u128();

    // Non-powers of 10 - all types
    test_is_power_10_false<u8>();
    test_is_power_10_false<u16>();
    test_is_power_10_false<u32>();
    test_is_power_10_false<u64>();
    test_is_power_10_false<u128>();
    test_is_power_10_false_u32();
    test_is_power_10_false_u64();
    test_is_power_10_false_u128();

    // Exhaustive correctness for u8 and u16
    test_is_power_10_exhaustive_u8();
    test_is_power_10_exhaustive_u16();

    // Constexpr evaluation
    test_is_power_10_constexpr();

    // Verified types (consteval)
    test_is_power_10_verified();

    return boost::report_errors();
}

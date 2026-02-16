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
// Runtime tests: powers of 2
// =============================================================================

template <typename T>
void test_is_power_2_true()
{
    using underlying = typename detail::underlying_type_t<T>;

    BOOST_TEST(is_power_2(T{static_cast<underlying>(1)}));
    BOOST_TEST(is_power_2(T{static_cast<underlying>(2)}));
    BOOST_TEST(is_power_2(T{static_cast<underlying>(4)}));
    BOOST_TEST(is_power_2(T{static_cast<underlying>(8)}));
    BOOST_TEST(is_power_2(T{static_cast<underlying>(16)}));
    BOOST_TEST(is_power_2(T{static_cast<underlying>(32)}));
    BOOST_TEST(is_power_2(T{static_cast<underlying>(64)}));
    BOOST_TEST(is_power_2(T{static_cast<underlying>(128)}));
}

void test_is_power_2_true_u16()
{
    BOOST_TEST(is_power_2(u16{static_cast<std::uint16_t>(256)}));
    BOOST_TEST(is_power_2(u16{static_cast<std::uint16_t>(512)}));
    BOOST_TEST(is_power_2(u16{static_cast<std::uint16_t>(1024)}));
    BOOST_TEST(is_power_2(u16{static_cast<std::uint16_t>(2048)}));
    BOOST_TEST(is_power_2(u16{static_cast<std::uint16_t>(4096)}));
    BOOST_TEST(is_power_2(u16{static_cast<std::uint16_t>(8192)}));
    BOOST_TEST(is_power_2(u16{static_cast<std::uint16_t>(16384)}));
    BOOST_TEST(is_power_2(u16{static_cast<std::uint16_t>(32768)}));
}

void test_is_power_2_true_u32()
{
    BOOST_TEST(is_power_2(u32{UINT32_C(65536)}));
    BOOST_TEST(is_power_2(u32{UINT32_C(1048576)}));
    BOOST_TEST(is_power_2(u32{UINT32_C(1073741824)}));
    BOOST_TEST(is_power_2(u32{UINT32_C(2147483648)}));
}

void test_is_power_2_true_u64()
{
    BOOST_TEST(is_power_2(u64{UINT64_C(4294967296)}));
    BOOST_TEST(is_power_2(u64{UINT64_C(1099511627776)}));
    BOOST_TEST(is_power_2(u64{UINT64_C(9223372036854775808)}));
}

void test_is_power_2_true_u128()
{
    using boost::int128::uint128_t;

    BOOST_TEST(is_power_2(u128{uint128_t{1}}));
    BOOST_TEST(is_power_2(u128{uint128_t{2}}));
    BOOST_TEST(is_power_2(u128{uint128_t{UINT64_C(4294967296)}}));

    // 2^64
    const auto two_pow_64 = uint128_t{1} << 64;
    BOOST_TEST(is_power_2(u128{two_pow_64}));

    // 2^100
    const auto two_pow_100 = uint128_t{1} << 100;
    BOOST_TEST(is_power_2(u128{two_pow_100}));

    // 2^127
    const auto two_pow_127 = uint128_t{1} << 127;
    BOOST_TEST(is_power_2(u128{two_pow_127}));
}

// =============================================================================
// Runtime tests: non-powers of 2
// =============================================================================

template <typename T>
void test_is_power_2_false()
{
    using underlying = typename detail::underlying_type_t<T>;

    BOOST_TEST(!is_power_2(T{static_cast<underlying>(3)}));
    BOOST_TEST(!is_power_2(T{static_cast<underlying>(5)}));
    BOOST_TEST(!is_power_2(T{static_cast<underlying>(6)}));
    BOOST_TEST(!is_power_2(T{static_cast<underlying>(7)}));
    BOOST_TEST(!is_power_2(T{static_cast<underlying>(9)}));
    BOOST_TEST(!is_power_2(T{static_cast<underlying>(10)}));
    BOOST_TEST(!is_power_2(T{static_cast<underlying>(12)}));
    BOOST_TEST(!is_power_2(T{static_cast<underlying>(15)}));
    BOOST_TEST(!is_power_2(T{static_cast<underlying>(100)}));
    BOOST_TEST(!is_power_2(T{static_cast<underlying>(255)}));
}

void test_is_power_2_false_u32()
{
    BOOST_TEST(!is_power_2(u32{UINT32_C(3)}));
    BOOST_TEST(!is_power_2(u32{UINT32_C(1000)}));
    BOOST_TEST(!is_power_2(u32{UINT32_C(1073741823)}));
    BOOST_TEST(!is_power_2(u32{UINT32_C(1073741825)}));
    BOOST_TEST(!is_power_2(u32{UINT32_C(4294967295)}));
}

void test_is_power_2_false_u64()
{
    BOOST_TEST(!is_power_2(u64{UINT64_C(4294967297)}));
    BOOST_TEST(!is_power_2(u64{UINT64_C(10000000000000000000)}));
    BOOST_TEST(!is_power_2(u64{UINT64_MAX}));
}

void test_is_power_2_false_u128()
{
    using boost::int128::uint128_t;

    BOOST_TEST(!is_power_2(u128{uint128_t{3}}));

    // 2^64 + 1
    const auto two_pow_64_plus_1 = (uint128_t{1} << 64) + uint128_t{1};
    BOOST_TEST(!is_power_2(u128{two_pow_64_plus_1}));

    // 2^100 - 1
    const auto two_pow_100_minus_1 = (uint128_t{1} << 100) - uint128_t{1};
    BOOST_TEST(!is_power_2(u128{two_pow_100_minus_1}));
}

// =============================================================================
// Exhaustive tests for u8 and u16
// =============================================================================

void test_is_power_2_exhaustive_u8()
{
    for (unsigned i {1}; i <= 255; ++i)
    {
        const auto n = static_cast<std::uint8_t>(i);
        auto result = is_power_2(u8{n});

        const auto expected = (i & (i - 1)) == 0;
        BOOST_TEST_EQ(result, expected);
    }
}

void test_is_power_2_exhaustive_u16()
{
    for (unsigned i {1}; i <= 65535; ++i)
    {
        const auto n = static_cast<std::uint16_t>(i);
        auto result = is_power_2(u16{n});

        const auto expected = (i & (i - 1)) == 0;
        BOOST_TEST_EQ(result, expected);
    }
}

// =============================================================================
// Constexpr tests
// =============================================================================

void test_is_power_2_constexpr()
{
    static_assert(is_power_2(u8{static_cast<std::uint8_t>(1)}));
    static_assert(is_power_2(u8{static_cast<std::uint8_t>(2)}));
    static_assert(is_power_2(u8{static_cast<std::uint8_t>(128)}));
    static_assert(!is_power_2(u8{static_cast<std::uint8_t>(3)}));
    static_assert(!is_power_2(u8{static_cast<std::uint8_t>(255)}));

    static_assert(is_power_2(u16{static_cast<std::uint16_t>(32768)}));
    static_assert(!is_power_2(u16{static_cast<std::uint16_t>(1000)}));

    static_assert(is_power_2(u32{UINT32_C(2147483648)}));
    static_assert(!is_power_2(u32{UINT32_C(2147483647)}));

    static_assert(is_power_2(u64{UINT64_C(9223372036854775808)}));
    static_assert(!is_power_2(u64{UINT64_C(9223372036854775807)}));
}

// =============================================================================
// Verified type tests (consteval)
// =============================================================================

void test_is_power_2_verified()
{
    static_assert(is_power_2(verified_u8{u8{static_cast<std::uint8_t>(1)}}));
    static_assert(is_power_2(verified_u8{u8{static_cast<std::uint8_t>(64)}}));
    static_assert(!is_power_2(verified_u8{u8{static_cast<std::uint8_t>(5)}}));

    static_assert(is_power_2(verified_u16{u16{static_cast<std::uint16_t>(4096)}}));
    static_assert(!is_power_2(verified_u16{u16{static_cast<std::uint16_t>(4095)}}));

    static_assert(is_power_2(verified_u32{u32{UINT32_C(1048576)}}));
    static_assert(!is_power_2(verified_u32{u32{UINT32_C(1000000)}}));

    static_assert(is_power_2(verified_u64{u64{UINT64_C(4294967296)}}));
    static_assert(!is_power_2(verified_u64{u64{UINT64_C(4294967295)}}));

    using boost::int128::uint128_t;
    static_assert(is_power_2(verified_u128{u128{uint128_t{UINT64_C(1099511627776)}}}));
    static_assert(!is_power_2(verified_u128{u128{uint128_t{UINT64_C(1099511627775)}}}));
}

// =============================================================================
// Zero input tests
// =============================================================================

template <typename T>
void test_is_power_2_zero()
{
    using underlying = typename detail::underlying_type_t<T>;
    BOOST_TEST(!is_power_2(T{static_cast<underlying>(0)}));
}

void test_is_power_2_zero_constexpr()
{
    static_assert(!is_power_2(u8{static_cast<std::uint8_t>(0)}));
    static_assert(!is_power_2(u32{UINT32_C(0)}));
    static_assert(!is_power_2(u64{UINT64_C(0)}));
}

void test_is_power_2_zero_verified()
{
    static_assert(!is_power_2(verified_u8{u8{static_cast<std::uint8_t>(0)}}));
    static_assert(!is_power_2(verified_u32{u32{UINT32_C(0)}}));
}

int main()
{
    // Zero input - all types
    test_is_power_2_zero<u8>();
    test_is_power_2_zero<u16>();
    test_is_power_2_zero<u32>();
    test_is_power_2_zero<u64>();
    test_is_power_2_zero<u128>();
    test_is_power_2_zero_constexpr();
    test_is_power_2_zero_verified();

    // Powers of 2 - all types
    test_is_power_2_true<u8>();
    test_is_power_2_true<u16>();
    test_is_power_2_true<u32>();
    test_is_power_2_true<u64>();
    test_is_power_2_true<u128>();
    test_is_power_2_true_u16();
    test_is_power_2_true_u32();
    test_is_power_2_true_u64();
    test_is_power_2_true_u128();

    // Non-powers of 2 - all types
    test_is_power_2_false<u8>();
    test_is_power_2_false<u16>();
    test_is_power_2_false<u32>();
    test_is_power_2_false<u64>();
    test_is_power_2_false<u128>();
    test_is_power_2_false_u32();
    test_is_power_2_false_u64();
    test_is_power_2_false_u128();

    // Exhaustive correctness for u8 and u16
    test_is_power_2_exhaustive_u8();
    test_is_power_2_exhaustive_u16();

    // Constexpr evaluation
    test_is_power_2_constexpr();

    // Verified types (consteval)
    test_is_power_2_verified();

    return boost::report_errors();
}

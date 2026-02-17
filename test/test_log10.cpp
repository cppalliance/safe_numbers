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
// Runtime tests: exact powers of 10
// =============================================================================

template <typename T>
void test_log10_powers_of_10()
{
    using underlying = typename detail::underlying_type_t<T>;

    BOOST_TEST_EQ(log10(T{static_cast<underlying>(1)}), 0);    // 10^0
    BOOST_TEST_EQ(log10(T{static_cast<underlying>(10)}), 1);   // 10^1
    BOOST_TEST_EQ(log10(T{static_cast<underlying>(100)}), 2);  // 10^2
}

void test_log10_powers_of_10_u16()
{
    BOOST_TEST_EQ(log10(u16{static_cast<std::uint16_t>(1000)}), 3);   // 10^3
    BOOST_TEST_EQ(log10(u16{static_cast<std::uint16_t>(10000)}), 4);  // 10^4
}

void test_log10_powers_of_10_u32()
{
    BOOST_TEST_EQ(log10(u32{UINT32_C(1000)}), 3);
    BOOST_TEST_EQ(log10(u32{UINT32_C(10000)}), 4);
    BOOST_TEST_EQ(log10(u32{UINT32_C(100000)}), 5);
    BOOST_TEST_EQ(log10(u32{UINT32_C(1000000)}), 6);
    BOOST_TEST_EQ(log10(u32{UINT32_C(10000000)}), 7);
    BOOST_TEST_EQ(log10(u32{UINT32_C(100000000)}), 8);
    BOOST_TEST_EQ(log10(u32{UINT32_C(1000000000)}), 9);
}

void test_log10_powers_of_10_u64()
{
    BOOST_TEST_EQ(log10(u64{UINT64_C(1000000000000)}), 12);
    BOOST_TEST_EQ(log10(u64{UINT64_C(10000000000000000)}), 16);
    BOOST_TEST_EQ(log10(u64{UINT64_C(1000000000000000000)}), 18);
    BOOST_TEST_EQ(log10(u64{UINT64_C(10000000000000000000)}), 19);
}

void test_log10_powers_of_10_u128()
{
    using boost::int128::uint128_t;

    BOOST_TEST_EQ(log10(u128{uint128_t{UINT64_C(1000000000000)}}), 12);

    // 10^20
    const auto ten_pow_20 = uint128_t{UINT64_C(10000000000)} * uint128_t{UINT64_C(10000000000)};
    BOOST_TEST_EQ(log10(u128{ten_pow_20}), 20);

    // 10^30
    const auto ten_pow_30 = ten_pow_20 * uint128_t{UINT64_C(10000000000)};
    BOOST_TEST_EQ(log10(u128{ten_pow_30}), 30);
}

// =============================================================================
// Runtime tests: non-powers of 10 (floor behavior)
// =============================================================================

template <typename T>
void test_log10_floor()
{
    using underlying = typename detail::underlying_type_t<T>;

    BOOST_TEST_EQ(log10(T{static_cast<underlying>(2)}), 0);    // floor(log10(2)) = 0
    BOOST_TEST_EQ(log10(T{static_cast<underlying>(5)}), 0);
    BOOST_TEST_EQ(log10(T{static_cast<underlying>(9)}), 0);
    BOOST_TEST_EQ(log10(T{static_cast<underlying>(11)}), 1);   // floor(log10(11)) = 1
    BOOST_TEST_EQ(log10(T{static_cast<underlying>(50)}), 1);
    BOOST_TEST_EQ(log10(T{static_cast<underlying>(99)}), 1);
    BOOST_TEST_EQ(log10(T{static_cast<underlying>(101)}), 2);  // floor(log10(101)) = 2
    BOOST_TEST_EQ(log10(T{static_cast<underlying>(200)}), 2);
    BOOST_TEST_EQ(log10(T{static_cast<underlying>(255)}), 2);
}

void test_log10_floor_u32()
{
    BOOST_TEST_EQ(log10(u32{UINT32_C(999)}), 2);
    BOOST_TEST_EQ(log10(u32{UINT32_C(1001)}), 3);
    BOOST_TEST_EQ(log10(u32{UINT32_C(999999999)}), 8);
    BOOST_TEST_EQ(log10(u32{UINT32_C(4294967295)}), 9);  // floor(log10(2^32 - 1))
}

void test_log10_floor_u64()
{
    BOOST_TEST_EQ(log10(u64{UINT64_C(9999999999999999)}), 15);
    BOOST_TEST_EQ(log10(u64{UINT64_C(10000000000000001)}), 16);
    BOOST_TEST_EQ(log10(u64{UINT64_MAX}), 19);  // floor(log10(2^64 - 1))
}

void test_log10_floor_u128()
{
    using boost::int128::uint128_t;

    BOOST_TEST_EQ(log10(u128{uint128_t{UINT64_C(9999999999999)}}), 12);

    // 10^20 + 1
    const auto ten_pow_20 = uint128_t{UINT64_C(10000000000)} * uint128_t{UINT64_C(10000000000)};
    BOOST_TEST_EQ(log10(u128{ten_pow_20 + uint128_t{1}}), 20);

    // 10^20 - 1
    BOOST_TEST_EQ(log10(u128{ten_pow_20 - uint128_t{1}}), 19);
}

// =============================================================================
// Tests with trailing zeros (exercises remove_trailing_zeros path)
// =============================================================================

template <typename T>
void test_log10_trailing_zeros()
{
    using underlying = typename detail::underlying_type_t<T>;

    // Numbers with trailing zeros where RTZ does the heavy lifting
    BOOST_TEST_EQ(log10(T{static_cast<underlying>(20)}), 1);
    BOOST_TEST_EQ(log10(T{static_cast<underlying>(30)}), 1);
    BOOST_TEST_EQ(log10(T{static_cast<underlying>(50)}), 1);
    BOOST_TEST_EQ(log10(T{static_cast<underlying>(90)}), 1);
}

void test_log10_trailing_zeros_u32()
{
    BOOST_TEST_EQ(log10(u32{UINT32_C(12300)}), 4);
    BOOST_TEST_EQ(log10(u32{UINT32_C(5000000)}), 6);
    BOOST_TEST_EQ(log10(u32{UINT32_C(4000000000)}), 9);
}

void test_log10_trailing_zeros_u64()
{
    BOOST_TEST_EQ(log10(u64{UINT64_C(123456789000)}), 11);
    BOOST_TEST_EQ(log10(u64{UINT64_C(5000000000000000000)}), 18);
}

// =============================================================================
// Exhaustive u8 tests
// =============================================================================

void test_log10_exhaustive_u8()
{
    for (unsigned i {1}; i <= 255; ++i)
    {
        const auto n = static_cast<std::uint8_t>(i);
        auto result = log10(u8{n});

        // Compute expected floor(log10(i))
        auto expected {0};
        auto tmp {i};
        while (tmp >= 10)
        {
            tmp /= 10;
            ++expected;
        }

        BOOST_TEST_EQ(result, expected);
    }
}

// =============================================================================
// Exhaustive u16 tests
// =============================================================================

void test_log10_exhaustive_u16()
{
    for (unsigned i {1}; i <= 65535; ++i)
    {
        const auto n = static_cast<std::uint16_t>(i);
        auto result = log10(u16{n});

        auto expected {0};
        auto tmp {i};
        while (tmp >= 10)
        {
            tmp /= 10;
            ++expected;
        }

        BOOST_TEST_EQ(result, expected);
    }
}

// =============================================================================
// Constexpr tests
// =============================================================================

void test_log10_constexpr()
{
    static_assert(log10(u8{static_cast<std::uint8_t>(1)}) == 0);
    static_assert(log10(u8{static_cast<std::uint8_t>(10)}) == 1);
    static_assert(log10(u8{static_cast<std::uint8_t>(100)}) == 2);
    static_assert(log10(u8{static_cast<std::uint8_t>(255)}) == 2);

    static_assert(log10(u16{static_cast<std::uint16_t>(10000)}) == 4);
    static_assert(log10(u16{static_cast<std::uint16_t>(65535)}) == 4);

    static_assert(log10(u32{UINT32_C(1000000000)}) == 9);
    static_assert(log10(u32{UINT32_C(4294967295)}) == 9);

    static_assert(log10(u64{UINT64_C(1)}) == 0);
    static_assert(log10(u64{UINT64_C(10000000000000000000)}) == 19);
}

// =============================================================================
// Verified type tests (consteval)
// =============================================================================

void test_log10_verified()
{
    static_assert(log10(verified_u8{u8{static_cast<std::uint8_t>(1)}}) == 0);
    static_assert(log10(verified_u8{u8{static_cast<std::uint8_t>(10)}}) == 1);
    static_assert(log10(verified_u8{u8{static_cast<std::uint8_t>(100)}}) == 2);

    static_assert(log10(verified_u16{u16{static_cast<std::uint16_t>(10000)}}) == 4);

    static_assert(log10(verified_u32{u32{UINT32_C(1000000000)}}) == 9);
    static_assert(log10(verified_u32{u32{UINT32_C(4294967295)}}) == 9);

    static_assert(log10(verified_u64{u64{UINT64_C(10000000000000000000)}}) == 19);

    using boost::int128::uint128_t;
    static_assert(log10(verified_u128{u128{uint128_t{UINT64_C(1000000000000)}}}) == 12);
}

int main()
{
    // Powers of 10 - all types
    test_log10_powers_of_10<u8>();
    test_log10_powers_of_10<u16>();
    test_log10_powers_of_10<u32>();
    test_log10_powers_of_10<u64>();
    test_log10_powers_of_10<u128>();
    test_log10_powers_of_10_u16();
    test_log10_powers_of_10_u32();
    test_log10_powers_of_10_u64();
    test_log10_powers_of_10_u128();

    // Floor behavior - all types
    test_log10_floor<u8>();
    test_log10_floor<u16>();
    test_log10_floor<u32>();
    test_log10_floor<u64>();
    test_log10_floor<u128>();
    test_log10_floor_u32();
    test_log10_floor_u64();
    test_log10_floor_u128();

    // Trailing zeros (RTZ path)
    test_log10_trailing_zeros<u8>();
    test_log10_trailing_zeros<u16>();
    test_log10_trailing_zeros<u32>();
    test_log10_trailing_zeros<u64>();
    test_log10_trailing_zeros<u128>();
    test_log10_trailing_zeros_u32();
    test_log10_trailing_zeros_u64();

    // Exhaustive u8 and u16
    test_log10_exhaustive_u8();
    test_log10_exhaustive_u16();

    // Constexpr evaluation
    test_log10_constexpr();

    // Verified types (consteval)
    test_log10_verified();

    return boost::report_errors();
}

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
void test_log2_powers_of_2()
{
    using underlying = typename detail::underlying_type_t<T>;

    BOOST_TEST_EQ(log2(T{static_cast<underlying>(1)}), 0);   // 2^0
    BOOST_TEST_EQ(log2(T{static_cast<underlying>(2)}), 1);   // 2^1
    BOOST_TEST_EQ(log2(T{static_cast<underlying>(4)}), 2);   // 2^2
    BOOST_TEST_EQ(log2(T{static_cast<underlying>(8)}), 3);   // 2^3
    BOOST_TEST_EQ(log2(T{static_cast<underlying>(16)}), 4);  // 2^4
    BOOST_TEST_EQ(log2(T{static_cast<underlying>(32)}), 5);  // 2^5
    BOOST_TEST_EQ(log2(T{static_cast<underlying>(64)}), 6);  // 2^6
    BOOST_TEST_EQ(log2(T{static_cast<underlying>(128)}), 7); // 2^7
}

// =============================================================================
// Runtime tests: non-powers of 2 (floor behavior)
// =============================================================================

template <typename T>
void test_log2_floor()
{
    using underlying = typename detail::underlying_type_t<T>;

    BOOST_TEST_EQ(log2(T{static_cast<underlying>(3)}), 1);   // floor(log2(3)) = 1
    BOOST_TEST_EQ(log2(T{static_cast<underlying>(5)}), 2);   // floor(log2(5)) = 2
    BOOST_TEST_EQ(log2(T{static_cast<underlying>(6)}), 2);   // floor(log2(6)) = 2
    BOOST_TEST_EQ(log2(T{static_cast<underlying>(7)}), 2);   // floor(log2(7)) = 2
    BOOST_TEST_EQ(log2(T{static_cast<underlying>(9)}), 3);   // floor(log2(9)) = 3
    BOOST_TEST_EQ(log2(T{static_cast<underlying>(15)}), 3);  // floor(log2(15)) = 3
    BOOST_TEST_EQ(log2(T{static_cast<underlying>(17)}), 4);  // floor(log2(17)) = 4
    BOOST_TEST_EQ(log2(T{static_cast<underlying>(100)}), 6); // floor(log2(100)) = 6
    BOOST_TEST_EQ(log2(T{static_cast<underlying>(255)}), 7); // floor(log2(255)) = 7
}

// =============================================================================
// Type-specific tests for larger values
// =============================================================================

void test_log2_u16()
{
    BOOST_TEST_EQ(log2(u16{static_cast<std::uint16_t>(256)}), 8);     // 2^8
    BOOST_TEST_EQ(log2(u16{static_cast<std::uint16_t>(1000)}), 9);    // floor(log2(1000))
    BOOST_TEST_EQ(log2(u16{static_cast<std::uint16_t>(1024)}), 10);   // 2^10
    BOOST_TEST_EQ(log2(u16{static_cast<std::uint16_t>(32768)}), 15);  // 2^15
    BOOST_TEST_EQ(log2(u16{static_cast<std::uint16_t>(65535)}), 15);  // floor(log2(65535))
}

void test_log2_u32()
{
    BOOST_TEST_EQ(log2(u32{UINT32_C(65536)}), 16);         // 2^16
    BOOST_TEST_EQ(log2(u32{UINT32_C(1000000)}), 19);       // floor(log2(10^6))
    BOOST_TEST_EQ(log2(u32{UINT32_C(1048576)}), 20);       // 2^20
    BOOST_TEST_EQ(log2(u32{UINT32_C(1073741824)}), 30);    // 2^30
    BOOST_TEST_EQ(log2(u32{UINT32_C(2147483648)}), 31);    // 2^31
    BOOST_TEST_EQ(log2(u32{UINT32_C(4294967295)}), 31);    // floor(log2(2^32 - 1))
}

void test_log2_u64()
{
    BOOST_TEST_EQ(log2(u64{UINT64_C(4294967296)}), 32);               // 2^32
    BOOST_TEST_EQ(log2(u64{UINT64_C(1099511627776)}), 40);            // 2^40
    BOOST_TEST_EQ(log2(u64{UINT64_C(1000000000000)}), 39);            // floor(log2(10^12))
    BOOST_TEST_EQ(log2(u64{UINT64_C(4611686018427387904)}), 62);      // 2^62
    BOOST_TEST_EQ(log2(u64{UINT64_C(9223372036854775808)}), 63);      // 2^63
    BOOST_TEST_EQ(log2(u64{UINT64_MAX}), 63);                         // floor(log2(2^64 - 1))
}

void test_log2_u128()
{
    using boost::int128::uint128_t;

    BOOST_TEST_EQ(log2(u128{uint128_t{1}}), 0);
    BOOST_TEST_EQ(log2(u128{uint128_t{UINT64_C(1099511627776)}}), 40);  // 2^40

    // 2^64
    const auto two_pow_64 = uint128_t{1} << 64U;
    BOOST_TEST_EQ(log2(u128{two_pow_64}), 64);

    // 2^100
    const auto two_pow_100 = uint128_t{1} << 100U;
    BOOST_TEST_EQ(log2(u128{two_pow_100}), 100);

    // 2^127
    const auto two_pow_127 = uint128_t{1} << 127U;
    BOOST_TEST_EQ(log2(u128{two_pow_127}), 127);

    // 2^127 + 1 (still 127)
    BOOST_TEST_EQ(log2(u128{two_pow_127 + uint128_t{1}}), 127);
}

// =============================================================================
// Exhaustive u8 tests
// =============================================================================

void test_log2_exhaustive_u8()
{
    for (unsigned i {1}; i <= 255; ++i)
    {
        const auto n = static_cast<std::uint8_t>(i);
        auto result = log2(u8{n});

        // Compute expected floor(log2(i))
        auto expected {0};
        auto tmp {i};
        while (tmp > 1)
        {
            tmp >>= 1U;
            ++expected;
        }

        BOOST_TEST_EQ(result, expected);
    }
}

// =============================================================================
// Constexpr tests
// =============================================================================

void test_log2_constexpr()
{
    static_assert(log2(u8{static_cast<std::uint8_t>(1)}) == 0);
    static_assert(log2(u8{static_cast<std::uint8_t>(2)}) == 1);
    static_assert(log2(u8{static_cast<std::uint8_t>(128)}) == 7);
    static_assert(log2(u8{static_cast<std::uint8_t>(255)}) == 7);

    static_assert(log2(u16{static_cast<std::uint16_t>(1024)}) == 10);
    static_assert(log2(u16{static_cast<std::uint16_t>(65535)}) == 15);

    static_assert(log2(u32{UINT32_C(1048576)}) == 20);
    static_assert(log2(u32{UINT32_C(4294967295)}) == 31);

    static_assert(log2(u64{UINT64_C(1)}) == 0);
    static_assert(log2(u64{UINT64_C(9223372036854775808)}) == 63);
}

// =============================================================================
// Verified type tests (consteval)
// =============================================================================

void test_log2_verified()
{
    static_assert(log2(verified_u8{u8{static_cast<std::uint8_t>(1)}}) == 0);
    static_assert(log2(verified_u8{u8{static_cast<std::uint8_t>(128)}}) == 7);
    static_assert(log2(verified_u8{u8{static_cast<std::uint8_t>(255)}}) == 7);

    static_assert(log2(verified_u16{u16{static_cast<std::uint16_t>(1024)}}) == 10);

    static_assert(log2(verified_u32{u32{UINT32_C(1048576)}}) == 20);
    static_assert(log2(verified_u32{u32{UINT32_C(4294967295)}}) == 31);

    static_assert(log2(verified_u64{u64{UINT64_C(9223372036854775808)}}) == 63);

    using boost::int128::uint128_t;
    static_assert(log2(verified_u128{u128{uint128_t{UINT64_C(1099511627776)}}}) == 40);
}

int main()
{
    // Powers of 2 - all types
    test_log2_powers_of_2<u8>();
    test_log2_powers_of_2<u16>();
    test_log2_powers_of_2<u32>();
    test_log2_powers_of_2<u64>();
    test_log2_powers_of_2<u128>();

    // Floor behavior - all types
    test_log2_floor<u8>();
    test_log2_floor<u16>();
    test_log2_floor<u32>();
    test_log2_floor<u64>();
    test_log2_floor<u128>();

    // Type-specific larger values
    test_log2_u16();
    test_log2_u32();
    test_log2_u64();
    test_log2_u128();

    // Exhaustive u8
    test_log2_exhaustive_u8();

    // Constexpr evaluation
    test_log2_constexpr();

    // Verified types (consteval)
    test_log2_verified();

    return boost::report_errors();
}

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
#include <stdexcept>

using namespace boost::safe_numbers;

// =============================================================================
// Runtime tests: exact powers of the base
// =============================================================================

template <typename T>
void test_ilog_exact_powers()
{
    using underlying = typename detail::underlying_type_t<T>;

    // Base 2
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(1)}, T{static_cast<underlying>(2)}), 0);   // 2^0
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(2)}, T{static_cast<underlying>(2)}), 1);   // 2^1
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(4)}, T{static_cast<underlying>(2)}), 2);   // 2^2
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(8)}, T{static_cast<underlying>(2)}), 3);   // 2^3
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(128)}, T{static_cast<underlying>(2)}), 7); // 2^7

    // Base 3
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(1)}, T{static_cast<underlying>(3)}), 0);   // 3^0
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(3)}, T{static_cast<underlying>(3)}), 1);   // 3^1
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(9)}, T{static_cast<underlying>(3)}), 2);   // 3^2
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(27)}, T{static_cast<underlying>(3)}), 3);  // 3^3
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(81)}, T{static_cast<underlying>(3)}), 4);  // 3^4

    // Base 10
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(1)}, T{static_cast<underlying>(10)}), 0);   // 10^0
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(10)}, T{static_cast<underlying>(10)}), 1);  // 10^1
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(100)}, T{static_cast<underlying>(10)}), 2); // 10^2

    // Base 16
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(1)}, T{static_cast<underlying>(16)}), 0);
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(16)}, T{static_cast<underlying>(16)}), 1);
}

// =============================================================================
// Runtime tests: non-powers (floor behavior)
// =============================================================================

template <typename T>
void test_ilog_floor()
{
    using underlying = typename detail::underlying_type_t<T>;

    // Base 2 floor
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(3)}, T{static_cast<underlying>(2)}), 1);   // floor(log2(3)) = 1
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(5)}, T{static_cast<underlying>(2)}), 2);   // floor(log2(5)) = 2
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(7)}, T{static_cast<underlying>(2)}), 2);   // floor(log2(7)) = 2
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(255)}, T{static_cast<underlying>(2)}), 7); // floor(log2(255)) = 7

    // Base 3 floor
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(2)}, T{static_cast<underlying>(3)}), 0);   // floor(log3(2)) = 0
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(4)}, T{static_cast<underlying>(3)}), 1);   // floor(log3(4)) = 1
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(8)}, T{static_cast<underlying>(3)}), 1);   // floor(log3(8)) = 1
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(26)}, T{static_cast<underlying>(3)}), 2);  // floor(log3(26)) = 2
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(80)}, T{static_cast<underlying>(3)}), 3);  // floor(log3(80)) = 3

    // Base 10 floor
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(5)}, T{static_cast<underlying>(10)}), 0);
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(9)}, T{static_cast<underlying>(10)}), 0);
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(11)}, T{static_cast<underlying>(10)}), 1);
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(99)}, T{static_cast<underlying>(10)}), 1);
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(101)}, T{static_cast<underlying>(10)}), 2);
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(255)}, T{static_cast<underlying>(10)}), 2);

    // Base 5 floor
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(1)}, T{static_cast<underlying>(5)}), 0);
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(4)}, T{static_cast<underlying>(5)}), 0);
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(5)}, T{static_cast<underlying>(5)}), 1);
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(24)}, T{static_cast<underlying>(5)}), 1);
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(25)}, T{static_cast<underlying>(5)}), 2);
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(124)}, T{static_cast<underlying>(5)}), 2);
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(125)}, T{static_cast<underlying>(5)}), 3);
}

// =============================================================================
// Consistency: ilog(n, 2) == log2(n) and ilog(n, 10) == log10(n)
// =============================================================================

template <typename T>
void test_ilog_consistency()
{
    using underlying = typename detail::underlying_type_t<T>;

    // Check against specialized log2
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(1)}, T{static_cast<underlying>(2)}), log2(T{static_cast<underlying>(1)}));
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(2)}, T{static_cast<underlying>(2)}), log2(T{static_cast<underlying>(2)}));
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(7)}, T{static_cast<underlying>(2)}), log2(T{static_cast<underlying>(7)}));
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(128)}, T{static_cast<underlying>(2)}), log2(T{static_cast<underlying>(128)}));
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(255)}, T{static_cast<underlying>(2)}), log2(T{static_cast<underlying>(255)}));

    // Check against specialized log10
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(1)}, T{static_cast<underlying>(10)}), log10(T{static_cast<underlying>(1)}));
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(10)}, T{static_cast<underlying>(10)}), log10(T{static_cast<underlying>(10)}));
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(99)}, T{static_cast<underlying>(10)}), log10(T{static_cast<underlying>(99)}));
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(100)}, T{static_cast<underlying>(10)}), log10(T{static_cast<underlying>(100)}));
    BOOST_TEST_EQ(ilog(T{static_cast<underlying>(255)}, T{static_cast<underlying>(10)}), log10(T{static_cast<underlying>(255)}));
}

// =============================================================================
// Type-specific tests for larger values
// =============================================================================

void test_ilog_u16()
{
    // Base 2
    BOOST_TEST_EQ(ilog(u16{static_cast<std::uint16_t>(1024)}, u16{static_cast<std::uint16_t>(2)}), 10);
    BOOST_TEST_EQ(ilog(u16{static_cast<std::uint16_t>(32768)}, u16{static_cast<std::uint16_t>(2)}), 15);
    BOOST_TEST_EQ(ilog(u16{static_cast<std::uint16_t>(65535)}, u16{static_cast<std::uint16_t>(2)}), 15);

    // Base 3
    BOOST_TEST_EQ(ilog(u16{static_cast<std::uint16_t>(243)}, u16{static_cast<std::uint16_t>(3)}), 5);     // 3^5
    BOOST_TEST_EQ(ilog(u16{static_cast<std::uint16_t>(59049)}, u16{static_cast<std::uint16_t>(3)}), 10);  // 3^10

    // Base 7
    BOOST_TEST_EQ(ilog(u16{static_cast<std::uint16_t>(2401)}, u16{static_cast<std::uint16_t>(7)}), 4);    // 7^4
    BOOST_TEST_EQ(ilog(u16{static_cast<std::uint16_t>(16807)}, u16{static_cast<std::uint16_t>(7)}), 5);   // 7^5

    // Base 256
    BOOST_TEST_EQ(ilog(u16{static_cast<std::uint16_t>(256)}, u16{static_cast<std::uint16_t>(256)}), 1);
    BOOST_TEST_EQ(ilog(u16{static_cast<std::uint16_t>(65535)}, u16{static_cast<std::uint16_t>(256)}), 1);
}

void test_ilog_u32()
{
    // Base 2
    BOOST_TEST_EQ(ilog(u32{UINT32_C(1073741824)}, u32{UINT32_C(2)}), 30);
    BOOST_TEST_EQ(ilog(u32{UINT32_C(4294967295)}, u32{UINT32_C(2)}), 31);

    // Base 3
    BOOST_TEST_EQ(ilog(u32{UINT32_C(1594323)}, u32{UINT32_C(3)}), 13);       // 3^13
    BOOST_TEST_EQ(ilog(u32{UINT32_C(3486784401)}, u32{UINT32_C(3)}), 20);    // 3^20

    // Base 10
    BOOST_TEST_EQ(ilog(u32{UINT32_C(1000000000)}, u32{UINT32_C(10)}), 9);
    BOOST_TEST_EQ(ilog(u32{UINT32_C(4294967295)}, u32{UINT32_C(10)}), 9);

    // Base 1000
    BOOST_TEST_EQ(ilog(u32{UINT32_C(1000000)}, u32{UINT32_C(1000)}), 2);
    BOOST_TEST_EQ(ilog(u32{UINT32_C(1000000000)}, u32{UINT32_C(1000)}), 3);
}

void test_ilog_u64()
{
    // Base 2
    BOOST_TEST_EQ(ilog(u64{UINT64_C(9223372036854775808)}, u64{UINT64_C(2)}), 63);
    BOOST_TEST_EQ(ilog(u64{UINT64_MAX}, u64{UINT64_C(2)}), 63);

    // Base 3
    BOOST_TEST_EQ(ilog(u64{UINT64_C(12157665459056928801)}, u64{UINT64_C(3)}), 40); // 3^40

    // Base 10
    BOOST_TEST_EQ(ilog(u64{UINT64_C(10000000000000000000)}, u64{UINT64_C(10)}), 19);
    BOOST_TEST_EQ(ilog(u64{UINT64_MAX}, u64{UINT64_C(10)}), 19);

    // Large base
    BOOST_TEST_EQ(ilog(u64{UINT64_C(1000000000000000000)}, u64{UINT64_C(1000000000)}), 2);
}

void test_ilog_u128()
{
    using boost::int128::uint128_t;

    // Small values
    BOOST_TEST_EQ(ilog(u128{uint128_t{1}}, u128{uint128_t{2}}), 0);
    BOOST_TEST_EQ(ilog(u128{uint128_t{100}}, u128{uint128_t{10}}), 2);

    // 2^64
    const auto two_pow_64 = uint128_t{1} << 64U;
    BOOST_TEST_EQ(ilog(u128{two_pow_64}, u128{uint128_t{2}}), 64);

    // 2^127
    const auto two_pow_127 = uint128_t{1} << 127U;
    BOOST_TEST_EQ(ilog(u128{two_pow_127}, u128{uint128_t{2}}), 127);

    // Base 10 large
    const auto ten_pow_20 = uint128_t{UINT64_C(10000000000)} * uint128_t{UINT64_C(10000000000)};
    BOOST_TEST_EQ(ilog(u128{ten_pow_20}, u128{uint128_t{10}}), 20);

    // Base 10^10
    BOOST_TEST_EQ(ilog(u128{ten_pow_20}, u128{uint128_t{UINT64_C(10000000000)}}), 2);

    // Large base equal to value
    BOOST_TEST_EQ(ilog(u128{two_pow_64}, u128{two_pow_64}), 1);
}

// =============================================================================
// Exhaustive u8 tests for bases 2..15
// =============================================================================

void test_ilog_exhaustive_u8()
{
    for (unsigned base {2}; base <= 15; ++base)
    {
        for (unsigned n {1}; n <= 255; ++n)
        {
            // Reference: floor(log_base(n)) by repeated division
            auto expected {0};
            auto tmp {n};
            while (tmp >= base)
            {
                tmp /= base;
                ++expected;
            }

            BOOST_TEST_EQ(ilog(u8{static_cast<std::uint8_t>(n)}, u8{static_cast<std::uint8_t>(base)}), expected);
        }
    }
}

// =============================================================================
// Domain error: ilog(0, base) throws
// =============================================================================

template <typename T>
void test_ilog_zero_throws()
{
    using underlying = typename detail::underlying_type_t<T>;

    BOOST_TEST_THROWS(static_cast<void>(ilog(T{static_cast<underlying>(0)}, T{static_cast<underlying>(2)})), std::domain_error);
    BOOST_TEST_THROWS(static_cast<void>(ilog(T{static_cast<underlying>(0)}, T{static_cast<underlying>(10)})), std::domain_error);
}

// =============================================================================
// Constexpr tests
// =============================================================================

void test_ilog_constexpr()
{
    // Base 2
    static_assert(ilog(u8{static_cast<std::uint8_t>(1)}, u8{static_cast<std::uint8_t>(2)}) == 0);
    static_assert(ilog(u8{static_cast<std::uint8_t>(128)}, u8{static_cast<std::uint8_t>(2)}) == 7);
    static_assert(ilog(u8{static_cast<std::uint8_t>(255)}, u8{static_cast<std::uint8_t>(2)}) == 7);

    // Base 3
    static_assert(ilog(u8{static_cast<std::uint8_t>(27)}, u8{static_cast<std::uint8_t>(3)}) == 3);
    static_assert(ilog(u8{static_cast<std::uint8_t>(80)}, u8{static_cast<std::uint8_t>(3)}) == 3);

    // Base 10
    static_assert(ilog(u16{static_cast<std::uint16_t>(10000)}, u16{static_cast<std::uint16_t>(10)}) == 4);
    static_assert(ilog(u16{static_cast<std::uint16_t>(65535)}, u16{static_cast<std::uint16_t>(10)}) == 4);

    static_assert(ilog(u32{UINT32_C(1000000000)}, u32{UINT32_C(10)}) == 9);
    static_assert(ilog(u32{UINT32_C(1048576)}, u32{UINT32_C(2)}) == 20);

    static_assert(ilog(u64{UINT64_C(9223372036854775808)}, u64{UINT64_C(2)}) == 63);

    // Base 5
    static_assert(ilog(u32{UINT32_C(3125)}, u32{UINT32_C(5)}) == 5);   // 5^5

    // Base 7
    static_assert(ilog(u32{UINT32_C(16807)}, u32{UINT32_C(7)}) == 5);  // 7^5
}

int main()
{
    // Exact powers - all types
    test_ilog_exact_powers<u8>();
    test_ilog_exact_powers<u16>();
    test_ilog_exact_powers<u32>();
    test_ilog_exact_powers<u64>();
    test_ilog_exact_powers<u128>();

    // Floor behavior - all types
    test_ilog_floor<u8>();
    test_ilog_floor<u16>();
    test_ilog_floor<u32>();
    test_ilog_floor<u64>();
    test_ilog_floor<u128>();

    // Consistency with log2/log10 - all types
    test_ilog_consistency<u8>();
    test_ilog_consistency<u16>();
    test_ilog_consistency<u32>();
    test_ilog_consistency<u64>();
    test_ilog_consistency<u128>();

    // Type-specific larger values
    test_ilog_u16();
    test_ilog_u32();
    test_ilog_u64();
    test_ilog_u128();

    // Exhaustive u8 for multiple bases
    test_ilog_exhaustive_u8();

    // Domain error: ilog(0, base)
    test_ilog_zero_throws<u8>();
    test_ilog_zero_throws<u16>();
    test_ilog_zero_throws<u32>();
    test_ilog_zero_throws<u64>();
    test_ilog_zero_throws<u128>();

    // Constexpr evaluation
    test_ilog_constexpr();

    return boost::report_errors();
}

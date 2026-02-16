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
// Runtime tests: base cases
// =============================================================================

template <typename T>
void test_ipow_zero_exponent()
{
    using underlying = typename detail::underlying_type_t<T>;

    // x^0 == 1 for any x
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(0)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(1)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(2)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(10)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(255)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(1)});
}

template <typename T>
void test_ipow_one_exponent()
{
    using underlying = typename detail::underlying_type_t<T>;

    // x^1 == x
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(0)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(1)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(2)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(2)});
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(7)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(7)});
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(100)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(100)});
}

template <typename T>
void test_ipow_base_one()
{
    using underlying = typename detail::underlying_type_t<T>;

    // 1^n == 1 for any n
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(1)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(1)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(1)}, T{static_cast<underlying>(5)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(1)}, T{static_cast<underlying>(100)}), T{static_cast<underlying>(1)});
}

template <typename T>
void test_ipow_base_zero()
{
    using underlying = typename detail::underlying_type_t<T>;

    // 0^n == 0 for n > 0
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(0)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(0)}, T{static_cast<underlying>(2)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(0)}, T{static_cast<underlying>(10)}), T{static_cast<underlying>(0)});
}

// =============================================================================
// Runtime tests: known values (small types)
// =============================================================================

template <typename T>
void test_ipow_small_values()
{
    using underlying = typename detail::underlying_type_t<T>;

    // 2^n
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(2)}, T{static_cast<underlying>(2)}), T{static_cast<underlying>(4)});
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(2)}, T{static_cast<underlying>(3)}), T{static_cast<underlying>(8)});
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(2)}, T{static_cast<underlying>(4)}), T{static_cast<underlying>(16)});
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(2)}, T{static_cast<underlying>(5)}), T{static_cast<underlying>(32)});
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(2)}, T{static_cast<underlying>(6)}), T{static_cast<underlying>(64)});
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(2)}, T{static_cast<underlying>(7)}), T{static_cast<underlying>(128)});

    // 3^n
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(3)}, T{static_cast<underlying>(2)}), T{static_cast<underlying>(9)});
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(3)}, T{static_cast<underlying>(3)}), T{static_cast<underlying>(27)});
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(3)}, T{static_cast<underlying>(4)}), T{static_cast<underlying>(81)});

    // 5^n
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(5)}, T{static_cast<underlying>(2)}), T{static_cast<underlying>(25)});
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(5)}, T{static_cast<underlying>(3)}), T{static_cast<underlying>(125)});

    // 10^n
    BOOST_TEST_EQ(ipow(T{static_cast<underlying>(10)}, T{static_cast<underlying>(2)}), T{static_cast<underlying>(100)});
}

// =============================================================================
// Type-specific tests for larger values
// =============================================================================

void test_ipow_u16()
{
    // 2^15 = 32768
    BOOST_TEST_EQ(ipow(u16{static_cast<std::uint16_t>(2)}, u16{static_cast<std::uint16_t>(15)}),
                  u16{static_cast<std::uint16_t>(32768)});

    // 10^3 = 1000
    BOOST_TEST_EQ(ipow(u16{static_cast<std::uint16_t>(10)}, u16{static_cast<std::uint16_t>(3)}),
                  u16{static_cast<std::uint16_t>(1000)});

    // 10^4 = 10000
    BOOST_TEST_EQ(ipow(u16{static_cast<std::uint16_t>(10)}, u16{static_cast<std::uint16_t>(4)}),
                  u16{static_cast<std::uint16_t>(10000)});

    // 7^4 = 2401
    BOOST_TEST_EQ(ipow(u16{static_cast<std::uint16_t>(7)}, u16{static_cast<std::uint16_t>(4)}),
                  u16{static_cast<std::uint16_t>(2401)});
}

void test_ipow_u32()
{
    // 2^20 = 1048576
    BOOST_TEST_EQ(ipow(u32{UINT32_C(2)}, u32{UINT32_C(20)}), u32{UINT32_C(1048576)});

    // 2^31 = 2147483648
    BOOST_TEST_EQ(ipow(u32{UINT32_C(2)}, u32{UINT32_C(31)}), u32{UINT32_C(2147483648)});

    // 10^9 = 1000000000
    BOOST_TEST_EQ(ipow(u32{UINT32_C(10)}, u32{UINT32_C(9)}), u32{UINT32_C(1000000000)});

    // 3^20 = 3486784401
    BOOST_TEST_EQ(ipow(u32{UINT32_C(3)}, u32{UINT32_C(20)}), u32{UINT32_C(3486784401)});

    // 7^11 = 1977326743
    BOOST_TEST_EQ(ipow(u32{UINT32_C(7)}, u32{UINT32_C(11)}), u32{UINT32_C(1977326743)});
}

void test_ipow_u64()
{
    // 2^63
    BOOST_TEST_EQ(ipow(u64{UINT64_C(2)}, u64{UINT64_C(63)}), u64{UINT64_C(9223372036854775808)});

    // 10^18
    BOOST_TEST_EQ(ipow(u64{UINT64_C(10)}, u64{UINT64_C(18)}), u64{UINT64_C(1000000000000000000)});

    // 3^30 = 205891132094649
    BOOST_TEST_EQ(ipow(u64{UINT64_C(3)}, u64{UINT64_C(30)}), u64{UINT64_C(205891132094649)});

    // 5^27 = 7450580596923828125
    BOOST_TEST_EQ(ipow(u64{UINT64_C(5)}, u64{UINT64_C(27)}), u64{UINT64_C(7450580596923828125)});
}

void test_ipow_u128()
{
    using boost::int128::uint128_t;

    // 2^64
    const auto two_pow_64 = uint128_t{UINT64_C(1)} << 64U;
    BOOST_TEST_EQ(ipow(u128{uint128_t{2}}, u128{uint128_t{64}}), u128{two_pow_64});

    // 10^20 = 10^10 * 10^10
    const auto ten_pow_20 = uint128_t{UINT64_C(10000000000)} * uint128_t{UINT64_C(10000000000)};
    BOOST_TEST_EQ(ipow(u128{uint128_t{10}}, u128{uint128_t{20}}), u128{ten_pow_20});

    // 2^100
    const auto two_pow_100 = uint128_t{UINT64_C(1)} << 100U;
    BOOST_TEST_EQ(ipow(u128{uint128_t{2}}, u128{uint128_t{100}}), u128{two_pow_100});

    // Small: 3^3 = 27
    BOOST_TEST_EQ(ipow(u128{uint128_t{3}}, u128{uint128_t{3}}), u128{uint128_t{27}});
}

// =============================================================================
// Exhaustive u8 tests
// =============================================================================

void test_ipow_exhaustive_u8()
{
    // Exhaustively check all (base, exp) pairs that don't overflow u8 (max 255)
    // 2^7 = 128, 2^8 = 256 (overflow)
    for (unsigned base {0}; base <= 255; ++base)
    {
        // exp = 0: always 1
        BOOST_TEST_EQ(ipow(u8{static_cast<std::uint8_t>(base)}, u8{static_cast<std::uint8_t>(0)}),
                      u8{static_cast<std::uint8_t>(1)});

        // exp = 1: always base
        BOOST_TEST_EQ(ipow(u8{static_cast<std::uint8_t>(base)}, u8{static_cast<std::uint8_t>(1)}),
                      u8{static_cast<std::uint8_t>(base)});
    }

    // Check specific small powers that fit in u8
    // 2^n: 2,4,8,16,32,64,128
    for (unsigned exp {1}; exp <= 7; ++exp)
    {
        auto expected {1U};
        for (unsigned i {0}; i < exp; ++i)
        {
            expected *= 2U;
        }
        BOOST_TEST_EQ(ipow(u8{static_cast<std::uint8_t>(2)}, u8{static_cast<std::uint8_t>(exp)}),
                      u8{static_cast<std::uint8_t>(expected)});
    }

    // 3^n: 3,9,27,81,243
    for (unsigned exp {1}; exp <= 5; ++exp)
    {
        auto expected {1U};
        for (unsigned i {0}; i < exp; ++i)
        {
            expected *= 3U;
        }
        BOOST_TEST_EQ(ipow(u8{static_cast<std::uint8_t>(3)}, u8{static_cast<std::uint8_t>(exp)}),
                      u8{static_cast<std::uint8_t>(expected)});
    }

    // 4^n: 4, 16, 64
    for (unsigned exp {1}; exp <= 3; ++exp)
    {
        auto expected {1U};
        for (unsigned i {0}; i < exp; ++i)
        {
            expected *= 4U;
        }
        BOOST_TEST_EQ(ipow(u8{static_cast<std::uint8_t>(4)}, u8{static_cast<std::uint8_t>(exp)}),
                      u8{static_cast<std::uint8_t>(expected)});
    }

    // 5^n: 5, 25, 125
    for (unsigned exp {1}; exp <= 3; ++exp)
    {
        auto expected {1U};
        for (unsigned i {0}; i < exp; ++i)
        {
            expected *= 5U;
        }
        BOOST_TEST_EQ(ipow(u8{static_cast<std::uint8_t>(5)}, u8{static_cast<std::uint8_t>(exp)}),
                      u8{static_cast<std::uint8_t>(expected)});
    }

    // 15^2 = 225
    BOOST_TEST_EQ(ipow(u8{static_cast<std::uint8_t>(15)}, u8{static_cast<std::uint8_t>(2)}),
                  u8{static_cast<std::uint8_t>(225)});
}

// =============================================================================
// Constexpr tests
// =============================================================================

void test_ipow_constexpr()
{
    static_assert(ipow(u8{static_cast<std::uint8_t>(2)}, u8{static_cast<std::uint8_t>(7)}) == u8{static_cast<std::uint8_t>(128)});
    static_assert(ipow(u8{static_cast<std::uint8_t>(3)}, u8{static_cast<std::uint8_t>(5)}) == u8{static_cast<std::uint8_t>(243)});
    static_assert(ipow(u8{static_cast<std::uint8_t>(10)}, u8{static_cast<std::uint8_t>(0)}) == u8{static_cast<std::uint8_t>(1)});

    static_assert(ipow(u16{static_cast<std::uint16_t>(10)}, u16{static_cast<std::uint16_t>(4)}) == u16{static_cast<std::uint16_t>(10000)});

    static_assert(ipow(u32{UINT32_C(10)}, u32{UINT32_C(9)}) == u32{UINT32_C(1000000000)});
    static_assert(ipow(u32{UINT32_C(2)}, u32{UINT32_C(20)}) == u32{UINT32_C(1048576)});

    static_assert(ipow(u64{UINT64_C(2)}, u64{UINT64_C(63)}) == u64{UINT64_C(9223372036854775808)});
    static_assert(ipow(u64{UINT64_C(10)}, u64{UINT64_C(18)}) == u64{UINT64_C(1000000000000000000)});
}

// =============================================================================
// Verified type tests (consteval)
// =============================================================================

void test_ipow_verified()
{
    static_assert(ipow(verified_u8{u8{static_cast<std::uint8_t>(2)}},
                       verified_u8{u8{static_cast<std::uint8_t>(7)}}) == u8{static_cast<std::uint8_t>(128)});
    static_assert(ipow(verified_u8{u8{static_cast<std::uint8_t>(5)}},
                       verified_u8{u8{static_cast<std::uint8_t>(3)}}) == u8{static_cast<std::uint8_t>(125)});

    static_assert(ipow(verified_u16{u16{static_cast<std::uint16_t>(10)}},
                       verified_u16{u16{static_cast<std::uint16_t>(4)}}) == u16{static_cast<std::uint16_t>(10000)});

    static_assert(ipow(verified_u32{u32{UINT32_C(10)}},
                       verified_u32{u32{UINT32_C(9)}}) == u32{UINT32_C(1000000000)});

    static_assert(ipow(verified_u64{u64{UINT64_C(2)}},
                       verified_u64{u64{UINT64_C(40)}}) == u64{UINT64_C(1099511627776)});

    using boost::int128::uint128_t;
    static_assert(ipow(verified_u128{u128{uint128_t{2}}},
                       verified_u128{u128{uint128_t{10}}}) == u128{uint128_t{1024}});
}

int main()
{
    // Base cases - all types
    test_ipow_zero_exponent<u8>();
    test_ipow_zero_exponent<u16>();
    test_ipow_zero_exponent<u32>();
    test_ipow_zero_exponent<u64>();
    test_ipow_zero_exponent<u128>();

    test_ipow_one_exponent<u8>();
    test_ipow_one_exponent<u16>();
    test_ipow_one_exponent<u32>();
    test_ipow_one_exponent<u64>();
    test_ipow_one_exponent<u128>();

    test_ipow_base_one<u8>();
    test_ipow_base_one<u16>();
    test_ipow_base_one<u32>();
    test_ipow_base_one<u64>();
    test_ipow_base_one<u128>();

    test_ipow_base_zero<u8>();
    test_ipow_base_zero<u16>();
    test_ipow_base_zero<u32>();
    test_ipow_base_zero<u64>();
    test_ipow_base_zero<u128>();

    // Known values - all types
    test_ipow_small_values<u8>();
    test_ipow_small_values<u16>();
    test_ipow_small_values<u32>();
    test_ipow_small_values<u64>();
    test_ipow_small_values<u128>();

    // Type-specific larger values
    test_ipow_u16();
    test_ipow_u32();
    test_ipow_u64();
    test_ipow_u128();

    // Exhaustive u8
    test_ipow_exhaustive_u8();

    // Constexpr evaluation
    test_ipow_constexpr();

    // Verified types (consteval)
    test_ipow_verified();

    return boost::report_errors();
}

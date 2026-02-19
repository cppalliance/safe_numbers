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
// Runtime tests: midpoint(a, a) == a
// =============================================================================

template <typename T>
void test_midpoint_equal()
{
    using underlying = typename detail::underlying_type_t<T>;

    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(0)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(1)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(7)}, T{static_cast<underlying>(7)}), T{static_cast<underlying>(7)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(42)}, T{static_cast<underlying>(42)}), T{static_cast<underlying>(42)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(100)}, T{static_cast<underlying>(100)}), T{static_cast<underlying>(100)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(255)}, T{static_cast<underlying>(255)}), T{static_cast<underlying>(255)});
}

// =============================================================================
// Runtime tests: midpoint with zero
// =============================================================================

template <typename T>
void test_midpoint_zero()
{
    using underlying = typename detail::underlying_type_t<T>;

    // midpoint(0, n) == n/2 (rounds towards 0, the first arg)
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(0)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(0)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(0)}, T{static_cast<underlying>(2)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(0)}, T{static_cast<underlying>(10)}), T{static_cast<underlying>(5)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(0)}, T{static_cast<underlying>(11)}), T{static_cast<underlying>(5)});

    // midpoint(n, 0) == (n+1)/2 (rounds towards n, the first arg)
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(1)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(2)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(10)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(5)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(11)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(6)});
}

// =============================================================================
// Runtime tests: even distance (exact midpoint)
// =============================================================================

template <typename T>
void test_midpoint_even_distance()
{
    using underlying = typename detail::underlying_type_t<T>;

    // When b - a is even, midpoint is exact regardless of direction
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(0)}, T{static_cast<underlying>(10)}), T{static_cast<underlying>(5)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(10)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(5)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(2)}, T{static_cast<underlying>(8)}), T{static_cast<underlying>(5)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(8)}, T{static_cast<underlying>(2)}), T{static_cast<underlying>(5)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(10)}, T{static_cast<underlying>(20)}), T{static_cast<underlying>(15)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(20)}, T{static_cast<underlying>(10)}), T{static_cast<underlying>(15)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(100)}, T{static_cast<underlying>(200)}), T{static_cast<underlying>(150)});
}

// =============================================================================
// Runtime tests: odd distance (rounds towards first argument)
// =============================================================================

template <typename T>
void test_midpoint_rounding()
{
    using underlying = typename detail::underlying_type_t<T>;

    // midpoint(1, 4) = 2 (rounds down towards a=1)
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(1)}, T{static_cast<underlying>(4)}), T{static_cast<underlying>(2)});
    // midpoint(4, 1) = 3 (rounds up towards a=4)
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(4)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(3)});

    // midpoint(0, 3) = 1 (rounds towards 0)
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(0)}, T{static_cast<underlying>(3)}), T{static_cast<underlying>(1)});
    // midpoint(3, 0) = 2 (rounds towards 3)
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(3)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(2)});

    // midpoint(10, 15) = 12 (rounds towards 10)
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(10)}, T{static_cast<underlying>(15)}), T{static_cast<underlying>(12)});
    // midpoint(15, 10) = 13 (rounds towards 15)
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(15)}, T{static_cast<underlying>(10)}), T{static_cast<underlying>(13)});

    // midpoint(0, 1) = 0 (rounds towards 0)
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(0)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(0)});
    // midpoint(1, 0) = 1 (rounds towards 1)
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(1)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(1)});
}

// =============================================================================
// Runtime tests: adjacent values
// =============================================================================

template <typename T>
void test_midpoint_adjacent()
{
    using underlying = typename detail::underlying_type_t<T>;

    // midpoint(n, n+1) == n (rounds towards a)
    // midpoint(n+1, n) == n+1 (rounds towards a)
    for (unsigned i {0}; i < 100; ++i)
    {
        const auto a = T{static_cast<underlying>(i)};
        const auto b = T{static_cast<underlying>(i + 1)};
        BOOST_TEST_EQ(midpoint(a, b), a);
        BOOST_TEST_EQ(midpoint(b, a), b);
    }
}

// =============================================================================
// Type-specific tests for larger values
// =============================================================================

void test_midpoint_u16()
{
    // Max value tests
    BOOST_TEST_EQ(midpoint(u16{static_cast<std::uint16_t>(0)}, u16{static_cast<std::uint16_t>(65534)}),
                  u16{static_cast<std::uint16_t>(32767)});
    BOOST_TEST_EQ(midpoint(u16{static_cast<std::uint16_t>(0)}, u16{static_cast<std::uint16_t>(65535)}),
                  u16{static_cast<std::uint16_t>(32767)});

    // Large values: no overflow
    BOOST_TEST_EQ(midpoint(u16{static_cast<std::uint16_t>(60000)}, u16{static_cast<std::uint16_t>(65000)}),
                  u16{static_cast<std::uint16_t>(62500)});
    BOOST_TEST_EQ(midpoint(u16{static_cast<std::uint16_t>(32000)}, u16{static_cast<std::uint16_t>(33000)}),
                  u16{static_cast<std::uint16_t>(32500)});
}

void test_midpoint_u32()
{
    // Values near max: would overflow with naive (a+b)/2
    BOOST_TEST_EQ(midpoint(u32{UINT32_C(4000000000)}, u32{UINT32_C(4294967295)}),
                  u32{UINT32_C(4147483647)});

    BOOST_TEST_EQ(midpoint(u32{UINT32_C(0)}, u32{UINT32_C(4294967294)}),
                  u32{UINT32_C(2147483647)});

    BOOST_TEST_EQ(midpoint(u32{UINT32_C(1000000000)}, u32{UINT32_C(3000000000)}),
                  u32{UINT32_C(2000000000)});

    // Rounding with large values
    BOOST_TEST_EQ(midpoint(u32{UINT32_C(0)}, u32{UINT32_C(4294967295)}),
                  u32{UINT32_C(2147483647)});
    BOOST_TEST_EQ(midpoint(u32{UINT32_C(4294967295)}, u32{UINT32_C(0)}),
                  u32{UINT32_C(2147483648)});
}

void test_midpoint_u64()
{
    // Values near max: would overflow with naive (a+b)/2
    BOOST_TEST_EQ(midpoint(u64{UINT64_C(18000000000000000000)}, u64{UINT64_C(18446744073709551615)}),
                  u64{UINT64_C(18223372036854775807)});

    BOOST_TEST_EQ(midpoint(u64{UINT64_C(0)}, u64{UINT64_C(18446744073709551614)}),
                  u64{UINT64_C(9223372036854775807)});

    BOOST_TEST_EQ(midpoint(u64{UINT64_C(1000000000000000000)}, u64{UINT64_C(9000000000000000000)}),
                  u64{UINT64_C(5000000000000000000)});

    // Rounding with max
    BOOST_TEST_EQ(midpoint(u64{UINT64_C(0)}, u64{UINT64_C(18446744073709551615)}),
                  u64{UINT64_C(9223372036854775807)});
    BOOST_TEST_EQ(midpoint(u64{UINT64_C(18446744073709551615)}, u64{UINT64_C(0)}),
                  u64{UINT64_C(9223372036854775808)});
}

void test_midpoint_u128()
{
    using boost::int128::uint128_t;

    // Small values
    BOOST_TEST_EQ(midpoint(u128{uint128_t{0}}, u128{uint128_t{10}}), u128{uint128_t{5}});
    BOOST_TEST_EQ(midpoint(u128{uint128_t{1}}, u128{uint128_t{4}}), u128{uint128_t{2}});
    BOOST_TEST_EQ(midpoint(u128{uint128_t{4}}, u128{uint128_t{1}}), u128{uint128_t{3}});

    // Large values: powers of 2
    const auto two_pow_64 = uint128_t{UINT64_C(1)} << 64U;
    const auto two_pow_63 = uint128_t{UINT64_C(1)} << 63U;
    BOOST_TEST_EQ(midpoint(u128{uint128_t{0}}, u128{two_pow_64}), u128{two_pow_63});

    // Equal large values
    const auto big_val = uint128_t{UINT64_C(1000000000000)} * uint128_t{UINT64_C(1000000000000)};
    BOOST_TEST_EQ(midpoint(u128{big_val}, u128{big_val}), u128{big_val});
}

// =============================================================================
// Exhaustive u8 tests
// =============================================================================

void test_midpoint_exhaustive_u8()
{
    for (unsigned a {0}; a <= 255; ++a)
    {
        for (unsigned b {0}; b <= 255; ++b)
        {
            // Reference: midpoint rounds towards a
            // For unsigned: if a <= b, result = a + (b - a) / 2
            //               if a > b,  result = a - (a - b) / 2
            unsigned ref_result;
            if (a <= b)
            {
                ref_result = a + (b - a) / 2;
            }
            else
            {
                ref_result = a - (a - b) / 2;
            }

            BOOST_TEST_EQ(midpoint(u8{static_cast<std::uint8_t>(a)}, u8{static_cast<std::uint8_t>(b)}),
                          u8{static_cast<std::uint8_t>(ref_result)});
        }
    }
}

// =============================================================================
// Constexpr tests
// =============================================================================

void test_midpoint_constexpr()
{
    static_assert(midpoint(u8{static_cast<std::uint8_t>(0)}, u8{static_cast<std::uint8_t>(10)}) == u8{static_cast<std::uint8_t>(5)});
    static_assert(midpoint(u8{static_cast<std::uint8_t>(1)}, u8{static_cast<std::uint8_t>(4)}) == u8{static_cast<std::uint8_t>(2)});
    static_assert(midpoint(u8{static_cast<std::uint8_t>(4)}, u8{static_cast<std::uint8_t>(1)}) == u8{static_cast<std::uint8_t>(3)});
    static_assert(midpoint(u8{static_cast<std::uint8_t>(255)}, u8{static_cast<std::uint8_t>(255)}) == u8{static_cast<std::uint8_t>(255)});

    static_assert(midpoint(u16{static_cast<std::uint16_t>(0)}, u16{static_cast<std::uint16_t>(65534)}) == u16{static_cast<std::uint16_t>(32767)});

    static_assert(midpoint(u32{UINT32_C(0)}, u32{UINT32_C(4294967294)}) == u32{UINT32_C(2147483647)});
    static_assert(midpoint(u32{UINT32_C(100)}, u32{UINT32_C(200)}) == u32{UINT32_C(150)});

    static_assert(midpoint(u64{UINT64_C(0)}, u64{UINT64_C(18446744073709551614)}) == u64{UINT64_C(9223372036854775807)});
    static_assert(midpoint(u64{UINT64_C(1000)}, u64{UINT64_C(3000)}) == u64{UINT64_C(2000)});
}

// =============================================================================
// Verified type tests (consteval)
// =============================================================================

void test_midpoint_verified()
{
    // verified_u8
    static_assert(midpoint(verified_u8{u8{static_cast<std::uint8_t>(0)}},
                           verified_u8{u8{static_cast<std::uint8_t>(10)}}) == verified_u8{u8{static_cast<std::uint8_t>(5)}});
    static_assert(midpoint(verified_u8{u8{static_cast<std::uint8_t>(1)}},
                           verified_u8{u8{static_cast<std::uint8_t>(4)}}) == verified_u8{u8{static_cast<std::uint8_t>(2)}});
    static_assert(midpoint(verified_u8{u8{static_cast<std::uint8_t>(4)}},
                           verified_u8{u8{static_cast<std::uint8_t>(1)}}) == verified_u8{u8{static_cast<std::uint8_t>(3)}});

    // verified_u16
    static_assert(midpoint(verified_u16{u16{static_cast<std::uint16_t>(0)}},
                           verified_u16{u16{static_cast<std::uint16_t>(65534)}}) == verified_u16{u16{static_cast<std::uint16_t>(32767)}});

    // verified_u32
    static_assert(midpoint(verified_u32{u32{UINT32_C(100)}},
                           verified_u32{u32{UINT32_C(200)}}) == verified_u32{u32{UINT32_C(150)}});

    // verified_u64
    static_assert(midpoint(verified_u64{u64{UINT64_C(1000)}},
                           verified_u64{u64{UINT64_C(3000)}}) == verified_u64{u64{UINT64_C(2000)}});

    // verified_u128
    using boost::int128::uint128_t;
    static_assert(midpoint(verified_u128{u128{uint128_t{0}}},
                           verified_u128{u128{uint128_t{10}}}) == verified_u128{u128{uint128_t{5}}});
    static_assert(midpoint(verified_u128{u128{uint128_t{4}}},
                           verified_u128{u128{uint128_t{1}}}) == verified_u128{u128{uint128_t{3}}});
}

int main()
{
    // Equal values - all types
    test_midpoint_equal<u8>();
    test_midpoint_equal<u16>();
    test_midpoint_equal<u32>();
    test_midpoint_equal<u64>();
    test_midpoint_equal<u128>();

    // Zero cases - all types
    test_midpoint_zero<u8>();
    test_midpoint_zero<u16>();
    test_midpoint_zero<u32>();
    test_midpoint_zero<u64>();
    test_midpoint_zero<u128>();

    // Even distance - all types
    test_midpoint_even_distance<u8>();
    test_midpoint_even_distance<u16>();
    test_midpoint_even_distance<u32>();
    test_midpoint_even_distance<u64>();
    test_midpoint_even_distance<u128>();

    // Rounding behaviour - all types
    test_midpoint_rounding<u8>();
    test_midpoint_rounding<u16>();
    test_midpoint_rounding<u32>();
    test_midpoint_rounding<u64>();
    test_midpoint_rounding<u128>();

    // Adjacent values - all types
    test_midpoint_adjacent<u8>();
    test_midpoint_adjacent<u16>();
    test_midpoint_adjacent<u32>();
    test_midpoint_adjacent<u64>();
    test_midpoint_adjacent<u128>();

    // Type-specific larger values
    test_midpoint_u16();
    test_midpoint_u32();
    test_midpoint_u64();
    test_midpoint_u128();

    // Exhaustive u8
    test_midpoint_exhaustive_u8();

    // Constexpr evaluation
    test_midpoint_constexpr();

    // Verified types (consteval)
    test_midpoint_verified();

    return boost::report_errors();
}

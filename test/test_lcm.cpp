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
// Runtime tests: lcm(0, x) and lcm(x, 0)
// =============================================================================

template <typename T>
void test_lcm_zero()
{
    using underlying = typename detail::underlying_type_t<T>;

    // lcm(0, 0) == 0
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(0)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(0)});

    // lcm(0, n) == 0
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(0)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(0)}, T{static_cast<underlying>(7)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(0)}, T{static_cast<underlying>(42)}), T{static_cast<underlying>(0)});

    // lcm(n, 0) == 0
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(1)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(7)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(42)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(0)});
}

// =============================================================================
// Runtime tests: lcm with 1
// =============================================================================

template <typename T>
void test_lcm_one()
{
    using underlying = typename detail::underlying_type_t<T>;

    // lcm(1, n) == n
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(1)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(1)}, T{static_cast<underlying>(2)}), T{static_cast<underlying>(2)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(1)}, T{static_cast<underlying>(100)}), T{static_cast<underlying>(100)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(1)}, T{static_cast<underlying>(255)}), T{static_cast<underlying>(255)});

    // lcm(n, 1) == n
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(2)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(2)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(100)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(100)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(255)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(255)});
}

// =============================================================================
// Runtime tests: lcm equal values
// =============================================================================

template <typename T>
void test_lcm_equal()
{
    using underlying = typename detail::underlying_type_t<T>;

    // lcm(n, n) == n
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(1)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(2)}, T{static_cast<underlying>(2)}), T{static_cast<underlying>(2)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(7)}, T{static_cast<underlying>(7)}), T{static_cast<underlying>(7)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(42)}, T{static_cast<underlying>(42)}), T{static_cast<underlying>(42)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(100)}, T{static_cast<underlying>(100)}), T{static_cast<underlying>(100)});
}

// =============================================================================
// Runtime tests: known values (small types)
// =============================================================================

template <typename T>
void test_lcm_known_values()
{
    using underlying = typename detail::underlying_type_t<T>;

    // Classic examples
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(4)}, T{static_cast<underlying>(6)}), T{static_cast<underlying>(12)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(6)}, T{static_cast<underlying>(8)}), T{static_cast<underlying>(24)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(12)}, T{static_cast<underlying>(18)}), T{static_cast<underlying>(36)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(15)}, T{static_cast<underlying>(20)}), T{static_cast<underlying>(60)});

    // Coprime pairs: lcm(a, b) == a * b
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(3)}, T{static_cast<underlying>(5)}), T{static_cast<underlying>(15)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(7)}, T{static_cast<underlying>(11)}), T{static_cast<underlying>(77)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(13)}, T{static_cast<underlying>(17)}), T{static_cast<underlying>(221)});

    // Powers of 2
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(8)}, T{static_cast<underlying>(4)}), T{static_cast<underlying>(8)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(16)}, T{static_cast<underlying>(64)}), T{static_cast<underlying>(64)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(32)}, T{static_cast<underlying>(128)}), T{static_cast<underlying>(128)});

    // One divides the other: lcm(a, b) == max(a, b)
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(6)}, T{static_cast<underlying>(12)}), T{static_cast<underlying>(12)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(15)}, T{static_cast<underlying>(45)}), T{static_cast<underlying>(45)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(25)}, T{static_cast<underlying>(100)}), T{static_cast<underlying>(100)});

    // Commutativity
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(4)}, T{static_cast<underlying>(6)}),
                  lcm(T{static_cast<underlying>(6)}, T{static_cast<underlying>(4)}));
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(12)}, T{static_cast<underlying>(18)}),
                  lcm(T{static_cast<underlying>(18)}, T{static_cast<underlying>(12)}));
}

// =============================================================================
// Type-specific tests for larger values
// =============================================================================

void test_lcm_u16()
{
    BOOST_TEST_EQ(lcm(u16{static_cast<std::uint16_t>(100)}, u16{static_cast<std::uint16_t>(75)}),
                  u16{static_cast<std::uint16_t>(300)});
    BOOST_TEST_EQ(lcm(u16{static_cast<std::uint16_t>(120)}, u16{static_cast<std::uint16_t>(90)}),
                  u16{static_cast<std::uint16_t>(360)});
    BOOST_TEST_EQ(lcm(u16{static_cast<std::uint16_t>(256)}, u16{static_cast<std::uint16_t>(192)}),
                  u16{static_cast<std::uint16_t>(768)});
    BOOST_TEST_EQ(lcm(u16{static_cast<std::uint16_t>(17)}, u16{static_cast<std::uint16_t>(19)}),
                  u16{static_cast<std::uint16_t>(323)});
}

void test_lcm_u32()
{
    BOOST_TEST_EQ(lcm(u32{UINT32_C(1000)}, u32{UINT32_C(750)}), u32{UINT32_C(3000)});
    BOOST_TEST_EQ(lcm(u32{UINT32_C(12345)}, u32{UINT32_C(6789)}), u32{UINT32_C(27936735)});
    BOOST_TEST_EQ(lcm(u32{UINT32_C(100000)}, u32{UINT32_C(12500)}), u32{UINT32_C(100000)});

    // Coprime Fibonacci numbers
    BOOST_TEST_EQ(lcm(u32{UINT32_C(46368)}, u32{UINT32_C(28657)}),
                  u32{UINT32_C(46368)} * u32{UINT32_C(28657)});
}

void test_lcm_u64()
{
    BOOST_TEST_EQ(lcm(u64{UINT64_C(1000000)}, u64{UINT64_C(750000)}), u64{UINT64_C(3000000)});
    BOOST_TEST_EQ(lcm(u64{UINT64_C(123456789)}, u64{UINT64_C(987654321)}),
                  u64{UINT64_C(13548070123626141)});

    // Coprime consecutive Fibonacci
    BOOST_TEST_EQ(lcm(u64{UINT64_C(1346269)}, u64{UINT64_C(832040)}),
                  u64{UINT64_C(1346269)} * u64{UINT64_C(832040)});

    // Powers of 2
    BOOST_TEST_EQ(lcm(u64{UINT64_C(4611686018427387904)}, u64{UINT64_C(2305843009213693952)}),
                  u64{UINT64_C(4611686018427387904)});
}

void test_lcm_u128()
{
    using boost::int128::uint128_t;

    // Small values
    BOOST_TEST_EQ(lcm(u128{uint128_t{4}}, u128{uint128_t{6}}), u128{uint128_t{12}});
    BOOST_TEST_EQ(lcm(u128{uint128_t{12}}, u128{uint128_t{18}}), u128{uint128_t{36}});

    // Large values: powers of 2
    const auto two_pow_64 = uint128_t{UINT64_C(1)} << 64U;
    const auto two_pow_32 = uint128_t{UINT64_C(1)} << 32U;
    BOOST_TEST_EQ(lcm(u128{two_pow_64}, u128{two_pow_32}), u128{two_pow_64});

    // Coprime pair
    BOOST_TEST_EQ(lcm(u128{uint128_t{7}}, u128{uint128_t{11}}), u128{uint128_t{77}});
}

// =============================================================================
// Exhaustive u8 tests
// =============================================================================

void test_lcm_exhaustive_u8()
{
    // Check all (m, n) pairs for u8 where lcm fits in u8
    for (unsigned m {0}; m <= 255; ++m)
    {
        for (unsigned n {0}; n <= 255; ++n)
        {
            // Reference lcm: lcm(a, b) = a / gcd(a, b) * b (avoids overflow)
            unsigned ref_result {0};
            if (m == 0 || n == 0)
            {
                ref_result = 0;
            }
            else
            {
                // Compute gcd
                auto ref_a = m;
                auto ref_b = n;
                while (ref_b != 0)
                {
                    auto t = ref_b;
                    ref_b = ref_a % ref_b;
                    ref_a = t;
                }
                ref_result = m / ref_a * n;
            }

            if (ref_result <= 255)
            {
                BOOST_TEST_EQ(lcm(u8{static_cast<std::uint8_t>(m)}, u8{static_cast<std::uint8_t>(n)}),
                              u8{static_cast<std::uint8_t>(ref_result)});
            }
        }
    }
}

// =============================================================================
// Property tests: lcm(a, b) * gcd(a, b) == a * b
// =============================================================================

template <typename T>
void test_lcm_gcd_identity()
{
    using underlying = typename detail::underlying_type_t<T>;

    // lcm(a, b) * gcd(a, b) == a * b (for small values that won't overflow)
    const underlying test_pairs[][2] = {
        {4, 6}, {12, 8}, {15, 20}, {7, 11}, {3, 5}, {6, 12}, {10, 25}, {9, 15}
    };

    for (const auto& pair : test_pairs)
    {
        const auto a = T{pair[0]};
        const auto b = T{pair[1]};
        BOOST_TEST_EQ(lcm(a, b) * gcd(a, b), a * b);
    }
}

// =============================================================================
// Constexpr tests
// =============================================================================

void test_lcm_constexpr()
{
    static_assert(lcm(u8{static_cast<std::uint8_t>(4)}, u8{static_cast<std::uint8_t>(6)}) == u8{static_cast<std::uint8_t>(12)});
    static_assert(lcm(u8{static_cast<std::uint8_t>(0)}, u8{static_cast<std::uint8_t>(5)}) == u8{static_cast<std::uint8_t>(0)});
    static_assert(lcm(u8{static_cast<std::uint8_t>(7)}, u8{static_cast<std::uint8_t>(7)}) == u8{static_cast<std::uint8_t>(7)});

    static_assert(lcm(u16{static_cast<std::uint16_t>(100)}, u16{static_cast<std::uint16_t>(75)}) == u16{static_cast<std::uint16_t>(300)});

    static_assert(lcm(u32{UINT32_C(1000)}, u32{UINT32_C(750)}) == u32{UINT32_C(3000)});
    static_assert(lcm(u32{UINT32_C(12)}, u32{UINT32_C(18)}) == u32{UINT32_C(36)});

    static_assert(lcm(u64{UINT64_C(1000000)}, u64{UINT64_C(750000)}) == u64{UINT64_C(3000000)});
    static_assert(lcm(u64{UINT64_C(7)}, u64{UINT64_C(11)}) == u64{UINT64_C(77)});
}

// =============================================================================
// Verified type tests (consteval)
// =============================================================================

void test_lcm_verified()
{
    // verified_u8
    static_assert(lcm(verified_u8{u8{static_cast<std::uint8_t>(4)}},
                      verified_u8{u8{static_cast<std::uint8_t>(6)}}) == verified_u8{u8{static_cast<std::uint8_t>(12)}});
    static_assert(lcm(verified_u8{u8{static_cast<std::uint8_t>(0)}},
                      verified_u8{u8{static_cast<std::uint8_t>(5)}}) == verified_u8{u8{static_cast<std::uint8_t>(0)}});
    static_assert(lcm(verified_u8{u8{static_cast<std::uint8_t>(7)}},
                      verified_u8{u8{static_cast<std::uint8_t>(11)}}) == verified_u8{u8{static_cast<std::uint8_t>(77)}});

    // verified_u16
    static_assert(lcm(verified_u16{u16{static_cast<std::uint16_t>(100)}},
                      verified_u16{u16{static_cast<std::uint16_t>(75)}}) == verified_u16{u16{static_cast<std::uint16_t>(300)}});

    // verified_u32
    static_assert(lcm(verified_u32{u32{UINT32_C(12)}},
                      verified_u32{u32{UINT32_C(18)}}) == verified_u32{u32{UINT32_C(36)}});

    // verified_u64
    static_assert(lcm(verified_u64{u64{UINT64_C(1000000)}},
                      verified_u64{u64{UINT64_C(750000)}}) == verified_u64{u64{UINT64_C(3000000)}});

    // verified_u128
    using boost::int128::uint128_t;
    static_assert(lcm(verified_u128{u128{uint128_t{4}}},
                      verified_u128{u128{uint128_t{6}}}) == verified_u128{u128{uint128_t{12}}});
    static_assert(lcm(verified_u128{u128{uint128_t{7}}},
                      verified_u128{u128{uint128_t{11}}}) == verified_u128{u128{uint128_t{77}}});
}

int main()
{
    // Zero cases - all types
    test_lcm_zero<u8>();
    test_lcm_zero<u16>();
    test_lcm_zero<u32>();
    test_lcm_zero<u64>();
    test_lcm_zero<u128>();

    // One cases - all types
    test_lcm_one<u8>();
    test_lcm_one<u16>();
    test_lcm_one<u32>();
    test_lcm_one<u64>();
    test_lcm_one<u128>();

    // Equal values - all types
    test_lcm_equal<u8>();
    test_lcm_equal<u16>();
    test_lcm_equal<u32>();
    test_lcm_equal<u64>();
    test_lcm_equal<u128>();

    // Known values - all types
    test_lcm_known_values<u8>();
    test_lcm_known_values<u16>();
    test_lcm_known_values<u32>();
    test_lcm_known_values<u64>();
    test_lcm_known_values<u128>();

    // Type-specific larger values
    test_lcm_u16();
    test_lcm_u32();
    test_lcm_u64();
    test_lcm_u128();

    // Exhaustive u8
    test_lcm_exhaustive_u8();

    // Property: lcm(a,b) * gcd(a,b) == a * b
    // Skip u8 because a * b easily overflows the type
    test_lcm_gcd_identity<u16>();
    test_lcm_gcd_identity<u32>();
    test_lcm_gcd_identity<u64>();
    test_lcm_gcd_identity<u128>();

    // Constexpr evaluation
    test_lcm_constexpr();

    // Verified types (consteval)
    test_lcm_verified();

    return boost::report_errors();
}

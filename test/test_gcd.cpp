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
// Runtime tests: gcd(0, x) and gcd(x, 0)
// =============================================================================

template <typename T>
void test_gcd_zero()
{
    using underlying = typename detail::underlying_type_t<T>;

    // gcd(0, 0) == 0
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(0)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(0)});

    // gcd(0, n) == n
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(0)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(0)}, T{static_cast<underlying>(7)}), T{static_cast<underlying>(7)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(0)}, T{static_cast<underlying>(42)}), T{static_cast<underlying>(42)});

    // gcd(n, 0) == n
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(1)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(7)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(7)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(42)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(42)});
}

// =============================================================================
// Runtime tests: gcd with 1
// =============================================================================

template <typename T>
void test_gcd_one()
{
    using underlying = typename detail::underlying_type_t<T>;

    // gcd(1, n) == 1 for any n
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(1)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(1)}, T{static_cast<underlying>(2)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(1)}, T{static_cast<underlying>(100)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(1)}, T{static_cast<underlying>(255)}), T{static_cast<underlying>(1)});

    // gcd(n, 1) == 1 for any n
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(2)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(100)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(255)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(1)});
}

// =============================================================================
// Runtime tests: gcd equal values
// =============================================================================

template <typename T>
void test_gcd_equal()
{
    using underlying = typename detail::underlying_type_t<T>;

    // gcd(n, n) == n
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(1)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(2)}, T{static_cast<underlying>(2)}), T{static_cast<underlying>(2)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(7)}, T{static_cast<underlying>(7)}), T{static_cast<underlying>(7)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(42)}, T{static_cast<underlying>(42)}), T{static_cast<underlying>(42)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(100)}, T{static_cast<underlying>(100)}), T{static_cast<underlying>(100)});
}

// =============================================================================
// Runtime tests: known values (small types)
// =============================================================================

template <typename T>
void test_gcd_known_values()
{
    using underlying = typename detail::underlying_type_t<T>;

    // Classic examples
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(12)}, T{static_cast<underlying>(8)}), T{static_cast<underlying>(4)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(54)}, T{static_cast<underlying>(24)}), T{static_cast<underlying>(6)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(48)}, T{static_cast<underlying>(18)}), T{static_cast<underlying>(6)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(56)}, T{static_cast<underlying>(98)}), T{static_cast<underlying>(14)});

    // Coprime pairs
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(3)}, T{static_cast<underlying>(5)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(7)}, T{static_cast<underlying>(11)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(13)}, T{static_cast<underlying>(17)}), T{static_cast<underlying>(1)});

    // Powers of 2
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(8)}, T{static_cast<underlying>(4)}), T{static_cast<underlying>(4)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(16)}, T{static_cast<underlying>(64)}), T{static_cast<underlying>(16)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(32)}, T{static_cast<underlying>(128)}), T{static_cast<underlying>(32)});

    // One divides the other
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(6)}, T{static_cast<underlying>(12)}), T{static_cast<underlying>(6)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(15)}, T{static_cast<underlying>(45)}), T{static_cast<underlying>(15)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(100)}, T{static_cast<underlying>(25)}), T{static_cast<underlying>(25)});

    // Commutativity
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(12)}, T{static_cast<underlying>(8)}),
                  gcd(T{static_cast<underlying>(8)}, T{static_cast<underlying>(12)}));
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(54)}, T{static_cast<underlying>(24)}),
                  gcd(T{static_cast<underlying>(24)}, T{static_cast<underlying>(54)}));
}

// =============================================================================
// Type-specific tests for larger values
// =============================================================================

void test_gcd_u16()
{
    BOOST_TEST_EQ(gcd(u16{static_cast<std::uint16_t>(1000)}, u16{static_cast<std::uint16_t>(750)}),
                  u16{static_cast<std::uint16_t>(250)});
    BOOST_TEST_EQ(gcd(u16{static_cast<std::uint16_t>(10000)}, u16{static_cast<std::uint16_t>(15000)}),
                  u16{static_cast<std::uint16_t>(5000)});
    BOOST_TEST_EQ(gcd(u16{static_cast<std::uint16_t>(32768)}, u16{static_cast<std::uint16_t>(16384)}),
                  u16{static_cast<std::uint16_t>(16384)});
    BOOST_TEST_EQ(gcd(u16{static_cast<std::uint16_t>(12345)}, u16{static_cast<std::uint16_t>(6789)}),
                  u16{static_cast<std::uint16_t>(3)});
}

void test_gcd_u32()
{
    BOOST_TEST_EQ(gcd(u32{UINT32_C(1000000)}, u32{UINT32_C(750000)}), u32{UINT32_C(250000)});
    BOOST_TEST_EQ(gcd(u32{UINT32_C(2147483648)}, u32{UINT32_C(1073741824)}), u32{UINT32_C(1073741824)});
    BOOST_TEST_EQ(gcd(u32{UINT32_C(1234567890)}, u32{UINT32_C(987654321)}), u32{UINT32_C(9)});
    BOOST_TEST_EQ(gcd(u32{UINT32_C(100000000)}, u32{UINT32_C(12500000)}), u32{UINT32_C(12500000)});

    // Fibonacci-adjacent (stress test for Euclidean algorithm)
    BOOST_TEST_EQ(gcd(u32{UINT32_C(46368)}, u32{UINT32_C(28657)}), u32{UINT32_C(1)});
}

void test_gcd_u64()
{
    BOOST_TEST_EQ(gcd(u64{UINT64_C(1000000000000)}, u64{UINT64_C(750000000000)}), u64{UINT64_C(250000000000)});
    BOOST_TEST_EQ(gcd(u64{UINT64_C(9223372036854775808)}, u64{UINT64_C(4611686018427387904)}),
                  u64{UINT64_C(4611686018427387904)});
    BOOST_TEST_EQ(gcd(u64{UINT64_C(123456789012345)}, u64{UINT64_C(987654321098765)}),
                  u64{UINT64_C(5)});

    // Large Fibonacci numbers are coprime to each other (consecutive Fibonacci)
    BOOST_TEST_EQ(gcd(u64{UINT64_C(1346269)}, u64{UINT64_C(832040)}), u64{UINT64_C(1)});
}

void test_gcd_u128()
{
    using boost::int128::uint128_t;

    // Small values
    BOOST_TEST_EQ(gcd(u128{uint128_t{12}}, u128{uint128_t{8}}), u128{uint128_t{4}});
    BOOST_TEST_EQ(gcd(u128{uint128_t{100}}, u128{uint128_t{75}}), u128{uint128_t{25}});

    // Large values: powers of 2
    const auto two_pow_64 = uint128_t{UINT64_C(1)} << 64U;
    const auto two_pow_32 = uint128_t{UINT64_C(1)} << 32U;
    BOOST_TEST_EQ(gcd(u128{two_pow_64}, u128{two_pow_32}), u128{two_pow_32});

    // Large values: multiples
    const auto big_a = uint128_t{UINT64_C(1000000000000)} * uint128_t{UINT64_C(1000000)};
    const auto big_b = uint128_t{UINT64_C(1000000000000)} * uint128_t{UINT64_C(500000)};
    const auto big_gcd = uint128_t{UINT64_C(1000000000000)} * uint128_t{UINT64_C(500000)};
    BOOST_TEST_EQ(gcd(u128{big_a}, u128{big_b}), u128{big_gcd});
}

// =============================================================================
// Exhaustive u8 tests
// =============================================================================

void test_gcd_exhaustive_u8()
{
    // Check all (m, n) pairs for u8 against a simple reference implementation
    for (unsigned m {0}; m <= 255; ++m)
    {
        for (unsigned n {0}; n <= 255; ++n)
        {
            // Reference gcd using Euclidean algorithm on unsigned
            auto ref_a = m;
            auto ref_b = n;
            while (ref_b != 0)
            {
                auto t = ref_b;
                ref_b = ref_a % ref_b;
                ref_a = t;
            }

            BOOST_TEST_EQ(gcd(u8{static_cast<std::uint8_t>(m)}, u8{static_cast<std::uint8_t>(n)}),
                          u8{static_cast<std::uint8_t>(ref_a)});
        }
    }
}

// =============================================================================
// Constexpr tests
// =============================================================================

void test_gcd_constexpr()
{
    static_assert(gcd(u8{static_cast<std::uint8_t>(12)}, u8{static_cast<std::uint8_t>(8)}) == u8{static_cast<std::uint8_t>(4)});
    static_assert(gcd(u8{static_cast<std::uint8_t>(0)}, u8{static_cast<std::uint8_t>(5)}) == u8{static_cast<std::uint8_t>(5)});
    static_assert(gcd(u8{static_cast<std::uint8_t>(7)}, u8{static_cast<std::uint8_t>(7)}) == u8{static_cast<std::uint8_t>(7)});

    static_assert(gcd(u16{static_cast<std::uint16_t>(1000)}, u16{static_cast<std::uint16_t>(750)}) == u16{static_cast<std::uint16_t>(250)});

    static_assert(gcd(u32{UINT32_C(1234567890)}, u32{UINT32_C(987654321)}) == u32{UINT32_C(9)});
    static_assert(gcd(u32{UINT32_C(100)}, u32{UINT32_C(75)}) == u32{UINT32_C(25)});

    static_assert(gcd(u64{UINT64_C(1000000000000)}, u64{UINT64_C(750000000000)}) == u64{UINT64_C(250000000000)});
    static_assert(gcd(u64{UINT64_C(1346269)}, u64{UINT64_C(832040)}) == u64{UINT64_C(1)});
}

// =============================================================================
// Verified type tests (consteval)
// =============================================================================

void test_gcd_verified()
{
    // verified_u8
    static_assert(gcd(verified_u8{u8{static_cast<std::uint8_t>(12)}},
                      verified_u8{u8{static_cast<std::uint8_t>(8)}}) == verified_u8{u8{static_cast<std::uint8_t>(4)}});
    static_assert(gcd(verified_u8{u8{static_cast<std::uint8_t>(0)}},
                      verified_u8{u8{static_cast<std::uint8_t>(5)}}) == verified_u8{u8{static_cast<std::uint8_t>(5)}});
    static_assert(gcd(verified_u8{u8{static_cast<std::uint8_t>(7)}},
                      verified_u8{u8{static_cast<std::uint8_t>(11)}}) == verified_u8{u8{static_cast<std::uint8_t>(1)}});

    // verified_u16
    static_assert(gcd(verified_u16{u16{static_cast<std::uint16_t>(1000)}},
                      verified_u16{u16{static_cast<std::uint16_t>(750)}}) == verified_u16{u16{static_cast<std::uint16_t>(250)}});

    // verified_u32
    static_assert(gcd(verified_u32{u32{UINT32_C(1234567890)}},
                      verified_u32{u32{UINT32_C(987654321)}}) == verified_u32{u32{UINT32_C(9)}});

    // verified_u64
    static_assert(gcd(verified_u64{u64{UINT64_C(1000000000000)}},
                      verified_u64{u64{UINT64_C(750000000000)}}) == verified_u64{u64{UINT64_C(250000000000)}});

    // verified_u128
    using boost::int128::uint128_t;
    static_assert(gcd(verified_u128{u128{uint128_t{12}}},
                      verified_u128{u128{uint128_t{8}}}) == verified_u128{u128{uint128_t{4}}});
    static_assert(gcd(verified_u128{u128{uint128_t{100}}},
                      verified_u128{u128{uint128_t{75}}}) == verified_u128{u128{uint128_t{25}}});
}

int main()
{
    // Zero cases - all types
    test_gcd_zero<u8>();
    test_gcd_zero<u16>();
    test_gcd_zero<u32>();
    test_gcd_zero<u64>();
    test_gcd_zero<u128>();

    // One cases - all types
    test_gcd_one<u8>();
    test_gcd_one<u16>();
    test_gcd_one<u32>();
    test_gcd_one<u64>();
    test_gcd_one<u128>();

    // Equal values - all types
    test_gcd_equal<u8>();
    test_gcd_equal<u16>();
    test_gcd_equal<u32>();
    test_gcd_equal<u64>();
    test_gcd_equal<u128>();

    // Known values - all types
    test_gcd_known_values<u8>();
    test_gcd_known_values<u16>();
    test_gcd_known_values<u32>();
    test_gcd_known_values<u64>();
    test_gcd_known_values<u128>();

    // Type-specific larger values
    test_gcd_u16();
    test_gcd_u32();
    test_gcd_u64();
    test_gcd_u128();

    // Exhaustive u8
    test_gcd_exhaustive_u8();

    // Constexpr evaluation
    test_gcd_constexpr();

    // Verified types (consteval)
    test_gcd_verified();

    return boost::report_errors();
}

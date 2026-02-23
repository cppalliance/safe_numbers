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
// Runtime tests: abs_diff(a, a) == 0
// =============================================================================

template <typename T>
void test_abs_diff_equal()
{
    using underlying = typename detail::underlying_type_t<T>;

    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(0)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(1)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(42)}, T{static_cast<underlying>(42)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(100)}, T{static_cast<underlying>(100)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(255)}, T{static_cast<underlying>(255)}), T{static_cast<underlying>(0)});
}

// =============================================================================
// Runtime tests: abs_diff with zero
// =============================================================================

template <typename T>
void test_abs_diff_zero()
{
    using underlying = typename detail::underlying_type_t<T>;

    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(0)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(0)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(1)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(0)}, T{static_cast<underlying>(10)}), T{static_cast<underlying>(10)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(10)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(10)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(0)}, T{static_cast<underlying>(255)}), T{static_cast<underlying>(255)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(255)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(255)});
}

// =============================================================================
// Runtime tests: commutativity abs_diff(a, b) == abs_diff(b, a)
// =============================================================================

template <typename T>
void test_abs_diff_commutativity()
{
    using underlying = typename detail::underlying_type_t<T>;

    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(3)}, T{static_cast<underlying>(7)}),
                  abs_diff(T{static_cast<underlying>(7)}, T{static_cast<underlying>(3)}));
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(10)}, T{static_cast<underlying>(20)}),
                  abs_diff(T{static_cast<underlying>(20)}, T{static_cast<underlying>(10)}));
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(0)}, T{static_cast<underlying>(100)}),
                  abs_diff(T{static_cast<underlying>(100)}, T{static_cast<underlying>(0)}));
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(42)}, T{static_cast<underlying>(99)}),
                  abs_diff(T{static_cast<underlying>(99)}, T{static_cast<underlying>(42)}));
}

// =============================================================================
// Runtime tests: known values
// =============================================================================

template <typename T>
void test_abs_diff_known_values()
{
    using underlying = typename detail::underlying_type_t<T>;

    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(10)}, T{static_cast<underlying>(3)}), T{static_cast<underlying>(7)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(3)}, T{static_cast<underlying>(10)}), T{static_cast<underlying>(7)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(100)}, T{static_cast<underlying>(200)}), T{static_cast<underlying>(100)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(200)}, T{static_cast<underlying>(100)}), T{static_cast<underlying>(100)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(1)}, T{static_cast<underlying>(2)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(50)}, T{static_cast<underlying>(51)}), T{static_cast<underlying>(1)});
}

// =============================================================================
// Type-specific tests for larger values
// =============================================================================

void test_abs_diff_u16()
{
    BOOST_TEST_EQ(abs_diff(u16{static_cast<std::uint16_t>(0)}, u16{static_cast<std::uint16_t>(65535)}),
                  u16{static_cast<std::uint16_t>(65535)});
    BOOST_TEST_EQ(abs_diff(u16{static_cast<std::uint16_t>(65535)}, u16{static_cast<std::uint16_t>(0)}),
                  u16{static_cast<std::uint16_t>(65535)});
    BOOST_TEST_EQ(abs_diff(u16{static_cast<std::uint16_t>(60000)}, u16{static_cast<std::uint16_t>(65000)}),
                  u16{static_cast<std::uint16_t>(5000)});
    BOOST_TEST_EQ(abs_diff(u16{static_cast<std::uint16_t>(32000)}, u16{static_cast<std::uint16_t>(33000)}),
                  u16{static_cast<std::uint16_t>(1000)});
}

void test_abs_diff_u32()
{
    BOOST_TEST_EQ(abs_diff(u32{UINT32_C(4000000000)}, u32{UINT32_C(4294967295)}),
                  u32{UINT32_C(294967295)});
    BOOST_TEST_EQ(abs_diff(u32{UINT32_C(0)}, u32{UINT32_C(4294967295)}),
                  u32{UINT32_C(4294967295)});
    BOOST_TEST_EQ(abs_diff(u32{UINT32_C(1000000000)}, u32{UINT32_C(3000000000)}),
                  u32{UINT32_C(2000000000)});
}

void test_abs_diff_u64()
{
    BOOST_TEST_EQ(abs_diff(u64{UINT64_C(18000000000000000000)}, u64{UINT64_C(18446744073709551615)}),
                  u64{UINT64_C(446744073709551615)});
    BOOST_TEST_EQ(abs_diff(u64{UINT64_C(0)}, u64{UINT64_C(18446744073709551615)}),
                  u64{UINT64_C(18446744073709551615)});
    BOOST_TEST_EQ(abs_diff(u64{UINT64_C(1000000000000000000)}, u64{UINT64_C(9000000000000000000)}),
                  u64{UINT64_C(8000000000000000000)});
}

void test_abs_diff_u128()
{
    using boost::int128::uint128_t;

    // Small values
    BOOST_TEST_EQ(abs_diff(u128{uint128_t{0}}, u128{uint128_t{10}}), u128{uint128_t{10}});
    BOOST_TEST_EQ(abs_diff(u128{uint128_t{10}}, u128{uint128_t{0}}), u128{uint128_t{10}});
    BOOST_TEST_EQ(abs_diff(u128{uint128_t{3}}, u128{uint128_t{7}}), u128{uint128_t{4}});
    BOOST_TEST_EQ(abs_diff(u128{uint128_t{7}}, u128{uint128_t{3}}), u128{uint128_t{4}});

    // Large values: powers of 2
    const auto two_pow_64 = uint128_t{UINT64_C(1)} << 64U;
    const auto two_pow_63 = uint128_t{UINT64_C(1)} << 63U;
    BOOST_TEST_EQ(abs_diff(u128{two_pow_64}, u128{two_pow_63}), u128{two_pow_63});

    // Equal large values
    const auto big_val = uint128_t{UINT64_C(1000000000000)} * uint128_t{UINT64_C(1000000000000)};
    BOOST_TEST_EQ(abs_diff(u128{big_val}, u128{big_val}), u128{uint128_t{0}});
}

// =============================================================================
// Exhaustive u8 tests
// =============================================================================

void test_abs_diff_exhaustive_u8()
{
    for (unsigned a {0}; a <= 255; ++a)
    {
        for (unsigned b {0}; b <= 255; ++b)
        {
            const auto expected = (a >= b) ? (a - b) : (b - a);

            BOOST_TEST_EQ(abs_diff(u8{static_cast<std::uint8_t>(a)}, u8{static_cast<std::uint8_t>(b)}),
                          u8{static_cast<std::uint8_t>(expected)});
        }
    }
}

// =============================================================================
// Bounded uint tests
// =============================================================================

void test_abs_diff_bounded()
{
    using byte_val = bounded_uint<0u, 255u>;
    using word_val = bounded_uint<0u, 65535u>;

    BOOST_TEST_EQ(abs_diff(byte_val{static_cast<std::uint8_t>(100)}, byte_val{static_cast<std::uint8_t>(200)}),
                  byte_val{static_cast<std::uint8_t>(100)});
    BOOST_TEST_EQ(abs_diff(byte_val{static_cast<std::uint8_t>(200)}, byte_val{static_cast<std::uint8_t>(100)}),
                  byte_val{static_cast<std::uint8_t>(100)});
    BOOST_TEST_EQ(abs_diff(byte_val{static_cast<std::uint8_t>(50)}, byte_val{static_cast<std::uint8_t>(50)}),
                  byte_val{static_cast<std::uint8_t>(0)});
    BOOST_TEST_EQ(abs_diff(byte_val{static_cast<std::uint8_t>(0)}, byte_val{static_cast<std::uint8_t>(255)}),
                  byte_val{static_cast<std::uint8_t>(255)});

    BOOST_TEST_EQ(abs_diff(word_val{static_cast<std::uint16_t>(60000)}, word_val{static_cast<std::uint16_t>(65000)}),
                  word_val{static_cast<std::uint16_t>(5000)});
    BOOST_TEST_EQ(abs_diff(word_val{static_cast<std::uint16_t>(0)}, word_val{static_cast<std::uint16_t>(0)}),
                  word_val{static_cast<std::uint16_t>(0)});
}

// =============================================================================
// Constexpr tests
// =============================================================================

void test_abs_diff_constexpr()
{
    static_assert(abs_diff(u8{static_cast<std::uint8_t>(10)}, u8{static_cast<std::uint8_t>(3)}) == u8{static_cast<std::uint8_t>(7)});
    static_assert(abs_diff(u8{static_cast<std::uint8_t>(3)}, u8{static_cast<std::uint8_t>(10)}) == u8{static_cast<std::uint8_t>(7)});
    static_assert(abs_diff(u8{static_cast<std::uint8_t>(0)}, u8{static_cast<std::uint8_t>(0)}) == u8{static_cast<std::uint8_t>(0)});
    static_assert(abs_diff(u8{static_cast<std::uint8_t>(255)}, u8{static_cast<std::uint8_t>(0)}) == u8{static_cast<std::uint8_t>(255)});

    static_assert(abs_diff(u16{static_cast<std::uint16_t>(1000)}, u16{static_cast<std::uint16_t>(3000)}) == u16{static_cast<std::uint16_t>(2000)});

    static_assert(abs_diff(u32{UINT32_C(100)}, u32{UINT32_C(200)}) == u32{UINT32_C(100)});
    static_assert(abs_diff(u32{UINT32_C(1000000000)}, u32{UINT32_C(3000000000)}) == u32{UINT32_C(2000000000)});

    static_assert(abs_diff(u64{UINT64_C(1000)}, u64{UINT64_C(3000)}) == u64{UINT64_C(2000)});
}

// =============================================================================
// Verified type tests (consteval)
// =============================================================================

void test_abs_diff_verified()
{
    // verified_u8
    static_assert(abs_diff(verified_u8{u8{static_cast<std::uint8_t>(10)}},
                           verified_u8{u8{static_cast<std::uint8_t>(3)}}) == verified_u8{u8{static_cast<std::uint8_t>(7)}});
    static_assert(abs_diff(verified_u8{u8{static_cast<std::uint8_t>(3)}},
                           verified_u8{u8{static_cast<std::uint8_t>(10)}}) == verified_u8{u8{static_cast<std::uint8_t>(7)}});
    static_assert(abs_diff(verified_u8{u8{static_cast<std::uint8_t>(0)}},
                           verified_u8{u8{static_cast<std::uint8_t>(0)}}) == verified_u8{u8{static_cast<std::uint8_t>(0)}});

    // verified_u16
    static_assert(abs_diff(verified_u16{u16{static_cast<std::uint16_t>(1000)}},
                           verified_u16{u16{static_cast<std::uint16_t>(3000)}}) == verified_u16{u16{static_cast<std::uint16_t>(2000)}});

    // verified_u32
    static_assert(abs_diff(verified_u32{u32{UINT32_C(100)}},
                           verified_u32{u32{UINT32_C(200)}}) == verified_u32{u32{UINT32_C(100)}});

    // verified_u64
    static_assert(abs_diff(verified_u64{u64{UINT64_C(1000)}},
                           verified_u64{u64{UINT64_C(3000)}}) == verified_u64{u64{UINT64_C(2000)}});

    // verified_u128
    using boost::int128::uint128_t;
    static_assert(abs_diff(verified_u128{u128{uint128_t{10}}},
                           verified_u128{u128{uint128_t{3}}}) == verified_u128{u128{uint128_t{7}}});
    static_assert(abs_diff(verified_u128{u128{uint128_t{3}}},
                           verified_u128{u128{uint128_t{10}}}) == verified_u128{u128{uint128_t{7}}});
}

int main()
{
    // Equal values - all types
    test_abs_diff_equal<u8>();
    test_abs_diff_equal<u16>();
    test_abs_diff_equal<u32>();
    test_abs_diff_equal<u64>();
    test_abs_diff_equal<u128>();

    // Zero cases - all types
    test_abs_diff_zero<u8>();
    test_abs_diff_zero<u16>();
    test_abs_diff_zero<u32>();
    test_abs_diff_zero<u64>();
    test_abs_diff_zero<u128>();

    // Commutativity - all types
    test_abs_diff_commutativity<u8>();
    test_abs_diff_commutativity<u16>();
    test_abs_diff_commutativity<u32>();
    test_abs_diff_commutativity<u64>();
    test_abs_diff_commutativity<u128>();

    // Known values - all types
    test_abs_diff_known_values<u8>();
    test_abs_diff_known_values<u16>();
    test_abs_diff_known_values<u32>();
    test_abs_diff_known_values<u64>();
    test_abs_diff_known_values<u128>();

    // Type-specific larger values
    test_abs_diff_u16();
    test_abs_diff_u32();
    test_abs_diff_u64();
    test_abs_diff_u128();

    // Exhaustive u8
    test_abs_diff_exhaustive_u8();

    // Bounded uint
    test_abs_diff_bounded();

    // Constexpr evaluation
    test_abs_diff_constexpr();

    // Verified types (consteval)
    test_abs_diff_verified();

    return boost::report_errors();
}

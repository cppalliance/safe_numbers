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
// Runtime tests: div_ceil(a, 1) == a
// =============================================================================

template <typename T>
void test_div_ceil_by_one()
{
    using underlying = typename detail::underlying_type_t<T>;

    BOOST_TEST_EQ(div_ceil(T{static_cast<underlying>(0)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(div_ceil(T{static_cast<underlying>(1)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(div_ceil(T{static_cast<underlying>(7)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(7)});
    BOOST_TEST_EQ(div_ceil(T{static_cast<underlying>(42)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(42)});
    BOOST_TEST_EQ(div_ceil(T{static_cast<underlying>(255)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(255)});
}

// =============================================================================
// Runtime tests: exact division (no remainder)
// =============================================================================

template <typename T>
void test_div_ceil_exact()
{
    using underlying = typename detail::underlying_type_t<T>;

    // When a is evenly divisible by b, div_ceil == a / b
    BOOST_TEST_EQ(div_ceil(T{static_cast<underlying>(0)}, T{static_cast<underlying>(5)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(div_ceil(T{static_cast<underlying>(10)}, T{static_cast<underlying>(5)}), T{static_cast<underlying>(2)});
    BOOST_TEST_EQ(div_ceil(T{static_cast<underlying>(10)}, T{static_cast<underlying>(2)}), T{static_cast<underlying>(5)});
    BOOST_TEST_EQ(div_ceil(T{static_cast<underlying>(100)}, T{static_cast<underlying>(10)}), T{static_cast<underlying>(10)});
    BOOST_TEST_EQ(div_ceil(T{static_cast<underlying>(12)}, T{static_cast<underlying>(4)}), T{static_cast<underlying>(3)});
    BOOST_TEST_EQ(div_ceil(T{static_cast<underlying>(200)}, T{static_cast<underlying>(25)}), T{static_cast<underlying>(8)});
}

// =============================================================================
// Runtime tests: rounding up (non-zero remainder)
// =============================================================================

template <typename T>
void test_div_ceil_rounding()
{
    using underlying = typename detail::underlying_type_t<T>;

    // When a % b != 0, div_ceil rounds up: div_ceil(a, b) == a / b + 1
    BOOST_TEST_EQ(div_ceil(T{static_cast<underlying>(1)}, T{static_cast<underlying>(2)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(div_ceil(T{static_cast<underlying>(3)}, T{static_cast<underlying>(2)}), T{static_cast<underlying>(2)});
    BOOST_TEST_EQ(div_ceil(T{static_cast<underlying>(7)}, T{static_cast<underlying>(3)}), T{static_cast<underlying>(3)});
    BOOST_TEST_EQ(div_ceil(T{static_cast<underlying>(10)}, T{static_cast<underlying>(3)}), T{static_cast<underlying>(4)});
    BOOST_TEST_EQ(div_ceil(T{static_cast<underlying>(11)}, T{static_cast<underlying>(3)}), T{static_cast<underlying>(4)});
    BOOST_TEST_EQ(div_ceil(T{static_cast<underlying>(99)}, T{static_cast<underlying>(10)}), T{static_cast<underlying>(10)});
    BOOST_TEST_EQ(div_ceil(T{static_cast<underlying>(101)}, T{static_cast<underlying>(10)}), T{static_cast<underlying>(11)});
    BOOST_TEST_EQ(div_ceil(T{static_cast<underlying>(255)}, T{static_cast<underlying>(2)}), T{static_cast<underlying>(128)});
}

// =============================================================================
// Runtime tests: div_ceil(a, a) == 1 for a > 0
// =============================================================================

template <typename T>
void test_div_ceil_self()
{
    using underlying = typename detail::underlying_type_t<T>;

    BOOST_TEST_EQ(div_ceil(T{static_cast<underlying>(1)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(div_ceil(T{static_cast<underlying>(7)}, T{static_cast<underlying>(7)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(div_ceil(T{static_cast<underlying>(42)}, T{static_cast<underlying>(42)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(div_ceil(T{static_cast<underlying>(100)}, T{static_cast<underlying>(100)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(div_ceil(T{static_cast<underlying>(255)}, T{static_cast<underlying>(255)}), T{static_cast<underlying>(1)});
}

// =============================================================================
// Type-specific tests for larger values
// =============================================================================

void test_div_ceil_u16()
{
    BOOST_TEST_EQ(div_ceil(u16{static_cast<std::uint16_t>(65535)}, u16{static_cast<std::uint16_t>(1)}),
                  u16{static_cast<std::uint16_t>(65535)});
    BOOST_TEST_EQ(div_ceil(u16{static_cast<std::uint16_t>(65535)}, u16{static_cast<std::uint16_t>(2)}),
                  u16{static_cast<std::uint16_t>(32768)});
    BOOST_TEST_EQ(div_ceil(u16{static_cast<std::uint16_t>(65534)}, u16{static_cast<std::uint16_t>(2)}),
                  u16{static_cast<std::uint16_t>(32767)});
    BOOST_TEST_EQ(div_ceil(u16{static_cast<std::uint16_t>(60000)}, u16{static_cast<std::uint16_t>(7)}),
                  u16{static_cast<std::uint16_t>(8572)});
}

void test_div_ceil_u32()
{
    BOOST_TEST_EQ(div_ceil(u32{UINT32_C(4294967295)}, u32{UINT32_C(1)}),
                  u32{UINT32_C(4294967295)});
    BOOST_TEST_EQ(div_ceil(u32{UINT32_C(4294967295)}, u32{UINT32_C(2)}),
                  u32{UINT32_C(2147483648)});
    BOOST_TEST_EQ(div_ceil(u32{UINT32_C(1000000000)}, u32{UINT32_C(3)}),
                  u32{UINT32_C(333333334)});
    BOOST_TEST_EQ(div_ceil(u32{UINT32_C(1000000000)}, u32{UINT32_C(1000000000)}),
                  u32{UINT32_C(1)});
    BOOST_TEST_EQ(div_ceil(u32{UINT32_C(1000000001)}, u32{UINT32_C(1000000000)}),
                  u32{UINT32_C(2)});
}

void test_div_ceil_u64()
{
    BOOST_TEST_EQ(div_ceil(u64{UINT64_C(18446744073709551615)}, u64{UINT64_C(1)}),
                  u64{UINT64_C(18446744073709551615)});
    BOOST_TEST_EQ(div_ceil(u64{UINT64_C(18446744073709551615)}, u64{UINT64_C(2)}),
                  u64{UINT64_C(9223372036854775808)});
    BOOST_TEST_EQ(div_ceil(u64{UINT64_C(1000000000000000000)}, u64{UINT64_C(3)}),
                  u64{UINT64_C(333333333333333334)});
    BOOST_TEST_EQ(div_ceil(u64{UINT64_C(10)}, u64{UINT64_C(3)}),
                  u64{UINT64_C(4)});
}

void test_div_ceil_u128()
{
    using boost::int128::uint128_t;

    // Small values
    BOOST_TEST_EQ(div_ceil(u128{uint128_t{0}}, u128{uint128_t{1}}), u128{uint128_t{0}});
    BOOST_TEST_EQ(div_ceil(u128{uint128_t{10}}, u128{uint128_t{3}}), u128{uint128_t{4}});
    BOOST_TEST_EQ(div_ceil(u128{uint128_t{10}}, u128{uint128_t{5}}), u128{uint128_t{2}});
    BOOST_TEST_EQ(div_ceil(u128{uint128_t{7}}, u128{uint128_t{7}}), u128{uint128_t{1}});

    // Large values
    const auto two_pow_64 = uint128_t{UINT64_C(1)} << 64U;
    BOOST_TEST_EQ(div_ceil(u128{two_pow_64}, u128{uint128_t{2}}), u128{uint128_t{UINT64_C(1)} << 63U});
    BOOST_TEST_EQ(div_ceil(u128{two_pow_64 + uint128_t{1}}, u128{uint128_t{2}}),
                  u128{(uint128_t{UINT64_C(1)} << 63U) + uint128_t{1}});

    // Equal large values
    const auto big_val = uint128_t{UINT64_C(1000000000000)} * uint128_t{UINT64_C(1000000000000)};
    BOOST_TEST_EQ(div_ceil(u128{big_val}, u128{big_val}), u128{uint128_t{1}});
}

// =============================================================================
// Exhaustive u8 tests
// =============================================================================

void test_div_ceil_exhaustive_u8()
{
    for (unsigned a {0}; a <= 255; ++a)
    {
        for (unsigned b {1}; b <= 255; ++b)
        {
            // Reference: ceil(a / b) = (a + b - 1) / b for unsigned
            const auto expected = (a + b - 1) / b;

            BOOST_TEST_EQ(div_ceil(u8{static_cast<std::uint8_t>(a)}, u8{static_cast<std::uint8_t>(b)}),
                          u8{static_cast<std::uint8_t>(expected)});
        }
    }
}

// =============================================================================
// Bounded uint tests
// =============================================================================

void test_div_ceil_bounded()
{
    using byte_val = bounded_uint<0u, 255u>;
    using word_val = bounded_uint<0u, 65535u>;

    BOOST_TEST_EQ(div_ceil(byte_val{static_cast<std::uint8_t>(10)}, byte_val{static_cast<std::uint8_t>(3)}),
                  byte_val{static_cast<std::uint8_t>(4)});
    BOOST_TEST_EQ(div_ceil(byte_val{static_cast<std::uint8_t>(9)}, byte_val{static_cast<std::uint8_t>(3)}),
                  byte_val{static_cast<std::uint8_t>(3)});
    BOOST_TEST_EQ(div_ceil(byte_val{static_cast<std::uint8_t>(0)}, byte_val{static_cast<std::uint8_t>(5)}),
                  byte_val{static_cast<std::uint8_t>(0)});
    BOOST_TEST_EQ(div_ceil(byte_val{static_cast<std::uint8_t>(255)}, byte_val{static_cast<std::uint8_t>(2)}),
                  byte_val{static_cast<std::uint8_t>(128)});

    BOOST_TEST_EQ(div_ceil(word_val{static_cast<std::uint16_t>(60001)}, word_val{static_cast<std::uint16_t>(10)}),
                  word_val{static_cast<std::uint16_t>(6001)});
    BOOST_TEST_EQ(div_ceil(word_val{static_cast<std::uint16_t>(60000)}, word_val{static_cast<std::uint16_t>(10)}),
                  word_val{static_cast<std::uint16_t>(6000)});
}

// =============================================================================
// Constexpr tests
// =============================================================================

void test_div_ceil_constexpr()
{
    static_assert(div_ceil(u8{static_cast<std::uint8_t>(10)}, u8{static_cast<std::uint8_t>(3)}) == u8{static_cast<std::uint8_t>(4)});
    static_assert(div_ceil(u8{static_cast<std::uint8_t>(9)}, u8{static_cast<std::uint8_t>(3)}) == u8{static_cast<std::uint8_t>(3)});
    static_assert(div_ceil(u8{static_cast<std::uint8_t>(0)}, u8{static_cast<std::uint8_t>(1)}) == u8{static_cast<std::uint8_t>(0)});
    static_assert(div_ceil(u8{static_cast<std::uint8_t>(255)}, u8{static_cast<std::uint8_t>(2)}) == u8{static_cast<std::uint8_t>(128)});

    static_assert(div_ceil(u16{static_cast<std::uint16_t>(1000)}, u16{static_cast<std::uint16_t>(3)}) == u16{static_cast<std::uint16_t>(334)});

    static_assert(div_ceil(u32{UINT32_C(100)}, u32{UINT32_C(7)}) == u32{UINT32_C(15)});
    static_assert(div_ceil(u32{UINT32_C(1000000000)}, u32{UINT32_C(3)}) == u32{UINT32_C(333333334)});

    static_assert(div_ceil(u64{UINT64_C(1000)}, u64{UINT64_C(3)}) == u64{UINT64_C(334)});
}

int main()
{
    // Division by one - all types
    test_div_ceil_by_one<u8>();
    test_div_ceil_by_one<u16>();
    test_div_ceil_by_one<u32>();
    test_div_ceil_by_one<u64>();
    test_div_ceil_by_one<u128>();

    // Exact division - all types
    test_div_ceil_exact<u8>();
    test_div_ceil_exact<u16>();
    test_div_ceil_exact<u32>();
    test_div_ceil_exact<u64>();
    test_div_ceil_exact<u128>();

    // Rounding up - all types
    test_div_ceil_rounding<u8>();
    test_div_ceil_rounding<u16>();
    test_div_ceil_rounding<u32>();
    test_div_ceil_rounding<u64>();
    test_div_ceil_rounding<u128>();

    // Self-division - all types
    test_div_ceil_self<u8>();
    test_div_ceil_self<u16>();
    test_div_ceil_self<u32>();
    test_div_ceil_self<u64>();
    test_div_ceil_self<u128>();

    // Type-specific larger values
    test_div_ceil_u16();
    test_div_ceil_u32();
    test_div_ceil_u64();
    test_div_ceil_u128();

    // Exhaustive u8
    test_div_ceil_exhaustive_u8();

    // Bounded uint
    test_div_ceil_bounded();

    // Constexpr evaluation
    test_div_ceil_constexpr();

    return boost::report_errors();
}

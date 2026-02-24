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
// Runtime tests: next_multiple_of(a, b) when a is already a multiple of b
// =============================================================================

template <typename T>
void test_already_multiple()
{
    using underlying = typename detail::underlying_type_t<T>;

    // 0 is a multiple of everything
    BOOST_TEST_EQ(next_multiple_of(T{static_cast<underlying>(0)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(next_multiple_of(T{static_cast<underlying>(0)}, T{static_cast<underlying>(5)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(next_multiple_of(T{static_cast<underlying>(0)}, T{static_cast<underlying>(100)}), T{static_cast<underlying>(0)});

    // Exact multiples
    BOOST_TEST_EQ(next_multiple_of(T{static_cast<underlying>(6)}, T{static_cast<underlying>(3)}), T{static_cast<underlying>(6)});
    BOOST_TEST_EQ(next_multiple_of(T{static_cast<underlying>(10)}, T{static_cast<underlying>(5)}), T{static_cast<underlying>(10)});
    BOOST_TEST_EQ(next_multiple_of(T{static_cast<underlying>(100)}, T{static_cast<underlying>(10)}), T{static_cast<underlying>(100)});
    BOOST_TEST_EQ(next_multiple_of(T{static_cast<underlying>(12)}, T{static_cast<underlying>(4)}), T{static_cast<underlying>(12)});
    BOOST_TEST_EQ(next_multiple_of(T{static_cast<underlying>(200)}, T{static_cast<underlying>(25)}), T{static_cast<underlying>(200)});
}

// =============================================================================
// Runtime tests: next_multiple_of(a, b) when a is NOT a multiple of b
// =============================================================================

template <typename T>
void test_rounding_up()
{
    using underlying = typename detail::underlying_type_t<T>;

    BOOST_TEST_EQ(next_multiple_of(T{static_cast<underlying>(1)}, T{static_cast<underlying>(3)}), T{static_cast<underlying>(3)});
    BOOST_TEST_EQ(next_multiple_of(T{static_cast<underlying>(2)}, T{static_cast<underlying>(3)}), T{static_cast<underlying>(3)});
    BOOST_TEST_EQ(next_multiple_of(T{static_cast<underlying>(4)}, T{static_cast<underlying>(3)}), T{static_cast<underlying>(6)});
    BOOST_TEST_EQ(next_multiple_of(T{static_cast<underlying>(5)}, T{static_cast<underlying>(3)}), T{static_cast<underlying>(6)});
    BOOST_TEST_EQ(next_multiple_of(T{static_cast<underlying>(7)}, T{static_cast<underlying>(5)}), T{static_cast<underlying>(10)});
    BOOST_TEST_EQ(next_multiple_of(T{static_cast<underlying>(11)}, T{static_cast<underlying>(10)}), T{static_cast<underlying>(20)});
    BOOST_TEST_EQ(next_multiple_of(T{static_cast<underlying>(99)}, T{static_cast<underlying>(10)}), T{static_cast<underlying>(100)});
    BOOST_TEST_EQ(next_multiple_of(T{static_cast<underlying>(101)}, T{static_cast<underlying>(10)}), T{static_cast<underlying>(110)});
}

// =============================================================================
// Runtime tests: next_multiple_of(a, 1) == a
// =============================================================================

template <typename T>
void test_multiple_of_one()
{
    using underlying = typename detail::underlying_type_t<T>;

    BOOST_TEST_EQ(next_multiple_of(T{static_cast<underlying>(0)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(next_multiple_of(T{static_cast<underlying>(1)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(next_multiple_of(T{static_cast<underlying>(7)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(7)});
    BOOST_TEST_EQ(next_multiple_of(T{static_cast<underlying>(42)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(42)});
    BOOST_TEST_EQ(next_multiple_of(T{static_cast<underlying>(255)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(255)});
}

// =============================================================================
// Runtime tests: next_multiple_of(a, a) == a for a > 0
// =============================================================================

template <typename T>
void test_multiple_of_self()
{
    using underlying = typename detail::underlying_type_t<T>;

    BOOST_TEST_EQ(next_multiple_of(T{static_cast<underlying>(1)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(next_multiple_of(T{static_cast<underlying>(7)}, T{static_cast<underlying>(7)}), T{static_cast<underlying>(7)});
    BOOST_TEST_EQ(next_multiple_of(T{static_cast<underlying>(42)}, T{static_cast<underlying>(42)}), T{static_cast<underlying>(42)});
    BOOST_TEST_EQ(next_multiple_of(T{static_cast<underlying>(100)}, T{static_cast<underlying>(100)}), T{static_cast<underlying>(100)});
    BOOST_TEST_EQ(next_multiple_of(T{static_cast<underlying>(255)}, T{static_cast<underlying>(255)}), T{static_cast<underlying>(255)});
}

// =============================================================================
// Type-specific tests for larger values
// =============================================================================

void test_next_multiple_of_u16()
{
    BOOST_TEST_EQ(next_multiple_of(u16{static_cast<std::uint16_t>(60001)}, u16{static_cast<std::uint16_t>(10)}),
                  u16{static_cast<std::uint16_t>(60010)});
    BOOST_TEST_EQ(next_multiple_of(u16{static_cast<std::uint16_t>(60000)}, u16{static_cast<std::uint16_t>(10)}),
                  u16{static_cast<std::uint16_t>(60000)});
    BOOST_TEST_EQ(next_multiple_of(u16{static_cast<std::uint16_t>(1)}, u16{static_cast<std::uint16_t>(256)}),
                  u16{static_cast<std::uint16_t>(256)});
    BOOST_TEST_EQ(next_multiple_of(u16{static_cast<std::uint16_t>(257)}, u16{static_cast<std::uint16_t>(256)}),
                  u16{static_cast<std::uint16_t>(512)});
}

void test_next_multiple_of_u32()
{
    BOOST_TEST_EQ(next_multiple_of(u32{UINT32_C(1000000001)}, u32{UINT32_C(1000000000)}),
                  u32{UINT32_C(2000000000)});
    BOOST_TEST_EQ(next_multiple_of(u32{UINT32_C(1000000000)}, u32{UINT32_C(1000000000)}),
                  u32{UINT32_C(1000000000)});
    BOOST_TEST_EQ(next_multiple_of(u32{UINT32_C(1)}, u32{UINT32_C(4096)}),
                  u32{UINT32_C(4096)});
    BOOST_TEST_EQ(next_multiple_of(u32{UINT32_C(4097)}, u32{UINT32_C(4096)}),
                  u32{UINT32_C(8192)});
}

void test_next_multiple_of_u64()
{
    BOOST_TEST_EQ(next_multiple_of(u64{UINT64_C(1000000000000000001)}, u64{UINT64_C(1000000000000000000)}),
                  u64{UINT64_C(2000000000000000000)});
    BOOST_TEST_EQ(next_multiple_of(u64{UINT64_C(1000000000000000000)}, u64{UINT64_C(1000000000000000000)}),
                  u64{UINT64_C(1000000000000000000)});
    BOOST_TEST_EQ(next_multiple_of(u64{UINT64_C(10)}, u64{UINT64_C(3)}),
                  u64{UINT64_C(12)});
    BOOST_TEST_EQ(next_multiple_of(u64{UINT64_C(1)}, u64{UINT64_C(65536)}),
                  u64{UINT64_C(65536)});
}

void test_next_multiple_of_u128()
{
    using boost::int128::uint128_t;

    // Small values
    BOOST_TEST_EQ(next_multiple_of(u128{uint128_t{0}}, u128{uint128_t{1}}), u128{uint128_t{0}});
    BOOST_TEST_EQ(next_multiple_of(u128{uint128_t{10}}, u128{uint128_t{3}}), u128{uint128_t{12}});
    BOOST_TEST_EQ(next_multiple_of(u128{uint128_t{10}}, u128{uint128_t{5}}), u128{uint128_t{10}});
    BOOST_TEST_EQ(next_multiple_of(u128{uint128_t{7}}, u128{uint128_t{7}}), u128{uint128_t{7}});

    // Large values
    const auto two_pow_64 = uint128_t{UINT64_C(1)} << 64U;
    BOOST_TEST_EQ(next_multiple_of(u128{two_pow_64}, u128{two_pow_64}), u128{two_pow_64});
    BOOST_TEST_EQ(next_multiple_of(u128{uint128_t{1}}, u128{two_pow_64}), u128{two_pow_64});

    // Equal large values
    const auto big_val = uint128_t{UINT64_C(1000000000000)} * uint128_t{UINT64_C(1000000000000)};
    BOOST_TEST_EQ(next_multiple_of(u128{big_val}, u128{big_val}), u128{big_val});
}

// =============================================================================
// Exhaustive u8 tests
// =============================================================================

void test_next_multiple_of_exhaustive_u8()
{
    for (unsigned a {0}; a <= 255; ++a)
    {
        for (unsigned b {1}; b <= 255; ++b)
        {
            // Reference: smallest multiple of b >= a
            const auto expected = ((a + b - 1) / b) * b;

            if (expected > 255)
            {
                continue; // Skip cases that overflow u8
            }

            BOOST_TEST_EQ(next_multiple_of(u8{static_cast<std::uint8_t>(a)}, u8{static_cast<std::uint8_t>(b)}),
                          u8{static_cast<std::uint8_t>(expected)});
        }
    }
}

// =============================================================================
// Bounded uint tests
// =============================================================================

void test_next_multiple_of_bounded()
{
    using byte_val = bounded_uint<0u, 255u>;
    using word_val = bounded_uint<0u, 65535u>;

    BOOST_TEST_EQ(next_multiple_of(byte_val{static_cast<std::uint8_t>(10)}, byte_val{static_cast<std::uint8_t>(3)}),
                  byte_val{static_cast<std::uint8_t>(12)});
    BOOST_TEST_EQ(next_multiple_of(byte_val{static_cast<std::uint8_t>(9)}, byte_val{static_cast<std::uint8_t>(3)}),
                  byte_val{static_cast<std::uint8_t>(9)});
    BOOST_TEST_EQ(next_multiple_of(byte_val{static_cast<std::uint8_t>(0)}, byte_val{static_cast<std::uint8_t>(5)}),
                  byte_val{static_cast<std::uint8_t>(0)});
    BOOST_TEST_EQ(next_multiple_of(byte_val{static_cast<std::uint8_t>(1)}, byte_val{static_cast<std::uint8_t>(128)}),
                  byte_val{static_cast<std::uint8_t>(128)});

    BOOST_TEST_EQ(next_multiple_of(word_val{static_cast<std::uint16_t>(60001)}, word_val{static_cast<std::uint16_t>(10)}),
                  word_val{static_cast<std::uint16_t>(60010)});
    BOOST_TEST_EQ(next_multiple_of(word_val{static_cast<std::uint16_t>(60000)}, word_val{static_cast<std::uint16_t>(10)}),
                  word_val{static_cast<std::uint16_t>(60000)});
}

// =============================================================================
// Constexpr tests
// =============================================================================

void test_next_multiple_of_constexpr()
{
    static_assert(next_multiple_of(u8{static_cast<std::uint8_t>(10)}, u8{static_cast<std::uint8_t>(3)}) == u8{static_cast<std::uint8_t>(12)});
    static_assert(next_multiple_of(u8{static_cast<std::uint8_t>(9)}, u8{static_cast<std::uint8_t>(3)}) == u8{static_cast<std::uint8_t>(9)});
    static_assert(next_multiple_of(u8{static_cast<std::uint8_t>(0)}, u8{static_cast<std::uint8_t>(1)}) == u8{static_cast<std::uint8_t>(0)});
    static_assert(next_multiple_of(u8{static_cast<std::uint8_t>(1)}, u8{static_cast<std::uint8_t>(5)}) == u8{static_cast<std::uint8_t>(5)});

    static_assert(next_multiple_of(u16{static_cast<std::uint16_t>(1000)}, u16{static_cast<std::uint16_t>(3)}) == u16{static_cast<std::uint16_t>(1002)});

    static_assert(next_multiple_of(u32{UINT32_C(100)}, u32{UINT32_C(7)}) == u32{UINT32_C(105)});
    static_assert(next_multiple_of(u32{UINT32_C(1000000000)}, u32{UINT32_C(3)}) == u32{UINT32_C(1000000002)});

    static_assert(next_multiple_of(u64{UINT64_C(1000)}, u64{UINT64_C(3)}) == u64{UINT64_C(1002)});
}

int main()
{
    // Already a multiple - all types
    test_already_multiple<u8>();
    test_already_multiple<u16>();
    test_already_multiple<u32>();
    test_already_multiple<u64>();
    test_already_multiple<u128>();

    // Rounding up - all types
    test_rounding_up<u8>();
    test_rounding_up<u16>();
    test_rounding_up<u32>();
    test_rounding_up<u64>();
    test_rounding_up<u128>();

    // Multiple of one - all types
    test_multiple_of_one<u8>();
    test_multiple_of_one<u16>();
    test_multiple_of_one<u32>();
    test_multiple_of_one<u64>();
    test_multiple_of_one<u128>();

    // Multiple of self - all types
    test_multiple_of_self<u8>();
    test_multiple_of_self<u16>();
    test_multiple_of_self<u32>();
    test_multiple_of_self<u64>();
    test_multiple_of_self<u128>();

    // Type-specific larger values
    test_next_multiple_of_u16();
    test_next_multiple_of_u32();
    test_next_multiple_of_u64();
    test_next_multiple_of_u128();

    // Exhaustive u8
    test_next_multiple_of_exhaustive_u8();

    // Bounded uint
    test_next_multiple_of_bounded();

    // Constexpr evaluation
    test_next_multiple_of_constexpr();

    return boost::report_errors();
}

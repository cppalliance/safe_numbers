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
// Runtime isqrt tests for unsigned_integer types
// =============================================================================

template <typename T>
void test_isqrt_perfect_squares()
{
    // Perfect squares
    BOOST_TEST_EQ(isqrt(T{0}), T{0});
    BOOST_TEST_EQ(isqrt(T{1}), T{1});
    BOOST_TEST_EQ(isqrt(T{4}), T{2});
    BOOST_TEST_EQ(isqrt(T{9}), T{3});
    BOOST_TEST_EQ(isqrt(T{16}), T{4});
    BOOST_TEST_EQ(isqrt(T{25}), T{5});
    BOOST_TEST_EQ(isqrt(T{36}), T{6});
    BOOST_TEST_EQ(isqrt(T{49}), T{7});
    BOOST_TEST_EQ(isqrt(T{64}), T{8});
    BOOST_TEST_EQ(isqrt(T{81}), T{9});
    BOOST_TEST_EQ(isqrt(T{100}), T{10});
    BOOST_TEST_EQ(isqrt(T{144}), T{12});
}

template <typename T>
void test_isqrt_non_perfect_squares()
{
    // Non-perfect squares should return floor(sqrt(n))
    BOOST_TEST_EQ(isqrt(T{2}), T{1});
    BOOST_TEST_EQ(isqrt(T{3}), T{1});
    BOOST_TEST_EQ(isqrt(T{5}), T{2});
    BOOST_TEST_EQ(isqrt(T{8}), T{2});
    BOOST_TEST_EQ(isqrt(T{10}), T{3});
    BOOST_TEST_EQ(isqrt(T{15}), T{3});
    BOOST_TEST_EQ(isqrt(T{17}), T{4});
    BOOST_TEST_EQ(isqrt(T{24}), T{4});
    BOOST_TEST_EQ(isqrt(T{99}), T{9});
    BOOST_TEST_EQ(isqrt(T{101}), T{10});
    BOOST_TEST_EQ(isqrt(T{200}), T{14});
}

template <typename T>
void test_isqrt_edge_cases()
{
    // Edge cases: 0 and 1
    BOOST_TEST_EQ(isqrt(T{0}), T{0});
    BOOST_TEST_EQ(isqrt(T{1}), T{1});
}

// Larger values for wider types
void test_isqrt_u16_large()
{
    BOOST_TEST_EQ(isqrt(u16{10000}), u16{100});
    BOOST_TEST_EQ(isqrt(u16{65025}), u16{255});  // 255^2 = 65025
    BOOST_TEST_EQ(isqrt(u16{65535}), u16{255});   // max u16
}

void test_isqrt_u32_large()
{
    BOOST_TEST_EQ(isqrt(u32{1000000}), u32{1000});
    BOOST_TEST_EQ(isqrt(u32{4294836225U}), u32{65535});  // 65535^2
    BOOST_TEST_EQ(isqrt(u32{4294967295U}), u32{65535});   // max u32
}

void test_isqrt_u64_large()
{
    BOOST_TEST_EQ(isqrt(u64{1000000000000ULL}), u64{1000000});
    BOOST_TEST_EQ(isqrt(u64{10000000000ULL}), u64{100000});
}

void test_isqrt_u128_large()
{
    using boost::int128::uint128_t;

    // Perfect squares
    BOOST_TEST_EQ(isqrt(u128{uint128_t{0}}), u128{uint128_t{0}});
    BOOST_TEST_EQ(isqrt(u128{uint128_t{1}}), u128{uint128_t{1}});
    BOOST_TEST_EQ(isqrt(u128{uint128_t{4}}), u128{uint128_t{2}});
    BOOST_TEST_EQ(isqrt(u128{uint128_t{100}}), u128{uint128_t{10}});
    BOOST_TEST_EQ(isqrt(u128{uint128_t{10000}}), u128{uint128_t{100}});
    BOOST_TEST_EQ(isqrt(u128{uint128_t{1000000}}), u128{uint128_t{1000}});

    // Non-perfect squares
    BOOST_TEST_EQ(isqrt(u128{uint128_t{2}}), u128{uint128_t{1}});
    BOOST_TEST_EQ(isqrt(u128{uint128_t{3}}), u128{uint128_t{1}});
    BOOST_TEST_EQ(isqrt(u128{uint128_t{10}}), u128{uint128_t{3}});
    BOOST_TEST_EQ(isqrt(u128{uint128_t{200}}), u128{uint128_t{14}});

    // Large values beyond u64 range
    // 10^20 = (10^10)^2, so isqrt(10^20) == 10^10
    const auto ten_pow_20 = uint128_t{10000000000ULL} * uint128_t{10000000000ULL};
    BOOST_TEST_EQ(isqrt(u128{ten_pow_20}), u128{uint128_t{10000000000ULL}});

    // 10^20 + 1 is not a perfect square, floor(sqrt) should still be 10^10
    const auto ten_pow_20_plus_1 = ten_pow_20 + uint128_t{1};
    BOOST_TEST_EQ(isqrt(u128{ten_pow_20_plus_1}), u128{uint128_t{10000000000ULL}});

    // (2^64)^2 = 2^128 doesn't fit, but (2^64 - 1)^2 does:
    // isqrt((2^64-1)^2) == 2^64-1
    const auto max_u64_val = uint128_t{UINT64_MAX};
    const auto max_u64_squared = max_u64_val * max_u64_val;
    BOOST_TEST_EQ(isqrt(u128{max_u64_squared}), u128{max_u64_val});
}

// =============================================================================
// Constexpr isqrt tests (runtime types, compile-time evaluation)
// =============================================================================

void test_isqrt_constexpr()
{
    constexpr auto r0 = isqrt(u32{0});
    constexpr auto r1 = isqrt(u32{1});
    constexpr auto r4 = isqrt(u32{4});
    constexpr auto r9 = isqrt(u32{9});
    constexpr auto r10 = isqrt(u32{10});

    BOOST_TEST_EQ(r0, u32{0});
    BOOST_TEST_EQ(r1, u32{1});
    BOOST_TEST_EQ(r4, u32{2});
    BOOST_TEST_EQ(r9, u32{3});
    BOOST_TEST_EQ(r10, u32{3});
}

// =============================================================================
// Verified type isqrt tests (consteval)
// =============================================================================

void test_isqrt_verified()
{
    constexpr auto r0 = isqrt(verified_u8{u8{0}});
    constexpr auto r1 = isqrt(verified_u8{u8{1}});
    constexpr auto r4 = isqrt(verified_u8{u8{4}});
    constexpr auto r9 = isqrt(verified_u8{u8{9}});

    BOOST_TEST_EQ(r0, verified_u8{u8{0}});
    BOOST_TEST_EQ(r1, verified_u8{u8{1}});
    BOOST_TEST_EQ(r4, verified_u8{u8{2}});
    BOOST_TEST_EQ(r9, verified_u8{u8{3}});

    constexpr auto r16 = isqrt(verified_u16{u16{16}});
    constexpr auto r25 = isqrt(verified_u16{u16{25}});
    constexpr auto r10 = isqrt(verified_u16{u16{10}});

    BOOST_TEST_EQ(r16, verified_u16{u16{4}});
    BOOST_TEST_EQ(r25, verified_u16{u16{5}});
    BOOST_TEST_EQ(r10, verified_u16{u16{3}});

    constexpr auto r100 = isqrt(verified_u32{u32{100}});
    constexpr auto r144 = isqrt(verified_u32{u32{144}});
    constexpr auto r200 = isqrt(verified_u32{u32{200}});

    BOOST_TEST_EQ(r100, verified_u32{u32{10}});
    BOOST_TEST_EQ(r144, verified_u32{u32{12}});
    BOOST_TEST_EQ(r200, verified_u32{u32{14}});

    constexpr auto r64_val = isqrt(verified_u64{u64{1000000}});
    BOOST_TEST_EQ(r64_val, verified_u64{u64{1000}});

    using boost::int128::uint128_t;
    constexpr auto r128_val = isqrt(verified_u128{u128{uint128_t{10000}}});
    BOOST_TEST_EQ(r128_val, verified_u128{u128{uint128_t{100}}});
}

// =============================================================================
// Verify isqrt result property: result^2 <= n < (result+1)^2
// =============================================================================

template <typename T>
void test_isqrt_property()
{
    for (unsigned i = 0; i < 256; ++i)
    {
        const auto n = T{static_cast<typename detail::underlying_type_t<T>>(i)};
        const auto r = isqrt(n);

        // r * r <= n
        BOOST_TEST(r * r <= n);

        // (r + 1) * (r + 1) > n
        const auto r_plus_1 = r + T{1};
        BOOST_TEST(r_plus_1 * r_plus_1 > n);
    }
}

int main()
{
    // Perfect squares
    test_isqrt_perfect_squares<u8>();
    test_isqrt_perfect_squares<u16>();
    test_isqrt_perfect_squares<u32>();
    test_isqrt_perfect_squares<u64>();
    test_isqrt_perfect_squares<u128>();

    // Non-perfect squares
    test_isqrt_non_perfect_squares<u8>();
    test_isqrt_non_perfect_squares<u16>();
    test_isqrt_non_perfect_squares<u32>();
    test_isqrt_non_perfect_squares<u64>();
    test_isqrt_non_perfect_squares<u128>();

    // Edge cases
    test_isqrt_edge_cases<u8>();
    test_isqrt_edge_cases<u16>();
    test_isqrt_edge_cases<u32>();
    test_isqrt_edge_cases<u64>();
    test_isqrt_edge_cases<u128>();

    // Larger values for wider types
    test_isqrt_u16_large();
    test_isqrt_u32_large();
    test_isqrt_u64_large();
    test_isqrt_u128_large();

    // Constexpr evaluation
    test_isqrt_constexpr();

    // Verified types (consteval)
    test_isqrt_verified();

    // Property: r^2 <= n < (r+1)^2 for all values 0..255
    test_isqrt_property<u16>();
    test_isqrt_property<u32>();
    test_isqrt_property<u128>();

    return boost::report_errors();
}

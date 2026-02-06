// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/lightweight_test.hpp>
#include <boost/safe_numbers.hpp>

#include <type_traits>
#include <limits>
#include <cstdint>

using namespace boost::safe_numbers;
using boost::int128::uint128_t;

// -----------------------------------------------
// Static assertions on basis_type selection
// -----------------------------------------------

// Max fits in uint8_t -> basis_type should be u8
static_assert(std::is_same_v<bounded_uint<0u, 200u>::basis_type, u8>);
static_assert(std::is_same_v<bounded_uint<0u, 255u>::basis_type, u8>);
static_assert(std::is_same_v<bounded_uint<10u, 100u>::basis_type, u8>);

// Max exceeds uint8_t but fits in uint16_t -> basis_type should be u16
static_assert(std::is_same_v<bounded_uint<0u, 256u>::basis_type, u16>);
static_assert(std::is_same_v<bounded_uint<0u, 40000u>::basis_type, u16>);
static_assert(std::is_same_v<bounded_uint<0u, 65535u>::basis_type, u16>);

// Max exceeds uint16_t but fits in uint32_t -> basis_type should be u32
static_assert(std::is_same_v<bounded_uint<0u, 65536u>::basis_type, u32>);
static_assert(std::is_same_v<bounded_uint<0u, 100000u>::basis_type, u32>);
static_assert(std::is_same_v<bounded_uint<0u, 4294967295u>::basis_type, u32>);

// Max exceeds uint32_t but fits in uint64_t -> basis_type should be u64
static_assert(std::is_same_v<bounded_uint<0ULL, 4294967296ULL>::basis_type, u64>);
static_assert(std::is_same_v<bounded_uint<0ULL, 5'000'000'000ULL>::basis_type, u64>);
static_assert(std::is_same_v<bounded_uint<0ULL, UINT64_MAX>::basis_type, u64>);

// Max exceeds uint64_t -> basis_type should be u128
static_assert(std::is_same_v<bounded_uint<uint128_t{0}, uint128_t{1, 0}>::basis_type, u128>);
static_assert(std::is_same_v<bounded_uint<uint128_t{0}, uint128_t{UINT64_MAX, UINT64_MAX}>::basis_type, u128>);

// -----------------------------------------------
// sizeof checks - bounded_uint should be the same
// size as the underlying safe type
// -----------------------------------------------

static_assert(sizeof(bounded_uint<0u, 200u>) == sizeof(u8));
static_assert(sizeof(bounded_uint<0u, 40000u>) == sizeof(u16));
static_assert(sizeof(bounded_uint<0u, 100000u>) == sizeof(u32));
static_assert(sizeof(bounded_uint<0ULL, 5'000'000'000ULL>) == sizeof(u64));
static_assert(sizeof(bounded_uint<uint128_t{0}, uint128_t{1, 0}>) == sizeof(u128));

// -----------------------------------------------
// Construction and comparison tests
// -----------------------------------------------

void test_u8_range()
{
    // Construction from value
    constexpr bounded_uint<0u, 255u> a {u8{0}};
    constexpr bounded_uint<0u, 255u> b {u8{0}};
    BOOST_TEST(a == b);

    constexpr bounded_uint<0u, 255u> c {u8{42}};
    constexpr bounded_uint<0u, 255u> d {u8{42}};
    BOOST_TEST(c == d);
    BOOST_TEST(c != a);

    constexpr bounded_uint<0u, 255u> e {u8{255}};
    BOOST_TEST(e > c);
    BOOST_TEST(c < e);
    BOOST_TEST(a <= c);
    BOOST_TEST(e >= c);

    // Non-zero minimum
    constexpr bounded_uint<10u, 200u> f {u8{10}};
    constexpr bounded_uint<10u, 200u> g {u8{200}};
    constexpr bounded_uint<10u, 200u> h {u8{100}};
    BOOST_TEST(f < g);
    BOOST_TEST(h > f);
    BOOST_TEST(h < g);
}

void test_u16_range()
{
    constexpr bounded_uint<0u, 1000u> a {u16{0}};
    constexpr bounded_uint<0u, 1000u> b {u16{0}};
    BOOST_TEST(a == b);

    constexpr bounded_uint<0u, 1000u> c {u16{500}};
    constexpr bounded_uint<0u, 1000u> d {u16{1000}};
    BOOST_TEST(c < d);
    BOOST_TEST(c != a);

    constexpr bounded_uint<256u, 65535u> e {u16{256}};
    constexpr bounded_uint<256u, 65535u> f {u16{65535}};
    constexpr bounded_uint<256u, 65535u> g {u16{30000}};
    BOOST_TEST(e < f);
    BOOST_TEST(g > e);
    BOOST_TEST(g < f);
}

void test_u32_range()
{
    constexpr bounded_uint<0u, 100000u> a {u32{0}};
    constexpr bounded_uint<0u, 100000u> b {u32{100000}};
    constexpr bounded_uint<0u, 100000u> c {u32{50000}};
    BOOST_TEST(a < b);
    BOOST_TEST(c > a);
    BOOST_TEST(c < b);

    constexpr bounded_uint<65536u, 4294967295u> d {u32{65536}};
    constexpr bounded_uint<65536u, 4294967295u> e {u32{4294967295u}};
    constexpr bounded_uint<65536u, 4294967295u> f {u32{1000000}};
    BOOST_TEST(d < e);
    BOOST_TEST(f > d);
    BOOST_TEST(f < e);
}

void test_u64_range()
{
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> a {u64{0}};
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> b {u64{5'000'000'000ULL}};
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> c {u64{2'500'000'000ULL}};
    BOOST_TEST(a < b);
    BOOST_TEST(c > a);
    BOOST_TEST(c < b);

    constexpr bounded_uint<4294967296ULL, UINT64_MAX> d {u64{4294967296ULL}};
    constexpr bounded_uint<4294967296ULL, UINT64_MAX> e {u64{UINT64_MAX}};
    BOOST_TEST(d < e);
    BOOST_TEST(d == d);
    BOOST_TEST(e == e);
}

void test_u128_range()
{
    // Min = 0, Max = 2^64 (one beyond uint64_t max)
    constexpr auto max_val = uint128_t{1, 0};
    using b128 = bounded_uint<uint128_t{0}, max_val>;

    constexpr b128 a {u128{uint128_t{0}}};
    constexpr b128 b {u128{uint128_t{42}}};
    constexpr b128 c {u128{uint128_t{1000}}};
    BOOST_TEST(a < b);
    BOOST_TEST(b < c);
    BOOST_TEST(a < c);
    BOOST_TEST(a == a);
    BOOST_TEST(b == b);

    // Max = uint128_t maximum, test values spanning both halves
    constexpr auto u128_max = uint128_t{UINT64_MAX, UINT64_MAX};
    using b128_full = bounded_uint<uint128_t{0}, u128_max>;

    constexpr b128_full d {u128{uint128_t{0}}};
    constexpr b128_full e {u128{uint128_t{1'000'000}}};
    constexpr b128_full f {u128{uint128_t{1, 0}}};
    constexpr b128_full g {u128{u128_max}};
    BOOST_TEST(d < e);
    BOOST_TEST(e < f);
    BOOST_TEST(f < g);
    BOOST_TEST(d < g);
}

void test_same_value_equality()
{
    // Verify that constructing the same value yields equal objects across all widths
    constexpr bounded_uint<0u, 200u> a8 {u8{100}};
    constexpr bounded_uint<0u, 200u> b8 {u8{100}};
    BOOST_TEST(a8 == b8);
    BOOST_TEST(!(a8 != b8));
    BOOST_TEST(!(a8 < b8));
    BOOST_TEST(!(a8 > b8));
    BOOST_TEST(a8 <= b8);
    BOOST_TEST(a8 >= b8);

    constexpr bounded_uint<0u, 1000u> a16 {u16{500}};
    constexpr bounded_uint<0u, 1000u> b16 {u16{500}};
    BOOST_TEST(a16 == b16);

    constexpr bounded_uint<0u, 100000u> a32 {u32{50000}};
    constexpr bounded_uint<0u, 100000u> b32 {u32{50000}};
    BOOST_TEST(a32 == b32);

    constexpr bounded_uint<0ULL, 5'000'000'000ULL> a64 {u64{2'500'000'000ULL}};
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> b64 {u64{2'500'000'000ULL}};
    BOOST_TEST(a64 == b64);
}

void test_constexpr_construction()
{
    // Verify all constructions are valid in constexpr context
    constexpr bounded_uint<0u, 255u> a {u8{0}};
    constexpr bounded_uint<0u, 255u> b {u8{255}};
    static_assert(a < b);
    static_assert(a == a);
    static_assert(b == b);
    static_assert(a != b);

    constexpr bounded_uint<0u, 65535u> c {u16{0}};
    constexpr bounded_uint<0u, 65535u> d {u16{65535}};
    static_assert(c < d);

    constexpr bounded_uint<0u, 4294967295u> e {u32{0}};
    constexpr bounded_uint<0u, 4294967295u> f {u32{4294967295u}};
    static_assert(e < f);

    constexpr bounded_uint<0ULL, UINT64_MAX> g {u64{0}};
    constexpr bounded_uint<0ULL, UINT64_MAX> h {u64{UINT64_MAX}};
    static_assert(g < h);
}

int main()
{
    test_u8_range();
    test_u16_range();
    test_u32_range();
    test_u64_range();
    test_u128_range();
    test_same_value_equality();
    test_constexpr_construction();

    return boost::report_errors();
}

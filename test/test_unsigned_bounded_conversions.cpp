// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/lightweight_test.hpp>

// Ignore [[nodiscard]] on the tests that we know are going to throw
#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunused-result"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wunused-result"
#elif defined(_MSC_VER)
#  pragma warning (push)
#  pragma warning (disable: 4834)
#endif

#include <boost/safe_numbers.hpp>

#include <stdexcept>
#include <limits>
#include <cstdint>

using namespace boost::safe_numbers;
using boost::int128::uint128_t;

// -----------------------------------------------
// Same basis type, different bounds
// -----------------------------------------------

void test_u8_to_u8_wider()
{
    // [10, 100] -> [0, 255]: value 50 fits in wider range
    constexpr bounded_uint<10u, 100u> a {u8{50}};
    const auto b {static_cast<bounded_uint<0u, 255u>>(a)};
    const bounded_uint<0u, 255u> expected {u8{50}};
    BOOST_TEST(b == expected);

    // Min boundary: [10, 100] -> [0, 255]: value 10
    constexpr bounded_uint<10u, 100u> c {u8{10}};
    const auto d {static_cast<bounded_uint<0u, 255u>>(c)};
    const bounded_uint<0u, 255u> expected_d {u8{10}};
    BOOST_TEST(d == expected_d);

    // Max boundary: [10, 100] -> [0, 255]: value 100
    constexpr bounded_uint<10u, 100u> e {u8{100}};
    const auto f {static_cast<bounded_uint<0u, 255u>>(e)};
    const bounded_uint<0u, 255u> expected_f {u8{100}};
    BOOST_TEST(f == expected_f);
}

void test_u8_to_u8_narrower()
{
    // [0, 255] -> [10, 100]: value 50 fits in narrower range
    constexpr bounded_uint<0u, 255u> a {u8{50}};
    const auto b {static_cast<bounded_uint<10u, 100u>>(a)};
    const bounded_uint<10u, 100u> expected {u8{50}};
    BOOST_TEST(b == expected);

    // Exact boundaries of target
    constexpr bounded_uint<0u, 255u> c {u8{10}};
    const auto d {static_cast<bounded_uint<10u, 100u>>(c)};
    const bounded_uint<10u, 100u> expected_d {u8{10}};
    BOOST_TEST(d == expected_d);

    constexpr bounded_uint<0u, 255u> e {u8{100}};
    const auto f {static_cast<bounded_uint<10u, 100u>>(e)};
    const bounded_uint<10u, 100u> expected_f {u8{100}};
    BOOST_TEST(f == expected_f);
}

void test_u8_to_u8_throwing()
{
    // [0, 255] -> [10, 100]: value 5 < 10, below target min
    constexpr bounded_uint<0u, 255u> a {u8{5}};
    BOOST_TEST_THROWS((static_cast<bounded_uint<10u, 100u>>(a)), std::domain_error);

    // [0, 255] -> [10, 100]: value 0 < 10
    constexpr bounded_uint<0u, 255u> b {u8{0}};
    BOOST_TEST_THROWS((static_cast<bounded_uint<10u, 100u>>(b)), std::domain_error);

    // [0, 255] -> [10, 100]: value 101 > 100, above target max
    constexpr bounded_uint<0u, 255u> c {u8{101}};
    BOOST_TEST_THROWS((static_cast<bounded_uint<10u, 100u>>(c)), std::domain_error);

    // [0, 255] -> [10, 100]: value 255 > 100
    constexpr bounded_uint<0u, 255u> d {u8{255}};
    BOOST_TEST_THROWS((static_cast<bounded_uint<10u, 100u>>(d)), std::domain_error);

    // [10, 200] -> [50, 150]: value 10 < 50
    constexpr bounded_uint<10u, 200u> e {u8{10}};
    BOOST_TEST_THROWS((static_cast<bounded_uint<50u, 150u>>(e)), std::domain_error);

    // [10, 200] -> [50, 150]: value 200 > 150
    constexpr bounded_uint<10u, 200u> f {u8{200}};
    BOOST_TEST_THROWS((static_cast<bounded_uint<50u, 150u>>(f)), std::domain_error);
}

// -----------------------------------------------
// Same basis type, u16 bounds
// -----------------------------------------------

void test_u16_to_u16_conversions()
{
    // [0, 65535] -> [256, 1000]: value 500
    constexpr bounded_uint<0u, 65535u> a {u16{500}};
    const auto b {static_cast<bounded_uint<256u, 1000u>>(a)};
    const bounded_uint<256u, 1000u> expected {u16{500}};
    BOOST_TEST(b == expected);

    // [256, 1000] -> [0, 65535]: value 500 (wider target always succeeds for in-range values)
    constexpr bounded_uint<256u, 1000u> c {u16{500}};
    const auto d {static_cast<bounded_uint<0u, 65535u>>(c)};
    const bounded_uint<0u, 65535u> expected_d {u16{500}};
    BOOST_TEST(d == expected_d);

    // Throwing: [0, 65535] -> [256, 1000]: value 100 < 256
    constexpr bounded_uint<0u, 65535u> e {u16{100}};
    BOOST_TEST_THROWS((static_cast<bounded_uint<256u, 1000u>>(e)), std::domain_error);

    // Throwing: [0, 65535] -> [256, 1000]: value 2000 > 1000
    constexpr bounded_uint<0u, 65535u> f {u16{2000}};
    BOOST_TEST_THROWS((static_cast<bounded_uint<256u, 1000u>>(f)), std::domain_error);
}

// -----------------------------------------------
// Cross-width conversions: smaller to larger basis
// -----------------------------------------------

void test_u8_to_u16()
{
    // [0, 255] -> [0, 1000]: value 200 fits (u8 -> u16 widening)
    constexpr bounded_uint<0u, 255u> a {u8{200}};
    const auto b {static_cast<bounded_uint<0u, 1000u>>(a)};
    const bounded_uint<0u, 1000u> expected {u16{200}};
    BOOST_TEST(b == expected);

    // [0, 255] -> [0, 65535]: full range value
    constexpr bounded_uint<0u, 255u> c {u8{255}};
    const auto d {static_cast<bounded_uint<0u, 65535u>>(c)};
    const bounded_uint<0u, 65535u> expected_d {u16{255}};
    BOOST_TEST(d == expected_d);

    // [10, 100] -> [0, 40000]: value 50
    constexpr bounded_uint<10u, 100u> e {u8{50}};
    const auto f {static_cast<bounded_uint<0u, 40000u>>(e)};
    const bounded_uint<0u, 40000u> expected_f {u16{50}};
    BOOST_TEST(f == expected_f);
}

void test_u8_to_u16_throwing()
{
    // [0, 255] -> [256, 1000]: any u8 value < 256, always below target min
    constexpr bounded_uint<0u, 255u> a {u8{255}};
    BOOST_TEST_THROWS((static_cast<bounded_uint<256u, 1000u>>(a)), std::domain_error);

    constexpr bounded_uint<0u, 255u> b {u8{0}};
    BOOST_TEST_THROWS((static_cast<bounded_uint<256u, 1000u>>(b)), std::domain_error);
}

void test_u8_to_u32()
{
    // [0, 255] -> [0, 100000]: value 100
    constexpr bounded_uint<0u, 255u> a {u8{100}};
    const auto b {static_cast<bounded_uint<0u, 100000u>>(a)};
    const bounded_uint<0u, 100000u> expected {u32{100}};
    BOOST_TEST(b == expected);
}

void test_u16_to_u32()
{
    // [0, 1000] -> [0, 100000]: value 1000
    constexpr bounded_uint<0u, 1000u> a {u16{1000}};
    const auto b {static_cast<bounded_uint<0u, 100000u>>(a)};
    const bounded_uint<0u, 100000u> expected {u32{1000}};
    BOOST_TEST(b == expected);

    // [256, 65535] -> [0, 4294967295]: value 30000
    constexpr bounded_uint<256u, 65535u> c {u16{30000}};
    const auto d {static_cast<bounded_uint<0u, 4294967295u>>(c)};
    const bounded_uint<0u, 4294967295u> expected_d {u32{30000}};
    BOOST_TEST(d == expected_d);
}

void test_u32_to_u64()
{
    // [0, 100000] -> [0, 5'000'000'000]: value 100000
    constexpr bounded_uint<0u, 100000u> a {u32{100000}};
    const auto b {static_cast<bounded_uint<0ULL, 5'000'000'000ULL>>(a)};
    const bounded_uint<0ULL, 5'000'000'000ULL> expected {u64{100000}};
    BOOST_TEST(b == expected);

    // Throwing: [0, 100000] -> [4294967296, UINT64_MAX]: any u32 value < 4294967296
    constexpr bounded_uint<0u, 100000u> c {u32{100000}};
    BOOST_TEST_THROWS((static_cast<bounded_uint<4294967296ULL, UINT64_MAX>>(c)), std::domain_error);
}

void test_u64_to_u128()
{
    // [0, UINT64_MAX] -> [0, 2^64]: value fits
    constexpr auto max_128 = uint128_t{1, 0};
    using b128 = bounded_uint<uint128_t{0}, max_128>;

    constexpr bounded_uint<0ULL, UINT64_MAX> a {u64{1'000'000ULL}};
    const auto b {static_cast<b128>(a)};
    const b128 expected {u128{uint128_t{1'000'000}}};
    BOOST_TEST(b == expected);

    // Max u64 value -> u128 range
    constexpr bounded_uint<0ULL, UINT64_MAX> c {u64{UINT64_MAX}};
    const auto d {static_cast<b128>(c)};
    const b128 expected_d {u128{uint128_t{UINT64_MAX}}};
    BOOST_TEST(d == expected_d);
}

// -----------------------------------------------
// Overlapping ranges (partially compatible)
// -----------------------------------------------

void test_overlapping_ranges()
{
    // [10, 100] -> [50, 200]: value 75 is in both ranges
    constexpr bounded_uint<10u, 100u> a {u8{75}};
    const auto b {static_cast<bounded_uint<50u, 200u>>(a)};
    const bounded_uint<50u, 200u> expected {u8{75}};
    BOOST_TEST(b == expected);

    // [10, 100] -> [50, 200]: value 50 is exactly at target min
    constexpr bounded_uint<10u, 100u> c {u8{50}};
    const auto d {static_cast<bounded_uint<50u, 200u>>(c)};
    const bounded_uint<50u, 200u> expected_d {u8{50}};
    BOOST_TEST(d == expected_d);

    // [10, 100] -> [50, 200]: value 100 is exactly at source max, within target
    constexpr bounded_uint<10u, 100u> e {u8{100}};
    const auto f {static_cast<bounded_uint<50u, 200u>>(e)};
    const bounded_uint<50u, 200u> expected_f {u8{100}};
    BOOST_TEST(f == expected_f);

    // Throwing: [10, 100] -> [50, 200]: value 30 is in source but below target min
    constexpr bounded_uint<10u, 100u> g {u8{30}};
    BOOST_TEST_THROWS((static_cast<bounded_uint<50u, 200u>>(g)), std::domain_error);

    // [50, 200] -> [10, 100]: value 75 is in both ranges
    constexpr bounded_uint<50u, 200u> h {u8{75}};
    const auto i {static_cast<bounded_uint<10u, 100u>>(h)};
    const bounded_uint<10u, 100u> expected_i {u8{75}};
    BOOST_TEST(i == expected_i);

    // Throwing: [50, 200] -> [10, 100]: value 150 is in source but above target max
    constexpr bounded_uint<50u, 200u> j {u8{150}};
    BOOST_TEST_THROWS((static_cast<bounded_uint<10u, 100u>>(j)), std::domain_error);
}

// -----------------------------------------------
// Disjoint ranges (no overlap)
// -----------------------------------------------

void test_disjoint_ranges()
{
    // [0, 50] -> [100, 200]: any value from source < 100, always throws
    constexpr bounded_uint<0u, 50u> a {u8{50}};
    BOOST_TEST_THROWS((static_cast<bounded_uint<100u, 200u>>(a)), std::domain_error);

    constexpr bounded_uint<0u, 50u> b {u8{0}};
    BOOST_TEST_THROWS((static_cast<bounded_uint<100u, 200u>>(b)), std::domain_error);

    // [100, 200] -> [0, 50]: any value from source > 50, always throws
    constexpr bounded_uint<100u, 200u> c {u8{100}};
    BOOST_TEST_THROWS((static_cast<bounded_uint<0u, 50u>>(c)), std::domain_error);

    constexpr bounded_uint<100u, 200u> d {u8{200}};
    BOOST_TEST_THROWS((static_cast<bounded_uint<0u, 50u>>(d)), std::domain_error);

    // u16 disjoint: [0, 500] -> [1000, 2000]
    constexpr bounded_uint<0u, 500u> e {u16{500}};
    BOOST_TEST_THROWS((static_cast<bounded_uint<1000u, 2000u>>(e)), std::domain_error);
}

// -----------------------------------------------
// Identity conversion (same bounds)
// -----------------------------------------------

void test_identity_conversion()
{
    // [0, 255] -> [0, 255]: should always work
    constexpr bounded_uint<0u, 255u> a {u8{42}};
    const auto b {static_cast<bounded_uint<0u, 255u>>(a)};
    BOOST_TEST(a == b);

    constexpr bounded_uint<0u, 255u> c {u8{0}};
    const auto d {static_cast<bounded_uint<0u, 255u>>(c)};
    BOOST_TEST(c == d);

    constexpr bounded_uint<0u, 255u> e {u8{255}};
    const auto f {static_cast<bounded_uint<0u, 255u>>(e)};
    BOOST_TEST(e == f);

    // [10, 200] -> [10, 200]
    constexpr bounded_uint<10u, 200u> g {u8{100}};
    const auto h {static_cast<bounded_uint<10u, 200u>>(g)};
    BOOST_TEST(g == h);
}

// -----------------------------------------------
// Constexpr conversion tests
// -----------------------------------------------

void test_constexpr_conversions()
{
    // u8 -> u8 wider
    constexpr bounded_uint<10u, 100u> a {u8{50}};
    constexpr auto b {static_cast<bounded_uint<0u, 255u>>(a)};
    constexpr bounded_uint<0u, 255u> expected_b {u8{50}};
    static_assert(b == expected_b);

    // u8 -> u8 narrower
    constexpr bounded_uint<0u, 255u> c {u8{75}};
    constexpr auto d {static_cast<bounded_uint<10u, 100u>>(c)};
    constexpr bounded_uint<10u, 100u> expected_d {u8{75}};
    static_assert(d == expected_d);

    // u8 -> u16 widening
    constexpr bounded_uint<0u, 255u> e {u8{200}};
    constexpr auto f {static_cast<bounded_uint<0u, 1000u>>(e)};
    constexpr bounded_uint<0u, 1000u> expected_f {u16{200}};
    static_assert(f == expected_f);

    // u16 -> u32 widening
    constexpr bounded_uint<0u, 1000u> g {u16{1000}};
    constexpr auto h {static_cast<bounded_uint<0u, 100000u>>(g)};
    constexpr bounded_uint<0u, 100000u> expected_h {u32{1000}};
    static_assert(h == expected_h);

    // Identity
    constexpr bounded_uint<0u, 255u> i {u8{42}};
    constexpr auto j {static_cast<bounded_uint<0u, 255u>>(i)};
    static_assert(i == j);
}

// -----------------------------------------------
// u128 conversions
// -----------------------------------------------

void test_u128_conversions()
{
    constexpr auto max_128 = uint128_t{1, 0};
    constexpr auto u128_max = uint128_t{UINT64_MAX, UINT64_MAX};

    // Narrower u128 range -> wider u128 range
    using b128_narrow = bounded_uint<uint128_t{1000}, max_128>;
    using b128_wide = bounded_uint<uint128_t{0}, u128_max>;

    const b128_narrow a {u128{uint128_t{5000}}};
    const auto b {static_cast<b128_wide>(a)};
    const b128_wide expected {u128{uint128_t{5000}}};
    BOOST_TEST(b == expected);

    // Wider -> narrower, value in range
    const b128_wide c {u128{uint128_t{5000}}};
    const auto d {static_cast<b128_narrow>(c)};
    const b128_narrow expected_d {u128{uint128_t{5000}}};
    BOOST_TEST(d == expected_d);

    // Wider -> narrower, value below min: 500 < 1000
    const b128_wide e {u128{uint128_t{500}}};
    BOOST_TEST_THROWS((static_cast<b128_narrow>(e)), std::domain_error);

    // Wider -> narrower, value above max: max_128 + 1 > max_128
    const b128_wide f {u128{uint128_t{1, 1}}};
    BOOST_TEST_THROWS((static_cast<b128_narrow>(f)), std::domain_error);
}

// -----------------------------------------------
// Narrowing conversions via operator OtherBasis()
// (bounded_uint -> smaller safe type)
// -----------------------------------------------

void test_bounded_to_basis_widening()
{
    // bounded_uint with u8 basis -> u16 (widening, always works)
    constexpr bounded_uint<0u, 255u> a {u8{200}};
    const auto b {static_cast<u16>(a)};
    const u16 expected {200};
    BOOST_TEST(b == expected);

    // bounded_uint with u8 basis -> u32
    const auto c {static_cast<u32>(a)};
    const u32 expected_c {200};
    BOOST_TEST(c == expected_c);

    // bounded_uint with u16 basis -> u32
    constexpr bounded_uint<0u, 1000u> d {u16{1000}};
    const auto e {static_cast<u32>(d)};
    const u32 expected_e {1000};
    BOOST_TEST(e == expected_e);

    // bounded_uint with u16 basis -> u64
    const auto f {static_cast<u64>(d)};
    const u64 expected_f {1000};
    BOOST_TEST(f == expected_f);

    // bounded_uint with u32 basis -> u64
    constexpr bounded_uint<0u, 100000u> g {u32{100000}};
    const auto h {static_cast<u64>(g)};
    const u64 expected_h {100000};
    BOOST_TEST(h == expected_h);
}

void test_bounded_to_basis_same_width()
{
    // bounded_uint with u8 basis -> u8 (same width, always works)
    constexpr bounded_uint<0u, 255u> a {u8{42}};
    const auto b {static_cast<u8>(a)};
    const u8 expected {42};
    BOOST_TEST(b == expected);

    constexpr bounded_uint<10u, 100u> c {u8{75}};
    const auto d {static_cast<u8>(c)};
    const u8 expected_d {75};
    BOOST_TEST(d == expected_d);

    // bounded_uint with u16 basis -> u16
    constexpr bounded_uint<0u, 1000u> e {u16{500}};
    const auto f {static_cast<u16>(e)};
    const u16 expected_f {500};
    BOOST_TEST(f == expected_f);
}

void test_bounded_to_basis_narrowing_fits()
{
    // bounded_uint with u16 basis -> u8: value 200 fits in u8
    constexpr bounded_uint<0u, 1000u> a {u16{200}};
    const auto b {static_cast<u8>(a)};
    const u8 expected {200};
    BOOST_TEST(b == expected);

    // bounded_uint with u16 basis -> u8: value 0 fits
    constexpr bounded_uint<0u, 1000u> c {u16{0}};
    const auto d {static_cast<u8>(c)};
    const u8 expected_d {0};
    BOOST_TEST(d == expected_d);

    // bounded_uint with u16 basis -> u8: value 255 fits (exact max)
    constexpr bounded_uint<0u, 1000u> e {u16{255}};
    const auto f {static_cast<u8>(e)};
    const u8 expected_f {255};
    BOOST_TEST(f == expected_f);

    // bounded_uint with u32 basis -> u16: value 30000 fits in u16
    constexpr bounded_uint<0u, 100000u> g {u32{30000}};
    const auto h {static_cast<u16>(g)};
    const u16 expected_h {30000};
    BOOST_TEST(h == expected_h);

    // bounded_uint with u32 basis -> u8: value 100 fits in u8
    constexpr bounded_uint<0u, 100000u> i {u32{100}};
    const auto j {static_cast<u8>(i)};
    const u8 expected_j {100};
    BOOST_TEST(j == expected_j);

    // bounded_uint with u64 basis -> u32: value fits
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> k {u64{1'000'000ULL}};
    const auto l {static_cast<u32>(k)};
    const u32 expected_l {1'000'000u};
    BOOST_TEST(l == expected_l);
}

void test_bounded_to_basis_narrowing_throws()
{
    // bounded_uint with u16 basis -> u8: value 256 > 255
    constexpr bounded_uint<0u, 1000u> a {u16{256}};
    BOOST_TEST_THROWS((static_cast<u8>(a)), std::domain_error);

    // bounded_uint with u16 basis -> u8: value 1000 > 255
    constexpr bounded_uint<0u, 1000u> b {u16{1000}};
    BOOST_TEST_THROWS((static_cast<u8>(b)), std::domain_error);

    // bounded_uint with u32 basis -> u8: value 300 > 255
    constexpr bounded_uint<0u, 100000u> c {u32{300}};
    BOOST_TEST_THROWS((static_cast<u8>(c)), std::domain_error);

    // bounded_uint with u32 basis -> u16: value 70000 > 65535
    constexpr bounded_uint<0u, 100000u> d {u32{70000}};
    BOOST_TEST_THROWS((static_cast<u16>(d)), std::domain_error);

    // bounded_uint with u64 basis -> u32: value 5'000'000'000 > UINT32_MAX
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> e {u64{5'000'000'000ULL}};
    BOOST_TEST_THROWS((static_cast<u32>(e)), std::domain_error);

    // bounded_uint with u64 basis -> u8: value 1000 > 255
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> f {u64{1000}};
    BOOST_TEST_THROWS((static_cast<u8>(f)), std::domain_error);

    // bounded_uint with u64 basis -> u16: value 100000 > 65535
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> g {u64{100000}};
    BOOST_TEST_THROWS((static_cast<u16>(g)), std::domain_error);
}

int main()
{
    test_u8_to_u8_wider();
    test_u8_to_u8_narrower();
    test_u8_to_u8_throwing();

    test_u16_to_u16_conversions();

    test_u8_to_u16();
    test_u8_to_u16_throwing();
    test_u8_to_u32();
    test_u16_to_u32();
    test_u32_to_u64();
    test_u64_to_u128();

    test_overlapping_ranges();
    test_disjoint_ranges();
    test_identity_conversion();
    test_constexpr_conversions();
    test_u128_conversions();

    // Narrowing operator OtherBasis() tests
    test_bounded_to_basis_widening();
    test_bounded_to_basis_same_width();
    test_bounded_to_basis_narrowing_fits();
    test_bounded_to_basis_narrowing_throws();

    return boost::report_errors();
}

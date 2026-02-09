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
// u8 basis: bounded_uint with Max <= 255
// -----------------------------------------------

void test_u8_valid_addition()
{
    // [0, 255] full range
    constexpr bounded_uint<0u, 255u> a {u8{10}};
    constexpr bounded_uint<0u, 255u> b {u8{20}};
    const auto c {a + b};
    const bounded_uint<0u, 255u> expected_c {u8{30}};
    BOOST_TEST(c == expected_c);

    // Adding zeros
    constexpr bounded_uint<0u, 255u> zero {u8{0}};
    const auto d {a + zero};
    BOOST_TEST(d == a);

    // Max boundary: 100 + 155 = 255
    constexpr bounded_uint<0u, 255u> e {u8{100}};
    constexpr bounded_uint<0u, 255u> f {u8{155}};
    const auto g {e + f};
    const bounded_uint<0u, 255u> expected_g {u8{255}};
    BOOST_TEST(g == expected_g);

    // Narrower range [10, 200]
    constexpr bounded_uint<10u, 200u> h {u8{50}};
    constexpr bounded_uint<10u, 200u> i {u8{30}};
    const auto j {h + i};
    const bounded_uint<10u, 200u> expected_j {u8{80}};
    BOOST_TEST(j == expected_j);

    // Narrower range boundary: 100 + 100 = 200
    constexpr bounded_uint<10u, 200u> k {u8{100}};
    constexpr bounded_uint<10u, 200u> l {u8{100}};
    const auto m {k + l};
    const bounded_uint<10u, 200u> expected_m {u8{200}};
    BOOST_TEST(m == expected_m);
}

void test_u8_throwing_addition()
{
    // Overflow of underlying u8: 200 + 200 = 400 > 255, u8 addition throws overflow_error
    constexpr bounded_uint<0u, 255u> a {u8{200}};
    constexpr bounded_uint<0u, 255u> b {u8{200}};
    BOOST_TEST_THROWS(a + b, std::overflow_error);

    // Result within u8 range but exceeds bounded max:
    // [10, 100]: 60 + 60 = 120 > 100, bounded_uint constructor throws domain_error
    constexpr bounded_uint<10u, 100u> c {u8{60}};
    constexpr bounded_uint<10u, 100u> d {u8{60}};
    BOOST_TEST_THROWS(c + d, std::domain_error);

    // Just one over the max: 51 + 50 = 101 > 100
    constexpr bounded_uint<10u, 100u> e {u8{51}};
    constexpr bounded_uint<10u, 100u> f {u8{50}};
    BOOST_TEST_THROWS(e + f, std::domain_error);
}

// -----------------------------------------------
// u16 basis: bounded_uint with Max in (255, 65535]
// -----------------------------------------------

void test_u16_valid_addition()
{
    // [0, 1000]
    constexpr bounded_uint<0u, 1000u> a {u16{100}};
    constexpr bounded_uint<0u, 1000u> b {u16{200}};
    const auto c {a + b};
    const bounded_uint<0u, 1000u> expected_c {u16{300}};
    BOOST_TEST(c == expected_c);

    // Adding zeros
    constexpr bounded_uint<0u, 1000u> zero {u16{0}};
    const auto d {a + zero};
    BOOST_TEST(d == a);

    // Max boundary: 500 + 500 = 1000
    constexpr bounded_uint<0u, 1000u> e {u16{500}};
    constexpr bounded_uint<0u, 1000u> f {u16{500}};
    const auto g {e + f};
    const bounded_uint<0u, 1000u> expected_g {u16{1000}};
    BOOST_TEST(g == expected_g);

    // [256, 65535] full u16 range with non-zero min
    constexpr bounded_uint<256u, 65535u> h {u16{1000}};
    constexpr bounded_uint<256u, 65535u> i {u16{2000}};
    const auto j {h + i};
    const bounded_uint<256u, 65535u> expected_j {u16{3000}};
    BOOST_TEST(j == expected_j);

    // [0, 40000]
    constexpr bounded_uint<0u, 40000u> k {u16{10000}};
    constexpr bounded_uint<0u, 40000u> l {u16{20000}};
    const auto m {k + l};
    const bounded_uint<0u, 40000u> expected_m {u16{30000}};
    BOOST_TEST(m == expected_m);
}

void test_u16_throwing_addition()
{
    // Overflow of underlying u16: 40000 + 40000 = 80000 > 65535
    constexpr bounded_uint<0u, 65535u> a {u16{40000}};
    constexpr bounded_uint<0u, 65535u> b {u16{40000}};
    BOOST_TEST_THROWS(a + b, std::overflow_error);

    // Result within u16 range but exceeds bounded max:
    // [0, 1000]: 600 + 600 = 1200 > 1000
    constexpr bounded_uint<0u, 1000u> c {u16{600}};
    constexpr bounded_uint<0u, 1000u> d {u16{600}};
    BOOST_TEST_THROWS(c + d, std::domain_error);

    // Just one over: 501 + 500 = 1001 > 1000
    constexpr bounded_uint<0u, 1000u> e {u16{501}};
    constexpr bounded_uint<0u, 1000u> f {u16{500}};
    BOOST_TEST_THROWS(e + f, std::domain_error);
}

// -----------------------------------------------
// u32 basis: bounded_uint with Max in (65535, 4294967295]
// -----------------------------------------------

void test_u32_valid_addition()
{
    // [0, 100000]
    constexpr bounded_uint<0u, 100000u> a {u32{25000}};
    constexpr bounded_uint<0u, 100000u> b {u32{30000}};
    const auto c {a + b};
    const bounded_uint<0u, 100000u> expected_c {u32{55000}};
    BOOST_TEST(c == expected_c);

    // Adding zeros
    constexpr bounded_uint<0u, 100000u> zero {u32{0}};
    const auto d {a + zero};
    BOOST_TEST(d == a);

    // Max boundary: 50000 + 50000 = 100000
    constexpr bounded_uint<0u, 100000u> e {u32{50000}};
    constexpr bounded_uint<0u, 100000u> f {u32{50000}};
    const auto g {e + f};
    const bounded_uint<0u, 100000u> expected_g {u32{100000}};
    BOOST_TEST(g == expected_g);

    // [65536, 4294967295] non-zero min with full u32 max
    constexpr bounded_uint<65536u, 4294967295u> h {u32{1000000}};
    constexpr bounded_uint<65536u, 4294967295u> i {u32{2000000}};
    const auto j {h + i};
    const bounded_uint<65536u, 4294967295u> expected_j {u32{3000000}};
    BOOST_TEST(j == expected_j);
}

void test_u32_throwing_addition()
{
    // Overflow of underlying u32
    constexpr bounded_uint<0u, 4294967295u> a {u32{3000000000u}};
    constexpr bounded_uint<0u, 4294967295u> b {u32{3000000000u}};
    BOOST_TEST_THROWS(a + b, std::overflow_error);

    // Result within u32 range but exceeds bounded max:
    // [0, 100000]: 60000 + 60000 = 120000 > 100000
    constexpr bounded_uint<0u, 100000u> c {u32{60000}};
    constexpr bounded_uint<0u, 100000u> d {u32{60000}};
    BOOST_TEST_THROWS(c + d, std::domain_error);

    // Just one over: 50001 + 50000 = 100001 > 100000
    constexpr bounded_uint<0u, 100000u> e {u32{50001}};
    constexpr bounded_uint<0u, 100000u> f {u32{50000}};
    BOOST_TEST_THROWS(e + f, std::domain_error);
}

// -----------------------------------------------
// u64 basis: bounded_uint with Max in (4294967295, UINT64_MAX]
// -----------------------------------------------

void test_u64_valid_addition()
{
    // [0, 5'000'000'000]
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> a {u64{1'000'000'000ULL}};
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> b {u64{2'000'000'000ULL}};
    const auto c {a + b};
    const bounded_uint<0ULL, 5'000'000'000ULL> expected_c {u64{3'000'000'000ULL}};
    BOOST_TEST(c == expected_c);

    // Adding zeros
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> zero {u64{0}};
    const auto d {a + zero};
    BOOST_TEST(d == a);

    // Max boundary: 2'500'000'000 + 2'500'000'000 = 5'000'000'000
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> e {u64{2'500'000'000ULL}};
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> f {u64{2'500'000'000ULL}};
    const auto g {e + f};
    const bounded_uint<0ULL, 5'000'000'000ULL> expected_g {u64{5'000'000'000ULL}};
    BOOST_TEST(g == expected_g);

    // [4294967296, UINT64_MAX] non-zero min
    constexpr bounded_uint<4294967296ULL, UINT64_MAX> h {u64{5'000'000'000ULL}};
    constexpr bounded_uint<4294967296ULL, UINT64_MAX> i {u64{6'000'000'000ULL}};
    const auto j {h + i};
    const bounded_uint<4294967296ULL, UINT64_MAX> expected_j {u64{11'000'000'000ULL}};
    BOOST_TEST(j == expected_j);
}

void test_u64_throwing_addition()
{
    // Overflow of underlying u64
    constexpr bounded_uint<0ULL, UINT64_MAX> a {u64{UINT64_MAX}};
    constexpr bounded_uint<0ULL, UINT64_MAX> b {u64{1}};
    BOOST_TEST_THROWS(a + b, std::overflow_error);

    // Result within u64 range but exceeds bounded max:
    // [0, 5'000'000'000]: 3'000'000'000 + 3'000'000'000 = 6'000'000'000 > 5'000'000'000
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> c {u64{3'000'000'000ULL}};
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> d {u64{3'000'000'000ULL}};
    BOOST_TEST_THROWS(c + d, std::domain_error);

    // Just one over
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> e {u64{2'500'000'001ULL}};
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> f {u64{2'500'000'000ULL}};
    BOOST_TEST_THROWS(e + f, std::domain_error);
}

// -----------------------------------------------
// u128 basis: bounded_uint with Max > UINT64_MAX
// -----------------------------------------------

void test_u128_valid_addition()
{
    // [0, 2^64] range
    constexpr auto max_val = uint128_t{1, 0};
    using b128 = bounded_uint<uint128_t{0}, max_val>;

    const b128 a {u128{uint128_t{100}}};
    const b128 b {u128{uint128_t{200}}};
    const auto c {a + b};
    const b128 expected_c {u128{uint128_t{300}}};
    BOOST_TEST(c == expected_c);

    // Adding zeros
    const b128 zero {u128{uint128_t{0}}};
    const auto d {a + zero};
    BOOST_TEST(d == a);

    // Larger values: UINT64_MAX + 1 = 2^64
    const b128 e {u128{uint128_t{UINT64_MAX}}};
    const b128 f {u128{uint128_t{1}}};
    const auto g {e + f};
    const b128 expected_g {u128{uint128_t{1, 0}}};
    BOOST_TEST(g == expected_g);

    // Full u128 range
    constexpr auto u128_max = uint128_t{UINT64_MAX, UINT64_MAX};
    using b128_full = bounded_uint<uint128_t{0}, u128_max>;

    const b128_full h {u128{uint128_t{1'000'000}}};
    const b128_full i {u128{uint128_t{2'000'000}}};
    const auto j {h + i};
    const b128_full expected_j {u128{uint128_t{3'000'000}}};
    BOOST_TEST(j == expected_j);
}

void test_u128_throwing_addition()
{
    // Result exceeds bounded max:
    // [0, 2^64]: max_val + 1 > max_val
    constexpr auto max_val = uint128_t{1, 0};
    using b128 = bounded_uint<uint128_t{0}, max_val>;

    const b128 a {u128{max_val}};
    const b128 b {u128{uint128_t{1}}};
    BOOST_TEST_THROWS(a + b, std::domain_error);

    // Non-zero min range: [1000, 2^64]
    constexpr auto min_val = uint128_t{1000};
    using b128_ranged = bounded_uint<min_val, max_val>;

    const b128_ranged c {u128{max_val}};
    const b128_ranged d {u128{uint128_t{1000}}};
    BOOST_TEST_THROWS(c + d, std::domain_error);
}

// -----------------------------------------------
// Constexpr addition tests
// -----------------------------------------------

void test_constexpr_addition()
{
    // u8 basis
    constexpr bounded_uint<0u, 255u> a {u8{10}};
    constexpr bounded_uint<0u, 255u> b {u8{20}};
    constexpr auto c {a + b};
    constexpr bounded_uint<0u, 255u> expected_c {u8{30}};
    static_assert(c == expected_c);

    // u16 basis
    constexpr bounded_uint<0u, 1000u> d {u16{100}};
    constexpr bounded_uint<0u, 1000u> e {u16{200}};
    constexpr auto f {d + e};
    constexpr bounded_uint<0u, 1000u> expected_f {u16{300}};
    static_assert(f == expected_f);

    // u32 basis
    constexpr bounded_uint<0u, 100000u> g {u32{25000}};
    constexpr bounded_uint<0u, 100000u> h {u32{30000}};
    constexpr auto i {g + h};
    constexpr bounded_uint<0u, 100000u> expected_i {u32{55000}};
    static_assert(i == expected_i);

    // u64 basis
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> j {u64{1'000'000'000ULL}};
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> k {u64{2'000'000'000ULL}};
    constexpr auto l {j + k};
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> expected_l {u64{3'000'000'000ULL}};
    static_assert(l == expected_l);
}

// -----------------------------------------------
// Various bound configurations
// -----------------------------------------------

void test_tight_bounds()
{
    // Very narrow range: [50, 60]
    constexpr bounded_uint<50u, 60u> a {u8{50}};
    constexpr bounded_uint<50u, 60u> b {u8{50}};

    // 50 + 50 = 100 which exceeds [50, 60]
    BOOST_TEST_THROWS(a + b, std::domain_error);

    // [100, 200]
    constexpr bounded_uint<100u, 200u> c {u8{100}};
    constexpr bounded_uint<100u, 200u> d {u8{100}};
    const auto e {c + d};
    const bounded_uint<100u, 200u> expected_e {u8{200}};
    BOOST_TEST(e == expected_e);

    // [100, 200]: 101 + 100 = 201 > 200
    constexpr bounded_uint<100u, 200u> f {u8{101}};
    constexpr bounded_uint<100u, 200u> g {u8{100}};
    BOOST_TEST_THROWS(f + g, std::domain_error);
}

void test_power_of_two_bounds()
{
    // [0, 128] - not a full byte
    constexpr bounded_uint<0u, 128u> a {u8{64}};
    constexpr bounded_uint<0u, 128u> b {u8{64}};
    const auto c {a + b};
    const bounded_uint<0u, 128u> expected_c {u8{128}};
    BOOST_TEST(c == expected_c);

    // 65 + 64 = 129 > 128
    constexpr bounded_uint<0u, 128u> d {u8{65}};
    constexpr bounded_uint<0u, 128u> e {u8{64}};
    BOOST_TEST_THROWS(d + e, std::domain_error);

    // [0, 1024]
    constexpr bounded_uint<0u, 1024u> f {u16{512}};
    constexpr bounded_uint<0u, 1024u> g {u16{512}};
    const auto h {f + g};
    const bounded_uint<0u, 1024u> expected_h {u16{1024}};
    BOOST_TEST(h == expected_h);

    // 513 + 512 = 1025 > 1024
    constexpr bounded_uint<0u, 1024u> i {u16{513}};
    constexpr bounded_uint<0u, 1024u> j {u16{512}};
    BOOST_TEST_THROWS(i + j, std::domain_error);
}

int main()
{
    test_u8_valid_addition();
    test_u8_throwing_addition();

    test_u16_valid_addition();
    test_u16_throwing_addition();

    test_u32_valid_addition();
    test_u32_throwing_addition();

    test_u64_valid_addition();
    test_u64_throwing_addition();

    test_u128_valid_addition();
    test_u128_throwing_addition();

    test_constexpr_addition();
    test_tight_bounds();
    test_power_of_two_bounds();

    return boost::report_errors();
}

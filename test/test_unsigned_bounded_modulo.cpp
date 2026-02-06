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

void test_u8_valid_modulo()
{
    // [0, 255] full range
    constexpr bounded_uint<0u, 255u> a {u8{100}};
    constexpr bounded_uint<0u, 255u> b {u8{30}};
    const auto c {a % b};
    const bounded_uint<0u, 255u> expected_c {u8{10}};  // 100 % 30 = 10
    BOOST_TEST(c == expected_c);

    // Modulo by one = 0
    constexpr bounded_uint<0u, 255u> one {u8{1}};
    const auto d {a % one};
    const bounded_uint<0u, 255u> expected_d {u8{0}};
    BOOST_TEST(d == expected_d);

    // Self modulo = 0
    const auto e {a % a};
    const bounded_uint<0u, 255u> expected_e {u8{0}};
    BOOST_TEST(e == expected_e);

    // Modulo with remainder: 255 % 100 = 55
    constexpr bounded_uint<0u, 255u> f {u8{255}};
    constexpr bounded_uint<0u, 255u> g {u8{100}};
    const auto h {f % g};
    const bounded_uint<0u, 255u> expected_h {u8{55}};
    BOOST_TEST(h == expected_h);

    // Smaller dividend: 10 % 100 = 10
    constexpr bounded_uint<0u, 255u> i {u8{10}};
    const auto j {i % g};
    const bounded_uint<0u, 255u> expected_j {u8{10}};
    BOOST_TEST(j == expected_j);
}

void test_u8_throwing_modulo()
{
    // Modulo by zero throws domain_error from underlying u8
    constexpr bounded_uint<0u, 255u> a {u8{100}};
    constexpr bounded_uint<0u, 255u> zero {u8{0}};
    BOOST_TEST_THROWS(a % zero, std::domain_error);

    // Result below bounded min:
    // [50, 200]: 100 % 60 = 40 < 50
    constexpr bounded_uint<50u, 200u> b {u8{100}};
    constexpr bounded_uint<50u, 200u> c {u8{60}};
    BOOST_TEST_THROWS(b % c, std::domain_error);

    // [10, 100]: 15 % 10 = 5 < 10
    constexpr bounded_uint<10u, 100u> d {u8{15}};
    constexpr bounded_uint<10u, 100u> e {u8{10}};
    BOOST_TEST_THROWS(d % e, std::domain_error);
}

// -----------------------------------------------
// u16 basis: bounded_uint with Max in (255, 65535]
// -----------------------------------------------

void test_u16_valid_modulo()
{
    // [0, 1000]
    constexpr bounded_uint<0u, 1000u> a {u16{1000}};
    constexpr bounded_uint<0u, 1000u> b {u16{300}};
    const auto c {a % b};
    const bounded_uint<0u, 1000u> expected_c {u16{100}};  // 1000 % 300 = 100
    BOOST_TEST(c == expected_c);

    // Modulo by one = 0
    constexpr bounded_uint<0u, 1000u> one {u16{1}};
    const auto d {a % one};
    const bounded_uint<0u, 1000u> expected_d {u16{0}};
    BOOST_TEST(d == expected_d);

    // [0, 40000]
    constexpr bounded_uint<0u, 40000u> e {u16{35000}};
    constexpr bounded_uint<0u, 40000u> f {u16{10000}};
    const auto g {e % f};
    const bounded_uint<0u, 40000u> expected_g {u16{5000}};  // 35000 % 10000 = 5000
    BOOST_TEST(g == expected_g);
}

void test_u16_throwing_modulo()
{
    // Modulo by zero
    constexpr bounded_uint<0u, 1000u> a {u16{500}};
    constexpr bounded_uint<0u, 1000u> zero {u16{0}};
    BOOST_TEST_THROWS(a % zero, std::domain_error);

    // Result below bounded min:
    // [256, 65535]: 300 % 256 = 44 < 256
    constexpr bounded_uint<256u, 65535u> b {u16{300}};
    constexpr bounded_uint<256u, 65535u> c {u16{256}};
    BOOST_TEST_THROWS(b % c, std::domain_error);

    // [500, 1000]: 700 % 500 = 200 < 500
    constexpr bounded_uint<500u, 1000u> d {u16{700}};
    constexpr bounded_uint<500u, 1000u> e {u16{500}};
    BOOST_TEST_THROWS(d % e, std::domain_error);
}

// -----------------------------------------------
// u32 basis: bounded_uint with Max in (65535, 4294967295]
// -----------------------------------------------

void test_u32_valid_modulo()
{
    // [0, 100000]
    constexpr bounded_uint<0u, 100000u> a {u32{100000}};
    constexpr bounded_uint<0u, 100000u> b {u32{30000}};
    const auto c {a % b};
    const bounded_uint<0u, 100000u> expected_c {u32{10000}};  // 100000 % 30000 = 10000
    BOOST_TEST(c == expected_c);

    // Modulo by one = 0
    constexpr bounded_uint<0u, 100000u> one {u32{1}};
    const auto d {a % one};
    const bounded_uint<0u, 100000u> expected_d {u32{0}};
    BOOST_TEST(d == expected_d);

    // [0, 4294967295]
    constexpr bounded_uint<0u, 4294967295u> e {u32{4294967295u}};
    constexpr bounded_uint<0u, 4294967295u> f {u32{1000000000u}};
    const auto g {e % f};
    const bounded_uint<0u, 4294967295u> expected_g {u32{4294967295u % 1000000000u}};
    BOOST_TEST(g == expected_g);
}

void test_u32_throwing_modulo()
{
    // Modulo by zero
    constexpr bounded_uint<0u, 100000u> a {u32{50000}};
    constexpr bounded_uint<0u, 100000u> zero {u32{0}};
    BOOST_TEST_THROWS(a % zero, std::domain_error);

    // Result below bounded min:
    // [65536, 4294967295]: 100000 % 65536 = 34464 < 65536
    constexpr bounded_uint<65536u, 4294967295u> b {u32{100000}};
    constexpr bounded_uint<65536u, 4294967295u> c {u32{65536}};
    BOOST_TEST_THROWS(b % c, std::domain_error);
}

// -----------------------------------------------
// u64 basis: bounded_uint with Max in (4294967295, UINT64_MAX]
// -----------------------------------------------

void test_u64_valid_modulo()
{
    // [0, 5'000'000'000]
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> a {u64{5'000'000'000ULL}};
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> b {u64{3'000'000'000ULL}};
    const auto c {a % b};
    const bounded_uint<0ULL, 5'000'000'000ULL> expected_c {u64{2'000'000'000ULL}};
    BOOST_TEST(c == expected_c);

    // Modulo by one = 0
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> one {u64{1}};
    const auto d {a % one};
    const bounded_uint<0ULL, 5'000'000'000ULL> expected_d {u64{0}};
    BOOST_TEST(d == expected_d);

    // Self modulo = 0
    const auto e {a % a};
    const bounded_uint<0ULL, 5'000'000'000ULL> expected_e {u64{0}};
    BOOST_TEST(e == expected_e);
}

void test_u64_throwing_modulo()
{
    // Modulo by zero
    constexpr bounded_uint<0ULL, UINT64_MAX> a {u64{1'000'000ULL}};
    constexpr bounded_uint<0ULL, UINT64_MAX> zero {u64{0}};
    BOOST_TEST_THROWS(a % zero, std::domain_error);

    // Result below bounded min:
    // [4294967296, UINT64_MAX]: 5'000'000'000 % 4294967296 = 705032704 < 4294967296
    constexpr bounded_uint<4294967296ULL, UINT64_MAX> b {u64{5'000'000'000ULL}};
    constexpr bounded_uint<4294967296ULL, UINT64_MAX> c {u64{4294967296ULL}};
    BOOST_TEST_THROWS(b % c, std::domain_error);
}

// -----------------------------------------------
// u128 basis: bounded_uint with Max > UINT64_MAX
// -----------------------------------------------

void test_u128_valid_modulo()
{
    constexpr auto max_val = uint128_t{1, 0};
    using b128 = bounded_uint<uint128_t{0}, max_val>;

    const b128 a {u128{uint128_t{10000}}};
    const b128 b {u128{uint128_t{3000}}};
    const auto c {a % b};
    const b128 expected_c {u128{uint128_t{1000}}};  // 10000 % 3000 = 1000
    BOOST_TEST(c == expected_c);

    // Modulo by one = 0
    const b128 one {u128{uint128_t{1}}};
    const auto d {a % one};
    const b128 expected_d {u128{uint128_t{0}}};
    BOOST_TEST(d == expected_d);

    // Full u128 range
    constexpr auto u128_max = uint128_t{UINT64_MAX, UINT64_MAX};
    using b128_full = bounded_uint<uint128_t{0}, u128_max>;

    const b128_full e {u128{uint128_t{3'000'000}}};
    const b128_full f {u128{uint128_t{1'000'000}}};
    const auto g {e % f};
    const b128_full expected_g {u128{uint128_t{0}}};
    BOOST_TEST(g == expected_g);
}

void test_u128_throwing_modulo()
{
    constexpr auto max_val = uint128_t{1, 0};
    using b128 = bounded_uint<uint128_t{0}, max_val>;

    // Modulo by zero
    const b128 a {u128{uint128_t{100}}};
    const b128 zero {u128{uint128_t{0}}};
    BOOST_TEST_THROWS(a % zero, std::domain_error);

    // Non-zero min: [1000, 2^64]: 1500 % 1000 = 500 < 1000
    constexpr auto min_val = uint128_t{1000};
    using b128_ranged = bounded_uint<min_val, max_val>;

    const b128_ranged b {u128{uint128_t{1500}}};
    const b128_ranged c {u128{uint128_t{1000}}};
    BOOST_TEST_THROWS(b % c, std::domain_error);
}

// -----------------------------------------------
// Constexpr modulo tests
// -----------------------------------------------

void test_constexpr_modulo()
{
    // u8 basis
    constexpr bounded_uint<0u, 255u> a {u8{100}};
    constexpr bounded_uint<0u, 255u> b {u8{30}};
    constexpr auto c {a % b};
    constexpr bounded_uint<0u, 255u> expected_c {u8{10}};
    static_assert(c == expected_c);

    // u16 basis
    constexpr bounded_uint<0u, 1000u> d {u16{1000}};
    constexpr bounded_uint<0u, 1000u> e {u16{300}};
    constexpr auto f {d % e};
    constexpr bounded_uint<0u, 1000u> expected_f {u16{100}};
    static_assert(f == expected_f);

    // u32 basis
    constexpr bounded_uint<0u, 100000u> g {u32{100000}};
    constexpr bounded_uint<0u, 100000u> h {u32{30000}};
    constexpr auto i {g % h};
    constexpr bounded_uint<0u, 100000u> expected_i {u32{10000}};
    static_assert(i == expected_i);

    // u64 basis
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> j {u64{5'000'000'000ULL}};
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> k {u64{3'000'000'000ULL}};
    constexpr auto l {j % k};
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> expected_l {u64{2'000'000'000ULL}};
    static_assert(l == expected_l);
}

int main()
{
    test_u8_valid_modulo();
    test_u8_throwing_modulo();

    test_u16_valid_modulo();
    test_u16_throwing_modulo();

    test_u32_valid_modulo();
    test_u32_throwing_modulo();

    test_u64_valid_modulo();
    test_u64_throwing_modulo();

    test_u128_valid_modulo();
    test_u128_throwing_modulo();

    test_constexpr_modulo();

    return boost::report_errors();
}

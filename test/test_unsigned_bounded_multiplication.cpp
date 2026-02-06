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

void test_u8_valid_multiplication()
{
    // [0, 255] full range
    constexpr bounded_uint<0u, 255u> a {u8{5}};
    constexpr bounded_uint<0u, 255u> b {u8{10}};
    const auto c {a * b};
    const bounded_uint<0u, 255u> expected_c {u8{50}};
    BOOST_TEST(c == expected_c);

    // Multiply by one (identity)
    constexpr bounded_uint<0u, 255u> one {u8{1}};
    const auto d {a * one};
    BOOST_TEST(d == a);

    // Multiply by zero
    constexpr bounded_uint<0u, 255u> zero {u8{0}};
    const auto e {a * zero};
    const bounded_uint<0u, 255u> expected_e {u8{0}};
    BOOST_TEST(e == expected_e);

    // Max boundary: 15 * 17 = 255
    constexpr bounded_uint<0u, 255u> f {u8{15}};
    constexpr bounded_uint<0u, 255u> g {u8{17}};
    const auto h {f * g};
    const bounded_uint<0u, 255u> expected_h {u8{255}};
    BOOST_TEST(h == expected_h);

    // Narrower range [10, 200]: 10 * 10 = 100
    constexpr bounded_uint<10u, 200u> i {u8{10}};
    constexpr bounded_uint<10u, 200u> j {u8{10}};
    const auto k {i * j};
    const bounded_uint<10u, 200u> expected_k {u8{100}};
    BOOST_TEST(k == expected_k);
}

void test_u8_throwing_multiplication()
{
    // Overflow of underlying u8: 20 * 20 = 400 > 255
    constexpr bounded_uint<0u, 255u> a {u8{20}};
    constexpr bounded_uint<0u, 255u> b {u8{20}};
    BOOST_TEST_THROWS(a * b, std::overflow_error);

    // Result within u8 range but exceeds bounded max:
    // [10, 100]: 11 * 10 = 110 > 100
    constexpr bounded_uint<10u, 100u> c {u8{11}};
    constexpr bounded_uint<10u, 100u> d {u8{10}};
    BOOST_TEST_THROWS(c * d, std::domain_error);
}

// -----------------------------------------------
// u16 basis: bounded_uint with Max in (255, 65535]
// -----------------------------------------------

void test_u16_valid_multiplication()
{
    // [0, 1000]
    constexpr bounded_uint<0u, 1000u> a {u16{10}};
    constexpr bounded_uint<0u, 1000u> b {u16{50}};
    const auto c {a * b};
    const bounded_uint<0u, 1000u> expected_c {u16{500}};
    BOOST_TEST(c == expected_c);

    // Multiply by one
    constexpr bounded_uint<0u, 1000u> one {u16{1}};
    const auto d {a * one};
    BOOST_TEST(d == a);

    // Max boundary: 100 * 10 = 1000
    constexpr bounded_uint<0u, 1000u> e {u16{100}};
    constexpr bounded_uint<0u, 1000u> f {u16{10}};
    const auto g {e * f};
    const bounded_uint<0u, 1000u> expected_g {u16{1000}};
    BOOST_TEST(g == expected_g);

    // [0, 40000]: 100 * 400 = 40000
    constexpr bounded_uint<0u, 40000u> h {u16{100}};
    constexpr bounded_uint<0u, 40000u> i {u16{400}};
    const auto j {h * i};
    const bounded_uint<0u, 40000u> expected_j {u16{40000}};
    BOOST_TEST(j == expected_j);
}

void test_u16_throwing_multiplication()
{
    // Overflow of underlying u16: 300 * 300 = 90000 > 65535
    constexpr bounded_uint<0u, 65535u> a {u16{300}};
    constexpr bounded_uint<0u, 65535u> b {u16{300}};
    BOOST_TEST_THROWS(a * b, std::overflow_error);

    // Result within u16 range but exceeds bounded max:
    // [0, 1000]: 100 * 20 = 2000 > 1000
    constexpr bounded_uint<0u, 1000u> c {u16{100}};
    constexpr bounded_uint<0u, 1000u> d {u16{20}};
    BOOST_TEST_THROWS(c * d, std::domain_error);
}

// -----------------------------------------------
// u32 basis: bounded_uint with Max in (65535, 4294967295]
// -----------------------------------------------

void test_u32_valid_multiplication()
{
    // [0, 100000]
    constexpr bounded_uint<0u, 100000u> a {u32{100}};
    constexpr bounded_uint<0u, 100000u> b {u32{500}};
    const auto c {a * b};
    const bounded_uint<0u, 100000u> expected_c {u32{50000}};
    BOOST_TEST(c == expected_c);

    // Multiply by one
    constexpr bounded_uint<0u, 100000u> one {u32{1}};
    const auto d {a * one};
    BOOST_TEST(d == a);

    // Max boundary: 1000 * 100 = 100000
    constexpr bounded_uint<0u, 100000u> e {u32{1000}};
    constexpr bounded_uint<0u, 100000u> f {u32{100}};
    const auto g {e * f};
    const bounded_uint<0u, 100000u> expected_g {u32{100000}};
    BOOST_TEST(g == expected_g);
}

void test_u32_throwing_multiplication()
{
    // Overflow of underlying u32: 100000 * 100000 > UINT32_MAX
    constexpr bounded_uint<0u, 4294967295u> a {u32{100000}};
    constexpr bounded_uint<0u, 4294967295u> b {u32{100000}};
    BOOST_TEST_THROWS(a * b, std::overflow_error);

    // Result within u32 range but exceeds bounded max:
    // [0, 100000]: 500 * 500 = 250000 > 100000
    constexpr bounded_uint<0u, 100000u> c {u32{500}};
    constexpr bounded_uint<0u, 100000u> d {u32{500}};
    BOOST_TEST_THROWS(c * d, std::domain_error);
}

// -----------------------------------------------
// u64 basis: bounded_uint with Max in (4294967295, UINT64_MAX]
// -----------------------------------------------

void test_u64_valid_multiplication()
{
    // [0, 5'000'000'000]
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> a {u64{50'000ULL}};
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> b {u64{100'000ULL}};
    const auto c {a * b};
    const bounded_uint<0ULL, 5'000'000'000ULL> expected_c {u64{5'000'000'000ULL}};
    BOOST_TEST(c == expected_c);

    // Multiply by one
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> one {u64{1}};
    const auto d {a * one};
    BOOST_TEST(d == a);

    // Self-multiplication with small value in wide range
    constexpr bounded_uint<0ULL, UINT64_MAX> e {u64{1'000'000ULL}};
    constexpr bounded_uint<0ULL, UINT64_MAX> f {u64{1'000'000ULL}};
    const auto g {e * f};
    const bounded_uint<0ULL, UINT64_MAX> expected_g {u64{1'000'000'000'000ULL}};
    BOOST_TEST(g == expected_g);
}

void test_u64_throwing_multiplication()
{
    // Overflow of underlying u64
    constexpr bounded_uint<0ULL, UINT64_MAX> a {u64{UINT64_MAX}};
    constexpr bounded_uint<0ULL, UINT64_MAX> b {u64{2}};
    BOOST_TEST_THROWS(a * b, std::overflow_error);

    // Result within u64 range but exceeds bounded max:
    // [0, 5'000'000'000]: 100'000 * 100'000 = 10'000'000'000 > 5'000'000'000
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> c {u64{100'000ULL}};
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> d {u64{100'000ULL}};
    BOOST_TEST_THROWS(c * d, std::domain_error);
}

// -----------------------------------------------
// u128 basis: bounded_uint with Max > UINT64_MAX
// -----------------------------------------------

void test_u128_valid_multiplication()
{
    constexpr auto max_val = uint128_t{1, 0};
    using b128 = bounded_uint<uint128_t{0}, max_val>;

    const b128 a {u128{uint128_t{100}}};
    const b128 b {u128{uint128_t{200}}};
    const auto c {a * b};
    const b128 expected_c {u128{uint128_t{20000}}};
    BOOST_TEST(c == expected_c);

    // Multiply by one
    const b128 one {u128{uint128_t{1}}};
    const auto d {a * one};
    BOOST_TEST(d == a);

    // Multiply by zero
    const b128 zero {u128{uint128_t{0}}};
    const auto e {a * zero};
    const b128 expected_e {u128{uint128_t{0}}};
    BOOST_TEST(e == expected_e);

    // Full u128 range
    constexpr auto u128_max = uint128_t{UINT64_MAX, UINT64_MAX};
    using b128_full = bounded_uint<uint128_t{0}, u128_max>;

    const b128_full f {u128{uint128_t{1'000}}};
    const b128_full g {u128{uint128_t{2'000}}};
    const auto h {f * g};
    const b128_full expected_h {u128{uint128_t{2'000'000}}};
    BOOST_TEST(h == expected_h);
}

void test_u128_throwing_multiplication()
{
    constexpr auto max_val = uint128_t{1, 0};
    using b128 = bounded_uint<uint128_t{0}, max_val>;

    // Result exceeds bounded max
    const b128 a {u128{max_val}};
    const b128 b {u128{uint128_t{2}}};
    BOOST_TEST_THROWS(a * b, std::domain_error);
}

// -----------------------------------------------
// Constexpr multiplication tests
// -----------------------------------------------

void test_constexpr_multiplication()
{
    // u8 basis
    constexpr bounded_uint<0u, 255u> a {u8{5}};
    constexpr bounded_uint<0u, 255u> b {u8{10}};
    constexpr auto c {a * b};
    constexpr bounded_uint<0u, 255u> expected_c {u8{50}};
    static_assert(c == expected_c);

    // u16 basis
    constexpr bounded_uint<0u, 1000u> d {u16{10}};
    constexpr bounded_uint<0u, 1000u> e {u16{50}};
    constexpr auto f {d * e};
    constexpr bounded_uint<0u, 1000u> expected_f {u16{500}};
    static_assert(f == expected_f);

    // u32 basis
    constexpr bounded_uint<0u, 100000u> g {u32{100}};
    constexpr bounded_uint<0u, 100000u> h {u32{500}};
    constexpr auto i {g * h};
    constexpr bounded_uint<0u, 100000u> expected_i {u32{50000}};
    static_assert(i == expected_i);

    // u64 basis
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> j {u64{50'000ULL}};
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> k {u64{100'000ULL}};
    constexpr auto l {j * k};
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> expected_l {u64{5'000'000'000ULL}};
    static_assert(l == expected_l);
}

int main()
{
    test_u8_valid_multiplication();
    test_u8_throwing_multiplication();

    test_u16_valid_multiplication();
    test_u16_throwing_multiplication();

    test_u32_valid_multiplication();
    test_u32_throwing_multiplication();

    test_u64_valid_multiplication();
    test_u64_throwing_multiplication();

    test_u128_valid_multiplication();
    test_u128_throwing_multiplication();

    test_constexpr_multiplication();

    return boost::report_errors();
}

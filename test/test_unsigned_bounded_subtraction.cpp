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

void test_u8_valid_subtraction()
{
    // [0, 255] full range
    constexpr bounded_uint<0u, 255u> a {u8{30}};
    constexpr bounded_uint<0u, 255u> b {u8{10}};
    const auto c {a - b};
    const bounded_uint<0u, 255u> expected_c {u8{20}};
    BOOST_TEST(c == expected_c);

    // Subtracting zero
    constexpr bounded_uint<0u, 255u> zero {u8{0}};
    const auto d {a - zero};
    BOOST_TEST(d == a);

    // Result is zero: 100 - 100 = 0
    constexpr bounded_uint<0u, 255u> e {u8{100}};
    constexpr bounded_uint<0u, 255u> f {u8{100}};
    const auto g {e - f};
    const bounded_uint<0u, 255u> expected_g {u8{0}};
    BOOST_TEST(g == expected_g);

    // Narrower range [10, 200]: 100 - 50 = 50
    constexpr bounded_uint<10u, 200u> h {u8{100}};
    constexpr bounded_uint<10u, 200u> i {u8{50}};
    const auto j {h - i};
    const bounded_uint<10u, 200u> expected_j {u8{50}};
    BOOST_TEST(j == expected_j);

    // Min boundary: 200 - 190 = 10
    constexpr bounded_uint<10u, 200u> k {u8{200}};
    constexpr bounded_uint<10u, 200u> l {u8{190}};
    const auto m {k - l};
    const bounded_uint<10u, 200u> expected_m {u8{10}};
    BOOST_TEST(m == expected_m);
}

void test_u8_throwing_subtraction()
{
    // Underflow of underlying u8: 10 - 20 wraps unsigned, throws underflow_error
    constexpr bounded_uint<0u, 255u> a {u8{10}};
    constexpr bounded_uint<0u, 255u> b {u8{20}};
    BOOST_TEST_THROWS(a - b, std::underflow_error);

    // Result within u8 range but below bounded min:
    // [50, 200]: 60 - 55 = 5 < 50
    constexpr bounded_uint<50u, 200u> c {u8{60}};
    constexpr bounded_uint<50u, 200u> d {u8{55}};
    BOOST_TEST_THROWS(c - d, std::domain_error);

    // Just one below min: [10, 200]: 20 - 11 = 9 < 10
    constexpr bounded_uint<10u, 200u> e {u8{20}};
    constexpr bounded_uint<10u, 200u> f {u8{11}};
    BOOST_TEST_THROWS(e - f, std::domain_error);
}

// -----------------------------------------------
// u16 basis: bounded_uint with Max in (255, 65535]
// -----------------------------------------------

void test_u16_valid_subtraction()
{
    // [0, 1000]
    constexpr bounded_uint<0u, 1000u> a {u16{500}};
    constexpr bounded_uint<0u, 1000u> b {u16{200}};
    const auto c {a - b};
    const bounded_uint<0u, 1000u> expected_c {u16{300}};
    BOOST_TEST(c == expected_c);

    // Subtracting zero
    constexpr bounded_uint<0u, 1000u> zero {u16{0}};
    const auto d {a - zero};
    BOOST_TEST(d == a);

    // Result is zero
    constexpr bounded_uint<0u, 1000u> e {u16{1000}};
    constexpr bounded_uint<0u, 1000u> f {u16{1000}};
    const auto g {e - f};
    const bounded_uint<0u, 1000u> expected_g {u16{0}};
    BOOST_TEST(g == expected_g);

    // [256, 65535] non-zero min
    constexpr bounded_uint<256u, 65535u> h {u16{10000}};
    constexpr bounded_uint<256u, 65535u> i {u16{5000}};
    const auto j {h - i};
    const bounded_uint<256u, 65535u> expected_j {u16{5000}};
    BOOST_TEST(j == expected_j);
}

void test_u16_throwing_subtraction()
{
    // Underflow of underlying u16: 100 - 200 wraps
    constexpr bounded_uint<0u, 65535u> a {u16{100}};
    constexpr bounded_uint<0u, 65535u> b {u16{200}};
    BOOST_TEST_THROWS(a - b, std::underflow_error);

    // Result below bounded min: [500, 1000]: 600 - 550 = 50 < 500
    constexpr bounded_uint<500u, 1000u> c {u16{600}};
    constexpr bounded_uint<500u, 1000u> d {u16{550}};
    BOOST_TEST_THROWS(c - d, std::domain_error);

    // Just one below: [256, 65535]: 300 - 256 = 44... wait, need to go below 256
    // [256, 65535]: 300 - 256 = 44 -- but 44 < 256
    // Actually 300 - 45 = 255 < 256
    constexpr bounded_uint<256u, 65535u> e {u16{300}};
    constexpr bounded_uint<256u, 65535u> f {u16{256}};
    BOOST_TEST_THROWS(e - f, std::domain_error);
}

// -----------------------------------------------
// u32 basis: bounded_uint with Max in (65535, 4294967295]
// -----------------------------------------------

void test_u32_valid_subtraction()
{
    // [0, 100000]
    constexpr bounded_uint<0u, 100000u> a {u32{80000}};
    constexpr bounded_uint<0u, 100000u> b {u32{30000}};
    const auto c {a - b};
    const bounded_uint<0u, 100000u> expected_c {u32{50000}};
    BOOST_TEST(c == expected_c);

    // Subtracting zero
    constexpr bounded_uint<0u, 100000u> zero {u32{0}};
    const auto d {a - zero};
    BOOST_TEST(d == a);

    // [65536, 4294967295] non-zero min
    constexpr bounded_uint<65536u, 4294967295u> e {u32{1000000}};
    constexpr bounded_uint<65536u, 4294967295u> f {u32{500000}};
    const auto g {e - f};
    const bounded_uint<65536u, 4294967295u> expected_g {u32{500000}};
    BOOST_TEST(g == expected_g);
}

void test_u32_throwing_subtraction()
{
    // Underflow of underlying u32
    constexpr bounded_uint<0u, 4294967295u> a {u32{100}};
    constexpr bounded_uint<0u, 4294967295u> b {u32{200}};
    BOOST_TEST_THROWS(a - b, std::underflow_error);

    // Result below bounded min: [65536, 4294967295]: 100000 - 65536 = 34464 < 65536
    constexpr bounded_uint<65536u, 4294967295u> c {u32{100000}};
    constexpr bounded_uint<65536u, 4294967295u> d {u32{65536}};
    BOOST_TEST_THROWS(c - d, std::domain_error);

    // [0, 100000]: result valid in u32 but subtraction causes underflow
    constexpr bounded_uint<0u, 100000u> e {u32{10}};
    constexpr bounded_uint<0u, 100000u> f {u32{20}};
    BOOST_TEST_THROWS(e - f, std::underflow_error);
}

// -----------------------------------------------
// u64 basis: bounded_uint with Max in (4294967295, UINT64_MAX]
// -----------------------------------------------

void test_u64_valid_subtraction()
{
    // [0, 5'000'000'000]
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> a {u64{4'000'000'000ULL}};
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> b {u64{1'000'000'000ULL}};
    const auto c {a - b};
    const bounded_uint<0ULL, 5'000'000'000ULL> expected_c {u64{3'000'000'000ULL}};
    BOOST_TEST(c == expected_c);

    // Subtracting zero
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> zero {u64{0}};
    const auto d {a - zero};
    BOOST_TEST(d == a);

    // [4294967296, UINT64_MAX] non-zero min
    constexpr bounded_uint<4294967296ULL, UINT64_MAX> e {u64{10'000'000'000ULL}};
    constexpr bounded_uint<4294967296ULL, UINT64_MAX> f {u64{5'000'000'000ULL}};
    const auto g {e - f};
    const bounded_uint<4294967296ULL, UINT64_MAX> expected_g {u64{5'000'000'000ULL}};
    BOOST_TEST(g == expected_g);
}

void test_u64_throwing_subtraction()
{
    // Underflow of underlying u64
    constexpr bounded_uint<0ULL, UINT64_MAX> a {u64{0}};
    constexpr bounded_uint<0ULL, UINT64_MAX> b {u64{1}};
    BOOST_TEST_THROWS(a - b, std::underflow_error);

    // Result below bounded min: [4294967296, UINT64_MAX]
    constexpr bounded_uint<4294967296ULL, UINT64_MAX> c {u64{5'000'000'000ULL}};
    constexpr bounded_uint<4294967296ULL, UINT64_MAX> d {u64{4294967296ULL}};
    BOOST_TEST_THROWS(c - d, std::domain_error);
}

// -----------------------------------------------
// u128 basis: bounded_uint with Max > UINT64_MAX
// -----------------------------------------------

void test_u128_valid_subtraction()
{
    constexpr auto max_val = uint128_t{1, 0};
    using b128 = bounded_uint<uint128_t{0}, max_val>;

    const b128 a {u128{uint128_t{300}}};
    const b128 b {u128{uint128_t{100}}};
    const auto c {a - b};
    const b128 expected_c {u128{uint128_t{200}}};
    BOOST_TEST(c == expected_c);

    // Subtracting zero
    const b128 zero {u128{uint128_t{0}}};
    const auto d {a - zero};
    BOOST_TEST(d == a);

    // Full u128 range
    constexpr auto u128_max = uint128_t{UINT64_MAX, UINT64_MAX};
    using b128_full = bounded_uint<uint128_t{0}, u128_max>;

    const b128_full e {u128{uint128_t{3'000'000}}};
    const b128_full f {u128{uint128_t{1'000'000}}};
    const auto g {e - f};
    const b128_full expected_g {u128{uint128_t{2'000'000}}};
    BOOST_TEST(g == expected_g);
}

void test_u128_throwing_subtraction()
{
    constexpr auto max_val = uint128_t{1, 0};
    using b128 = bounded_uint<uint128_t{0}, max_val>;

    // Underflow: 0 - 1
    const b128 a {u128{uint128_t{0}}};
    const b128 b {u128{uint128_t{1}}};
    BOOST_TEST_THROWS(a - b, std::underflow_error);

    // Non-zero min: [1000, 2^64]: 1500 - 1000 = 500 < 1000
    constexpr auto min_val = uint128_t{1000};
    using b128_ranged = bounded_uint<min_val, max_val>;

    const b128_ranged c {u128{uint128_t{1500}}};
    const b128_ranged d {u128{uint128_t{1000}}};
    BOOST_TEST_THROWS(c - d, std::domain_error);
}

// -----------------------------------------------
// Constexpr subtraction tests
// -----------------------------------------------

void test_constexpr_subtraction()
{
    // u8 basis
    constexpr bounded_uint<0u, 255u> a {u8{30}};
    constexpr bounded_uint<0u, 255u> b {u8{10}};
    constexpr auto c {a - b};
    constexpr bounded_uint<0u, 255u> expected_c {u8{20}};
    static_assert(c == expected_c);

    // u16 basis
    constexpr bounded_uint<0u, 1000u> d {u16{500}};
    constexpr bounded_uint<0u, 1000u> e {u16{200}};
    constexpr auto f {d - e};
    constexpr bounded_uint<0u, 1000u> expected_f {u16{300}};
    static_assert(f == expected_f);

    // u32 basis
    constexpr bounded_uint<0u, 100000u> g {u32{80000}};
    constexpr bounded_uint<0u, 100000u> h {u32{30000}};
    constexpr auto i {g - h};
    constexpr bounded_uint<0u, 100000u> expected_i {u32{50000}};
    static_assert(i == expected_i);

    // u64 basis
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> j {u64{4'000'000'000ULL}};
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> k {u64{1'000'000'000ULL}};
    constexpr auto l {j - k};
    constexpr bounded_uint<0ULL, 5'000'000'000ULL> expected_l {u64{3'000'000'000ULL}};
    static_assert(l == expected_l);
}

int main()
{
    test_u8_valid_subtraction();
    test_u8_throwing_subtraction();

    test_u16_valid_subtraction();
    test_u16_throwing_subtraction();

    test_u32_valid_subtraction();
    test_u32_throwing_subtraction();

    test_u64_valid_subtraction();
    test_u64_throwing_subtraction();

    test_u128_valid_subtraction();
    test_u128_throwing_subtraction();

    test_constexpr_subtraction();

    return boost::report_errors();
}

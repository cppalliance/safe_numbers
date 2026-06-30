// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>

#endif

#include <boost/core/lightweight_test.hpp>

#include <cfloat>
#include <limits>
#include <stdexcept>

using namespace boost::safe_numbers;
using namespace boost::safe_numbers::literals;

// Each specialization exercises both evaluation paths of the literal operator:
// the compile-time path via constexpr + static_assert, and the runtime path via
// a non-constexpr call plus the throwing overflow branch (which can only run at
// runtime).

template <typename T>
void test();

template <>
void test<u8>()
{
    // Compile-time path.
    constexpr auto ct {42_u8};
    static_assert(ct == u8{42u}, "compile-time _u8");
    BOOST_TEST_EQ(ct, u8{42u});

    constexpr auto ct_max {255_u8};
    static_assert(ct_max == u8{255u}, "compile-time _u8 max");
    BOOST_TEST_EQ(ct_max, u8{255u});

    // Runtime path (non-constexpr call executes the operator body at runtime).
    const auto rt {42_u8};
    BOOST_TEST_EQ(rt, u8{42u});

    // Runtime overflow branch.
    BOOST_TEST_THROWS(256_u8, std::overflow_error);
    BOOST_TEST_THROWS(18446744073709551615_u8, std::overflow_error);
}

template <>
void test<u16>()
{
    constexpr auto ct {42_u16};
    static_assert(ct == u16{42u}, "compile-time _u16");
    BOOST_TEST_EQ(ct, u16{42u});

    constexpr auto ct_max {65535_u16};
    static_assert(ct_max == u16{65535u}, "compile-time _u16 max");
    BOOST_TEST_EQ(ct_max, u16{65535u});

    const auto rt {42_u16};
    BOOST_TEST_EQ(rt, u16{42u});

    BOOST_TEST_THROWS(65536_u16, std::overflow_error);
    BOOST_TEST_THROWS(18446744073709551615_u16, std::overflow_error);
}

template <>
void test<u32>()
{
    constexpr auto ct {42_u32};
    static_assert(ct == u32{42u}, "compile-time _u32");
    BOOST_TEST_EQ(ct, u32{42u});

    constexpr auto ct_max {4294967295_u32};
    static_assert(ct_max == u32{4294967295u}, "compile-time _u32 max");
    BOOST_TEST_EQ(ct_max, u32{4294967295u});

    const auto rt {42_u32};
    BOOST_TEST_EQ(rt, u32{42u});

    BOOST_TEST_THROWS(4294967296_u32, std::overflow_error);
    BOOST_TEST_THROWS(18446744073709551615_u32, std::overflow_error);
}

template <>
void test<u64>()
{
    // _u64 is noexcept: unsigned long long covers the whole range, so there is
    // no overflow branch to exercise.
    constexpr auto ct {42_u64};
    static_assert(ct == u64{42u}, "compile-time _u64");
    BOOST_TEST_EQ(ct, u64{42u});

    constexpr auto ct_max {18446744073709551615_u64};
    static_assert(ct_max == u64{18446744073709551615ULL}, "compile-time _u64 max");
    BOOST_TEST_EQ(ct_max, u64{18446744073709551615ULL});

    const auto rt {42_u64};
    BOOST_TEST_EQ(rt, u64{42u});
}

template <>
void test<u128>()
{
    // _u128 takes the raw digit string (const char*) and parses with from_chars.
    constexpr auto ct {42_u128};
    static_assert(ct == u128{42u}, "compile-time _u128");
    BOOST_TEST_EQ(ct, u128{42u});

    constexpr auto ct_zero {0_u128};
    static_assert(ct_zero == u128{0u}, "compile-time _u128 zero");
    BOOST_TEST_EQ(ct_zero, u128{0u});

    // UINT128_MAX is representable.
    constexpr auto ct_max {340282366920938463463374607431768211455_u128};
    static_cast<void>(ct_max);

    const auto rt {42_u128};
    BOOST_TEST_EQ(rt, u128{42u});

    // Runtime EDOM branch: UINT128_MAX+1 (2^128) overflows from_chars.
    BOOST_TEST_THROWS(340282366920938463463374607431768211456_u128, std::overflow_error);

    // Runtime EINVAL branch ("Invalid conversion from literal"). A raw integer
    // token cannot carry non-digit junk, and "junk"_u128 will not compile (there
    // is no string-literal overload); even operator""_u128("junk") parses zero
    // digits and returns 0 rather than failing. The EINVAL path is reached only
    // by an empty string or a string carrying a sign, which we feed by calling
    // the literal operator directly.
    BOOST_TEST_THROWS(operator""_u128(""), std::invalid_argument);
    BOOST_TEST_THROWS(operator""_u128("-1"), std::invalid_argument);
    BOOST_TEST_THROWS(operator""_u128("+1"), std::invalid_argument);
}

template <>
void test<i8>()
{
    constexpr auto ct {42_i8};
    static_assert(ct == i8{42}, "compile-time _i8");
    BOOST_TEST_EQ(ct, i8{42});

    // Negative values: -42_i8 parses as -(42_i8).
    constexpr auto ct_neg {-42_i8};
    static_assert(ct_neg == i8{-42}, "compile-time negative _i8");
    BOOST_TEST_EQ(ct_neg, i8{-42});

    constexpr auto ct_max {127_i8};
    static_assert(ct_max == i8{127}, "compile-time _i8 max");
    BOOST_TEST_EQ(ct_max, i8{127});

    const auto rt {42_i8};
    BOOST_TEST_EQ(rt, i8{42});

    const auto rt_neg {-42_i8};
    BOOST_TEST_EQ(rt_neg, i8{-42});

    // INT8_MAX+1 overflows the positive-magnitude check.
    BOOST_TEST_THROWS(128_i8, std::overflow_error);
    BOOST_TEST_THROWS(18446744073709551615_i8, std::overflow_error);
}

template <>
void test<i16>()
{
    constexpr auto ct {42_i16};
    static_assert(ct == i16{42}, "compile-time _i16");
    BOOST_TEST_EQ(ct, i16{42});

    constexpr auto ct_neg {-42_i16};
    static_assert(ct_neg == i16{-42}, "compile-time negative _i16");
    BOOST_TEST_EQ(ct_neg, i16{-42});

    constexpr auto ct_max {32767_i16};
    static_assert(ct_max == i16{32767}, "compile-time _i16 max");
    BOOST_TEST_EQ(ct_max, i16{32767});

    const auto rt {42_i16};
    BOOST_TEST_EQ(rt, i16{42});

    const auto rt_neg {-42_i16};
    BOOST_TEST_EQ(rt_neg, i16{-42});

    BOOST_TEST_THROWS(32768_i16, std::overflow_error);
    BOOST_TEST_THROWS(18446744073709551615_i16, std::overflow_error);
}

template <>
void test<i32>()
{
    constexpr auto ct {42_i32};
    static_assert(ct == i32{42}, "compile-time _i32");
    BOOST_TEST_EQ(ct, i32{42});

    constexpr auto ct_neg {-42_i32};
    static_assert(ct_neg == i32{-42}, "compile-time negative _i32");
    BOOST_TEST_EQ(ct_neg, i32{-42});

    constexpr auto ct_max {2147483647_i32};
    static_assert(ct_max == i32{2147483647}, "compile-time _i32 max");
    BOOST_TEST_EQ(ct_max, i32{2147483647});

    const auto rt {42_i32};
    BOOST_TEST_EQ(rt, i32{42});

    const auto rt_neg {-42_i32};
    BOOST_TEST_EQ(rt_neg, i32{-42});

    BOOST_TEST_THROWS(2147483648_i32, std::overflow_error);
    BOOST_TEST_THROWS(18446744073709551615_i32, std::overflow_error);
}

template <>
void test<i64>()
{
    constexpr auto ct {42_i64};
    static_assert(ct == i64{42}, "compile-time _i64");
    BOOST_TEST_EQ(ct, i64{42});

    constexpr auto ct_neg {-42_i64};
    static_assert(ct_neg == i64{-42}, "compile-time negative _i64");
    BOOST_TEST_EQ(ct_neg, i64{-42});

    constexpr auto ct_max {9223372036854775807_i64};
    static_assert(ct_max == i64{9223372036854775807LL}, "compile-time _i64 max");
    BOOST_TEST_EQ(ct_max, i64{9223372036854775807LL});

    const auto rt {42_i64};
    BOOST_TEST_EQ(rt, i64{42});

    const auto rt_neg {-42_i64};
    BOOST_TEST_EQ(rt_neg, i64{-42});

    // INT64_MAX+1 = 2^63 fits in unsigned long long, so the token is well-formed
    // and the range check rejects it.
    BOOST_TEST_THROWS(9223372036854775808_i64, std::overflow_error);
    BOOST_TEST_THROWS(18446744073709551615_i64, std::overflow_error);
}

template <>
void test<i128>()
{
    // _i128 takes the raw digit string (const char*) and parses with from_chars.
    constexpr auto ct {42_i128};
    static_assert(ct == i128{42}, "compile-time _i128");
    BOOST_TEST_EQ(ct, i128{42});

    constexpr auto ct_neg {-42_i128};
    static_assert(ct_neg == i128{-42}, "compile-time negative _i128");
    BOOST_TEST_EQ(ct_neg, i128{-42});

    constexpr auto ct_zero {0_i128};
    static_assert(ct_zero == i128{0}, "compile-time _i128 zero");
    BOOST_TEST_EQ(ct_zero, i128{0});

    // INT128_MAX (2^127 - 1) is representable.
    constexpr auto ct_max {170141183460469231731687303715884105727_i128};
    BOOST_TEST_EQ(ct_max, i128{(std::numeric_limits<i128::basis_type>::max)()});

    const auto rt {42_i128};
    BOOST_TEST_EQ(rt, i128{42});

    const auto rt_neg {-42_i128};
    BOOST_TEST_EQ(rt_neg, i128{-42});

    // Runtime EDOM branch: INT128_MAX+1 (2^127) overflows from_chars.
    BOOST_TEST_THROWS(170141183460469231731687303715884105728_i128, std::overflow_error);

    // Runtime EINVAL branch ("Invalid conversion from literal"). As with _u128,
    // "junk"_i128 will not compile and operator""_i128("junk") parses zero digits
    // and returns 0; the EINVAL path is reached only by an empty or sign-only
    // string, fed by calling the literal operator directly.
    BOOST_TEST_THROWS(operator""_i128(""), std::invalid_argument);
    BOOST_TEST_THROWS(operator""_i128("-"), std::invalid_argument);
}

template <>
void test<f32>()
{
    constexpr auto ct {1.5_f32};
    static_assert(ct == f32{1.5f}, "compile-time _f32");
    BOOST_TEST(ct == f32{1.5f});

    constexpr auto ct_zero {0.0_f32};
    static_assert(ct_zero == f32{0.0f}, "compile-time _f32 zero");
    BOOST_TEST(ct_zero == f32{0.0f});

    // Scientific-notation literal still routes through the long double overload.
    constexpr auto ct_sci {1.0e10_f32};
    BOOST_TEST(ct_sci == f32{1.0e10f});

    const auto rt {1.5_f32};
    BOOST_TEST(rt == f32{1.5f});

    // The literal parameter is long double, so a finite value above float max
    // throws before the narrowing cast.
    BOOST_TEST_THROWS(1.0e40_f32, std::overflow_error);
    BOOST_TEST_THROWS(3.5e38_f32, std::overflow_error);
}

template <>
void test<f64>()
{
    constexpr auto ct {1.5_f64};
    static_assert(ct == f64{1.5}, "compile-time _f64");
    BOOST_TEST(ct == f64{1.5});

    constexpr auto ct_zero {0.0_f64};
    static_assert(ct_zero == f64{0.0}, "compile-time _f64 zero");
    BOOST_TEST(ct_zero == f64{0.0});

    constexpr auto ct_sci {1.0e100_f64};
    BOOST_TEST(ct_sci == f64{1.0e100});

    const auto rt {1.5_f64};
    BOOST_TEST(rt == f64{1.5});

    // Overflow can only be expressed as a source literal when long double has a
    // wider exponent range than double; otherwise the token would be ill-formed
    // on the host (e.g. Apple Silicon, where long double matches double).
#if LDBL_MAX_EXP > DBL_MAX_EXP
    BOOST_TEST_THROWS(1.0e310_f64, std::overflow_error);
    BOOST_TEST_THROWS(1.0e1000_f64, std::overflow_error);
#endif
}

int main()
{
    test<u8>();
    test<u16>();
    test<u32>();
    test<u64>();
    test<u128>();

    test<i8>();
    test<i16>();
    test<i32>();
    test<i64>();
    test<i128>();

    test<f32>();
    test<f64>();

    return boost::report_errors();
}

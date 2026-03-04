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

// The conversion operator OtherBasis() converts to raw signed integral types
// (std::int8_t, std::int16_t, etc.), not to other library types.

// -----------------------------------------------
// Widening conversions (should always succeed)
// -----------------------------------------------

void test_widening_i8_to_larger()
{
    const i8 pos {100};
    BOOST_TEST_EQ(static_cast<std::int16_t>(pos), static_cast<std::int16_t>(100));
    BOOST_TEST_EQ(static_cast<std::int32_t>(pos), static_cast<std::int32_t>(100));
    BOOST_TEST_EQ(static_cast<std::int64_t>(pos), static_cast<std::int64_t>(100));

    const i8 neg {-100};
    BOOST_TEST_EQ(static_cast<std::int16_t>(neg), static_cast<std::int16_t>(-100));
    BOOST_TEST_EQ(static_cast<std::int32_t>(neg), static_cast<std::int32_t>(-100));
    BOOST_TEST_EQ(static_cast<std::int64_t>(neg), static_cast<std::int64_t>(-100));

    const i8 zero {0};
    BOOST_TEST_EQ(static_cast<std::int16_t>(zero), static_cast<std::int16_t>(0));
    BOOST_TEST_EQ(static_cast<std::int32_t>(zero), static_cast<std::int32_t>(0));
    BOOST_TEST_EQ(static_cast<std::int64_t>(zero), static_cast<std::int64_t>(0));

    const i8 max8 {127};
    BOOST_TEST_EQ(static_cast<std::int16_t>(max8), static_cast<std::int16_t>(127));
    BOOST_TEST_EQ(static_cast<std::int32_t>(max8), static_cast<std::int32_t>(127));
    BOOST_TEST_EQ(static_cast<std::int64_t>(max8), static_cast<std::int64_t>(127));

    const i8 min8 {-128};
    BOOST_TEST_EQ(static_cast<std::int16_t>(min8), static_cast<std::int16_t>(-128));
    BOOST_TEST_EQ(static_cast<std::int32_t>(min8), static_cast<std::int32_t>(-128));
    BOOST_TEST_EQ(static_cast<std::int64_t>(min8), static_cast<std::int64_t>(-128));
}

void test_widening_i16_to_larger()
{
    const i16 a {32000};
    BOOST_TEST_EQ(static_cast<std::int32_t>(a), static_cast<std::int32_t>(32000));
    BOOST_TEST_EQ(static_cast<std::int64_t>(a), static_cast<std::int64_t>(32000));

    const i16 neg {-32000};
    BOOST_TEST_EQ(static_cast<std::int32_t>(neg), static_cast<std::int32_t>(-32000));
    BOOST_TEST_EQ(static_cast<std::int64_t>(neg), static_cast<std::int64_t>(-32000));

    const i16 zero {0};
    BOOST_TEST_EQ(static_cast<std::int32_t>(zero), static_cast<std::int32_t>(0));
    BOOST_TEST_EQ(static_cast<std::int64_t>(zero), static_cast<std::int64_t>(0));
}

void test_widening_i32_to_i64()
{
    const i32 a {2'000'000'000};
    BOOST_TEST_EQ(static_cast<std::int64_t>(a), static_cast<std::int64_t>(2'000'000'000));

    const i32 neg {-2'000'000'000};
    BOOST_TEST_EQ(static_cast<std::int64_t>(neg), static_cast<std::int64_t>(-2'000'000'000));

    const i32 zero {0};
    BOOST_TEST_EQ(static_cast<std::int64_t>(zero), static_cast<std::int64_t>(0));
}

// -----------------------------------------------
// Same-width conversions (identity, always succeed)
// -----------------------------------------------

void test_same_width()
{
    const i8 a {42};
    BOOST_TEST_EQ(static_cast<std::int8_t>(a), static_cast<std::int8_t>(42));

    const i8 a_neg {-42};
    BOOST_TEST_EQ(static_cast<std::int8_t>(a_neg), static_cast<std::int8_t>(-42));

    const i16 b {1000};
    BOOST_TEST_EQ(static_cast<std::int16_t>(b), static_cast<std::int16_t>(1000));

    const i16 b_neg {-1000};
    BOOST_TEST_EQ(static_cast<std::int16_t>(b_neg), static_cast<std::int16_t>(-1000));

    const i32 c {100000};
    BOOST_TEST_EQ(static_cast<std::int32_t>(c), static_cast<std::int32_t>(100000));

    const i32 c_neg {-100000};
    BOOST_TEST_EQ(static_cast<std::int32_t>(c_neg), static_cast<std::int32_t>(-100000));

    const i64 d {1'000'000'000LL};
    BOOST_TEST_EQ(static_cast<std::int64_t>(d), static_cast<std::int64_t>(1'000'000'000LL));

    const i64 d_neg {-1'000'000'000LL};
    BOOST_TEST_EQ(static_cast<std::int64_t>(d_neg), static_cast<std::int64_t>(-1'000'000'000LL));
}

// -----------------------------------------------
// Narrowing conversions that fit (should succeed)
// -----------------------------------------------

void test_narrowing_i16_to_i8_fits()
{
    const i16 a {0};
    BOOST_TEST_EQ(static_cast<std::int8_t>(a), static_cast<std::int8_t>(0));

    const i16 b {127};
    BOOST_TEST_EQ(static_cast<std::int8_t>(b), static_cast<std::int8_t>(127));

    const i16 c {-128};
    BOOST_TEST_EQ(static_cast<std::int8_t>(c), static_cast<std::int8_t>(-128));

    const i16 d {42};
    BOOST_TEST_EQ(static_cast<std::int8_t>(d), static_cast<std::int8_t>(42));

    const i16 e {-42};
    BOOST_TEST_EQ(static_cast<std::int8_t>(e), static_cast<std::int8_t>(-42));
}

void test_narrowing_i32_to_i8_fits()
{
    const i32 a {0};
    BOOST_TEST_EQ(static_cast<std::int8_t>(a), static_cast<std::int8_t>(0));

    const i32 b {127};
    BOOST_TEST_EQ(static_cast<std::int8_t>(b), static_cast<std::int8_t>(127));

    const i32 c {-128};
    BOOST_TEST_EQ(static_cast<std::int8_t>(c), static_cast<std::int8_t>(-128));
}

void test_narrowing_i32_to_i16_fits()
{
    const i32 a {0};
    BOOST_TEST_EQ(static_cast<std::int16_t>(a), static_cast<std::int16_t>(0));

    const i32 b {32767};
    BOOST_TEST_EQ(static_cast<std::int16_t>(b), static_cast<std::int16_t>(32767));

    const i32 c {-32768};
    BOOST_TEST_EQ(static_cast<std::int16_t>(c), static_cast<std::int16_t>(-32768));
}

void test_narrowing_i64_to_i8_fits()
{
    const i64 a {0};
    BOOST_TEST_EQ(static_cast<std::int8_t>(a), static_cast<std::int8_t>(0));

    const i64 b {127};
    BOOST_TEST_EQ(static_cast<std::int8_t>(b), static_cast<std::int8_t>(127));

    const i64 c {-128};
    BOOST_TEST_EQ(static_cast<std::int8_t>(c), static_cast<std::int8_t>(-128));
}

void test_narrowing_i64_to_i16_fits()
{
    const i64 a {32767};
    BOOST_TEST_EQ(static_cast<std::int16_t>(a), static_cast<std::int16_t>(32767));

    const i64 b {-32768};
    BOOST_TEST_EQ(static_cast<std::int16_t>(b), static_cast<std::int16_t>(-32768));
}

void test_narrowing_i64_to_i32_fits()
{
    const i64 a {2'147'483'647LL};
    BOOST_TEST_EQ(static_cast<std::int32_t>(a), static_cast<std::int32_t>(2'147'483'647));

    const i64 b {-2'147'483'648LL};
    BOOST_TEST_EQ(static_cast<std::int32_t>(b), static_cast<std::int32_t>(-2'147'483'648LL));

    const i64 c {0};
    BOOST_TEST_EQ(static_cast<std::int32_t>(c), static_cast<std::int32_t>(0));
}

void test_narrowing_i128_to_smaller_fits()
{
    using int128_t = boost::int128::int128_t;

    const i128 a {int128_t{0, 127}};
    BOOST_TEST_EQ(static_cast<std::int8_t>(a), static_cast<std::int8_t>(127));

    const i128 b {int128_t{0, 32767}};
    BOOST_TEST_EQ(static_cast<std::int16_t>(b), static_cast<std::int16_t>(32767));

    const i128 c {int128_t{0, 2'147'483'647}};
    BOOST_TEST_EQ(static_cast<std::int32_t>(c), static_cast<std::int32_t>(2'147'483'647));

    const i128 d {int128_t{0, static_cast<std::uint64_t>(INT64_MAX)}};
    BOOST_TEST_EQ(static_cast<std::int64_t>(d), INT64_MAX);

    const i128 zero {int128_t{0, 0}};
    BOOST_TEST_EQ(static_cast<std::int8_t>(zero), static_cast<std::int8_t>(0));
    BOOST_TEST_EQ(static_cast<std::int16_t>(zero), static_cast<std::int16_t>(0));
    BOOST_TEST_EQ(static_cast<std::int32_t>(zero), static_cast<std::int32_t>(0));
    BOOST_TEST_EQ(static_cast<std::int64_t>(zero), static_cast<std::int64_t>(0));
}

// -----------------------------------------------
// Narrowing conversions - positive overflow (should throw)
// -----------------------------------------------

void test_narrowing_i16_to_i8_positive_overflow()
{
    // 128 > 127 (i8 max)
    const i16 a {128};
    BOOST_TEST_THROWS((static_cast<std::int8_t>(a)), std::domain_error);

    // Max i16 value
    const i16 b {32767};
    BOOST_TEST_THROWS((static_cast<std::int8_t>(b)), std::domain_error);

    // Just above i8 max
    const i16 c {200};
    BOOST_TEST_THROWS((static_cast<std::int8_t>(c)), std::domain_error);
}

void test_narrowing_i32_to_i8_positive_overflow()
{
    const i32 a {128};
    BOOST_TEST_THROWS((static_cast<std::int8_t>(a)), std::domain_error);

    const i32 b {100000};
    BOOST_TEST_THROWS((static_cast<std::int8_t>(b)), std::domain_error);
}

void test_narrowing_i32_to_i16_positive_overflow()
{
    // 32768 > 32767 (i16 max)
    const i32 a {32768};
    BOOST_TEST_THROWS((static_cast<std::int16_t>(a)), std::domain_error);

    const i32 b {2'147'483'647};
    BOOST_TEST_THROWS((static_cast<std::int16_t>(b)), std::domain_error);
}

void test_narrowing_i64_to_i8_positive_overflow()
{
    const i64 a {128};
    BOOST_TEST_THROWS((static_cast<std::int8_t>(a)), std::domain_error);

    const i64 b {1'000'000};
    BOOST_TEST_THROWS((static_cast<std::int8_t>(b)), std::domain_error);
}

void test_narrowing_i64_to_i16_positive_overflow()
{
    const i64 a {32768};
    BOOST_TEST_THROWS((static_cast<std::int16_t>(a)), std::domain_error);

    const i64 b {INT64_MAX};
    BOOST_TEST_THROWS((static_cast<std::int16_t>(b)), std::domain_error);
}

void test_narrowing_i64_to_i32_positive_overflow()
{
    // 2147483648 > INT32_MAX
    const i64 a {2'147'483'648LL};
    BOOST_TEST_THROWS((static_cast<std::int32_t>(a)), std::domain_error);

    const i64 b {INT64_MAX};
    BOOST_TEST_THROWS((static_cast<std::int32_t>(b)), std::domain_error);
}

void test_narrowing_i128_to_smaller_positive_overflow()
{
    using int128_t = boost::int128::int128_t;

    // Large i128 value that overflows all smaller types
    const i128 big {int128_t{1, 0}};
    BOOST_TEST_THROWS((static_cast<std::int8_t>(big)), std::domain_error);
    BOOST_TEST_THROWS((static_cast<std::int16_t>(big)), std::domain_error);
    BOOST_TEST_THROWS((static_cast<std::int32_t>(big)), std::domain_error);
    BOOST_TEST_THROWS((static_cast<std::int64_t>(big)), std::domain_error);

    // Just above i8 max in i128
    const i128 over_i8 {int128_t{0, 128}};
    BOOST_TEST_THROWS((static_cast<std::int8_t>(over_i8)), std::domain_error);

    // Just above i16 max in i128
    const i128 over_i16 {int128_t{0, 32768}};
    BOOST_TEST_THROWS((static_cast<std::int16_t>(over_i16)), std::domain_error);

    // Just above i32 max in i128
    const i128 over_i32 {int128_t{0, 2'147'483'648ULL}};
    BOOST_TEST_THROWS((static_cast<std::int32_t>(over_i32)), std::domain_error);

    // Just above i64 max in i128
    const i128 over_i64 {int128_t{0, static_cast<std::uint64_t>(INT64_MAX) + 1}};
    BOOST_TEST_THROWS((static_cast<std::int64_t>(over_i64)), std::domain_error);
}

// -----------------------------------------------
// Narrowing conversions - negative overflow (should throw)
// -----------------------------------------------

void test_narrowing_i16_to_i8_negative_overflow()
{
    // -129 < -128 (i8 min)
    const i16 a {-129};
    BOOST_TEST_THROWS((static_cast<std::int8_t>(a)), std::domain_error);

    // Min i16 value
    const i16 b {-32768};
    BOOST_TEST_THROWS((static_cast<std::int8_t>(b)), std::domain_error);

    // Well below i8 min
    const i16 c {-200};
    BOOST_TEST_THROWS((static_cast<std::int8_t>(c)), std::domain_error);
}

void test_narrowing_i32_to_i8_negative_overflow()
{
    const i32 a {-129};
    BOOST_TEST_THROWS((static_cast<std::int8_t>(a)), std::domain_error);

    const i32 b {-100000};
    BOOST_TEST_THROWS((static_cast<std::int8_t>(b)), std::domain_error);
}

void test_narrowing_i32_to_i16_negative_overflow()
{
    // -32769 < -32768 (i16 min)
    const i32 a {-32769};
    BOOST_TEST_THROWS((static_cast<std::int16_t>(a)), std::domain_error);

    const i32 b {-2'147'483'647 - 1};
    BOOST_TEST_THROWS((static_cast<std::int16_t>(b)), std::domain_error);
}

void test_narrowing_i64_to_i8_negative_overflow()
{
    const i64 a {-129};
    BOOST_TEST_THROWS((static_cast<std::int8_t>(a)), std::domain_error);

    const i64 b {-1'000'000};
    BOOST_TEST_THROWS((static_cast<std::int8_t>(b)), std::domain_error);
}

void test_narrowing_i64_to_i16_negative_overflow()
{
    const i64 a {-32769};
    BOOST_TEST_THROWS((static_cast<std::int16_t>(a)), std::domain_error);

    const i64 b {INT64_MIN};
    BOOST_TEST_THROWS((static_cast<std::int16_t>(b)), std::domain_error);
}

void test_narrowing_i64_to_i32_negative_overflow()
{
    // -2147483649 < INT32_MIN
    const i64 a {-2'147'483'649LL};
    BOOST_TEST_THROWS((static_cast<std::int32_t>(a)), std::domain_error);

    const i64 b {INT64_MIN};
    BOOST_TEST_THROWS((static_cast<std::int32_t>(b)), std::domain_error);
}

void test_narrowing_i128_to_smaller_negative_overflow()
{
    using int128_t = boost::int128::int128_t;

    // Large negative i128 value
    const i128 big_neg {int128_t{-1, 0}};
    BOOST_TEST_THROWS((static_cast<std::int8_t>(big_neg)), std::domain_error);
    BOOST_TEST_THROWS((static_cast<std::int16_t>(big_neg)), std::domain_error);
    BOOST_TEST_THROWS((static_cast<std::int32_t>(big_neg)), std::domain_error);

    // Just below i8 min in i128
    const i128 under_i8 {int128_t{-1, static_cast<std::uint64_t>(-129)}};
    BOOST_TEST_THROWS((static_cast<std::int8_t>(under_i8)), std::domain_error);

    // Just below i16 min in i128
    const i128 under_i16 {int128_t{-1, static_cast<std::uint64_t>(-32769)}};
    BOOST_TEST_THROWS((static_cast<std::int16_t>(under_i16)), std::domain_error);

    // Just below i32 min in i128
    const i128 under_i32 {int128_t{-1, static_cast<std::uint64_t>(-2'147'483'649LL)}};
    BOOST_TEST_THROWS((static_cast<std::int32_t>(under_i32)), std::domain_error);
}

// -----------------------------------------------
// Boundary values (exact min/max fits, min-1/max+1 throws)
// -----------------------------------------------

void test_narrowing_boundaries()
{
    // i16 -> i8: 127 fits, 128 does not; -128 fits, -129 does not
    const i16 max_fits {127};
    BOOST_TEST_EQ(static_cast<std::int8_t>(max_fits), static_cast<std::int8_t>(127));

    const i16 max_no_fit {128};
    BOOST_TEST_THROWS((static_cast<std::int8_t>(max_no_fit)), std::domain_error);

    const i16 min_fits {-128};
    BOOST_TEST_EQ(static_cast<std::int8_t>(min_fits), static_cast<std::int8_t>(-128));

    const i16 min_no_fit {-129};
    BOOST_TEST_THROWS((static_cast<std::int8_t>(min_no_fit)), std::domain_error);

    // i32 -> i16: 32767 fits, 32768 does not; -32768 fits, -32769 does not
    const i32 i16_max_fits {32767};
    BOOST_TEST_EQ(static_cast<std::int16_t>(i16_max_fits), static_cast<std::int16_t>(32767));

    const i32 i16_max_no_fit {32768};
    BOOST_TEST_THROWS((static_cast<std::int16_t>(i16_max_no_fit)), std::domain_error);

    const i32 i16_min_fits {-32768};
    BOOST_TEST_EQ(static_cast<std::int16_t>(i16_min_fits), static_cast<std::int16_t>(-32768));

    const i32 i16_min_no_fit {-32769};
    BOOST_TEST_THROWS((static_cast<std::int16_t>(i16_min_no_fit)), std::domain_error);

    // i64 -> i32: INT32_MAX fits, INT32_MAX+1 does not; INT32_MIN fits, INT32_MIN-1 does not
    const i64 i32_max_fits {2'147'483'647LL};
    BOOST_TEST_EQ(static_cast<std::int32_t>(i32_max_fits), static_cast<std::int32_t>(2'147'483'647));

    const i64 i32_max_no_fit {2'147'483'648LL};
    BOOST_TEST_THROWS((static_cast<std::int32_t>(i32_max_no_fit)), std::domain_error);

    const i64 i32_min_fits {-2'147'483'648LL};
    BOOST_TEST_EQ(static_cast<std::int32_t>(i32_min_fits), static_cast<std::int32_t>(-2'147'483'648LL));

    const i64 i32_min_no_fit {-2'147'483'649LL};
    BOOST_TEST_THROWS((static_cast<std::int32_t>(i32_min_no_fit)), std::domain_error);
}

int main()
{
    // Widening (always succeed)
    test_widening_i8_to_larger();
    test_widening_i16_to_larger();
    test_widening_i32_to_i64();

    // Same-width
    test_same_width();

    // Narrowing that fits (succeed)
    test_narrowing_i16_to_i8_fits();
    test_narrowing_i32_to_i8_fits();
    test_narrowing_i32_to_i16_fits();
    test_narrowing_i64_to_i8_fits();
    test_narrowing_i64_to_i16_fits();
    test_narrowing_i64_to_i32_fits();
    test_narrowing_i128_to_smaller_fits();

    // Narrowing - positive overflow (throw)
    test_narrowing_i16_to_i8_positive_overflow();
    test_narrowing_i32_to_i8_positive_overflow();
    test_narrowing_i32_to_i16_positive_overflow();
    test_narrowing_i64_to_i8_positive_overflow();
    test_narrowing_i64_to_i16_positive_overflow();
    test_narrowing_i64_to_i32_positive_overflow();
    test_narrowing_i128_to_smaller_positive_overflow();

    // Narrowing - negative overflow (throw)
    test_narrowing_i16_to_i8_negative_overflow();
    test_narrowing_i32_to_i8_negative_overflow();
    test_narrowing_i32_to_i16_negative_overflow();
    test_narrowing_i64_to_i8_negative_overflow();
    test_narrowing_i64_to_i16_negative_overflow();
    test_narrowing_i64_to_i32_negative_overflow();
    test_narrowing_i128_to_smaller_negative_overflow();

    // Boundary values
    test_narrowing_boundaries();

    return boost::report_errors();
}

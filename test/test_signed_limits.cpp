// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE
import boost.safe_numbers;
#else
#include <boost/safe_numbers.hpp>
#include <limits>
#include <type_traits>
#endif

#include <boost/core/lightweight_test.hpp>

using namespace boost::safe_numbers;

// =============================================================================
// Test is_signed for signed types
// =============================================================================

void test_is_signed()
{
    BOOST_TEST(std::numeric_limits<i8>::is_signed);
    BOOST_TEST(std::numeric_limits<i16>::is_signed);
    BOOST_TEST(std::numeric_limits<i32>::is_signed);
    BOOST_TEST(std::numeric_limits<i64>::is_signed);
    BOOST_TEST(std::numeric_limits<i128>::is_signed);
}

// =============================================================================
// Test is_integer for signed types
// =============================================================================

void test_is_integer()
{
    BOOST_TEST(std::numeric_limits<i8>::is_integer);
    BOOST_TEST(std::numeric_limits<i16>::is_integer);
    BOOST_TEST(std::numeric_limits<i32>::is_integer);
    BOOST_TEST(std::numeric_limits<i64>::is_integer);
    BOOST_TEST(std::numeric_limits<i128>::is_integer);
}

// =============================================================================
// Test is_specialized
// =============================================================================

void test_is_specialized()
{
    BOOST_TEST(std::numeric_limits<i8>::is_specialized);
    BOOST_TEST(std::numeric_limits<i16>::is_specialized);
    BOOST_TEST(std::numeric_limits<i32>::is_specialized);
    BOOST_TEST(std::numeric_limits<i64>::is_specialized);
    BOOST_TEST(std::numeric_limits<i128>::is_specialized);
}

// =============================================================================
// Test digits (number of value bits, excluding sign bit)
// =============================================================================

void test_digits()
{
    BOOST_TEST_EQ(std::numeric_limits<i8>::digits, 7);
    BOOST_TEST_EQ(std::numeric_limits<i16>::digits, 15);
    BOOST_TEST_EQ(std::numeric_limits<i32>::digits, 31);
    BOOST_TEST_EQ(std::numeric_limits<i64>::digits, 63);
    BOOST_TEST_EQ(std::numeric_limits<i128>::digits, 127);
}

// =============================================================================
// Test min() and max() values
// =============================================================================

void test_min_max()
{
    // i8
    BOOST_TEST(std::numeric_limits<i8>::min() == i8{std::numeric_limits<std::int8_t>::min()});
    BOOST_TEST(std::numeric_limits<i8>::max() == i8{std::numeric_limits<std::int8_t>::max()});
    BOOST_TEST(std::numeric_limits<i8>::min() == i8{static_cast<std::int8_t>(-128)});
    BOOST_TEST(std::numeric_limits<i8>::max() == i8{static_cast<std::int8_t>(127)});

    // i16
    BOOST_TEST(std::numeric_limits<i16>::min() == i16{std::numeric_limits<std::int16_t>::min()});
    BOOST_TEST(std::numeric_limits<i16>::max() == i16{std::numeric_limits<std::int16_t>::max()});
    BOOST_TEST(std::numeric_limits<i16>::min() == i16{static_cast<std::int16_t>(-32768)});
    BOOST_TEST(std::numeric_limits<i16>::max() == i16{static_cast<std::int16_t>(32767)});

    // i32
    BOOST_TEST(std::numeric_limits<i32>::min() == i32{std::numeric_limits<std::int32_t>::min()});
    BOOST_TEST(std::numeric_limits<i32>::max() == i32{std::numeric_limits<std::int32_t>::max()});

    // i64
    BOOST_TEST(std::numeric_limits<i64>::min() == i64{std::numeric_limits<std::int64_t>::min()});
    BOOST_TEST(std::numeric_limits<i64>::max() == i64{std::numeric_limits<std::int64_t>::max()});
}

// =============================================================================
// Test lowest() == min() for integer types
// =============================================================================

void test_lowest()
{
    BOOST_TEST(std::numeric_limits<i8>::lowest() == std::numeric_limits<i8>::min());
    BOOST_TEST(std::numeric_limits<i16>::lowest() == std::numeric_limits<i16>::min());
    BOOST_TEST(std::numeric_limits<i32>::lowest() == std::numeric_limits<i32>::min());
    BOOST_TEST(std::numeric_limits<i64>::lowest() == std::numeric_limits<i64>::min());
    BOOST_TEST(std::numeric_limits<i128>::lowest() == std::numeric_limits<i128>::min());
}

// =============================================================================
// Test min() < 0 for signed types
// =============================================================================

void test_min_is_negative()
{
    BOOST_TEST(std::numeric_limits<i8>::min() < i8{static_cast<std::int8_t>(0)});
    BOOST_TEST(std::numeric_limits<i16>::min() < i16{static_cast<std::int16_t>(0)});
    BOOST_TEST(std::numeric_limits<i32>::min() < i32{static_cast<std::int32_t>(0)});
    BOOST_TEST(std::numeric_limits<i64>::min() < i64{static_cast<std::int64_t>(0)});
}

// =============================================================================
// Test max() > 0 for signed types
// =============================================================================

void test_max_is_positive()
{
    BOOST_TEST(std::numeric_limits<i8>::max() > i8{static_cast<std::int8_t>(0)});
    BOOST_TEST(std::numeric_limits<i16>::max() > i16{static_cast<std::int16_t>(0)});
    BOOST_TEST(std::numeric_limits<i32>::max() > i32{static_cast<std::int32_t>(0)});
    BOOST_TEST(std::numeric_limits<i64>::max() > i64{static_cast<std::int64_t>(0)});
}

// =============================================================================
// Constexpr tests
// =============================================================================

void test_constexpr()
{
    static_assert(std::numeric_limits<i8>::is_signed);
    static_assert(std::numeric_limits<i8>::is_integer);
    static_assert(std::numeric_limits<i8>::digits == 7);
    static_assert(std::numeric_limits<i16>::digits == 15);
    static_assert(std::numeric_limits<i32>::digits == 31);
    static_assert(std::numeric_limits<i64>::digits == 63);
    static_assert(std::numeric_limits<i128>::digits == 127);

    static_assert(std::numeric_limits<i8>::min() == i8{static_cast<std::int8_t>(-128)});
    static_assert(std::numeric_limits<i8>::max() == i8{static_cast<std::int8_t>(127)});
    static_assert(std::numeric_limits<i16>::min() == i16{static_cast<std::int16_t>(-32768)});
    static_assert(std::numeric_limits<i16>::max() == i16{static_cast<std::int16_t>(32767)});
}

// =============================================================================
// bounded_int limits
// =============================================================================

void test_bounded_int_limits()
{
    // bounded_int with negative min
    using temp_range = bounded_int<static_cast<std::int32_t>(-100), static_cast<std::int32_t>(100)>;
    BOOST_TEST(std::numeric_limits<temp_range>::is_signed);
    BOOST_TEST(std::numeric_limits<temp_range>::is_integer);
    BOOST_TEST(std::numeric_limits<temp_range>::min() == temp_range{i8{static_cast<std::int8_t>(-100)}});
    BOOST_TEST(std::numeric_limits<temp_range>::max() == temp_range{i8{static_cast<std::int8_t>(100)}});

    // Non-zero minimum bounded_int
    using positive_range = bounded_int<static_cast<std::int32_t>(10), static_cast<std::int32_t>(50)>;
    BOOST_TEST(std::numeric_limits<positive_range>::is_signed);
    BOOST_TEST(std::numeric_limits<positive_range>::min() == positive_range{i8{static_cast<std::int8_t>(10)}});
    BOOST_TEST(std::numeric_limits<positive_range>::max() == positive_range{i8{static_cast<std::int8_t>(50)}});

    // Negative range bounded_int
    using neg_range = bounded_int<static_cast<std::int32_t>(-50), static_cast<std::int32_t>(-10)>;
    BOOST_TEST(std::numeric_limits<neg_range>::is_signed);
    BOOST_TEST(std::numeric_limits<neg_range>::min() == neg_range{i8{static_cast<std::int8_t>(-50)}});
    BOOST_TEST(std::numeric_limits<neg_range>::max() == neg_range{i8{static_cast<std::int8_t>(-10)}});

    // Wider bounded_int
    using wide_range = bounded_int<static_cast<std::int32_t>(-10000), static_cast<std::int32_t>(10000)>;
    BOOST_TEST(std::numeric_limits<wide_range>::is_signed);
    BOOST_TEST(std::numeric_limits<wide_range>::min() == wide_range{i16{static_cast<std::int16_t>(-10000)}});
    BOOST_TEST(std::numeric_limits<wide_range>::max() == wide_range{i16{static_cast<std::int16_t>(10000)}});
}

// =============================================================================
// Test radix
// =============================================================================

void test_radix()
{
    BOOST_TEST_EQ(std::numeric_limits<i8>::radix, 2);
    BOOST_TEST_EQ(std::numeric_limits<i16>::radix, 2);
    BOOST_TEST_EQ(std::numeric_limits<i32>::radix, 2);
    BOOST_TEST_EQ(std::numeric_limits<i64>::radix, 2);
    BOOST_TEST_EQ(std::numeric_limits<i128>::radix, 2);
}

int main()
{
    test_is_signed();
    test_is_integer();
    test_is_specialized();
    test_digits();
    test_min_max();
    test_lowest();
    test_min_is_negative();
    test_max_is_positive();
    test_constexpr();
    test_bounded_int_limits();
    test_radix();

    return boost::report_errors();
}

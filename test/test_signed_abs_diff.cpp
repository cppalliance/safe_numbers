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
// abs_diff(a, a) == 0
// =============================================================================

template <typename T>
void test_abs_diff_equal()
{
    using underlying = typename detail::underlying_type_t<T>;

    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(0)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(1)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(-1)}, T{static_cast<underlying>(-1)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(42)}, T{static_cast<underlying>(42)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(-42)}, T{static_cast<underlying>(-42)}), T{static_cast<underlying>(0)});
}

// =============================================================================
// Same-sign differences (no subtraction overflow possible)
// =============================================================================

template <typename T>
void test_abs_diff_same_sign()
{
    using underlying = typename detail::underlying_type_t<T>;

    // Positive - positive
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(10)}, T{static_cast<underlying>(3)}), T{static_cast<underlying>(7)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(3)}, T{static_cast<underlying>(10)}), T{static_cast<underlying>(7)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(50)}, T{static_cast<underlying>(25)}), T{static_cast<underlying>(25)});

    // Negative - negative
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(-10)}, T{static_cast<underlying>(-3)}), T{static_cast<underlying>(7)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(-3)}, T{static_cast<underlying>(-10)}), T{static_cast<underlying>(7)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(-1)}, T{static_cast<underlying>(-2)}), T{static_cast<underlying>(1)});
}

// =============================================================================
// Small cross-sign differences (safe for i8: result fits in type)
// =============================================================================

template <typename T>
void test_abs_diff_cross_sign_small()
{
    using underlying = typename detail::underlying_type_t<T>;

    // These are safe: the subtraction result fits in the type
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(5)}, T{static_cast<underlying>(-5)}), T{static_cast<underlying>(10)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(-5)}, T{static_cast<underlying>(5)}), T{static_cast<underlying>(10)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(1)}, T{static_cast<underlying>(-1)}), T{static_cast<underlying>(2)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(0)}, T{static_cast<underlying>(-42)}), T{static_cast<underlying>(42)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(-42)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(42)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(0)}, T{static_cast<underlying>(10)}), T{static_cast<underlying>(10)});
    BOOST_TEST_EQ(abs_diff(T{static_cast<underlying>(0)}, T{static_cast<underlying>(-10)}), T{static_cast<underlying>(10)});
}

// =============================================================================
// Type-specific tests with wider ranges
// =============================================================================

void test_abs_diff_i16()
{
    // Same-sign large values
    BOOST_TEST_EQ(abs_diff(i16{static_cast<std::int16_t>(30000)}, i16{static_cast<std::int16_t>(25000)}),
                  i16{static_cast<std::int16_t>(5000)});
    BOOST_TEST_EQ(abs_diff(i16{static_cast<std::int16_t>(-30000)}, i16{static_cast<std::int16_t>(-25000)}),
                  i16{static_cast<std::int16_t>(5000)});
    // Cross-sign but result fits in i16
    BOOST_TEST_EQ(abs_diff(i16{static_cast<std::int16_t>(-1000)}, i16{static_cast<std::int16_t>(1000)}),
                  i16{static_cast<std::int16_t>(2000)});
}

void test_abs_diff_i32()
{
    BOOST_TEST_EQ(abs_diff(i32{static_cast<std::int32_t>(-100)}, i32{static_cast<std::int32_t>(200)}),
                  i32{static_cast<std::int32_t>(300)});
    BOOST_TEST_EQ(abs_diff(i32{static_cast<std::int32_t>(1000000)}, i32{static_cast<std::int32_t>(-1000000)}),
                  i32{static_cast<std::int32_t>(2000000)});
}

void test_abs_diff_i64()
{
    BOOST_TEST_EQ(abs_diff(i64{static_cast<std::int64_t>(-1000000000000LL)}, i64{static_cast<std::int64_t>(1000000000000LL)}),
                  i64{static_cast<std::int64_t>(2000000000000LL)});
    BOOST_TEST_EQ(abs_diff(i64{static_cast<std::int64_t>(-1LL)}, i64{static_cast<std::int64_t>(1LL)}),
                  i64{static_cast<std::int64_t>(2)});
}

// =============================================================================
// Constexpr tests
// =============================================================================

void test_abs_diff_constexpr()
{
    static_assert(abs_diff(i8{static_cast<std::int8_t>(10)}, i8{static_cast<std::int8_t>(3)}) == i8{static_cast<std::int8_t>(7)});
    static_assert(abs_diff(i8{static_cast<std::int8_t>(-3)}, i8{static_cast<std::int8_t>(-10)}) == i8{static_cast<std::int8_t>(7)});
    static_assert(abs_diff(i8{static_cast<std::int8_t>(5)}, i8{static_cast<std::int8_t>(-5)}) == i8{static_cast<std::int8_t>(10)});
    static_assert(abs_diff(i8{static_cast<std::int8_t>(0)}, i8{static_cast<std::int8_t>(0)}) == i8{static_cast<std::int8_t>(0)});
    static_assert(abs_diff(i16{static_cast<std::int16_t>(1000)}, i16{static_cast<std::int16_t>(-1000)}) == i16{static_cast<std::int16_t>(2000)});
    static_assert(abs_diff(i32{static_cast<std::int32_t>(100)}, i32{static_cast<std::int32_t>(-200)}) == i32{static_cast<std::int32_t>(300)});
}

int main()
{
    test_abs_diff_equal<i8>();
    test_abs_diff_equal<i16>();
    test_abs_diff_equal<i32>();
    test_abs_diff_equal<i64>();

    test_abs_diff_same_sign<i8>();
    test_abs_diff_same_sign<i16>();
    test_abs_diff_same_sign<i32>();
    test_abs_diff_same_sign<i64>();

    test_abs_diff_cross_sign_small<i8>();
    test_abs_diff_cross_sign_small<i16>();
    test_abs_diff_cross_sign_small<i32>();
    test_abs_diff_cross_sign_small<i64>();

    test_abs_diff_i16();
    test_abs_diff_i32();
    test_abs_diff_i64();

    test_abs_diff_constexpr();

    return boost::report_errors();
}

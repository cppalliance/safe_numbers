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
// Runtime tests: midpoint(a, a) == a
// =============================================================================

template <typename T>
void test_midpoint_equal()
{
    using underlying = typename detail::underlying_type_t<T>;

    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(0)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(1)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(-1)}, T{static_cast<underlying>(-1)}), T{static_cast<underlying>(-1)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(42)}, T{static_cast<underlying>(42)}), T{static_cast<underlying>(42)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(-42)}, T{static_cast<underlying>(-42)}), T{static_cast<underlying>(-42)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(100)}, T{static_cast<underlying>(100)}), T{static_cast<underlying>(100)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(-100)}, T{static_cast<underlying>(-100)}), T{static_cast<underlying>(-100)});
}

// =============================================================================
// Runtime tests: cross-zero midpoints
// =============================================================================

template <typename T>
void test_midpoint_cross_zero()
{
    using underlying = typename detail::underlying_type_t<T>;

    // midpoint(-10, 10) = 0 (even distance, exact)
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(-10)}, T{static_cast<underlying>(10)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(10)}, T{static_cast<underlying>(-10)}), T{static_cast<underlying>(0)});

    // midpoint(-11, 10) = -1 (odd distance, rounds toward first arg)
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(-11)}, T{static_cast<underlying>(10)}), T{static_cast<underlying>(-1)});
    // midpoint(10, -11) = 0 (rounds toward first arg = 10, so rounds up)
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(10)}, T{static_cast<underlying>(-11)}), T{static_cast<underlying>(0)});

    // midpoint(-1, 1) = 0 (even distance)
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(-1)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(1)}, T{static_cast<underlying>(-1)}), T{static_cast<underlying>(0)});

    // midpoint(-1, 2) = 0 (rounds toward first arg = -1)
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(-1)}, T{static_cast<underlying>(2)}), T{static_cast<underlying>(0)});
    // midpoint(2, -1) = 1 (rounds toward first arg = 2)
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(2)}, T{static_cast<underlying>(-1)}), T{static_cast<underlying>(1)});
}

// =============================================================================
// Runtime tests: negative midpoints
// =============================================================================

template <typename T>
void test_midpoint_negative()
{
    using underlying = typename detail::underlying_type_t<T>;

    // midpoint(-10, -20) = -15 (even distance)
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(-10)}, T{static_cast<underlying>(-20)}), T{static_cast<underlying>(-15)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(-20)}, T{static_cast<underlying>(-10)}), T{static_cast<underlying>(-15)});

    // midpoint(-10, -21) = -15 (odd distance, rounds toward first arg)
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(-10)}, T{static_cast<underlying>(-21)}), T{static_cast<underlying>(-15)});
    // midpoint(-21, -10) = -16 (rounds toward first arg = -21)
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(-21)}, T{static_cast<underlying>(-10)}), T{static_cast<underlying>(-16)});

    // midpoint(-50, -100) = -75
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(-50)}, T{static_cast<underlying>(-100)}), T{static_cast<underlying>(-75)});
}

// =============================================================================
// Runtime tests: midpoint with zero
// =============================================================================

template <typename T>
void test_midpoint_zero()
{
    using underlying = typename detail::underlying_type_t<T>;

    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(0)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(0)});

    // midpoint(0, 10) = 5
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(0)}, T{static_cast<underlying>(10)}), T{static_cast<underlying>(5)});
    // midpoint(0, -10) = -5
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(0)}, T{static_cast<underlying>(-10)}), T{static_cast<underlying>(-5)});

    // midpoint(0, 11) = 5 (rounds toward first arg = 0)
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(0)}, T{static_cast<underlying>(11)}), T{static_cast<underlying>(5)});
    // midpoint(0, -11) = -5 (rounds toward first arg = 0)
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(0)}, T{static_cast<underlying>(-11)}), T{static_cast<underlying>(-5)});
}

// =============================================================================
// Runtime tests: even distance (exact midpoint)
// =============================================================================

template <typename T>
void test_midpoint_even_distance()
{
    using underlying = typename detail::underlying_type_t<T>;

    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(2)}, T{static_cast<underlying>(8)}), T{static_cast<underlying>(5)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(8)}, T{static_cast<underlying>(2)}), T{static_cast<underlying>(5)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(-2)}, T{static_cast<underlying>(-8)}), T{static_cast<underlying>(-5)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(-8)}, T{static_cast<underlying>(-2)}), T{static_cast<underlying>(-5)});
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(10)}, T{static_cast<underlying>(20)}), T{static_cast<underlying>(15)});
}

// =============================================================================
// Runtime tests: rounding towards first argument
// =============================================================================

template <typename T>
void test_midpoint_rounding()
{
    using underlying = typename detail::underlying_type_t<T>;

    // midpoint(1, 4) = 2 (rounds down towards a=1)
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(1)}, T{static_cast<underlying>(4)}), T{static_cast<underlying>(2)});
    // midpoint(4, 1) = 3 (rounds up towards a=4)
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(4)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(3)});

    // midpoint(-4, -1) = -3 (rounds towards a=-4, which is more negative)
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(-4)}, T{static_cast<underlying>(-1)}), T{static_cast<underlying>(-3)});
    // midpoint(-1, -4) = -2 (rounds towards a=-1, which is less negative)
    BOOST_TEST_EQ(midpoint(T{static_cast<underlying>(-1)}, T{static_cast<underlying>(-4)}), T{static_cast<underlying>(-2)});
}

// =============================================================================
// Type-specific tests for larger values
// =============================================================================

void test_midpoint_i16()
{
    // Near extremes
    BOOST_TEST_EQ(midpoint(i16{std::numeric_limits<std::int16_t>::min()}, i16{std::numeric_limits<std::int16_t>::max()}),
                  i16{static_cast<std::int16_t>(-1)});
    BOOST_TEST_EQ(midpoint(i16{std::numeric_limits<std::int16_t>::max()}, i16{std::numeric_limits<std::int16_t>::min()}),
                  i16{static_cast<std::int16_t>(0)});

    BOOST_TEST_EQ(midpoint(i16{static_cast<std::int16_t>(-1000)}, i16{static_cast<std::int16_t>(1000)}),
                  i16{static_cast<std::int16_t>(0)});
}

void test_midpoint_i32()
{
    // Values near extremes: would overflow with naive (a+b)/2
    BOOST_TEST_EQ(midpoint(i32{std::numeric_limits<std::int32_t>::min()}, i32{std::numeric_limits<std::int32_t>::max()}),
                  i32{static_cast<std::int32_t>(-1)});
    BOOST_TEST_EQ(midpoint(i32{std::numeric_limits<std::int32_t>::max()}, i32{std::numeric_limits<std::int32_t>::min()}),
                  i32{static_cast<std::int32_t>(0)});

    BOOST_TEST_EQ(midpoint(i32{static_cast<std::int32_t>(-1000000000)}, i32{static_cast<std::int32_t>(1000000000)}),
                  i32{static_cast<std::int32_t>(0)});
}

void test_midpoint_i64()
{
    // Values near extremes
    BOOST_TEST_EQ(midpoint(i64{std::numeric_limits<std::int64_t>::min()}, i64{std::numeric_limits<std::int64_t>::max()}),
                  i64{static_cast<std::int64_t>(-1)});
    BOOST_TEST_EQ(midpoint(i64{std::numeric_limits<std::int64_t>::max()}, i64{std::numeric_limits<std::int64_t>::min()}),
                  i64{static_cast<std::int64_t>(0)});

    BOOST_TEST_EQ(midpoint(i64{static_cast<std::int64_t>(-1000000000000LL)}, i64{static_cast<std::int64_t>(1000000000000LL)}),
                  i64{static_cast<std::int64_t>(0)});
}

// =============================================================================
// Adjacent values
// =============================================================================

template <typename T>
void test_midpoint_adjacent()
{
    using underlying = typename detail::underlying_type_t<T>;

    // midpoint(n, n+1) == n (rounds towards a)
    // midpoint(n+1, n) == n+1 (rounds towards a)
    for (int i {-50}; i < 50; ++i)
    {
        const auto a = T{static_cast<underlying>(i)};
        const auto b = T{static_cast<underlying>(i + 1)};
        BOOST_TEST_EQ(midpoint(a, b), a);
        BOOST_TEST_EQ(midpoint(b, a), b);
    }
}

int main()
{
    // Equal values - all types
    test_midpoint_equal<i8>();
    test_midpoint_equal<i16>();
    test_midpoint_equal<i32>();
    test_midpoint_equal<i64>();

    // Cross-zero - all types
    test_midpoint_cross_zero<i8>();
    test_midpoint_cross_zero<i16>();
    test_midpoint_cross_zero<i32>();
    test_midpoint_cross_zero<i64>();

    // Negative - all types
    test_midpoint_negative<i8>();
    test_midpoint_negative<i16>();
    test_midpoint_negative<i32>();
    test_midpoint_negative<i64>();

    // Zero cases - all types
    test_midpoint_zero<i8>();
    test_midpoint_zero<i16>();
    test_midpoint_zero<i32>();
    test_midpoint_zero<i64>();

    // Even distance - all types
    test_midpoint_even_distance<i8>();
    test_midpoint_even_distance<i16>();
    test_midpoint_even_distance<i32>();
    test_midpoint_even_distance<i64>();

    // Rounding behaviour - all types
    test_midpoint_rounding<i8>();
    test_midpoint_rounding<i16>();
    test_midpoint_rounding<i32>();
    test_midpoint_rounding<i64>();

    // Adjacent values - all types
    test_midpoint_adjacent<i8>();
    test_midpoint_adjacent<i16>();
    test_midpoint_adjacent<i32>();
    test_midpoint_adjacent<i64>();

    // Type-specific larger values
    test_midpoint_i16();
    test_midpoint_i32();
    test_midpoint_i64();

    return boost::report_errors();
}

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
// Runtime tests: lcm(0, x) and lcm(x, 0)
// =============================================================================

template <typename T>
void test_lcm_zero()
{
    using underlying = typename detail::underlying_type_t<T>;

    // lcm(0, 0) == 0
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(0)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(0)});

    // lcm(0, n) == 0
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(0)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(0)}, T{static_cast<underlying>(7)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(0)}, T{static_cast<underlying>(-7)}), T{static_cast<underlying>(0)});

    // lcm(n, 0) == 0
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(1)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(7)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(0)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(-7)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(0)});
}

// =============================================================================
// Runtime tests: lcm with 1 and -1
// =============================================================================

template <typename T>
void test_lcm_one()
{
    using underlying = typename detail::underlying_type_t<T>;

    // lcm(1, n) == |n|
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(1)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(1)}, T{static_cast<underlying>(2)}), T{static_cast<underlying>(2)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(1)}, T{static_cast<underlying>(100)}), T{static_cast<underlying>(100)});

    // lcm(-1, n) == |n|
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(-1)}, T{static_cast<underlying>(2)}), T{static_cast<underlying>(2)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(-1)}, T{static_cast<underlying>(-5)}), T{static_cast<underlying>(5)});

    // lcm(n, 1) == |n|
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(2)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(2)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(100)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(100)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(-100)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(100)});
}

// =============================================================================
// Runtime tests: lcm with negative values -- result should be non-negative
// =============================================================================

template <typename T>
void test_lcm_negative_values()
{
    using underlying = typename detail::underlying_type_t<T>;

    // lcm(-a, b) == lcm(a, b) (result is always non-negative)
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(-4)}, T{static_cast<underlying>(6)}),
                  lcm(T{static_cast<underlying>(4)}, T{static_cast<underlying>(6)}));

    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(4)}, T{static_cast<underlying>(-6)}),
                  lcm(T{static_cast<underlying>(4)}, T{static_cast<underlying>(6)}));

    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(-4)}, T{static_cast<underlying>(-6)}),
                  lcm(T{static_cast<underlying>(4)}, T{static_cast<underlying>(6)}));

    // The result should be non-negative
    const auto result = lcm(T{static_cast<underlying>(-12)}, T{static_cast<underlying>(-18)});
    BOOST_TEST(result >= T{static_cast<underlying>(0)});
}

// =============================================================================
// Runtime tests: known values
// =============================================================================

template <typename T>
void test_lcm_known_values()
{
    using underlying = typename detail::underlying_type_t<T>;

    // Classic examples
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(4)}, T{static_cast<underlying>(6)}), T{static_cast<underlying>(12)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(6)}, T{static_cast<underlying>(8)}), T{static_cast<underlying>(24)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(12)}, T{static_cast<underlying>(18)}), T{static_cast<underlying>(36)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(15)}, T{static_cast<underlying>(20)}), T{static_cast<underlying>(60)});

    // Coprime pairs: lcm(a, b) == |a * b|
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(3)}, T{static_cast<underlying>(5)}), T{static_cast<underlying>(15)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(7)}, T{static_cast<underlying>(11)}), T{static_cast<underlying>(77)});

    // One divides the other: lcm(a, b) == max(|a|, |b|)
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(6)}, T{static_cast<underlying>(12)}), T{static_cast<underlying>(12)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(15)}, T{static_cast<underlying>(45)}), T{static_cast<underlying>(45)});

    // With negative values
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(-4)}, T{static_cast<underlying>(6)}), T{static_cast<underlying>(12)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(4)}, T{static_cast<underlying>(-6)}), T{static_cast<underlying>(12)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(-4)}, T{static_cast<underlying>(-6)}), T{static_cast<underlying>(12)});

    // Commutativity
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(4)}, T{static_cast<underlying>(6)}),
                  lcm(T{static_cast<underlying>(6)}, T{static_cast<underlying>(4)}));
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(-12)}, T{static_cast<underlying>(18)}),
                  lcm(T{static_cast<underlying>(18)}, T{static_cast<underlying>(-12)}));
}

// =============================================================================
// Runtime tests: lcm equal values
// =============================================================================

template <typename T>
void test_lcm_equal()
{
    using underlying = typename detail::underlying_type_t<T>;

    // lcm(n, n) == |n|
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(1)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(7)}, T{static_cast<underlying>(7)}), T{static_cast<underlying>(7)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(42)}, T{static_cast<underlying>(42)}), T{static_cast<underlying>(42)});

    // lcm(-n, -n) == |n|
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(-7)}, T{static_cast<underlying>(-7)}), T{static_cast<underlying>(7)});
    BOOST_TEST_EQ(lcm(T{static_cast<underlying>(-42)}, T{static_cast<underlying>(-42)}), T{static_cast<underlying>(42)});
}

// =============================================================================
// Type-specific tests for larger values
// =============================================================================

void test_lcm_i16()
{
    BOOST_TEST_EQ(lcm(i16{static_cast<std::int16_t>(100)}, i16{static_cast<std::int16_t>(75)}),
                  i16{static_cast<std::int16_t>(300)});
    BOOST_TEST_EQ(lcm(i16{static_cast<std::int16_t>(-100)}, i16{static_cast<std::int16_t>(75)}),
                  i16{static_cast<std::int16_t>(300)});
    BOOST_TEST_EQ(lcm(i16{static_cast<std::int16_t>(120)}, i16{static_cast<std::int16_t>(90)}),
                  i16{static_cast<std::int16_t>(360)});
}

void test_lcm_i32()
{
    BOOST_TEST_EQ(lcm(i32{static_cast<std::int32_t>(1000)}, i32{static_cast<std::int32_t>(750)}),
                  i32{static_cast<std::int32_t>(3000)});
    BOOST_TEST_EQ(lcm(i32{static_cast<std::int32_t>(-1000)}, i32{static_cast<std::int32_t>(750)}),
                  i32{static_cast<std::int32_t>(3000)});
    BOOST_TEST_EQ(lcm(i32{static_cast<std::int32_t>(100000)}, i32{static_cast<std::int32_t>(12500)}),
                  i32{static_cast<std::int32_t>(100000)});
}

void test_lcm_i64()
{
    BOOST_TEST_EQ(lcm(i64{static_cast<std::int64_t>(1000000LL)}, i64{static_cast<std::int64_t>(750000LL)}),
                  i64{static_cast<std::int64_t>(3000000LL)});
    BOOST_TEST_EQ(lcm(i64{static_cast<std::int64_t>(-1000000LL)}, i64{static_cast<std::int64_t>(750000LL)}),
                  i64{static_cast<std::int64_t>(3000000LL)});

    // Coprime consecutive Fibonacci
    BOOST_TEST_EQ(lcm(i64{static_cast<std::int64_t>(1346269LL)}, i64{static_cast<std::int64_t>(832040LL)}),
                  i64{static_cast<std::int64_t>(1346269LL)} * i64{static_cast<std::int64_t>(832040LL)});
}

int main()
{
    // Zero cases - all types
    test_lcm_zero<i8>();
    test_lcm_zero<i16>();
    test_lcm_zero<i32>();
    test_lcm_zero<i64>();

    // One cases - all types
    test_lcm_one<i8>();
    test_lcm_one<i16>();
    test_lcm_one<i32>();
    test_lcm_one<i64>();

    // Negative values - all types
    test_lcm_negative_values<i8>();
    test_lcm_negative_values<i16>();
    test_lcm_negative_values<i32>();
    test_lcm_negative_values<i64>();

    // Equal values - all types
    test_lcm_equal<i8>();
    test_lcm_equal<i16>();
    test_lcm_equal<i32>();
    test_lcm_equal<i64>();

    // Known values - all types
    test_lcm_known_values<i8>();
    test_lcm_known_values<i16>();
    test_lcm_known_values<i32>();
    test_lcm_known_values<i64>();

    // Type-specific larger values
    test_lcm_i16();
    test_lcm_i32();
    test_lcm_i64();

    return boost::report_errors();
}

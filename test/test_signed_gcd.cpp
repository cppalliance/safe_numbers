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
// Runtime tests: gcd(0, x) and gcd(x, 0)
// =============================================================================

template <typename T>
void test_gcd_zero()
{
    using underlying = typename detail::underlying_type_t<T>;

    // gcd(0, 0) == 0
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(0)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(0)});

    // gcd(0, n) == |n|
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(0)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(0)}, T{static_cast<underlying>(7)}), T{static_cast<underlying>(7)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(0)}, T{static_cast<underlying>(42)}), T{static_cast<underlying>(42)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(0)}, T{static_cast<underlying>(-1)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(0)}, T{static_cast<underlying>(-7)}), T{static_cast<underlying>(7)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(0)}, T{static_cast<underlying>(-42)}), T{static_cast<underlying>(42)});

    // gcd(n, 0) == |n|
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(1)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(7)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(7)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(-1)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(-7)}, T{static_cast<underlying>(0)}), T{static_cast<underlying>(7)});
}

// =============================================================================
// Runtime tests: sign doesn't affect result
// =============================================================================

template <typename T>
void test_gcd_sign_invariance()
{
    using underlying = typename detail::underlying_type_t<T>;

    // gcd(-a, b) == gcd(a, b)
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(-12)}, T{static_cast<underlying>(8)}),
                  gcd(T{static_cast<underlying>(12)}, T{static_cast<underlying>(8)}));

    // gcd(a, -b) == gcd(a, b)
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(12)}, T{static_cast<underlying>(-8)}),
                  gcd(T{static_cast<underlying>(12)}, T{static_cast<underlying>(8)}));

    // gcd(-a, -b) == gcd(a, b)
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(-12)}, T{static_cast<underlying>(-8)}),
                  gcd(T{static_cast<underlying>(12)}, T{static_cast<underlying>(8)}));

    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(-54)}, T{static_cast<underlying>(-24)}),
                  gcd(T{static_cast<underlying>(54)}, T{static_cast<underlying>(24)}));
}

// =============================================================================
// Runtime tests: gcd with 1 and -1
// =============================================================================

template <typename T>
void test_gcd_one()
{
    using underlying = typename detail::underlying_type_t<T>;

    // gcd(1, n) == 1 for any n
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(1)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(1)}, T{static_cast<underlying>(-1)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(1)}, T{static_cast<underlying>(100)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(1)}, T{static_cast<underlying>(-100)}), T{static_cast<underlying>(1)});

    // gcd(-1, n) == 1 for any n
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(-1)}, T{static_cast<underlying>(2)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(-1)}, T{static_cast<underlying>(-2)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(-1)}, T{static_cast<underlying>(100)}), T{static_cast<underlying>(1)});
}

// =============================================================================
// Runtime tests: known values
// =============================================================================

template <typename T>
void test_gcd_known_values()
{
    using underlying = typename detail::underlying_type_t<T>;

    // Classic examples
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(12)}, T{static_cast<underlying>(18)}), T{static_cast<underlying>(6)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(54)}, T{static_cast<underlying>(24)}), T{static_cast<underlying>(6)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(48)}, T{static_cast<underlying>(18)}), T{static_cast<underlying>(6)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(56)}, T{static_cast<underlying>(98)}), T{static_cast<underlying>(14)});

    // Coprime pairs
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(3)}, T{static_cast<underlying>(5)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(7)}, T{static_cast<underlying>(11)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(13)}, T{static_cast<underlying>(17)}), T{static_cast<underlying>(1)});

    // With negative values
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(-12)}, T{static_cast<underlying>(18)}), T{static_cast<underlying>(6)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(12)}, T{static_cast<underlying>(-18)}), T{static_cast<underlying>(6)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(-12)}, T{static_cast<underlying>(-18)}), T{static_cast<underlying>(6)});

    // Commutativity
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(12)}, T{static_cast<underlying>(8)}),
                  gcd(T{static_cast<underlying>(8)}, T{static_cast<underlying>(12)}));
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(-54)}, T{static_cast<underlying>(24)}),
                  gcd(T{static_cast<underlying>(24)}, T{static_cast<underlying>(-54)}));
}

// =============================================================================
// Type-specific tests for larger values
// =============================================================================

void test_gcd_i16()
{
    BOOST_TEST_EQ(gcd(i16{static_cast<std::int16_t>(1000)}, i16{static_cast<std::int16_t>(750)}),
                  i16{static_cast<std::int16_t>(250)});
    BOOST_TEST_EQ(gcd(i16{static_cast<std::int16_t>(-1000)}, i16{static_cast<std::int16_t>(750)}),
                  i16{static_cast<std::int16_t>(250)});
    BOOST_TEST_EQ(gcd(i16{static_cast<std::int16_t>(12345)}, i16{static_cast<std::int16_t>(6789)}),
                  i16{static_cast<std::int16_t>(3)});
}

void test_gcd_i32()
{
    BOOST_TEST_EQ(gcd(i32{static_cast<std::int32_t>(1000000)}, i32{static_cast<std::int32_t>(750000)}),
                  i32{static_cast<std::int32_t>(250000)});
    BOOST_TEST_EQ(gcd(i32{static_cast<std::int32_t>(-1000000)}, i32{static_cast<std::int32_t>(750000)}),
                  i32{static_cast<std::int32_t>(250000)});
    BOOST_TEST_EQ(gcd(i32{static_cast<std::int32_t>(1234567890)}, i32{static_cast<std::int32_t>(987654321)}),
                  i32{static_cast<std::int32_t>(9)});

    // Fibonacci-adjacent (stress test for Euclidean algorithm)
    BOOST_TEST_EQ(gcd(i32{static_cast<std::int32_t>(46368)}, i32{static_cast<std::int32_t>(28657)}),
                  i32{static_cast<std::int32_t>(1)});
}

void test_gcd_i64()
{
    BOOST_TEST_EQ(gcd(i64{static_cast<std::int64_t>(1000000000000LL)}, i64{static_cast<std::int64_t>(750000000000LL)}),
                  i64{static_cast<std::int64_t>(250000000000LL)});
    BOOST_TEST_EQ(gcd(i64{static_cast<std::int64_t>(-1000000000000LL)}, i64{static_cast<std::int64_t>(750000000000LL)}),
                  i64{static_cast<std::int64_t>(250000000000LL)});

    // Large Fibonacci numbers are coprime to each other
    BOOST_TEST_EQ(gcd(i64{static_cast<std::int64_t>(1346269LL)}, i64{static_cast<std::int64_t>(832040LL)}),
                  i64{static_cast<std::int64_t>(1)});
}

// =============================================================================
// Equal values
// =============================================================================

template <typename T>
void test_gcd_equal()
{
    using underlying = typename detail::underlying_type_t<T>;

    // gcd(n, n) == |n|
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(1)}, T{static_cast<underlying>(1)}), T{static_cast<underlying>(1)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(7)}, T{static_cast<underlying>(7)}), T{static_cast<underlying>(7)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(42)}, T{static_cast<underlying>(42)}), T{static_cast<underlying>(42)});

    // gcd(-n, -n) == |n|
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(-7)}, T{static_cast<underlying>(-7)}), T{static_cast<underlying>(7)});
    BOOST_TEST_EQ(gcd(T{static_cast<underlying>(-42)}, T{static_cast<underlying>(-42)}), T{static_cast<underlying>(42)});
}

int main()
{
    // Zero cases - all types
    test_gcd_zero<i8>();
    test_gcd_zero<i16>();
    test_gcd_zero<i32>();
    test_gcd_zero<i64>();

    // Sign invariance - all types
    test_gcd_sign_invariance<i8>();
    test_gcd_sign_invariance<i16>();
    test_gcd_sign_invariance<i32>();
    test_gcd_sign_invariance<i64>();

    // One cases - all types
    test_gcd_one<i8>();
    test_gcd_one<i16>();
    test_gcd_one<i32>();
    test_gcd_one<i64>();

    // Equal values - all types
    test_gcd_equal<i8>();
    test_gcd_equal<i16>();
    test_gcd_equal<i32>();
    test_gcd_equal<i64>();

    // Known values - all types
    test_gcd_known_values<i8>();
    test_gcd_known_values<i16>();
    test_gcd_known_values<i32>();
    test_gcd_known_values<i64>();

    // Type-specific larger values
    test_gcd_i16();
    test_gcd_i32();
    test_gcd_i64();

    return boost::report_errors();
}

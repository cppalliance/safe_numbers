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

// -----------------------------------------------
// Unary plus: +x == x for all values
// -----------------------------------------------

template <typename T>
void test_unary_plus()
{
    using basis = typename T::basis_type;

    const T zero {basis{0}};
    BOOST_TEST(+zero == zero);

    const T one {basis{1}};
    BOOST_TEST(+one == one);

    const T neg_one {basis{-1}};
    BOOST_TEST(+neg_one == neg_one);

    const T max_val {std::numeric_limits<basis>::max()};
    BOOST_TEST(+max_val == max_val);

    const T min_val {std::numeric_limits<basis>::min()};
    BOOST_TEST(+min_val == min_val);

    const T pos {basis{42}};
    BOOST_TEST(+pos == pos);

    const T neg {basis{-42}};
    BOOST_TEST(+neg == neg);
}

// -----------------------------------------------
// Unary minus: basic negation
// -----------------------------------------------

template <typename T>
void test_unary_minus_basic()
{
    using basis = typename T::basis_type;

    // -0 == 0
    const T zero {basis{0}};
    BOOST_TEST(-zero == zero);

    // -1 == T{-1}
    const T one {basis{1}};
    const T neg_one {basis{-1}};
    BOOST_TEST(-one == neg_one);
    BOOST_TEST(-neg_one == one);

    // -42 == T{-42}
    const T pos {basis{42}};
    const T neg {basis{-42}};
    BOOST_TEST(-pos == neg);
    BOOST_TEST(-neg == pos);

    // Double negation: -(-x) == x
    const T val {basis{100}};
    BOOST_TEST(-(-val) == val);

    const T neg_val {basis{-100}};
    BOOST_TEST(-(-neg_val) == neg_val);
}

// -----------------------------------------------
// Unary minus: max value can be negated safely
// -----------------------------------------------

template <typename T>
void test_unary_minus_max()
{
    using basis = typename T::basis_type;

    // Negating max should succeed: e.g., -127 is valid for int8_t
    const T max_val {std::numeric_limits<basis>::max()};
    const T expected {static_cast<basis>(-std::numeric_limits<basis>::max())};
    BOOST_TEST(-max_val == expected);
}

// -----------------------------------------------
// Unary minus: min value cannot be negated (UB / overflow)
// Negating INT_MIN is undefined behavior because the result
// does not fit in the signed type.
// -----------------------------------------------

template <typename T>
void test_unary_minus_min_throws()
{
    using basis = typename T::basis_type;

    // Negating min must throw: e.g., -(-128) = 128, which overflows int8_t
    const T min_val {std::numeric_limits<basis>::min()};
    BOOST_TEST_THROWS((-min_val), std::domain_error);
}

// -----------------------------------------------
// Unary minus: values near boundaries
// -----------------------------------------------

template <typename T>
void test_unary_minus_near_boundaries()
{
    using basis = typename T::basis_type;

    // min + 1 can be negated safely
    const T near_min {static_cast<basis>(std::numeric_limits<basis>::min() + 1)};
    const T expected {static_cast<basis>(std::numeric_limits<basis>::max())};
    BOOST_TEST(-near_min == expected);

    // max can be negated safely (gives min + 1)
    const T max_val {std::numeric_limits<basis>::max()};
    BOOST_TEST(-max_val == near_min);
}

int main()
{
    // Unary plus
    test_unary_plus<i8>();
    test_unary_plus<i16>();
    test_unary_plus<i32>();
    test_unary_plus<i64>();
    test_unary_plus<i128>();

    // Unary minus basic
    test_unary_minus_basic<i8>();
    test_unary_minus_basic<i16>();
    test_unary_minus_basic<i32>();
    test_unary_minus_basic<i64>();
    test_unary_minus_basic<i128>();

    // Unary minus max (should succeed)
    test_unary_minus_max<i8>();
    test_unary_minus_max<i16>();
    test_unary_minus_max<i32>();
    test_unary_minus_max<i64>();
    test_unary_minus_max<i128>();

    // Unary minus min (should throw)
    test_unary_minus_min_throws<i8>();
    test_unary_minus_min_throws<i16>();
    test_unary_minus_min_throws<i32>();
    test_unary_minus_min_throws<i64>();
    test_unary_minus_min_throws<i128>();

    // Near boundary values
    test_unary_minus_near_boundaries<i8>();
    test_unary_minus_near_boundaries<i16>();
    test_unary_minus_near_boundaries<i32>();
    test_unary_minus_near_boundaries<i64>();
    test_unary_minus_near_boundaries<i128>();

    return boost::report_errors();
}

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// Boundary tests for u16, u32, u64, u128 arithmetic operations.
// Tests mathematically significant boundary values for all operations and policies.

#include <boost/core/lightweight_test.hpp>

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>
#include <cstdint>
#include <limits>
#include <vector>

#endif

using namespace boost::safe_numbers;
using boost::int128::uint128_t;

// ============================================
// Boundary value generators
// ============================================

template <typename BasisType>
auto get_boundary_values() -> std::vector<BasisType>
{
    constexpr auto max_val {std::numeric_limits<BasisType>::max()};
    constexpr auto mid_val {max_val / 2U};

    std::vector<BasisType> values;

    // Near minimum
    values.push_back(0U);
    values.push_back(1U);
    values.push_back(2U);

    // Near maximum
    values.push_back(static_cast<BasisType>(max_val - 2U));
    values.push_back(static_cast<BasisType>(max_val - 1U));
    values.push_back(max_val);

    // Midpoint
    values.push_back(static_cast<BasisType>(mid_val - 1U));
    values.push_back(mid_val);
    values.push_back(static_cast<BasisType>(mid_val + 1U));

    // Powers of 2 (up to bit width)
    for (unsigned k {2}; k < sizeof(BasisType) * 8U; ++k)
    {
        const auto pow2 {static_cast<BasisType>(BasisType{1U} << k)};
        if (pow2 > 2U && pow2 < max_val - 1U)
        {
            values.push_back(static_cast<BasisType>(pow2 - 1U));
            values.push_back(pow2);
            values.push_back(static_cast<BasisType>(pow2 + 1U));
        }
    }

    return values;
}

// ============================================
// Helper type to get wider type for verification
// ============================================

template <typename T>
struct wider_type;

template <>
struct wider_type<std::uint16_t>
{
    using type = std::uint32_t;
};

template <>
struct wider_type<std::uint32_t>
{
    using type = std::uint64_t;
};

template <>
struct wider_type<std::uint64_t>
{
    using type = uint128_t;
};

// For u128, we use the same type but handle overflow detection differently
template <>
struct wider_type<uint128_t>
{
    using type = uint128_t;
};

template <typename T>
using wider_type_t = typename wider_type<T>::type;

// ============================================
// Addition boundary tests
// ============================================

template <typename SafeT, typename BasisType>
void test_boundary_saturating_add()
{
    const auto boundary_vals {get_boundary_values<BasisType>()};
    constexpr auto max_val {std::numeric_limits<BasisType>::max()};

    for (const auto lhs_val : boundary_vals)
    {
        for (const auto rhs_val : boundary_vals)
        {
            const SafeT lhs {lhs_val};
            const SafeT rhs {rhs_val};

            // Use wider type to compute expected value
            using wider = wider_type_t<BasisType>;
            const auto wide_sum {static_cast<wider>(lhs_val) + static_cast<wider>(rhs_val)};
            const auto expected {static_cast<BasisType>(wide_sum > static_cast<wider>(max_val) ? max_val : static_cast<BasisType>(wide_sum))};

            const auto result {saturating_add(lhs, rhs)};
            BOOST_TEST_EQ(static_cast<BasisType>(result), expected);
        }
    }
}

template <typename SafeT, typename BasisType>
void test_boundary_overflowing_add()
{
    const auto boundary_vals {get_boundary_values<BasisType>()};
    constexpr auto max_val {std::numeric_limits<BasisType>::max()};

    for (const auto lhs_val : boundary_vals)
    {
        for (const auto rhs_val : boundary_vals)
        {
            const SafeT lhs {lhs_val};
            const SafeT rhs {rhs_val};

            using wider = wider_type_t<BasisType>;
            const auto wide_sum {static_cast<wider>(lhs_val) + static_cast<wider>(rhs_val)};
            const auto expected_result {static_cast<BasisType>(wide_sum)};
            const auto expected_overflow {wide_sum > static_cast<wider>(max_val)};

            const auto [result, overflowed] {overflowing_add(lhs, rhs)};
            BOOST_TEST_EQ(static_cast<BasisType>(result), expected_result);
            BOOST_TEST_EQ(overflowed, expected_overflow);
        }
    }
}

template <typename SafeT, typename BasisType>
void test_boundary_checked_add()
{
    const auto boundary_vals {get_boundary_values<BasisType>()};
    constexpr auto max_val {std::numeric_limits<BasisType>::max()};

    for (const auto lhs_val : boundary_vals)
    {
        for (const auto rhs_val : boundary_vals)
        {
            const SafeT lhs {lhs_val};
            const SafeT rhs {rhs_val};

            // For u128, the wider type is the same as the type itself,
            // so we need to detect overflow by checking if sum < lhs
            using wider = wider_type_t<BasisType>;
            const auto wide_sum {static_cast<wider>(lhs_val) + static_cast<wider>(rhs_val)};

            bool should_overflow {};
            if constexpr (std::is_same_v<BasisType, uint128_t>)
            {
                // For u128, overflow if wrap-around occurred
                should_overflow = wide_sum < lhs_val;
            }
            else
            {
                should_overflow = wide_sum > static_cast<wider>(max_val);
            }

            const auto result {checked_add(lhs, rhs)};

            if (should_overflow)
            {
                BOOST_TEST(!result.has_value());
            }
            else
            {
                BOOST_TEST(result.has_value());
                BOOST_TEST_EQ(static_cast<BasisType>(result.value()), static_cast<BasisType>(wide_sum));
            }
        }
    }
}

template <typename SafeT, typename BasisType>
void test_boundary_wrapping_add()
{
    const auto boundary_vals {get_boundary_values<BasisType>()};

    for (const auto lhs_val : boundary_vals)
    {
        for (const auto rhs_val : boundary_vals)
        {
            const SafeT lhs {lhs_val};
            const SafeT rhs {rhs_val};

            using wider = wider_type_t<BasisType>;
            const auto wide_sum {static_cast<wider>(lhs_val) + static_cast<wider>(rhs_val)};
            const auto expected {static_cast<BasisType>(wide_sum)};

            const auto result {wrapping_add(lhs, rhs)};
            BOOST_TEST_EQ(static_cast<BasisType>(result), expected);
        }
    }
}

// ============================================
// Subtraction boundary tests
// ============================================

template <typename SafeT, typename BasisType>
void test_boundary_saturating_sub()
{
    const auto boundary_vals {get_boundary_values<BasisType>()};

    for (const auto lhs_val : boundary_vals)
    {
        for (const auto rhs_val : boundary_vals)
        {
            const SafeT lhs {lhs_val};
            const SafeT rhs {rhs_val};

            const auto expected {static_cast<BasisType>(rhs_val > lhs_val ? BasisType{0U} : lhs_val - rhs_val)};

            const auto result {saturating_sub(lhs, rhs)};
            BOOST_TEST_EQ(static_cast<BasisType>(result), expected);
        }
    }
}

template <typename SafeT, typename BasisType>
void test_boundary_overflowing_sub()
{
    const auto boundary_vals {get_boundary_values<BasisType>()};

    for (const auto lhs_val : boundary_vals)
    {
        for (const auto rhs_val : boundary_vals)
        {
            const SafeT lhs {lhs_val};
            const SafeT rhs {rhs_val};

            // Wrapping subtraction
            const auto expected_result {static_cast<BasisType>(lhs_val - rhs_val)};
            const auto expected_underflow {rhs_val > lhs_val};

            const auto [result, underflowed] {overflowing_sub(lhs, rhs)};
            BOOST_TEST_EQ(static_cast<BasisType>(result), expected_result);
            BOOST_TEST_EQ(underflowed, expected_underflow);
        }
    }
}

template <typename SafeT, typename BasisType>
void test_boundary_checked_sub()
{
    const auto boundary_vals {get_boundary_values<BasisType>()};

    for (const auto lhs_val : boundary_vals)
    {
        for (const auto rhs_val : boundary_vals)
        {
            const SafeT lhs {lhs_val};
            const SafeT rhs {rhs_val};

            const auto should_underflow {rhs_val > lhs_val};

            const auto result {checked_sub(lhs, rhs)};

            if (should_underflow)
            {
                BOOST_TEST(!result.has_value());
            }
            else
            {
                BOOST_TEST(result.has_value());
                BOOST_TEST_EQ(static_cast<BasisType>(result.value()), static_cast<BasisType>(lhs_val - rhs_val));
            }
        }
    }
}

template <typename SafeT, typename BasisType>
void test_boundary_wrapping_sub()
{
    const auto boundary_vals {get_boundary_values<BasisType>()};

    for (const auto lhs_val : boundary_vals)
    {
        for (const auto rhs_val : boundary_vals)
        {
            const SafeT lhs {lhs_val};
            const SafeT rhs {rhs_val};

            const auto expected {static_cast<BasisType>(lhs_val - rhs_val)};

            const auto result {wrapping_sub(lhs, rhs)};
            BOOST_TEST_EQ(static_cast<BasisType>(result), expected);
        }
    }
}

// ============================================
// Multiplication boundary tests
// ============================================

template <typename SafeT, typename BasisType>
void test_boundary_saturating_mul()
{
    const auto boundary_vals {get_boundary_values<BasisType>()};
    constexpr auto max_val {std::numeric_limits<BasisType>::max()};

    for (const auto lhs_val : boundary_vals)
    {
        for (const auto rhs_val : boundary_vals)
        {
            const SafeT lhs {lhs_val};
            const SafeT rhs {rhs_val};

            using wider = wider_type_t<BasisType>;
            const auto wide_product {static_cast<wider>(lhs_val) * static_cast<wider>(rhs_val)};
            const auto expected {static_cast<BasisType>(wide_product > static_cast<wider>(max_val) ? max_val : static_cast<BasisType>(wide_product))};

            const auto result {saturating_mul(lhs, rhs)};
            BOOST_TEST_EQ(static_cast<BasisType>(result), expected);
        }
    }
}

template <typename SafeT, typename BasisType>
void test_boundary_overflowing_mul()
{
    const auto boundary_vals {get_boundary_values<BasisType>()};
    constexpr auto max_val {std::numeric_limits<BasisType>::max()};

    for (const auto lhs_val : boundary_vals)
    {
        for (const auto rhs_val : boundary_vals)
        {
            const SafeT lhs {lhs_val};
            const SafeT rhs {rhs_val};

            using wider = wider_type_t<BasisType>;
            const auto wide_product {static_cast<wider>(lhs_val) * static_cast<wider>(rhs_val)};
            const auto expected_result {static_cast<BasisType>(wide_product)};
            const auto expected_overflow {wide_product > static_cast<wider>(max_val)};

            const auto [result, overflowed] {overflowing_mul(lhs, rhs)};
            BOOST_TEST_EQ(static_cast<BasisType>(result), expected_result);
            BOOST_TEST_EQ(overflowed, expected_overflow);
        }
    }
}

template <typename SafeT, typename BasisType>
void test_boundary_checked_mul()
{
    const auto boundary_vals {get_boundary_values<BasisType>()};
    constexpr auto max_val {std::numeric_limits<BasisType>::max()};

    for (const auto lhs_val : boundary_vals)
    {
        for (const auto rhs_val : boundary_vals)
        {
            const SafeT lhs {lhs_val};
            const SafeT rhs {rhs_val};

            // For u128, the wider type is the same as the type itself
            using wider = wider_type_t<BasisType>;
            const auto wide_product {static_cast<wider>(lhs_val) * static_cast<wider>(rhs_val)};

            bool should_overflow {};
            if constexpr (std::is_same_v<BasisType, uint128_t>)
            {
                // For u128, overflow if rhs != 0 and lhs > max/rhs
                should_overflow = rhs_val != BasisType{0U} && lhs_val > max_val / rhs_val;
            }
            else
            {
                should_overflow = wide_product > static_cast<wider>(max_val);
            }

            const auto result {checked_mul(lhs, rhs)};

            if (should_overflow)
            {
                BOOST_TEST(!result.has_value());
            }
            else
            {
                BOOST_TEST(result.has_value());
                BOOST_TEST_EQ(static_cast<BasisType>(result.value()), static_cast<BasisType>(wide_product));
            }
        }
    }
}

template <typename SafeT, typename BasisType>
void test_boundary_wrapping_mul()
{
    const auto boundary_vals {get_boundary_values<BasisType>()};

    for (const auto lhs_val : boundary_vals)
    {
        for (const auto rhs_val : boundary_vals)
        {
            const SafeT lhs {lhs_val};
            const SafeT rhs {rhs_val};

            using wider = wider_type_t<BasisType>;
            const auto wide_product {static_cast<wider>(lhs_val) * static_cast<wider>(rhs_val)};
            const auto expected {static_cast<BasisType>(wide_product)};

            const auto result {wrapping_mul(lhs, rhs)};
            BOOST_TEST_EQ(static_cast<BasisType>(result), expected);
        }
    }
}

// ============================================
// Division boundary tests
// ============================================

template <typename SafeT, typename BasisType>
void test_boundary_checked_div()
{
    const auto boundary_vals {get_boundary_values<BasisType>()};

    for (const auto lhs_val : boundary_vals)
    {
        for (const auto rhs_val : boundary_vals)
        {
            const SafeT lhs {lhs_val};
            const SafeT rhs {rhs_val};

            const auto result {checked_div(lhs, rhs)};

            if (rhs_val == 0U)
            {
                BOOST_TEST(!result.has_value());
            }
            else
            {
                BOOST_TEST(result.has_value());
                BOOST_TEST_EQ(static_cast<BasisType>(result.value()), static_cast<BasisType>(lhs_val / rhs_val));
            }
        }
    }
}

// ============================================
// Modulo boundary tests
// ============================================

template <typename SafeT, typename BasisType>
void test_boundary_checked_mod()
{
    const auto boundary_vals {get_boundary_values<BasisType>()};

    for (const auto lhs_val : boundary_vals)
    {
        for (const auto rhs_val : boundary_vals)
        {
            const SafeT lhs {lhs_val};
            const SafeT rhs {rhs_val};

            const auto result {checked_mod(lhs, rhs)};

            if (rhs_val == 0U)
            {
                BOOST_TEST(!result.has_value());
            }
            else
            {
                BOOST_TEST(result.has_value());
                BOOST_TEST_EQ(static_cast<BasisType>(result.value()), static_cast<BasisType>(lhs_val % rhs_val));
            }
        }
    }
}

// ============================================
// u128 specific tests (no wider type available)
// Uses overflow detection from library
// ============================================

void test_boundary_u128_saturating_add()
{
    const auto boundary_vals {get_boundary_values<uint128_t>()};
    constexpr auto max_val {std::numeric_limits<uint128_t>::max()};

    for (const auto lhs_val : boundary_vals)
    {
        for (const auto rhs_val : boundary_vals)
        {
            const u128 lhs {lhs_val};
            const u128 rhs {rhs_val};

            // Detect overflow: sum would wrap if sum < lhs
            const auto sum {static_cast<uint128_t>(lhs_val + rhs_val)};
            const auto would_overflow {sum < lhs_val};
            const auto expected {would_overflow ? max_val : sum};

            const auto result {saturating_add(lhs, rhs)};
            BOOST_TEST_EQ(static_cast<uint128_t>(result), expected);
        }
    }
}

void test_boundary_u128_overflowing_add()
{
    const auto boundary_vals {get_boundary_values<uint128_t>()};

    for (const auto lhs_val : boundary_vals)
    {
        for (const auto rhs_val : boundary_vals)
        {
            const u128 lhs {lhs_val};
            const u128 rhs {rhs_val};

            const auto sum {static_cast<uint128_t>(lhs_val + rhs_val)};
            const auto expected_overflow {sum < lhs_val};

            const auto [result, overflowed] {overflowing_add(lhs, rhs)};
            BOOST_TEST_EQ(static_cast<uint128_t>(result), sum);
            BOOST_TEST_EQ(overflowed, expected_overflow);
        }
    }
}

void test_boundary_u128_saturating_mul()
{
    const auto boundary_vals {get_boundary_values<uint128_t>()};
    constexpr auto max_val {std::numeric_limits<uint128_t>::max()};

    for (const auto lhs_val : boundary_vals)
    {
        for (const auto rhs_val : boundary_vals)
        {
            const u128 lhs {lhs_val};
            const u128 rhs {rhs_val};

            // Detect overflow: if rhs != 0 and lhs > max/rhs, overflow
            const auto would_overflow {rhs_val != 0U && lhs_val > max_val / rhs_val};
            const auto product {static_cast<uint128_t>(lhs_val * rhs_val)};
            const auto expected {would_overflow ? max_val : product};

            const auto result {saturating_mul(lhs, rhs)};
            BOOST_TEST_EQ(static_cast<uint128_t>(result), expected);
        }
    }
}

void test_boundary_u128_overflowing_mul()
{
    const auto boundary_vals {get_boundary_values<uint128_t>()};
    constexpr auto max_val {std::numeric_limits<uint128_t>::max()};

    for (const auto lhs_val : boundary_vals)
    {
        for (const auto rhs_val : boundary_vals)
        {
            const u128 lhs {lhs_val};
            const u128 rhs {rhs_val};

            const auto expected_overflow {rhs_val != 0U && lhs_val > max_val / rhs_val};
            const auto product {static_cast<uint128_t>(lhs_val * rhs_val)};

            const auto [result, overflowed] {overflowing_mul(lhs, rhs)};
            BOOST_TEST_EQ(static_cast<uint128_t>(result), product);
            BOOST_TEST_EQ(overflowed, expected_overflow);
        }
    }
}

// ============================================
// Test runners for each type
// ============================================

template <typename SafeT, typename BasisType>
void run_all_boundary_tests()
{
    // Addition
    test_boundary_saturating_add<SafeT, BasisType>();
    test_boundary_overflowing_add<SafeT, BasisType>();
    test_boundary_checked_add<SafeT, BasisType>();
    test_boundary_wrapping_add<SafeT, BasisType>();

    // Subtraction
    test_boundary_saturating_sub<SafeT, BasisType>();
    test_boundary_overflowing_sub<SafeT, BasisType>();
    test_boundary_checked_sub<SafeT, BasisType>();
    test_boundary_wrapping_sub<SafeT, BasisType>();

    // Multiplication
    test_boundary_saturating_mul<SafeT, BasisType>();
    test_boundary_overflowing_mul<SafeT, BasisType>();
    test_boundary_checked_mul<SafeT, BasisType>();
    test_boundary_wrapping_mul<SafeT, BasisType>();

    // Division (checked only, others throw on div by zero)
    test_boundary_checked_div<SafeT, BasisType>();

    // Modulo (checked only, others throw on mod by zero)
    test_boundary_checked_mod<SafeT, BasisType>();
}

void run_u128_boundary_tests()
{
    // u128 needs special handling since there's no wider type
    test_boundary_u128_saturating_add();
    test_boundary_u128_overflowing_add();
    test_boundary_saturating_sub<u128, uint128_t>();
    test_boundary_overflowing_sub<u128, uint128_t>();
    test_boundary_checked_sub<u128, uint128_t>();
    test_boundary_wrapping_sub<u128, uint128_t>();
    test_boundary_u128_saturating_mul();
    test_boundary_u128_overflowing_mul();
    test_boundary_checked_div<u128, uint128_t>();
    test_boundary_checked_mod<u128, uint128_t>();

    // These use the generic templates which work for subtraction, div, mod
    test_boundary_checked_add<u128, uint128_t>();
    test_boundary_wrapping_add<u128, uint128_t>();
    test_boundary_checked_mul<u128, uint128_t>();
    test_boundary_wrapping_mul<u128, uint128_t>();
}

int main()
{
    // u16 boundary tests
    run_all_boundary_tests<u16, std::uint16_t>();

    // u32 boundary tests
    run_all_boundary_tests<u32, std::uint32_t>();

    // u64 boundary tests
    run_all_boundary_tests<u64, std::uint64_t>();

    // u128 boundary tests (special handling)
    run_u128_boundary_tests();

    return boost::report_errors();
}

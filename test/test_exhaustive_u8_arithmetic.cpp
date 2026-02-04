// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// Exhaustive verification tests for u8 arithmetic operations.
// Tests all 256 x 256 = 65,536 input combinations for mathematical correctness.
// This provides a rigorous proof that the algorithm logic is correct.

#include <boost/core/lightweight_test.hpp>

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>
#include <cstdint>
#include <limits>

#endif

using namespace boost::safe_numbers;

// Use u32 as wider type for computing reference values
constexpr std::uint32_t U8_MAX {255U};

// ============================================
// Addition Tests
// ============================================

void test_exhaustive_saturating_add()
{
    for (std::uint32_t lhs_val {0}; lhs_val <= U8_MAX; ++lhs_val)
    {
        for (std::uint32_t rhs_val {0}; rhs_val <= U8_MAX; ++rhs_val)
        {
            const auto lhs {u8{static_cast<std::uint8_t>(lhs_val)}};
            const auto rhs {u8{static_cast<std::uint8_t>(rhs_val)}};

            const auto wide_sum {lhs_val + rhs_val};
            const auto expected {static_cast<std::uint8_t>(wide_sum > U8_MAX ? U8_MAX : wide_sum)};

            const auto result {saturating_add(lhs, rhs)};
            BOOST_TEST_EQ(static_cast<std::uint8_t>(result), expected);
        }
    }
}

void test_exhaustive_overflowing_add()
{
    for (std::uint32_t lhs_val {0}; lhs_val <= U8_MAX; ++lhs_val)
    {
        for (std::uint32_t rhs_val {0}; rhs_val <= U8_MAX; ++rhs_val)
        {
            const auto lhs {u8{static_cast<std::uint8_t>(lhs_val)}};
            const auto rhs {u8{static_cast<std::uint8_t>(rhs_val)}};

            const auto wide_sum {lhs_val + rhs_val};
            const auto expected_result {static_cast<std::uint8_t>(wide_sum & 0xFFU)};
            const auto expected_overflow {wide_sum > U8_MAX};

            const auto [result, overflowed] {overflowing_add(lhs, rhs)};
            BOOST_TEST_EQ(static_cast<std::uint8_t>(result), expected_result);
            BOOST_TEST_EQ(overflowed, expected_overflow);
        }
    }
}

void test_exhaustive_checked_add()
{
    for (std::uint32_t lhs_val {0}; lhs_val <= U8_MAX; ++lhs_val)
    {
        for (std::uint32_t rhs_val {0}; rhs_val <= U8_MAX; ++rhs_val)
        {
            const auto lhs {u8{static_cast<std::uint8_t>(lhs_val)}};
            const auto rhs {u8{static_cast<std::uint8_t>(rhs_val)}};

            const auto wide_sum {lhs_val + rhs_val};
            const auto should_overflow {wide_sum > U8_MAX};

            const auto result {checked_add(lhs, rhs)};

            if (should_overflow)
            {
                BOOST_TEST(!result.has_value());
            }
            else
            {
                BOOST_TEST(result.has_value());
                BOOST_TEST_EQ(static_cast<std::uint8_t>(result.value()), static_cast<std::uint8_t>(wide_sum));
            }
        }
    }
}

void test_exhaustive_wrapping_add()
{
    for (std::uint32_t lhs_val {0}; lhs_val <= U8_MAX; ++lhs_val)
    {
        for (std::uint32_t rhs_val {0}; rhs_val <= U8_MAX; ++rhs_val)
        {
            const auto lhs {u8{static_cast<std::uint8_t>(lhs_val)}};
            const auto rhs {u8{static_cast<std::uint8_t>(rhs_val)}};

            const auto wide_sum {lhs_val + rhs_val};
            const auto expected {static_cast<std::uint8_t>(wide_sum & 0xFFU)};

            const auto result {wrapping_add(lhs, rhs)};
            BOOST_TEST_EQ(static_cast<std::uint8_t>(result), expected);
        }
    }
}

// ============================================
// Subtraction Tests
// ============================================

void test_exhaustive_saturating_sub()
{
    for (std::uint32_t lhs_val {0}; lhs_val <= U8_MAX; ++lhs_val)
    {
        for (std::uint32_t rhs_val {0}; rhs_val <= U8_MAX; ++rhs_val)
        {
            const auto lhs {u8{static_cast<std::uint8_t>(lhs_val)}};
            const auto rhs {u8{static_cast<std::uint8_t>(rhs_val)}};

            const auto expected {static_cast<std::uint8_t>(rhs_val > lhs_val ? 0U : lhs_val - rhs_val)};

            const auto result {saturating_sub(lhs, rhs)};
            BOOST_TEST_EQ(static_cast<std::uint8_t>(result), expected);
        }
    }
}

void test_exhaustive_overflowing_sub()
{
    for (std::uint32_t lhs_val {0}; lhs_val <= U8_MAX; ++lhs_val)
    {
        for (std::uint32_t rhs_val {0}; rhs_val <= U8_MAX; ++rhs_val)
        {
            const auto lhs {u8{static_cast<std::uint8_t>(lhs_val)}};
            const auto rhs {u8{static_cast<std::uint8_t>(rhs_val)}};

            // Wrapping subtraction: (lhs - rhs + 256) % 256
            const auto wide_diff {static_cast<std::int32_t>(lhs_val) - static_cast<std::int32_t>(rhs_val)};
            const auto expected_result {static_cast<std::uint8_t>((wide_diff + 256) & 0xFFU)};
            const auto expected_underflow {rhs_val > lhs_val};

            const auto [result, underflowed] {overflowing_sub(lhs, rhs)};
            BOOST_TEST_EQ(static_cast<std::uint8_t>(result), expected_result);
            BOOST_TEST_EQ(underflowed, expected_underflow);
        }
    }
}

void test_exhaustive_checked_sub()
{
    for (std::uint32_t lhs_val {0}; lhs_val <= U8_MAX; ++lhs_val)
    {
        for (std::uint32_t rhs_val {0}; rhs_val <= U8_MAX; ++rhs_val)
        {
            const auto lhs {u8{static_cast<std::uint8_t>(lhs_val)}};
            const auto rhs {u8{static_cast<std::uint8_t>(rhs_val)}};

            const auto should_underflow {rhs_val > lhs_val};

            const auto result {checked_sub(lhs, rhs)};

            if (should_underflow)
            {
                BOOST_TEST(!result.has_value());
            }
            else
            {
                BOOST_TEST(result.has_value());
                BOOST_TEST_EQ(static_cast<std::uint8_t>(result.value()), static_cast<std::uint8_t>(lhs_val - rhs_val));
            }
        }
    }
}

void test_exhaustive_wrapping_sub()
{
    for (std::uint32_t lhs_val {0}; lhs_val <= U8_MAX; ++lhs_val)
    {
        for (std::uint32_t rhs_val {0}; rhs_val <= U8_MAX; ++rhs_val)
        {
            const auto lhs {u8{static_cast<std::uint8_t>(lhs_val)}};
            const auto rhs {u8{static_cast<std::uint8_t>(rhs_val)}};

            const auto wide_diff {static_cast<std::int32_t>(lhs_val) - static_cast<std::int32_t>(rhs_val)};
            const auto expected {static_cast<std::uint8_t>((wide_diff + 256) & 0xFFU)};

            const auto result {wrapping_sub(lhs, rhs)};
            BOOST_TEST_EQ(static_cast<std::uint8_t>(result), expected);
        }
    }
}

// ============================================
// Multiplication Tests
// ============================================

void test_exhaustive_saturating_mul()
{
    for (std::uint32_t lhs_val {0}; lhs_val <= U8_MAX; ++lhs_val)
    {
        for (std::uint32_t rhs_val {0}; rhs_val <= U8_MAX; ++rhs_val)
        {
            const auto lhs {u8{static_cast<std::uint8_t>(lhs_val)}};
            const auto rhs {u8{static_cast<std::uint8_t>(rhs_val)}};

            const auto wide_product {lhs_val * rhs_val};
            const auto expected {static_cast<std::uint8_t>(wide_product > U8_MAX ? U8_MAX : wide_product)};

            const auto result {saturating_mul(lhs, rhs)};
            BOOST_TEST_EQ(static_cast<std::uint8_t>(result), expected);
        }
    }
}

void test_exhaustive_overflowing_mul()
{
    for (std::uint32_t lhs_val {0}; lhs_val <= U8_MAX; ++lhs_val)
    {
        for (std::uint32_t rhs_val {0}; rhs_val <= U8_MAX; ++rhs_val)
        {
            const auto lhs {u8{static_cast<std::uint8_t>(lhs_val)}};
            const auto rhs {u8{static_cast<std::uint8_t>(rhs_val)}};

            const auto wide_product {lhs_val * rhs_val};
            const auto expected_result {static_cast<std::uint8_t>(wide_product & 0xFFU)};
            const auto expected_overflow {wide_product > U8_MAX};

            const auto [result, overflowed] {overflowing_mul(lhs, rhs)};
            BOOST_TEST_EQ(static_cast<std::uint8_t>(result), expected_result);
            BOOST_TEST_EQ(overflowed, expected_overflow);
        }
    }
}

void test_exhaustive_checked_mul()
{
    for (std::uint32_t lhs_val {0}; lhs_val <= U8_MAX; ++lhs_val)
    {
        for (std::uint32_t rhs_val {0}; rhs_val <= U8_MAX; ++rhs_val)
        {
            const auto lhs {u8{static_cast<std::uint8_t>(lhs_val)}};
            const auto rhs {u8{static_cast<std::uint8_t>(rhs_val)}};

            const auto wide_product {lhs_val * rhs_val};
            const auto should_overflow {wide_product > U8_MAX};

            const auto result {checked_mul(lhs, rhs)};

            if (should_overflow)
            {
                BOOST_TEST(!result.has_value());
            }
            else
            {
                BOOST_TEST(result.has_value());
                BOOST_TEST_EQ(static_cast<std::uint8_t>(result.value()), static_cast<std::uint8_t>(wide_product));
            }
        }
    }
}

void test_exhaustive_wrapping_mul()
{
    for (std::uint32_t lhs_val {0}; lhs_val <= U8_MAX; ++lhs_val)
    {
        for (std::uint32_t rhs_val {0}; rhs_val <= U8_MAX; ++rhs_val)
        {
            const auto lhs {u8{static_cast<std::uint8_t>(lhs_val)}};
            const auto rhs {u8{static_cast<std::uint8_t>(rhs_val)}};

            const auto wide_product {lhs_val * rhs_val};
            const auto expected {static_cast<std::uint8_t>(wide_product & 0xFFU)};

            const auto result {wrapping_mul(lhs, rhs)};
            BOOST_TEST_EQ(static_cast<std::uint8_t>(result), expected);
        }
    }
}

// ============================================
// Division Tests
// ============================================

void test_exhaustive_checked_div()
{
    for (std::uint32_t lhs_val {0}; lhs_val <= U8_MAX; ++lhs_val)
    {
        for (std::uint32_t rhs_val {0}; rhs_val <= U8_MAX; ++rhs_val)
        {
            const auto lhs {u8{static_cast<std::uint8_t>(lhs_val)}};
            const auto rhs {u8{static_cast<std::uint8_t>(rhs_val)}};

            const auto result {checked_div(lhs, rhs)};

            if (rhs_val == 0U)
            {
                BOOST_TEST(!result.has_value());
            }
            else
            {
                BOOST_TEST(result.has_value());
                BOOST_TEST_EQ(static_cast<std::uint8_t>(result.value()), static_cast<std::uint8_t>(lhs_val / rhs_val));
            }
        }
    }
}

// ============================================
// Modulo Tests
// ============================================

void test_exhaustive_checked_mod()
{
    for (std::uint32_t lhs_val {0}; lhs_val <= U8_MAX; ++lhs_val)
    {
        for (std::uint32_t rhs_val {0}; rhs_val <= U8_MAX; ++rhs_val)
        {
            const auto lhs {u8{static_cast<std::uint8_t>(lhs_val)}};
            const auto rhs {u8{static_cast<std::uint8_t>(rhs_val)}};

            const auto result {checked_mod(lhs, rhs)};

            if (rhs_val == 0U)
            {
                BOOST_TEST(!result.has_value());
            }
            else
            {
                BOOST_TEST(result.has_value());
                BOOST_TEST_EQ(static_cast<std::uint8_t>(result.value()), static_cast<std::uint8_t>(lhs_val % rhs_val));
            }
        }
    }
}

int main()
{
    // Addition: 4 policies tested exhaustively
    test_exhaustive_saturating_add();
    test_exhaustive_overflowing_add();
    test_exhaustive_checked_add();
    test_exhaustive_wrapping_add();

    // Subtraction: 4 policies tested exhaustively
    test_exhaustive_saturating_sub();
    test_exhaustive_overflowing_sub();
    test_exhaustive_checked_sub();
    test_exhaustive_wrapping_sub();

    // Multiplication: 4 policies tested exhaustively
    test_exhaustive_saturating_mul();
    test_exhaustive_overflowing_mul();
    test_exhaustive_checked_mul();
    test_exhaustive_wrapping_mul();

    // Division: checked policy tested exhaustively (others throw on div by zero)
    test_exhaustive_checked_div();

    // Modulo: checked policy tested exhaustively (others throw on mod by zero)
    test_exhaustive_checked_mod();

    return boost::report_errors();
}

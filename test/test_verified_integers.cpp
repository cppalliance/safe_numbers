// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>
#include <limits>
#include <cstdint>
#include <compare>

#endif

#include <boost/core/lightweight_test.hpp>

using namespace boost::safe_numbers;

// =============================================================================
// Construction tests (consteval - verified via constexpr variables)
// =============================================================================

template <typename VerifiedT, typename BasisT>
void test_construction()
{
    constexpr auto val1 = VerifiedT{BasisT{0}};
    constexpr auto val2 = VerifiedT{BasisT{42}};
    constexpr auto val3 = VerifiedT{BasisT{100}};

    BOOST_TEST(val1 == VerifiedT{BasisT{0}});
    BOOST_TEST(val2 == VerifiedT{BasisT{42}});
    BOOST_TEST(val3 == VerifiedT{BasisT{100}});

    // From underlying type
    using underlying = detail::underlying_type_t<BasisT>;
    constexpr auto val4 = VerifiedT{underlying{42}};
    BOOST_TEST(val4 == val2);
}

// =============================================================================
// Conversion tests (constexpr - runtime testable)
// =============================================================================

template <typename VerifiedT, typename BasisT>
void test_conversions()
{
    using underlying = detail::underlying_type_t<BasisT>;

    constexpr auto val = VerifiedT{BasisT{42}};

    // Convert to BasisType
    constexpr auto basis_val = static_cast<BasisT>(val);
    BOOST_TEST(basis_val == BasisT{42});

    // Convert to underlying type
    constexpr auto raw_val = static_cast<underlying>(val);
    BOOST_TEST(raw_val == underlying{42});
}

// =============================================================================
// Comparison tests (constexpr - runtime testable)
// =============================================================================

template <typename VerifiedT, typename BasisT>
void test_comparisons()
{
    constexpr auto a = VerifiedT{BasisT{10}};
    constexpr auto b = VerifiedT{BasisT{20}};
    constexpr auto c = VerifiedT{BasisT{10}};

    BOOST_TEST(a == c);
    BOOST_TEST(a != b);
    BOOST_TEST(a < b);
    BOOST_TEST(a <= b);
    BOOST_TEST(a <= c);
    BOOST_TEST(b > a);
    BOOST_TEST(b >= a);
    BOOST_TEST(a >= c);
    BOOST_TEST((a <=> c) == std::strong_ordering::equal);
    BOOST_TEST((a <=> b) == std::strong_ordering::less);
    BOOST_TEST((b <=> a) == std::strong_ordering::greater);
}

// =============================================================================
// Arithmetic tests (consteval - verified via static_assert)
// =============================================================================

template <typename VerifiedT, typename BasisT>
consteval auto test_addition_impl() -> bool
{
    const auto a = VerifiedT{BasisT{10}};
    const auto b = VerifiedT{BasisT{20}};
    const auto c = a + b;
    return c == VerifiedT{BasisT{30}};
}

template <typename VerifiedT, typename BasisT>
consteval auto test_subtraction_impl() -> bool
{
    const auto a = VerifiedT{BasisT{30}};
    const auto b = VerifiedT{BasisT{10}};
    const auto c = a - b;
    return c == VerifiedT{BasisT{20}};
}

template <typename VerifiedT, typename BasisT>
consteval auto test_multiplication_impl() -> bool
{
    const auto a = VerifiedT{BasisT{5}};
    const auto b = VerifiedT{BasisT{6}};
    const auto c = a * b;
    return c == VerifiedT{BasisT{30}};
}

template <typename VerifiedT, typename BasisT>
consteval auto test_division_impl() -> bool
{
    const auto a = VerifiedT{BasisT{30}};
    const auto b = VerifiedT{BasisT{5}};
    const auto c = a / b;
    return c == VerifiedT{BasisT{6}};
}

template <typename VerifiedT, typename BasisT>
consteval auto test_modulo_impl() -> bool
{
    const auto a = VerifiedT{BasisT{17}};
    const auto b = VerifiedT{BasisT{5}};
    const auto c = a % b;
    return c == VerifiedT{BasisT{2}};
}

// --- Addition ---
static_assert(test_addition_impl<verified_u8, u8>());
static_assert(test_addition_impl<verified_u16, u16>());
static_assert(test_addition_impl<verified_u32, u32>());
static_assert(test_addition_impl<verified_u64, u64>());
static_assert(test_addition_impl<verified_u128, u128>());

// --- Subtraction ---
static_assert(test_subtraction_impl<verified_u8, u8>());
static_assert(test_subtraction_impl<verified_u16, u16>());
static_assert(test_subtraction_impl<verified_u32, u32>());
static_assert(test_subtraction_impl<verified_u64, u64>());
static_assert(test_subtraction_impl<verified_u128, u128>());

// --- Multiplication ---
static_assert(test_multiplication_impl<verified_u8, u8>());
static_assert(test_multiplication_impl<verified_u16, u16>());
static_assert(test_multiplication_impl<verified_u32, u32>());
static_assert(test_multiplication_impl<verified_u64, u64>());
static_assert(test_multiplication_impl<verified_u128, u128>());

// --- Division ---
static_assert(test_division_impl<verified_u8, u8>());
static_assert(test_division_impl<verified_u16, u16>());
static_assert(test_division_impl<verified_u32, u32>());
static_assert(test_division_impl<verified_u64, u64>());
static_assert(test_division_impl<verified_u128, u128>());

// --- Modulo ---
static_assert(test_modulo_impl<verified_u8, u8>());
static_assert(test_modulo_impl<verified_u16, u16>());
static_assert(test_modulo_impl<verified_u32, u32>());
static_assert(test_modulo_impl<verified_u64, u64>());
static_assert(test_modulo_impl<verified_u128, u128>());

// =============================================================================
// Compound assignment tests (consteval - verified via static_assert)
// =============================================================================

template <typename VerifiedT, typename BasisT>
consteval auto test_compound_addition_impl() -> bool
{
    auto a = VerifiedT{BasisT{10}};
    a += VerifiedT{BasisT{20}};
    return a == VerifiedT{BasisT{30}};
}

template <typename VerifiedT, typename BasisT>
consteval auto test_compound_subtraction_impl() -> bool
{
    auto a = VerifiedT{BasisT{30}};
    a -= VerifiedT{BasisT{10}};
    return a == VerifiedT{BasisT{20}};
}

template <typename VerifiedT, typename BasisT>
consteval auto test_compound_multiplication_impl() -> bool
{
    auto a = VerifiedT{BasisT{5}};
    a *= VerifiedT{BasisT{6}};
    return a == VerifiedT{BasisT{30}};
}

template <typename VerifiedT, typename BasisT>
consteval auto test_compound_division_impl() -> bool
{
    auto a = VerifiedT{BasisT{30}};
    a /= VerifiedT{BasisT{5}};
    return a == VerifiedT{BasisT{6}};
}

template <typename VerifiedT, typename BasisT>
consteval auto test_compound_modulo_impl() -> bool
{
    auto a = VerifiedT{BasisT{17}};
    a %= VerifiedT{BasisT{5}};
    return a == VerifiedT{BasisT{2}};
}

// --- Compound addition ---
static_assert(test_compound_addition_impl<verified_u8, u8>());
static_assert(test_compound_addition_impl<verified_u16, u16>());
static_assert(test_compound_addition_impl<verified_u32, u32>());
static_assert(test_compound_addition_impl<verified_u64, u64>());
static_assert(test_compound_addition_impl<verified_u128, u128>());

// --- Compound subtraction ---
static_assert(test_compound_subtraction_impl<verified_u8, u8>());
static_assert(test_compound_subtraction_impl<verified_u16, u16>());
static_assert(test_compound_subtraction_impl<verified_u32, u32>());
static_assert(test_compound_subtraction_impl<verified_u64, u64>());
static_assert(test_compound_subtraction_impl<verified_u128, u128>());

// --- Compound multiplication ---
static_assert(test_compound_multiplication_impl<verified_u8, u8>());
static_assert(test_compound_multiplication_impl<verified_u16, u16>());
static_assert(test_compound_multiplication_impl<verified_u32, u32>());
static_assert(test_compound_multiplication_impl<verified_u64, u64>());
static_assert(test_compound_multiplication_impl<verified_u128, u128>());

// --- Compound division ---
static_assert(test_compound_division_impl<verified_u8, u8>());
static_assert(test_compound_division_impl<verified_u16, u16>());
static_assert(test_compound_division_impl<verified_u32, u32>());
static_assert(test_compound_division_impl<verified_u64, u64>());
static_assert(test_compound_division_impl<verified_u128, u128>());

// --- Compound modulo ---
static_assert(test_compound_modulo_impl<verified_u8, u8>());
static_assert(test_compound_modulo_impl<verified_u16, u16>());
static_assert(test_compound_modulo_impl<verified_u32, u32>());
static_assert(test_compound_modulo_impl<verified_u64, u64>());
static_assert(test_compound_modulo_impl<verified_u128, u128>());

// =============================================================================
// Increment/decrement tests (consteval - verified via static_assert)
// =============================================================================

template <typename VerifiedT, typename BasisT>
consteval auto test_prefix_increment_impl() -> bool
{
    auto a = VerifiedT{BasisT{5}};
    ++a;
    return a == VerifiedT{BasisT{6}};
}

template <typename VerifiedT, typename BasisT>
consteval auto test_postfix_increment_impl() -> bool
{
    auto a = VerifiedT{BasisT{5}};
    const auto old = a++;
    return a == VerifiedT{BasisT{6}} && old == VerifiedT{BasisT{5}};
}

template <typename VerifiedT, typename BasisT>
consteval auto test_prefix_decrement_impl() -> bool
{
    auto a = VerifiedT{BasisT{5}};
    --a;
    return a == VerifiedT{BasisT{4}};
}

template <typename VerifiedT, typename BasisT>
consteval auto test_postfix_decrement_impl() -> bool
{
    auto a = VerifiedT{BasisT{5}};
    const auto old = a--;
    return a == VerifiedT{BasisT{4}} && old == VerifiedT{BasisT{5}};
}

// --- Prefix increment ---
static_assert(test_prefix_increment_impl<verified_u8, u8>());
static_assert(test_prefix_increment_impl<verified_u16, u16>());
static_assert(test_prefix_increment_impl<verified_u32, u32>());
static_assert(test_prefix_increment_impl<verified_u64, u64>());
static_assert(test_prefix_increment_impl<verified_u128, u128>());

// --- Postfix increment ---
static_assert(test_postfix_increment_impl<verified_u8, u8>());
static_assert(test_postfix_increment_impl<verified_u16, u16>());
static_assert(test_postfix_increment_impl<verified_u32, u32>());
static_assert(test_postfix_increment_impl<verified_u64, u64>());
static_assert(test_postfix_increment_impl<verified_u128, u128>());

// --- Prefix decrement ---
static_assert(test_prefix_decrement_impl<verified_u8, u8>());
static_assert(test_prefix_decrement_impl<verified_u16, u16>());
static_assert(test_prefix_decrement_impl<verified_u32, u32>());
static_assert(test_prefix_decrement_impl<verified_u64, u64>());
static_assert(test_prefix_decrement_impl<verified_u128, u128>());

// --- Postfix decrement ---
static_assert(test_postfix_decrement_impl<verified_u8, u8>());
static_assert(test_postfix_decrement_impl<verified_u16, u16>());
static_assert(test_postfix_decrement_impl<verified_u32, u32>());
static_assert(test_postfix_decrement_impl<verified_u64, u64>());
static_assert(test_postfix_decrement_impl<verified_u128, u128>());

// =============================================================================
// Bounded integer tests (consteval - verified via static_assert)
// =============================================================================

using test_bounded = bounded_uint<0u, 100u>;
using test_verified_bounded = verified_bounded_integer<0u, 100u>;

static_assert(test_addition_impl<test_verified_bounded, test_bounded>());
static_assert(test_subtraction_impl<test_verified_bounded, test_bounded>());
static_assert(test_multiplication_impl<test_verified_bounded, test_bounded>());
static_assert(test_division_impl<test_verified_bounded, test_bounded>());
static_assert(test_modulo_impl<test_verified_bounded, test_bounded>());

static_assert(test_compound_addition_impl<test_verified_bounded, test_bounded>());
static_assert(test_compound_subtraction_impl<test_verified_bounded, test_bounded>());
static_assert(test_compound_multiplication_impl<test_verified_bounded, test_bounded>());
static_assert(test_compound_division_impl<test_verified_bounded, test_bounded>());
static_assert(test_compound_modulo_impl<test_verified_bounded, test_bounded>());

static_assert(test_prefix_increment_impl<test_verified_bounded, test_bounded>());
static_assert(test_postfix_increment_impl<test_verified_bounded, test_bounded>());
static_assert(test_prefix_decrement_impl<test_verified_bounded, test_bounded>());
static_assert(test_postfix_decrement_impl<test_verified_bounded, test_bounded>());

// =============================================================================
// Main - runtime tests for constexpr operations (comparisons, conversions)
// =============================================================================

int main()
{
    test_construction<verified_u8, u8>();
    test_construction<verified_u16, u16>();
    test_construction<verified_u32, u32>();
    test_construction<verified_u64, u64>();
    test_construction<verified_u128, u128>();

    test_conversions<verified_u8, u8>();
    test_conversions<verified_u16, u16>();
    test_conversions<verified_u32, u32>();
    test_conversions<verified_u64, u64>();
    test_conversions<verified_u128, u128>();

    test_comparisons<verified_u8, u8>();
    test_comparisons<verified_u16, u16>();
    test_comparisons<verified_u32, u32>();
    test_comparisons<verified_u64, u64>();
    test_comparisons<verified_u128, u128>();

    test_comparisons<test_verified_bounded, test_bounded>();

    return boost::report_errors();
}

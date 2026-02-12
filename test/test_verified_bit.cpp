// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>
#include <boost/safe_numbers/bit.hpp>
#include <cstdint>

#endif

#include <boost/core/lightweight_test.hpp>

using namespace boost::safe_numbers;

// =============================================================================
// has_single_bit (returns bool - runtime testable)
// =============================================================================

template <typename VerifiedT, typename BasisT>
void test_has_single_bit()
{
    BOOST_TEST_EQ(boost::safe_numbers::has_single_bit(VerifiedT{BasisT{0}}), false);
    BOOST_TEST_EQ(boost::safe_numbers::has_single_bit(VerifiedT{BasisT{1}}), true);
    BOOST_TEST_EQ(boost::safe_numbers::has_single_bit(VerifiedT{BasisT{2}}), true);
    BOOST_TEST_EQ(boost::safe_numbers::has_single_bit(VerifiedT{BasisT{3}}), false);
    BOOST_TEST_EQ(boost::safe_numbers::has_single_bit(VerifiedT{BasisT{4}}), true);
    BOOST_TEST_EQ(boost::safe_numbers::has_single_bit(VerifiedT{BasisT{42}}), false);
    BOOST_TEST_EQ(boost::safe_numbers::has_single_bit(VerifiedT{BasisT{64}}), true);
    BOOST_TEST_EQ(boost::safe_numbers::has_single_bit(VerifiedT{BasisT{128}}), true);
}

// =============================================================================
// bit_width (returns int - runtime testable)
// =============================================================================

template <typename VerifiedT, typename BasisT>
void test_bit_width()
{
    // Compare verified result against basis type result
    BOOST_TEST_EQ(boost::safe_numbers::bit_width(VerifiedT{BasisT{0}}),
                  boost::safe_numbers::bit_width(BasisT{0}));
    BOOST_TEST_EQ(boost::safe_numbers::bit_width(VerifiedT{BasisT{1}}),
                  boost::safe_numbers::bit_width(BasisT{1}));
    BOOST_TEST_EQ(boost::safe_numbers::bit_width(VerifiedT{BasisT{42}}),
                  boost::safe_numbers::bit_width(BasisT{42}));
    BOOST_TEST_EQ(boost::safe_numbers::bit_width(VerifiedT{BasisT{255}}),
                  boost::safe_numbers::bit_width(BasisT{255}));
}

// =============================================================================
// countl_zero (returns int - runtime testable)
// =============================================================================

template <typename VerifiedT, typename BasisT>
void test_countl_zero()
{
    BOOST_TEST_EQ(boost::safe_numbers::countl_zero(VerifiedT{BasisT{0}}),
                  boost::safe_numbers::countl_zero(BasisT{0}));
    BOOST_TEST_EQ(boost::safe_numbers::countl_zero(VerifiedT{BasisT{1}}),
                  boost::safe_numbers::countl_zero(BasisT{1}));
    BOOST_TEST_EQ(boost::safe_numbers::countl_zero(VerifiedT{BasisT{42}}),
                  boost::safe_numbers::countl_zero(BasisT{42}));
    BOOST_TEST_EQ(boost::safe_numbers::countl_zero(VerifiedT{BasisT{128}}),
                  boost::safe_numbers::countl_zero(BasisT{128}));
}

// =============================================================================
// countl_one (returns int - runtime testable)
// =============================================================================

template <typename VerifiedT, typename BasisT>
void test_countl_one()
{
    BOOST_TEST_EQ(boost::safe_numbers::countl_one(VerifiedT{BasisT{0}}),
                  boost::safe_numbers::countl_one(BasisT{0}));
    BOOST_TEST_EQ(boost::safe_numbers::countl_one(VerifiedT{BasisT{1}}),
                  boost::safe_numbers::countl_one(BasisT{1}));
    BOOST_TEST_EQ(boost::safe_numbers::countl_one(VerifiedT{BasisT{42}}),
                  boost::safe_numbers::countl_one(BasisT{42}));
    BOOST_TEST_EQ(boost::safe_numbers::countl_one(VerifiedT{BasisT{255}}),
                  boost::safe_numbers::countl_one(BasisT{255}));
}

// =============================================================================
// countr_zero (returns int - runtime testable)
// =============================================================================

template <typename VerifiedT, typename BasisT>
void test_countr_zero()
{
    BOOST_TEST_EQ(boost::safe_numbers::countr_zero(VerifiedT{BasisT{0}}),
                  boost::safe_numbers::countr_zero(BasisT{0}));
    BOOST_TEST_EQ(boost::safe_numbers::countr_zero(VerifiedT{BasisT{1}}),
                  boost::safe_numbers::countr_zero(BasisT{1}));
    BOOST_TEST_EQ(boost::safe_numbers::countr_zero(VerifiedT{BasisT{42}}),
                  boost::safe_numbers::countr_zero(BasisT{42}));
    BOOST_TEST_EQ(boost::safe_numbers::countr_zero(VerifiedT{BasisT{128}}),
                  boost::safe_numbers::countr_zero(BasisT{128}));
}

// =============================================================================
// countr_one (returns int - runtime testable)
// =============================================================================

template <typename VerifiedT, typename BasisT>
void test_countr_one()
{
    BOOST_TEST_EQ(boost::safe_numbers::countr_one(VerifiedT{BasisT{0}}),
                  boost::safe_numbers::countr_one(BasisT{0}));
    BOOST_TEST_EQ(boost::safe_numbers::countr_one(VerifiedT{BasisT{1}}),
                  boost::safe_numbers::countr_one(BasisT{1}));
    BOOST_TEST_EQ(boost::safe_numbers::countr_one(VerifiedT{BasisT{42}}),
                  boost::safe_numbers::countr_one(BasisT{42}));
    BOOST_TEST_EQ(boost::safe_numbers::countr_one(VerifiedT{BasisT{255}}),
                  boost::safe_numbers::countr_one(BasisT{255}));
}

// =============================================================================
// popcount (returns int - runtime testable)
// =============================================================================

template <typename VerifiedT, typename BasisT>
void test_popcount()
{
    BOOST_TEST_EQ(boost::safe_numbers::popcount(VerifiedT{BasisT{0}}),
                  boost::safe_numbers::popcount(BasisT{0}));
    BOOST_TEST_EQ(boost::safe_numbers::popcount(VerifiedT{BasisT{1}}),
                  boost::safe_numbers::popcount(BasisT{1}));
    BOOST_TEST_EQ(boost::safe_numbers::popcount(VerifiedT{BasisT{42}}),
                  boost::safe_numbers::popcount(BasisT{42}));
    BOOST_TEST_EQ(boost::safe_numbers::popcount(VerifiedT{BasisT{255}}),
                  boost::safe_numbers::popcount(BasisT{255}));
}

// =============================================================================
// bit_ceil (returns UnsignedInt - compile-time only for verified types)
// =============================================================================

template <typename VerifiedT, typename BasisT>
consteval auto test_bit_ceil_impl() -> bool
{
    return boost::safe_numbers::bit_ceil(VerifiedT{BasisT{1}}) == VerifiedT{BasisT{1}}
        && boost::safe_numbers::bit_ceil(VerifiedT{BasisT{2}}) == VerifiedT{BasisT{2}}
        && boost::safe_numbers::bit_ceil(VerifiedT{BasisT{3}}) == VerifiedT{BasisT{4}}
        && boost::safe_numbers::bit_ceil(VerifiedT{BasisT{5}}) == VerifiedT{BasisT{8}}
        && boost::safe_numbers::bit_ceil(VerifiedT{BasisT{42}}) == VerifiedT{BasisT{64}}
        && boost::safe_numbers::bit_ceil(VerifiedT{BasisT{128}}) == VerifiedT{BasisT{128}};
}

static_assert(test_bit_ceil_impl<verified_u8, u8>());
static_assert(test_bit_ceil_impl<verified_u16, u16>());
static_assert(test_bit_ceil_impl<verified_u32, u32>());
static_assert(test_bit_ceil_impl<verified_u64, u64>());
static_assert(test_bit_ceil_impl<verified_u128, u128>());

// =============================================================================
// bit_floor (returns UnsignedInt - compile-time only for verified types)
// =============================================================================

template <typename VerifiedT, typename BasisT>
consteval auto test_bit_floor_impl() -> bool
{
    return boost::safe_numbers::bit_floor(VerifiedT{BasisT{0}}) == VerifiedT{BasisT{0}}
        && boost::safe_numbers::bit_floor(VerifiedT{BasisT{1}}) == VerifiedT{BasisT{1}}
        && boost::safe_numbers::bit_floor(VerifiedT{BasisT{3}}) == VerifiedT{BasisT{2}}
        && boost::safe_numbers::bit_floor(VerifiedT{BasisT{7}}) == VerifiedT{BasisT{4}}
        && boost::safe_numbers::bit_floor(VerifiedT{BasisT{42}}) == VerifiedT{BasisT{32}}
        && boost::safe_numbers::bit_floor(VerifiedT{BasisT{128}}) == VerifiedT{BasisT{128}};
}

static_assert(test_bit_floor_impl<verified_u8, u8>());
static_assert(test_bit_floor_impl<verified_u16, u16>());
static_assert(test_bit_floor_impl<verified_u32, u32>());
static_assert(test_bit_floor_impl<verified_u64, u64>());
static_assert(test_bit_floor_impl<verified_u128, u128>());

// =============================================================================
// rotl (returns UnsignedInt - compile-time only, non-bounded only)
// =============================================================================

template <typename VerifiedT, typename BasisT>
consteval auto test_rotl_impl() -> bool
{
    // rotl(1, 0) == 1
    // rotl(1, 1) == 2
    return boost::safe_numbers::rotl(VerifiedT{BasisT{1}}, 0) == VerifiedT{BasisT{1}}
        && boost::safe_numbers::rotl(VerifiedT{BasisT{1}}, 1) == VerifiedT{BasisT{2}};
}

static_assert(test_rotl_impl<verified_u8, u8>());
static_assert(test_rotl_impl<verified_u16, u16>());
static_assert(test_rotl_impl<verified_u32, u32>());
static_assert(test_rotl_impl<verified_u64, u64>());
static_assert(test_rotl_impl<verified_u128, u128>());

// =============================================================================
// rotr (returns UnsignedInt - compile-time only, non-bounded only)
// =============================================================================

template <typename VerifiedT, typename BasisT>
consteval auto test_rotr_impl() -> bool
{
    // rotr(2, 1) == 1
    return boost::safe_numbers::rotr(VerifiedT{BasisT{2}}, 0) == VerifiedT{BasisT{2}}
        && boost::safe_numbers::rotr(VerifiedT{BasisT{2}}, 1) == VerifiedT{BasisT{1}};
}

static_assert(test_rotr_impl<verified_u8, u8>());
static_assert(test_rotr_impl<verified_u16, u16>());
static_assert(test_rotr_impl<verified_u32, u32>());
static_assert(test_rotr_impl<verified_u64, u64>());
static_assert(test_rotr_impl<verified_u128, u128>());

// =============================================================================
// byteswap (returns UnsignedInt - compile-time only, non-bounded only)
// =============================================================================

template <typename VerifiedT, typename BasisT>
consteval auto test_byteswap_impl() -> bool
{
    // byteswap(0) == 0 for any width
    return boost::safe_numbers::byteswap(VerifiedT{BasisT{0}}) == VerifiedT{BasisT{0}};
}

static_assert(test_byteswap_impl<verified_u8, u8>());
static_assert(test_byteswap_impl<verified_u16, u16>());
static_assert(test_byteswap_impl<verified_u32, u32>());
static_assert(test_byteswap_impl<verified_u64, u64>());
static_assert(test_byteswap_impl<verified_u128, u128>());

// =============================================================================
// Main - runtime tests for functions returning int/bool
// =============================================================================

int main()
{
    test_has_single_bit<verified_u8, u8>();
    test_has_single_bit<verified_u16, u16>();
    test_has_single_bit<verified_u32, u32>();
    test_has_single_bit<verified_u64, u64>();
    test_has_single_bit<verified_u128, u128>();

    test_bit_width<verified_u8, u8>();
    test_bit_width<verified_u16, u16>();
    test_bit_width<verified_u32, u32>();
    test_bit_width<verified_u64, u64>();
    test_bit_width<verified_u128, u128>();

    test_countl_zero<verified_u8, u8>();
    test_countl_zero<verified_u16, u16>();
    test_countl_zero<verified_u32, u32>();
    test_countl_zero<verified_u64, u64>();
    test_countl_zero<verified_u128, u128>();

    test_countl_one<verified_u8, u8>();
    test_countl_one<verified_u16, u16>();
    test_countl_one<verified_u32, u32>();
    test_countl_one<verified_u64, u64>();
    test_countl_one<verified_u128, u128>();

    test_countr_zero<verified_u8, u8>();
    test_countr_zero<verified_u16, u16>();
    test_countr_zero<verified_u32, u32>();
    test_countr_zero<verified_u64, u64>();
    test_countr_zero<verified_u128, u128>();

    test_countr_one<verified_u8, u8>();
    test_countr_one<verified_u16, u16>();
    test_countr_one<verified_u32, u32>();
    test_countr_one<verified_u64, u64>();
    test_countr_one<verified_u128, u128>();

    test_popcount<verified_u8, u8>();
    test_popcount<verified_u16, u16>();
    test_popcount<verified_u32, u32>();
    test_popcount<verified_u64, u64>();
    test_popcount<verified_u128, u128>();

    return boost::report_errors();
}

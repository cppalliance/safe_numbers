// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Verifies that the saturating type-level policy behaves exactly like the
// saturating_* free functions on the default (throwing) types

#include <boost/core/lightweight_test.hpp>

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <cstdint>

#endif

using namespace boost::safe_numbers;

// Every operator result must match the equivalent saturating free function
// applied to the default policy type carrying the same values
void test_exhaustive_u8()
{
    for (std::uint32_t i {0U}; i <= 255U; ++i)
    {
        for (std::uint32_t j {0U}; j <= 255U; ++j)
        {
            const auto a {static_cast<std::uint8_t>(i)};
            const auto b {static_cast<std::uint8_t>(j)};

            const sat_u8 sa {a};
            const sat_u8 sb {b};
            const u8 ua {a};
            const u8 ub {b};

            BOOST_TEST_EQ(static_cast<std::uint8_t>(sa + sb), static_cast<std::uint8_t>(saturating_add(ua, ub)));
            BOOST_TEST_EQ(static_cast<std::uint8_t>(sa - sb), static_cast<std::uint8_t>(saturating_sub(ua, ub)));
            BOOST_TEST_EQ(static_cast<std::uint8_t>(sa * sb), static_cast<std::uint8_t>(saturating_mul(ua, ub)));

            if (b != 0U)
            {
                BOOST_TEST_EQ(static_cast<std::uint8_t>(sa / sb), static_cast<std::uint8_t>(saturating_div(ua, ub)));
                BOOST_TEST_EQ(static_cast<std::uint8_t>(sa % sb), static_cast<std::uint8_t>(saturating_mod(ua, ub)));
            }
        }
    }
}

void test_shifts_u8()
{
    for (std::uint32_t i {0U}; i <= 255U; ++i)
    {
        for (std::uint32_t j {0U}; j <= 9U; ++j)
        {
            const auto a {static_cast<std::uint8_t>(i)};
            const auto b {static_cast<std::uint8_t>(j)};

            const sat_u8 sa {a};
            const sat_u8 sb {b};
            const u8 ua {a};
            const u8 ub {b};

            BOOST_TEST_EQ(static_cast<std::uint8_t>(sa << sb), static_cast<std::uint8_t>(saturating_shl(ua, ub)));
            BOOST_TEST_EQ(static_cast<std::uint8_t>(sa >> sb), static_cast<std::uint8_t>(saturating_shr(ua, ub)));
        }
    }
}

template <typename SatType, typename DefaultType>
void test_spot_checks()
{
    using basis_type = typename SatType::basis_type;

    constexpr auto max_val {std::numeric_limits<basis_type>::max()};

    const SatType sa {max_val};
    const SatType sb {basis_type{2U}};
    const DefaultType ua {max_val};
    const DefaultType ub {basis_type{2U}};

    BOOST_TEST(static_cast<basis_type>(sa + sb) == static_cast<basis_type>(saturating_add(ua, ub)));
    BOOST_TEST(static_cast<basis_type>(sb - sa) == static_cast<basis_type>(saturating_sub(ub, ua)));
    BOOST_TEST(static_cast<basis_type>(sa * sb) == static_cast<basis_type>(saturating_mul(ua, ub)));
    BOOST_TEST(static_cast<basis_type>(sa / sb) == static_cast<basis_type>(saturating_div(ua, ub)));
    BOOST_TEST(static_cast<basis_type>(sa % sb) == static_cast<basis_type>(saturating_mod(ua, ub)));
}

void test_increment_decrement_bounds()
{
    sat_u8 at_max {255U};
    ++at_max;
    BOOST_TEST_EQ(static_cast<std::uint8_t>(at_max), 255U);

    const auto old_max {at_max++};
    BOOST_TEST_EQ(static_cast<std::uint8_t>(old_max), 255U);
    BOOST_TEST_EQ(static_cast<std::uint8_t>(at_max), 255U);

    sat_u8 at_min {0U};
    --at_min;
    BOOST_TEST_EQ(static_cast<std::uint8_t>(at_min), 0U);

    const auto old_min {at_min--};
    BOOST_TEST_EQ(static_cast<std::uint8_t>(old_min), 0U);
    BOOST_TEST_EQ(static_cast<std::uint8_t>(at_min), 0U);
}

void test_compound_assignment()
{
    sat_u8 val {250U};
    val += sat_u8{10U};
    BOOST_TEST_EQ(static_cast<std::uint8_t>(val), 255U);

    val -= sat_u8{5U};
    BOOST_TEST_EQ(static_cast<std::uint8_t>(val), 250U);

    val *= sat_u8{2U};
    BOOST_TEST_EQ(static_cast<std::uint8_t>(val), 255U);

    val /= sat_u8{5U};
    BOOST_TEST_EQ(static_cast<std::uint8_t>(val), 51U);

    val %= sat_u8{10U};
    BOOST_TEST_EQ(static_cast<std::uint8_t>(val), 1U);

    val <<= sat_u8{200U};
    BOOST_TEST_EQ(static_cast<std::uint8_t>(val), 255U);

    val >>= sat_u8{200U};
    BOOST_TEST_EQ(static_cast<std::uint8_t>(val), 0U);
}

// Division and modulo by zero still throw under the saturating policy,
// exactly like saturating_div and saturating_mod
void test_divide_by_zero_still_throws()
{
    const sat_u8 val {1U};
    const sat_u8 zero {0U};

    bool caught {false};
    try
    {
        const auto res {val / zero};
        static_cast<void>(res);
    }
    catch (const std::domain_error&)
    {
        caught = true;
    }
    BOOST_TEST(caught);

    caught = false;
    try
    {
        const auto res {val % zero};
        static_cast<void>(res);
    }
    catch (const std::domain_error&)
    {
        caught = true;
    }
    BOOST_TEST(caught);
}

// Saturating overflow is a defined value, so it must be usable at constant evaluation
void test_constexpr_saturation()
{
    constexpr sat_u8 over {sat_u8{255U} + sat_u8{1U}};
    static_assert(static_cast<std::uint8_t>(over) == 255U);

    constexpr sat_u8 under {sat_u8{0U} - sat_u8{1U}};
    static_assert(static_cast<std::uint8_t>(under) == 0U);

    constexpr sat_u8 mul_over {sat_u8{16U} * sat_u8{16U}};
    static_assert(static_cast<std::uint8_t>(mul_over) == 255U);

    constexpr sat_u8 shl_over {sat_u8{1U} << sat_u8{8U}};
    static_assert(static_cast<std::uint8_t>(shl_over) == 255U);

    // The generalized free functions saturate at constant evaluation too
    constexpr u8 free_over {saturating_add(u8{255U}, u8{1U})};
    static_assert(static_cast<std::uint8_t>(free_over) == 255U);

    BOOST_TEST(static_cast<std::uint8_t>(over) == 255U);
}

void test_noexcept_matrix()
{
    const sat_u8 s {1U};
    const u8 d {1U};

    static_assert(noexcept(s + s));
    static_assert(noexcept(s - s));
    static_assert(noexcept(s * s));
    static_assert(!noexcept(s / s));
    static_assert(!noexcept(s % s));
    static_assert(noexcept(s << s));
    static_assert(noexcept(s >> s));
    static_assert(noexcept(++sat_u8{0U}));
    static_assert(noexcept(--sat_u8{1U}));

    static_assert(!noexcept(d + d));
    static_assert(!noexcept(d - d));
    static_assert(!noexcept(d * d));
    static_assert(!noexcept(d / d));
    static_assert(!noexcept(d << d));

    static_cast<void>(s);
    static_cast<void>(d);
}

int main()
{
    test_exhaustive_u8();
    test_shifts_u8();

    test_spot_checks<sat_u16, u16>();
    test_spot_checks<sat_u32, u32>();
    test_spot_checks<sat_u64, u64>();
    test_spot_checks<sat_u128, u128>();

    test_increment_decrement_bounds();
    test_compound_assignment();
    test_divide_by_zero_still_throws();
    test_constexpr_saturation();
    test_noexcept_matrix();

    return boost::report_errors();
}

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Verifies that the saturating type-level policy behaves exactly like the
// saturating_* free functions on the default (throwing) signed types

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

void test_exhaustive_i8()
{
    for (std::int32_t i {-128}; i <= 127; ++i)
    {
        for (std::int32_t j {-128}; j <= 127; ++j)
        {
            const auto a {static_cast<std::int8_t>(i)};
            const auto b {static_cast<std::int8_t>(j)};

            const sat_i8 sa {a};
            const sat_i8 sb {b};
            const i8 ia {a};
            const i8 ib {b};

            BOOST_TEST_EQ(static_cast<std::int8_t>(sa + sb), static_cast<std::int8_t>(saturating_add(ia, ib)));
            BOOST_TEST_EQ(static_cast<std::int8_t>(sa - sb), static_cast<std::int8_t>(saturating_sub(ia, ib)));
            BOOST_TEST_EQ(static_cast<std::int8_t>(sa * sb), static_cast<std::int8_t>(saturating_mul(ia, ib)));

            if (b != 0)
            {
                BOOST_TEST_EQ(static_cast<std::int8_t>(sa / sb), static_cast<std::int8_t>(saturating_div(ia, ib)));
                BOOST_TEST_EQ(static_cast<std::int8_t>(sa % sb), static_cast<std::int8_t>(saturating_mod(ia, ib)));
            }
        }
    }
}

template <typename SatType, typename DefaultType>
void test_spot_checks()
{
    using basis_type = typename SatType::basis_type;

    constexpr auto max_val {std::numeric_limits<basis_type>::max()};
    constexpr auto min_val {std::numeric_limits<basis_type>::min()};

    const SatType smax {max_val};
    const SatType smin {min_val};
    const SatType stwo {basis_type{2}};
    const DefaultType dmax {max_val};
    const DefaultType dmin {min_val};
    const DefaultType dtwo {basis_type{2}};

    BOOST_TEST(static_cast<basis_type>(smax + stwo) == static_cast<basis_type>(saturating_add(dmax, dtwo)));
    BOOST_TEST(static_cast<basis_type>(smin - stwo) == static_cast<basis_type>(saturating_sub(dmin, dtwo)));
    BOOST_TEST(static_cast<basis_type>(smax * stwo) == static_cast<basis_type>(saturating_mul(dmax, dtwo)));

    // MIN / -1 saturates to max instead of throwing
    const SatType sneg_one {basis_type{-1}};
    const DefaultType dneg_one {basis_type{-1}};
    BOOST_TEST(static_cast<basis_type>(smin / sneg_one) == static_cast<basis_type>(saturating_div(dmin, dneg_one)));
    BOOST_TEST(static_cast<basis_type>(smin / sneg_one) == max_val);
    BOOST_TEST(static_cast<basis_type>(smin % sneg_one) == static_cast<basis_type>(saturating_mod(dmin, dneg_one)));
}

// Negating the minimum saturates to max, matching saturating_sub(0, min)
void test_unary_minus()
{
    const sat_i8 smin {std::int8_t{-128}};
    BOOST_TEST_EQ(static_cast<std::int8_t>(-smin), std::int8_t{127});
    BOOST_TEST_EQ(static_cast<std::int8_t>(-smin),
                  static_cast<std::int8_t>(saturating_sub(i8{std::int8_t{0}}, i8{std::int8_t{-128}})));

    const sat_i8 pos {std::int8_t{5}};
    BOOST_TEST_EQ(static_cast<std::int8_t>(-pos), std::int8_t{-5});
    BOOST_TEST_EQ(static_cast<std::int8_t>(+pos), std::int8_t{5});

    static_assert(noexcept(-sat_i8{std::int8_t{1}}));
    static_assert(!noexcept(-i8{std::int8_t{1}}));
}

void test_increment_decrement_bounds()
{
    sat_i8 at_max {std::int8_t{127}};
    ++at_max;
    BOOST_TEST_EQ(static_cast<std::int8_t>(at_max), std::int8_t{127});

    const auto old_max {at_max++};
    BOOST_TEST_EQ(static_cast<std::int8_t>(old_max), std::int8_t{127});
    BOOST_TEST_EQ(static_cast<std::int8_t>(at_max), std::int8_t{127});

    sat_i8 at_min {std::int8_t{-128}};
    --at_min;
    BOOST_TEST_EQ(static_cast<std::int8_t>(at_min), std::int8_t{-128});

    const auto old_min {at_min--};
    BOOST_TEST_EQ(static_cast<std::int8_t>(old_min), std::int8_t{-128});
    BOOST_TEST_EQ(static_cast<std::int8_t>(at_min), std::int8_t{-128});
}

void test_divide_by_zero_still_throws()
{
    const sat_i8 val {std::int8_t{1}};
    const sat_i8 zero {std::int8_t{0}};

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

void test_constexpr_saturation()
{
    constexpr sat_i8 over {sat_i8{std::int8_t{127}} + sat_i8{std::int8_t{1}}};
    static_assert(static_cast<std::int8_t>(over) == std::int8_t{127});

    constexpr sat_i8 under {sat_i8{std::int8_t{-128}} - sat_i8{std::int8_t{1}}};
    static_assert(static_cast<std::int8_t>(under) == std::int8_t{-128});

    constexpr sat_i8 mul_under {sat_i8{std::int8_t{-128}} * sat_i8{std::int8_t{2}}};
    static_assert(static_cast<std::int8_t>(mul_under) == std::int8_t{-128});

    constexpr i8 free_over {saturating_add(i8{std::int8_t{127}}, i8{std::int8_t{1}})};
    static_assert(static_cast<std::int8_t>(free_over) == std::int8_t{127});

    BOOST_TEST(static_cast<std::int8_t>(over) == std::int8_t{127});
}

void test_noexcept_matrix()
{
    const sat_i8 s {std::int8_t{1}};
    const i8 d {std::int8_t{1}};

    static_assert(noexcept(s + s));
    static_assert(noexcept(s - s));
    static_assert(noexcept(s * s));
    static_assert(!noexcept(s / s));
    static_assert(!noexcept(s % s));
    static_assert(noexcept(++sat_i8{std::int8_t{0}}));
    static_assert(noexcept(--sat_i8{std::int8_t{0}}));

    static_assert(!noexcept(d + d));
    static_assert(!noexcept(d - d));
    static_assert(!noexcept(d * d));
    static_assert(!noexcept(d / d));

    static_cast<void>(s);
    static_cast<void>(d);
}

int main()
{
    test_exhaustive_i8();

    test_spot_checks<sat_i16, i16>();
    test_spot_checks<sat_i32, i32>();
    test_spot_checks<sat_i64, i64>();
    test_spot_checks<sat_i128, i128>();

    test_unary_minus();
    test_increment_decrement_bounds();
    test_divide_by_zero_still_throws();
    test_constexpr_saturation();
    test_noexcept_matrix();

    return boost::report_errors();
}

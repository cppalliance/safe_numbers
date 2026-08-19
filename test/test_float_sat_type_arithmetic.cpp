// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Verifies that the saturating float policy is raw IEEE 754 arithmetic:
// bit for bit identical to the value component of the overflowing_* functions

#include <boost/core/lightweight_test.hpp>

// The saturating results compared here are exact IEEE 754 values
#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wfloat-equal"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>
#include <bit>
#include <cmath>
#include <compare>
#include <cstdint>
#include <limits>

#endif

using namespace boost::safe_numbers;

// Excess precision on 32-bit x87 makes value comparison of float results
// unreliable, so compare the stored bit patterns instead
template <typename T>
auto same_bits(const T lhs, const T rhs) noexcept -> bool
{
    if constexpr (sizeof(T) == sizeof(std::uint32_t))
    {
        return std::bit_cast<std::uint32_t>(lhs) == std::bit_cast<std::uint32_t>(rhs);
    }
    else
    {
        return std::bit_cast<std::uint64_t>(lhs) == std::bit_cast<std::uint64_t>(rhs);
    }
}

template <typename SatType, typename DefaultType>
void test_equivalence_with_overflowing()
{
    using basis_type = typename SatType::basis_type;

    constexpr basis_type interesting[] {
        basis_type{0},
        basis_type{-0.0},
        basis_type{1.5},
        basis_type{-2.5},
        std::numeric_limits<basis_type>::max(),
        std::numeric_limits<basis_type>::lowest(),
        std::numeric_limits<basis_type>::min(),
        std::numeric_limits<basis_type>::denorm_min(),
        std::numeric_limits<basis_type>::infinity(),
        -std::numeric_limits<basis_type>::infinity(),
        std::numeric_limits<basis_type>::quiet_NaN(),
        basis_type{3} * (std::numeric_limits<basis_type>::max() / basis_type{4}),
    };

    for (const auto a : interesting)
    {
        for (const auto b : interesting)
        {
            const SatType sa {a};
            const SatType sb {b};
            const DefaultType da {a};
            const DefaultType db {b};

            BOOST_TEST(same_bits(static_cast<basis_type>(sa + sb),
                                 static_cast<basis_type>(overflowing_add(da, db).first)));
            BOOST_TEST(same_bits(static_cast<basis_type>(sa - sb),
                                 static_cast<basis_type>(overflowing_sub(da, db).first)));
            BOOST_TEST(same_bits(static_cast<basis_type>(sa * sb),
                                 static_cast<basis_type>(overflowing_mul(da, db).first)));
            BOOST_TEST(same_bits(static_cast<basis_type>(sa / sb),
                                 static_cast<basis_type>(overflowing_div(da, db).first)));
        }
    }
}

void test_ieee_semantics()
{
    constexpr auto fmax {std::numeric_limits<float>::max()};

    // Overflow saturates to infinity
    const auto over {sat_f32{fmax} + sat_f32{fmax}};
    BOOST_TEST(std::isinf(static_cast<float>(over)));

    // Division by zero yields infinity
    const auto div_zero {sat_f32{1.0F} / sat_f32{0.0F}};
    BOOST_TEST(std::isinf(static_cast<float>(div_zero)));

    // NaN propagates
    const auto nan_res {sat_f32{std::numeric_limits<float>::quiet_NaN()} + sat_f32{1.0F}};
    BOOST_TEST(std::isnan(static_cast<float>(nan_res)));

    // inf - inf is NaN
    constexpr auto inf {std::numeric_limits<float>::infinity()};
    const auto invalid {sat_f32{inf} - sat_f32{inf}};
    BOOST_TEST(std::isnan(static_cast<float>(invalid)));
}

void test_comparisons()
{
    const sat_f32 one {1.0F};
    const sat_f32 two {2.0F};
    const sat_f32 nan_val {std::numeric_limits<float>::quiet_NaN()};

    BOOST_TEST(one < two);
    BOOST_TEST(one == one);
    BOOST_TEST((one <=> nan_val) == std::partial_ordering::unordered);
    BOOST_TEST(!(nan_val == nan_val));
}

void test_constexpr_saturate()
{
    constexpr sat_f32 finite {sat_f32{1.5F} + sat_f32{2.0F}};
    static_assert(static_cast<float>(finite) == 3.5F);

    constexpr sat_f64 product {sat_f64{3.0} * sat_f64{0.5}};
    static_assert(static_cast<double>(product) == 1.5);

    // Overflow to infinity at constant evaluation is accepted by Clang but
    // rejected by GCC, so it is exercised at runtime in test_ieee_semantics only

    BOOST_TEST(static_cast<float>(finite) == 3.5F);
}

void test_noexcept_matrix()
{
    const sat_f32 s {1.0F};
    const f32 d {1.0F};

    static_assert(noexcept(s + s));
    static_assert(noexcept(s - s));
    static_assert(noexcept(s * s));
    static_assert(noexcept(s / s));

    static_assert(!noexcept(d + d));
    static_assert(!noexcept(d - d));
    static_assert(!noexcept(d * d));
    static_assert(!noexcept(d / d));

    static_cast<void>(s);
    static_cast<void>(d);
}

int main()
{
    test_equivalence_with_overflowing<sat_f32, f32>();
    test_equivalence_with_overflowing<sat_f64, f64>();

    test_ieee_semantics();
    test_comparisons();
    test_constexpr_saturate();
    test_noexcept_matrix();

    return boost::report_errors();
}

#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#endif

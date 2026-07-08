// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_BOUNDED_FLOATS_HPP
#define BOOST_SAFE_NUMBERS_BOUNDED_FLOATS_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>
#include <boost/safe_numbers/detail/throw_exception.hpp>
#include <boost/safe_numbers/detail/float_basis.hpp>
#include <boost/safe_numbers/floats.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/throw_exception.hpp>
#include <concepts>
#include <compare>
#include <limits>
#include <stdexcept>
#include <type_traits>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

// bounded_float requires C++20 floating-point non-type template parameters (P1907R1).
// On compilers without that support (notably Clang 13-15) this header expands to
// nothing, and any reference to bounded_float will fail with a "no such type" error.
// Users can test for availability with BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT.
#if BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT

namespace boost::safe_numbers {

template <auto Min, auto Max>
    requires (detail::valid_float_bound<decltype(Min)> &&
              detail::valid_float_bound<decltype(Max)> &&
              std::is_same_v<decltype(Min), decltype(Max)> &&
              detail::float_raw_value(Min) == detail::float_raw_value(Min) &&
              detail::float_raw_value(Max) == detail::float_raw_value(Max) &&
              detail::float_raw_value(Max) > detail::float_raw_value(Min))
class bounded_float
{
public:

    // basis_type follows the bound's underlying type rather than fitting smallest type like for integers
    // This preserves the precision that the user may want
    using basis_type = std::conditional_t<
        std::is_same_v<detail::underlying_type_t<decltype(Min)>, float>,
        f32, f64>;

private:

    using underlying_type = detail::underlying_type_t<basis_type>;
    basis_type basis_ {};

public:

    BOOST_SAFE_NUMBERS_HOST_DEVICE explicit constexpr bounded_float(const basis_type val)
    {
        const auto raw {static_cast<underlying_type>(val)};

        // NaN comparisons are unordered: a naked range check would silently accept NaN.
        if (detail::impl::constexpr_isnan(raw))
        {
            #if !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))
            if (std::is_constant_evaluated())
            {
                throw std::domain_error("bounded_float NaN value"); // LCOV_EXCL_LINE
            }
            else
            #endif
            {
                BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_float NaN value");
            }
        }

        constexpr auto min_raw {static_cast<underlying_type>(detail::float_raw_value(Min))};
        constexpr auto max_raw {static_cast<underlying_type>(detail::float_raw_value(Max))};

        if (raw < min_raw || raw > max_raw)
        {
            #if !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))
            if (std::is_constant_evaluated())
            {
                throw std::domain_error("bounded_float value out of range"); // LCOV_EXCL_LINE
            }
            else
            #endif
            {
                BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_float value out of range");
            }
        }

        basis_ = val;
    }

    BOOST_SAFE_NUMBERS_HOST_DEVICE explicit constexpr bounded_float(const underlying_type val) : bounded_float{basis_type{val}} {}

    template <typename OtherBasis>
        requires (detail::is_compatible_float_type<OtherBasis>)
    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] explicit constexpr operator OtherBasis() const
    {
        const auto raw {static_cast<underlying_type>(basis_)};

        if constexpr (sizeof(OtherBasis) < sizeof(underlying_type))
        {
            const auto result {static_cast<OtherBasis>(raw)};

            if (detail::impl::constexpr_isinf(result) && !detail::impl::constexpr_isinf(raw))
            {
                #if !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))
                if (std::is_constant_evaluated())
                {
                    throw std::overflow_error("bounded_float narrowing conversion overflow"); // LCOV_EXCL_LINE
                }
                else
                #endif
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "bounded_float narrowing conversion overflow");
                }
            }

            return result;
        }
        else
        {
            return static_cast<OtherBasis>(raw);
        }
    }

    template <auto Min2, auto Max2>
    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] explicit constexpr operator bounded_float<Min2, Max2>() const
    {
        using target_basis = typename bounded_float<Min2, Max2>::basis_type;
        using target_underlying = detail::underlying_type_t<target_basis>;
        const auto raw {static_cast<underlying_type>(basis_)};
        return bounded_float<Min2, Max2>{target_basis{static_cast<target_underlying>(raw)}};
    }

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto to_basis() const noexcept -> basis_type { return basis_; }

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] friend constexpr auto operator==(bounded_float lhs, bounded_float rhs) noexcept -> bool = default;

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] friend constexpr auto operator<=>(bounded_float lhs, bounded_float rhs) noexcept
        -> std::partial_ordering = default;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator+=(bounded_float<Min, Max> rhs) -> bounded_float&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator-=(bounded_float<Min, Max> rhs) -> bounded_float&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator*=(bounded_float<Min, Max> rhs) -> bounded_float&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator/=(bounded_float<Min, Max> rhs) -> bounded_float&;
};

// ------------------------------
// Free-function arithmetic
// ------------------------------
//
// Each operator delegates to the underlying float_basis arithmetic, which already
// throws on IEEE 754 issues (overflow, underflow, NaN, invalid_op, divide_by_zero).
// The bounded_float constructor then re-validates the result against [Min, Max].

template <auto Min, auto Max>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto operator+(const bounded_float<Min, Max> lhs,
                                       const bounded_float<Min, Max> rhs) -> bounded_float<Min, Max>
{
    using basis = typename bounded_float<Min, Max>::basis_type;
    using underlying = detail::underlying_type_t<basis>;
    const basis lhs_b {static_cast<underlying>(lhs)};
    const basis rhs_b {static_cast<underlying>(rhs)};
    return bounded_float<Min, Max>{lhs_b + rhs_b};
}

template <auto Min, auto Max>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto operator-(const bounded_float<Min, Max> lhs,
                                       const bounded_float<Min, Max> rhs) -> bounded_float<Min, Max>
{
    using basis = typename bounded_float<Min, Max>::basis_type;
    using underlying = detail::underlying_type_t<basis>;
    const basis lhs_b {static_cast<underlying>(lhs)};
    const basis rhs_b {static_cast<underlying>(rhs)};
    return bounded_float<Min, Max>{lhs_b - rhs_b};
}

template <auto Min, auto Max>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto operator*(const bounded_float<Min, Max> lhs,
                                       const bounded_float<Min, Max> rhs) -> bounded_float<Min, Max>
{
    using basis = typename bounded_float<Min, Max>::basis_type;
    using underlying = detail::underlying_type_t<basis>;
    const basis lhs_b {static_cast<underlying>(lhs)};
    const basis rhs_b {static_cast<underlying>(rhs)};
    return bounded_float<Min, Max>{lhs_b * rhs_b};
}

template <auto Min, auto Max>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto operator/(const bounded_float<Min, Max> lhs,
                                       const bounded_float<Min, Max> rhs) -> bounded_float<Min, Max>
{
    using basis = typename bounded_float<Min, Max>::basis_type;
    using underlying = detail::underlying_type_t<basis>;
    const basis lhs_b {static_cast<underlying>(lhs)};
    const basis rhs_b {static_cast<underlying>(rhs)};
    return bounded_float<Min, Max>{lhs_b / rhs_b};
}

// ------------------------------
// Compound assignment
// ------------------------------

template <auto Min, auto Max>
    requires (detail::valid_float_bound<decltype(Min)> &&
              detail::valid_float_bound<decltype(Max)> &&
              std::is_same_v<decltype(Min), decltype(Max)> &&
              detail::float_raw_value(Min) == detail::float_raw_value(Min) &&
              detail::float_raw_value(Max) == detail::float_raw_value(Max) &&
              detail::float_raw_value(Max) > detail::float_raw_value(Min))
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto bounded_float<Min, Max>::operator+=(bounded_float<Min, Max> rhs) -> bounded_float&
{
    *this = *this + rhs;
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_float_bound<decltype(Min)> &&
              detail::valid_float_bound<decltype(Max)> &&
              std::is_same_v<decltype(Min), decltype(Max)> &&
              detail::float_raw_value(Min) == detail::float_raw_value(Min) &&
              detail::float_raw_value(Max) == detail::float_raw_value(Max) &&
              detail::float_raw_value(Max) > detail::float_raw_value(Min))
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto bounded_float<Min, Max>::operator-=(bounded_float<Min, Max> rhs) -> bounded_float&
{
    *this = *this - rhs;
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_float_bound<decltype(Min)> &&
              detail::valid_float_bound<decltype(Max)> &&
              std::is_same_v<decltype(Min), decltype(Max)> &&
              detail::float_raw_value(Min) == detail::float_raw_value(Min) &&
              detail::float_raw_value(Max) == detail::float_raw_value(Max) &&
              detail::float_raw_value(Max) > detail::float_raw_value(Min))
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto bounded_float<Min, Max>::operator*=(bounded_float<Min, Max> rhs) -> bounded_float&
{
    *this = *this * rhs;
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_float_bound<decltype(Min)> &&
              detail::valid_float_bound<decltype(Max)> &&
              std::is_same_v<decltype(Min), decltype(Max)> &&
              detail::float_raw_value(Min) == detail::float_raw_value(Min) &&
              detail::float_raw_value(Max) == detail::float_raw_value(Max) &&
              detail::float_raw_value(Max) > detail::float_raw_value(Min))
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto bounded_float<Min, Max>::operator/=(bounded_float<Min, Max> rhs) -> bounded_float&
{
    *this = *this / rhs;
    return *this;
}

} // namespace boost::safe_numbers

// Mixed-bounds blocking for bounded_float

#define BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_FLOAT_OP(OP_NAME, OP_SYMBOL)                                                            \
template <auto LHSMin, auto LHSMax, auto RHSMin, auto RHSMax>                                                                            \
    requires (LHSMin != RHSMin || LHSMax != RHSMax)                                                                                      \
constexpr auto OP_SYMBOL(const boost::safe_numbers::bounded_float<LHSMin, LHSMax>,                                                       \
                         const boost::safe_numbers::bounded_float<RHSMin, RHSMax>)                                                       \
{                                                                                                                                         \
    static_assert(boost::safe_numbers::detail::dependent_false<                                                                          \
                      boost::safe_numbers::bounded_float<LHSMin, LHSMax>,                                                                \
                      boost::safe_numbers::bounded_float<RHSMin, RHSMax>>,                                                               \
                  "Can not perform " OP_NAME " between bounded_float types with different bounds. "                                      \
                  "Both operands must have the same Min and Max.");                                                                      \
                                                                                                                                          \
    return boost::safe_numbers::bounded_float<LHSMin, LHSMax>(                                                                           \
        typename boost::safe_numbers::bounded_float<LHSMin, LHSMax>::basis_type{0});                                                     \
}

namespace boost::safe_numbers {

BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_FLOAT_OP("comparison", operator<=>)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_FLOAT_OP("equality", operator==)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_FLOAT_OP("addition", operator+)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_FLOAT_OP("subtraction", operator-)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_FLOAT_OP("multiplication", operator*)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_FLOAT_OP("division", operator/)

} // namespace boost::safe_numbers

#undef BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_FLOAT_OP

#endif // BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT

#endif // BOOST_SAFE_NUMBERS_BOUNDED_FLOATS_HPP

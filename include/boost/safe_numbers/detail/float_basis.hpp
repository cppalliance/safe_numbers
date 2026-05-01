// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BASIS_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BASIS_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>
#include <boost/safe_numbers/detail/throw_exception.hpp>
#include <boost/safe_numbers/overflow_policy.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/core/bit.hpp>
#include <concepts>
#include <compare>
#include <limits>
#include <stdexcept>
#include <cstdint>
#include <cstdlib>
#include <utility>
#include <optional>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers::detail {

template <compatible_float_type BasisType>
class float_basis
{
public:

    // This is exposed to the user so that they can convert back to built-in
    using basis_type = BasisType;

private:

    BasisType basis_ {};

public:

    constexpr float_basis() noexcept = default;

    BOOST_SAFE_NUMBERS_HOST_DEVICE explicit constexpr float_basis(const BasisType val) noexcept : basis_{val} {}

    // Reject construction from any non-basis type
    // This eliminates any potential narrowing
    template <typename T>
        requires (!std::is_same_v<T, BasisType>)
    BOOST_SAFE_NUMBERS_HOST_DEVICE explicit constexpr float_basis(T) noexcept = delete;

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] explicit constexpr operator BasisType() const noexcept { return basis_; }

    // Defined manually so -Wfloat-equal stays contained here rather than
    // leaking into user code through a defaulted operator==.
    // We want the behavior to generally match the built-ins,
    // outside of the exceptional cases
    #ifdef __clang__
    #  pragma clang diagnostic push
    #  pragma clang diagnostic ignored "-Wfloat-equal"
    #elif defined(__GNUC__)
    #  pragma GCC diagnostic push
    #  pragma GCC diagnostic ignored "-Wfloat-equal"
    #endif

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] friend constexpr auto operator==(float_basis lhs, float_basis rhs) noexcept -> bool
    {
        return lhs.basis_ == rhs.basis_;
    }

    #ifdef __clang__
    #  pragma clang diagnostic pop
    #elif defined(__GNUC__)
    #  pragma GCC diagnostic pop
    #endif

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] friend constexpr auto operator<=>(float_basis lhs, float_basis rhs) noexcept -> std::partial_ordering = default;
};

// Helper to map BasisType to a short name for diagnostic messages.
template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto float_type_name() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, float>)
    {
        return "f32";
    }
    else
    {
        return "f64";
    }
}

// Device-friendly error message helpers returning const char* string literals
// These avoid std::string concatenation which is not available on CUDA device

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto overflow_add_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, float>)
    {
        return "Overflow detected in f32 addition";
    }
    else
    {
        return "Overflow detected in f64 addition";
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto underflow_add_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, float>)
    {
        return "Underflow detected in f32 addition";
    }
    else
    {
        return "Underflow detected in f64 addition";
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto overflow_sub_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, float>)
    {
        return "Overflow detected in f32 subtraction";
    }
    else
    {
        return "Overflow detected in f64 subtraction";
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto underflow_sub_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, float>)
    {
        return "Underflow detected in f32 subtraction";
    }
    else
    {
        return "Underflow detected in f64 subtraction";
    }
}

} // namespace boost::safe_numbers::detail

#endif // BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BASIS_HPP

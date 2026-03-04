// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_SIGNED_INTEGER_BASIS_HPP
#define BOOST_SAFE_NUMBERS_SIGNED_INTEGER_BASIS_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>
#include <boost/safe_numbers/overflow_policy.hpp>
#include <boost/safe_numbers/detail/int128/bit.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/throw_exception.hpp>
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

template <fundamental_signed_integral BasisType>
class signed_integer_basis
{
public:

    // This is exposed to the user so that they can convert back to built-in
    using basis_type = BasisType;

private:

    BasisType basis_ {0U};

public:

    constexpr signed_integer_basis() noexcept = default;

    explicit constexpr signed_integer_basis(const BasisType val) noexcept : basis_{val} {}

    template <typename T>
        requires std::is_same_v<T, bool>
    explicit constexpr signed_integer_basis(T) noexcept
    {
        static_assert(dependent_false<T>, "Construction from bool is not allowed");
    }

    template <fundamental_signed_integral OtherBasis>
    [[nodiscard]] explicit constexpr operator OtherBasis() const;

    [[nodiscard]] explicit constexpr operator BasisType() const noexcept { return basis_; }
};

// Helper for diagnostic messages
template <typename BasisType>
constexpr auto signed_type_name() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::int8_t>)
    {
        return "i8";
    }
    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
    {
        return "i16";
    }
    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
    {
        return "i32";
    }
    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
    {
        return "i64";
    }
    else
    {
        return "i128";
    }
}

template<fundamental_signed_integral BasisType>
template<fundamental_signed_integral OtherBasis>
constexpr signed_integer_basis<BasisType>::operator OtherBasis() const
{
    if constexpr (sizeof(OtherBasis) < sizeof(BasisType))
    {
        if (basis_ > static_cast<BasisType>(std::numeric_limits<OtherBasis>::max()))
        {
            BOOST_THROW_EXCEPTION(std::domain_error(std::string("Overflow in ") + signed_type_name<BasisType>() + " to " + signed_type_name<OtherBasis>() + " conversion"));
        }
        else if (basis_ < static_cast<BasisType>(std::numeric_limits<OtherBasis>::min()))
        {
            BOOST_THROW_EXCEPTION(std::domain_error(std::string("Underflow in ") + signed_type_name<BasisType>() + " to " + signed_type_name<OtherBasis>() + " conversion"));
        }
    }

    return static_cast<OtherBasis>(basis_);
}

} // namespace boost::safe_numbers::detail

#endif //BOOST_SAFE_NUMBERS_SIGNED_INTEGER_BASIS_HPP

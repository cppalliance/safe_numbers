// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_VERIFIED_INTEGER_BASIS_HPP
#define BOOST_SAFE_NUMBERS_VERIFIED_INTEGER_BASIS_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>
#include <boost/safe_numbers/overflow_policy.hpp>
#include <boost/safe_numbers/unsigned_integers.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/throw_exception.hpp>
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

template <library_type BasisType>
class verified_type_basis
{
private:

    using underlying_type = underlying_type_t<BasisType>;

    BasisType basis_ {};

public:

    explicit consteval verified_type_basis(const BasisType basis) : basis_{basis} {}

    explicit consteval verified_type_basis(const underlying_type val) : verified_type_basis{BasisType{val}} {}

    [[nodiscard]] explicit constexpr operator BasisType() const noexcept { return basis_; }

    [[nodiscard]] explicit constexpr operator underlying_type() const noexcept { return static_cast<underlying_type>(basis_); }

    [[nodiscard]] friend constexpr auto operator<=>(verified_type_basis lhs, verified_type_basis rhs) noexcept
        -> std::strong_ordering = default;

    consteval auto operator+=(verified_type_basis rhs) -> verified_type_basis&;

    consteval auto operator-=(verified_type_basis rhs) -> verified_type_basis&;

    consteval auto operator*=(verified_type_basis rhs) -> verified_type_basis&;

    consteval auto operator/=(verified_type_basis rhs) -> verified_type_basis&;

    consteval auto operator%=(verified_type_basis rhs) -> verified_type_basis&;

    consteval auto operator++() -> verified_type_basis&;

    consteval auto operator++(int) -> verified_type_basis;

    consteval auto operator--() -> verified_type_basis&;

    consteval auto operator--(int) -> verified_type_basis;
};

template <library_type BasisType>
[[nodiscard]] consteval auto operator+(const verified_type_basis<BasisType> lhs,
                                       const verified_type_basis<BasisType> rhs) -> verified_type_basis<BasisType>
{
    return verified_type_basis<BasisType>{static_cast<BasisType>(lhs) + static_cast<BasisType>(rhs)};
}

template <library_type BasisType>
[[nodiscard]] consteval auto operator-(const verified_type_basis<BasisType> lhs,
                                       const verified_type_basis<BasisType> rhs) -> verified_type_basis<BasisType>
{
    return verified_type_basis<BasisType>{static_cast<BasisType>(lhs) - static_cast<BasisType>(rhs)};
}

template <library_type BasisType>
[[nodiscard]] consteval auto operator*(const verified_type_basis<BasisType> lhs,
                                       const verified_type_basis<BasisType> rhs) -> verified_type_basis<BasisType>
{
    return verified_type_basis<BasisType>{static_cast<BasisType>(lhs) * static_cast<BasisType>(rhs)};
}

template <library_type BasisType>
[[nodiscard]] consteval auto operator/(const verified_type_basis<BasisType> lhs,
                                       const verified_type_basis<BasisType> rhs) -> verified_type_basis<BasisType>
{
    return verified_type_basis<BasisType>{static_cast<BasisType>(lhs) / static_cast<BasisType>(rhs)};
}

template <library_type BasisType>
[[nodiscard]] consteval auto operator%(const verified_type_basis<BasisType> lhs,
                                       const verified_type_basis<BasisType> rhs) -> verified_type_basis<BasisType>
{
    return verified_type_basis<BasisType>{static_cast<BasisType>(lhs) % static_cast<BasisType>(rhs)};
}

template <library_type BasisType>
consteval auto verified_type_basis<BasisType>::operator+=(verified_type_basis rhs) -> verified_type_basis&
{
    *this = *this + rhs;
    return *this;
}

template <library_type BasisType>
consteval auto verified_type_basis<BasisType>::operator-=(verified_type_basis rhs) -> verified_type_basis&
{
    *this = *this - rhs;
    return *this;
}

template <library_type BasisType>
consteval auto verified_type_basis<BasisType>::operator*=(verified_type_basis rhs) -> verified_type_basis&
{
    *this = *this * rhs;
    return *this;
}

template <library_type BasisType>
consteval auto verified_type_basis<BasisType>::operator/=(verified_type_basis rhs) -> verified_type_basis&
{
    *this = *this / rhs;
    return *this;
}

template <library_type BasisType>
consteval auto verified_type_basis<BasisType>::operator%=(verified_type_basis rhs) -> verified_type_basis&
{
    *this = *this % rhs;
    return *this;
}

template <library_type BasisType>
consteval auto verified_type_basis<BasisType>::operator++() -> verified_type_basis&
{
    ++basis_;
    return *this;
}

template <library_type BasisType>
consteval auto verified_type_basis<BasisType>::operator++(int) -> verified_type_basis
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

template <library_type BasisType>
consteval auto verified_type_basis<BasisType>::operator--() -> verified_type_basis&
{
    --basis_;
    return *this;
}

template <library_type BasisType>
consteval auto verified_type_basis<BasisType>::operator--(int) -> verified_type_basis
{
    auto tmp = *this;
    --(*this);
    return tmp;
}

} // namespace boost::safe_numbers::detail

#endif // BOOST_SAFE_NUMBERS_VERIFIED_INTEGER_BASIS_HPP

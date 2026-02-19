// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_NUMERIC_HPP
#define BOOST_SAFE_NUMBERS_NUMERIC_HPP

#include <boost/safe_numbers/detail/verified_type_basis.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>
#include <boost/safe_numbers/detail/int128/numeric.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <numeric>

#endif

namespace boost::safe_numbers {

template <detail::non_bounded_integral_library_type T>
    requires (!detail::is_verified_type_v<T>)
constexpr auto gcd(const T m, const T n) noexcept -> T
{
    using underlying_type = detail::underlying_type_t<T>;

    if constexpr (std::is_same_v<underlying_type, int128::uint128_t> || std::is_same_v<underlying_type, int128::int128_t>)
    {
        return T{int128::gcd(static_cast<underlying_type>(m), static_cast<underlying_type>(n))};
    }
    else
    {
        return T{static_cast<underlying_type>(std::gcd(static_cast<underlying_type>(m), static_cast<underlying_type>(n)))};
    }
}

template <detail::non_bounded_integral_library_type T>
consteval auto gcd(const detail::verified_type_basis<T> m, const detail::verified_type_basis<T> n) noexcept -> detail::verified_type_basis<T>
{
    using underlying_type = detail::underlying_type_t<T>;
    using return_type = detail::verified_type_basis<T>;

    if constexpr (std::is_same_v<underlying_type, int128::uint128_t> || std::is_same_v<underlying_type, int128::int128_t>)
    {
        return return_type{T{int128::gcd(static_cast<underlying_type>(m), static_cast<underlying_type>(n))}};
    }
    else
    {
        return return_type{T{static_cast<underlying_type>(std::gcd(static_cast<underlying_type>(m), static_cast<underlying_type>(n)))}};
    }
}

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_NUMERIC_HPP

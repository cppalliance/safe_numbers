// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_NUMERIC_HPP
#define BOOST_SAFE_NUMBERS_NUMERIC_HPP

#include <boost/safe_numbers/detail/type_traits.hpp>
#include <boost/safe_numbers/detail/int128/numeric.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <numeric>

#endif

namespace boost::safe_numbers {

template <detail::non_bounded_integral_library_type T>
[[nodiscard]] constexpr auto gcd(const T m, const T n) noexcept -> T
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
[[nodiscard]] constexpr auto lcm(const T m, const T n) noexcept -> T
{
    using underlying_type = detail::underlying_type_t<T>;

    if constexpr (std::is_same_v<underlying_type, int128::uint128_t> || std::is_same_v<underlying_type, int128::int128_t>)
    {
        return T{int128::lcm(static_cast<underlying_type>(m), static_cast<underlying_type>(n))};
    }
    else
    {
        return T{static_cast<underlying_type>(std::lcm(static_cast<underlying_type>(m), static_cast<underlying_type>(n)))};
    }
}

template <detail::non_bounded_integral_library_type T>
[[nodiscard]] constexpr auto midpoint(const T a, const T b) noexcept -> T
{
    using underlying_type = detail::underlying_type_t<T>;

    if constexpr (std::is_same_v<underlying_type, int128::uint128_t> || std::is_same_v<underlying_type, int128::int128_t>)
    {
        return T{int128::midpoint(static_cast<underlying_type>(a), static_cast<underlying_type>(b))};
    }
    else
    {
        return T{static_cast<underlying_type>(std::midpoint(static_cast<underlying_type>(a), static_cast<underlying_type>(b)))};
    }
}

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_NUMERIC_HPP

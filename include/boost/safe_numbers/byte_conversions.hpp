// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_BYTE_CONVERSIONS_HPP
#define BOOST_SAFE_NUMBERS_BYTE_CONVERSIONS_HPP

#include <boost/safe_numbers/bit.hpp>
#include <boost/safe_numbers/verified_integers.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <bit>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers {

template <detail::non_bounded_integral_library_type T>
    requires (!detail::is_verified_type_v<T>)
constexpr auto to_be(const T value) noexcept -> T
{
    if constexpr (std::endian::native == std::endian::big)
    {
        return value;
    }
    else
    {
        return byteswap(value);
    }
}

template <detail::non_bounded_integral_library_type T>
consteval auto to_be(const detail::verified_type_basis<T> value) noexcept -> detail::verified_type_basis<T>
{
    if constexpr (std::endian::native == std::endian::big)
    {
        return value;
    }
    else
    {
        return byteswap(value);
    }
}

template <detail::non_bounded_integral_library_type T>
    requires (!detail::is_verified_type_v<T>)
constexpr auto from_be(const T value) noexcept -> T
{
    // Self-inverse
    return to_be(value);
}

template <detail::non_bounded_integral_library_type T>
consteval auto from_be(const detail::verified_type_basis<T> value) noexcept -> detail::verified_type_basis<T>
{
    // Self-inverse
    return to_be(value);
}

} // namespace boost::safe_numbers

#endif //BOOST_SAFE_NUMBERS_BYTE_CONVERSIONS_HPP

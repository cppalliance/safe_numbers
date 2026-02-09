// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_FORMAT_HPP
#define BOOST_SAFE_NUMBERS_FORMAT_HPP

#include <boost/safe_numbers/detail/unsigned_integer_basis.hpp>
#include <boost/safe_numbers/detail/int128/format.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_FORMAT

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <format>
#include <concepts>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

template <boost::safe_numbers::detail::library_type T>
struct std::formatter<T>
    : std::formatter<boost::safe_numbers::detail::underlying_type_t<T>>
{
    using underlying_type = boost::safe_numbers::detail::underlying_type_t<T>;

    auto format(const T& val, std::format_context& ctx) const
    {
        return std::formatter<underlying_type>::format(static_cast<underlying_type>(val), ctx);
    }
};

#endif // Has format

#endif // BOOST_SAFE_NUMBERS_FORMAT_HPP

// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_FMT_FORMAT_HPP
#define BOOST_SAFE_NUMBERS_FMT_FORMAT_HPP

#include <boost/safe_numbers/detail/unsigned_integer_basis.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>
#include <boost/safe_numbers/detail/int128/fmt_format.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <fmt/format.h>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

template <boost::safe_numbers::detail::library_type T>
struct fmt::formatter<T>
    : fmt::formatter<boost::safe_numbers::detail::underlying_type_t<T>>
{
    using underlying_type = boost::safe_numbers::detail::underlying_type_t<T>;

    auto format(const T& val, fmt::format_context& ctx) const
    {
        return fmt::formatter<underlying_type>::format(static_cast<underlying_type>(val), ctx);
    }
};

#endif // BOOST_SAFE_NUMBERS_FMT_FORMAT_HPP

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_CHARCONV_HPP
#define BOOST_SAFE_NUMBERS_CHARCONV_HPP

#include <boost/safe_numbers/detail/type_traits.hpp>
#include <boost/safe_numbers/detail/int128/charconv.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/charconv.hpp>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::charconv {

template <safe_numbers::detail::library_type T>
    requires (!safe_numbers::detail::is_verified_type_v<T>)
constexpr auto from_chars(const char* first, const char* last, T& value, int base = 10)
    -> charconv::from_chars_result
{
    using underlying_type = safe_numbers::detail::underlying_type_t<T>;

    underlying_type result {};
    const auto r {charconv::from_chars(first, last, result, base)};
    value = T{result};

    return r;
}

template <safe_numbers::detail::library_type T>
    requires safe_numbers::detail::is_verified_type_v<T>
consteval auto from_chars(const char* first, const char* last, T& value, int base = 10)
    -> charconv::from_chars_result
{
    using underlying_type = safe_numbers::detail::underlying_type_t<T>;

    underlying_type result {};
    const auto r {charconv::from_chars(first, last, result, base)};
    value = T{result};

    return r;
}

template <safe_numbers::detail::library_type T>
constexpr auto to_chars(char* first, char* last, const T value, int base = 10)
    -> charconv::to_chars_result
{
    using underlying_type = safe_numbers::detail::underlying_type_t<T>;
    return charconv::to_chars(first, last, static_cast<underlying_type>(value), base);
}

} // namespace boost::charconv

#endif // BOOST_SAFE_NUMBERS_CHARCONV_HPP

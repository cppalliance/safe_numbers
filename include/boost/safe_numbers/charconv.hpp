// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_CHARCONV_HPP
#define BOOST_SAFE_NUMBERS_CHARCONV_HPP

#include <boost/safe_numbers/detail/unsigned_integer_basis.hpp>
#include <boost/safe_numbers/detail/concepts.hpp>
#include <boost/safe_numbers/detail/int128/charconv.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/charconv.hpp>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers {

template <detail::unsigned_integral BasisType>
constexpr auto from_chars(const char* first, const char* last, detail::unsigned_integer_basis<BasisType>& value, int base = 10)
    -> charconv::from_chars_result
{
    BasisType result {};
    const auto r {charconv::from_chars(first, last, result, base)};
    value = detail::unsigned_integer_basis<BasisType>{result};

    return r;
}

template <detail::unsigned_integral BasisType>
constexpr auto to_chars(char* first, char* last, const detail::unsigned_integer_basis<BasisType> value, int base = 10)
    -> charconv::to_chars_result
{
    return charconv::to_chars(first, last, static_cast<BasisType>(value), base);
}

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_CHARCONV_HPP

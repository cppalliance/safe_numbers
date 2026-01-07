// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_CHARCONV_HPP
#define BOOST_SAFE_NUMBERS_CHARCONV_HPP

#include <boost/safe_numbers/detail/unsigned_integer_basis.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <charconv>
#include <concepts>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

#if defined(__cpp_lib_constexpr_charconv) && __cpp_lib_constexpr_charconv >= 202207L
#  define BOOST_SAFE_NUMBERS_CONSTEXPR_CHARCONV constexpr
#else
#  define BOOST_SAFE_NUMBERS_CONSTEXPR_CHARCONV
#endif

namespace boost::safe_numbers {

template <std::unsigned_integral BasisType>
BOOST_SAFE_NUMBERS_CONSTEXPR_CHARCONV
auto from_chars(const char* first, const char* last, detail::unsigned_integer_basis<BasisType>& value, int base = 10)
    -> std::from_chars_result
{
    BasisType result {};
    const auto r {std::from_chars(first, last, result, base)};
    value = detail::unsigned_integer_basis<BasisType>{result};

    return r;
}

template <std::unsigned_integral BasisType>
BOOST_SAFE_NUMBERS_CONSTEXPR_CHARCONV
auto to_chars(char* first, char* last, const detail::unsigned_integer_basis<BasisType> value, int base = 10)
    -> std::to_chars_result
{
    return std::to_chars(first, last, static_cast<BasisType>(value), base);
}

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_CHARCONV_HPP

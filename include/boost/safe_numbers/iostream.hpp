// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFENUMBERS_IOSTREAM_HPP
#define BOOST_SAFENUMBERS_IOSTREAM_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <concepts>

#endif // ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers {

BOOST_SAFE_NUMBERS_EXPORT template <typename charT, typename traits, detail::library_type LibType>
auto operator>>(std::basic_istream<charT, traits>& is, LibType& v)
{
    using underlying_type = detail::underlying_type_t<LibType>::type;

    underlying_type temp;
    is >> temp;
    v = temp;

    return is;
}

BOOST_SAFE_NUMBERS_EXPORT template <typename charT, typename traits, detail::library_type LibType>
auto operator<<(std::basic_ostream<charT, traits>& os, const LibType& v)
{
    using underlying_type = detail::underlying_type_t<LibType>::type;

    const auto temp {static_cast<underlying_type>(v)};
    os << temp;

    return os;
}

}  // namespace boost::safe_numbers

#endif // BOOST_SAFENUMBERS_IOSTREAM_HPP

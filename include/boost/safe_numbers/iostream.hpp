// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_IOSTREAM_HPP
#define BOOST_SAFE_NUMBERS_IOSTREAM_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/throw_exception.hpp>
#include <iostream>

#endif // ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers::detail {

BOOST_SAFE_NUMBERS_EXPORT template <typename charT, typename traits, library_type LibType>
auto operator>>(std::basic_istream<charT, traits>& is, LibType& v) -> std::basic_istream<charT, traits>&
{
    using underlying_type = underlying_type_t<LibType>;

    if (is.peek() == static_cast<charT>('-'))
    {
        BOOST_THROW_EXCEPTION(std::domain_error("Attempting to construct negative value with unsigned safe integer"));
        v = std::numeric_limits<LibType>::max();
    }
    else
    {
        if constexpr (std::is_same_v<underlying_type, std::uint8_t>)
        {
            std::uint32_t temp;
            is >> temp;
            v = static_cast<LibType>(static_cast<std::uint8_t>(temp));
        }
        else
        {
            underlying_type temp;
            is >> temp;
            v = static_cast<LibType>(temp);
        }
    }

    return is;
}

BOOST_SAFE_NUMBERS_EXPORT template <typename charT, typename traits, library_type LibType>
auto operator<<(std::basic_ostream<charT, traits>& os, const LibType& v) -> std::basic_ostream<charT, traits>&
{
    using underlying_type = underlying_type_t<LibType>;

    const auto temp {static_cast<underlying_type>(v)};

    if constexpr (std::is_same_v<underlying_type, std::uint8_t>)
    {
        // Display the value not the underlying representation like a carriage return
        os << static_cast<std::uint32_t>(temp);
    }
    else
    {
        os << temp;
    }

    return os;
}

}  // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_IOSTREAM_HPP

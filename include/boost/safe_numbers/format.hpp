// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_FORMAT_HPP
#define BOOST_SAFE_NUMBERS_FORMAT_HPP

#include <boost/safe_numbers/detail/unsigned_integer_basis.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <format>
#include <concepts>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

template <std::unsigned_integral BasisType>
struct std::formatter<boost::safe_numbers::detail::unsigned_integer_basis<BasisType>>
    : std::formatter<BasisType>
{
    auto format(const boost::safe_numbers::detail::unsigned_integer_basis<BasisType>& val,
                std::format_context& ctx) const
    {
        return std::formatter<BasisType>::format(static_cast<BasisType>(val), ctx);
    }
};

#endif // BOOST_SAFE_NUMBERS_FORMAT_HPP

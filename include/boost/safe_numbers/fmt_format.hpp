// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_FMT_FORMAT_HPP
#define BOOST_SAFE_NUMBERS_FMT_FORMAT_HPP

#include <boost/safe_numbers/detail/unsigned_integer_basis.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <fmt/format.h>
#include <concepts>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

template <std::unsigned_integral BasisType>
struct fmt::formatter<boost::safe_numbers::detail::unsigned_integer_basis<BasisType>>
    : fmt::formatter<BasisType>
{
    auto format(const boost::safe_numbers::detail::unsigned_integer_basis<BasisType>& val,
                fmt::format_context& ctx) const
    {
        return fmt::formatter<BasisType>::format(static_cast<BasisType>(val), ctx);
    }
};

#endif // BOOST_SAFE_NUMBERS_FMT_FORMAT_HPP

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_FUNCTIONAL_HPP
#define BOOST_SAFE_NUMBERS_FUNCTIONAL_HPP

#include <boost/safe_numbers/detail/type_traits.hpp>
#include <boost/safe_numbers/detail/int128/hash.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <functional>
#include <cstddef>
#include <cstdint>
#include <type_traits>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

template <boost::safe_numbers::detail::library_type T>
struct std::hash<T>
{
    [[nodiscard]] auto operator()(const T& value) const noexcept -> std::size_t
    {
        using underlying_type = boost::safe_numbers::detail::underlying_type_t<T>;
        const auto raw {static_cast<underlying_type>(value)};

        return std::hash<underlying_type>{}(raw);
    }
};

#endif // BOOST_SAFE_NUMBERS_FUNCTIONAL_HPP

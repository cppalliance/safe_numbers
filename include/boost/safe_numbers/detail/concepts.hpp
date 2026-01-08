// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_CONCEPTS_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_CONCEPTS_HPP

#include "int128/int128.hpp"

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <concepts>
#include <type_traits>

#endif // ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers::detail {

template <typename T>
concept unsigned_integral = std::unsigned_integral<T> || std::is_same_v<T, int128::uint128_t>;

} // namespace boost::safe_numbers::detail

#endif // BOOST_SAFE_NUMBERS_DETAIL_CONCEPTS_HPP

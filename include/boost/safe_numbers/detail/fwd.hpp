// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_FWD_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_FWD_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/int128/int128.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <concepts>
#include <type_traits>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers::detail {

template <typename T>
concept unsigned_integral = std::unsigned_integral<T> || std::is_same_v<T, int128::uint128_t>;

// Forward declaration of unsigned_integer_basis
template <unsigned_integral BasisType>
class unsigned_integer_basis;

} // namespace boost::safe_numbers::detail

#endif // BOOST_SAFE_NUMBERS_DETAIL_FWD_HPP

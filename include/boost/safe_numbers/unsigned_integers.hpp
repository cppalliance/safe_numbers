// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_UNSIGNED_INTEGERS_HPP
#define BOOST_SAFE_NUMBERS_UNSIGNED_INTEGERS_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/unsigned_integer_basis.hpp>
#include "detail/int128/int128.hpp"

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <cstdint>

#endif

namespace boost::safe_numbers {

BOOST_SAFE_NUMBERS_EXPORT using u8 = detail::unsigned_integer_basis<std::uint8_t>;

BOOST_SAFE_NUMBERS_EXPORT using u16 = detail::unsigned_integer_basis<std::uint16_t>;

BOOST_SAFE_NUMBERS_EXPORT using u32 = detail::unsigned_integer_basis<std::uint32_t>;

BOOST_SAFE_NUMBERS_EXPORT using u64 = detail::unsigned_integer_basis<std::uint64_t>;

BOOST_SAFE_NUMBERS_EXPORT using u128 = detail::unsigned_integer_basis<int128::uint128_t>;

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_UNSIGNED_INTEGERS_HPP

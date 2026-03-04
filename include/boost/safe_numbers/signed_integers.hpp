// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_SIGNED_INTEGERS_HPP
#define BOOST_SAFE_NUMBERS_SIGNED_INTEGERS_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/signed_integer_basis.hpp>
#include "detail/int128/int128.hpp"

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <cstdint>

#endif

namespace boost::safe_numbers {

BOOST_SAFE_NUMBERS_EXPORT using i8 = detail::signed_integer_basis<std::int8_t>;

BOOST_SAFE_NUMBERS_EXPORT using i16 = detail::signed_integer_basis<std::int16_t>;

BOOST_SAFE_NUMBERS_EXPORT using i32 = detail::signed_integer_basis<std::int32_t>;

BOOST_SAFE_NUMBERS_EXPORT using i64 = detail::signed_integer_basis<std::int64_t>;

BOOST_SAFE_NUMBERS_EXPORT using i128 = detail::signed_integer_basis<int128::int128_t>;

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_SIGNED_INTEGERS_HPP

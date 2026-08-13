// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_SIGNED_INTEGERS_HPP
#define BOOST_SAFE_NUMBERS_SIGNED_INTEGERS_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/signed_integer_basis.hpp>
#include "detail/int128/int128.hpp"
#include "detail/int128/random.hpp"

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <cstdint>

#endif

namespace boost::safe_numbers {

BOOST_SAFE_NUMBERS_EXPORT using i8 = detail::signed_integer_basis<std::int8_t>;

BOOST_SAFE_NUMBERS_EXPORT using i16 = detail::signed_integer_basis<std::int16_t>;

BOOST_SAFE_NUMBERS_EXPORT using i32 = detail::signed_integer_basis<std::int32_t>;

BOOST_SAFE_NUMBERS_EXPORT using i64 = detail::signed_integer_basis<std::int64_t>;

BOOST_SAFE_NUMBERS_EXPORT using i128 = detail::signed_integer_basis<int128::int128_t>;

// Saturating counterparts: overflow and underflow clamp to the numeric limits
// instead of throwing (including MIN / -1 and negation of MIN).
// Division and modulo by zero still throw std::domain_error.

BOOST_SAFE_NUMBERS_EXPORT using sat_i8 = detail::signed_integer_basis<std::int8_t, overflow_policy::saturate>;

BOOST_SAFE_NUMBERS_EXPORT using sat_i16 = detail::signed_integer_basis<std::int16_t, overflow_policy::saturate>;

BOOST_SAFE_NUMBERS_EXPORT using sat_i32 = detail::signed_integer_basis<std::int32_t, overflow_policy::saturate>;

BOOST_SAFE_NUMBERS_EXPORT using sat_i64 = detail::signed_integer_basis<std::int64_t, overflow_policy::saturate>;

BOOST_SAFE_NUMBERS_EXPORT using sat_i128 = detail::signed_integer_basis<int128::int128_t, overflow_policy::saturate>;

// Strict counterparts: any error terminates via std::exit(EXIT_FAILURE). Host only.

BOOST_SAFE_NUMBERS_EXPORT using strict_i8 = detail::signed_integer_basis<std::int8_t, overflow_policy::strict>;

BOOST_SAFE_NUMBERS_EXPORT using strict_i16 = detail::signed_integer_basis<std::int16_t, overflow_policy::strict>;

BOOST_SAFE_NUMBERS_EXPORT using strict_i32 = detail::signed_integer_basis<std::int32_t, overflow_policy::strict>;

BOOST_SAFE_NUMBERS_EXPORT using strict_i64 = detail::signed_integer_basis<std::int64_t, overflow_policy::strict>;

BOOST_SAFE_NUMBERS_EXPORT using strict_i128 = detail::signed_integer_basis<int128::int128_t, overflow_policy::strict>;

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_SIGNED_INTEGERS_HPP

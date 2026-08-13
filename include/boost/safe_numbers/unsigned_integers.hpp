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

// Saturating counterparts: overflow clamps to the numeric limits instead of throwing.
// Division and modulo by zero still throw std::domain_error.

BOOST_SAFE_NUMBERS_EXPORT using sat_u8 = detail::unsigned_integer_basis<std::uint8_t, overflow_policy::saturate>;

BOOST_SAFE_NUMBERS_EXPORT using sat_u16 = detail::unsigned_integer_basis<std::uint16_t, overflow_policy::saturate>;

BOOST_SAFE_NUMBERS_EXPORT using sat_u32 = detail::unsigned_integer_basis<std::uint32_t, overflow_policy::saturate>;

BOOST_SAFE_NUMBERS_EXPORT using sat_u64 = detail::unsigned_integer_basis<std::uint64_t, overflow_policy::saturate>;

BOOST_SAFE_NUMBERS_EXPORT using sat_u128 = detail::unsigned_integer_basis<int128::uint128_t, overflow_policy::saturate>;

// Strict counterparts: any error terminates via std::exit(EXIT_FAILURE). Host only.

BOOST_SAFE_NUMBERS_EXPORT using strict_u8 = detail::unsigned_integer_basis<std::uint8_t, overflow_policy::strict>;

BOOST_SAFE_NUMBERS_EXPORT using strict_u16 = detail::unsigned_integer_basis<std::uint16_t, overflow_policy::strict>;

BOOST_SAFE_NUMBERS_EXPORT using strict_u32 = detail::unsigned_integer_basis<std::uint32_t, overflow_policy::strict>;

BOOST_SAFE_NUMBERS_EXPORT using strict_u64 = detail::unsigned_integer_basis<std::uint64_t, overflow_policy::strict>;

BOOST_SAFE_NUMBERS_EXPORT using strict_u128 = detail::unsigned_integer_basis<int128::uint128_t, overflow_policy::strict>;

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_UNSIGNED_INTEGERS_HPP

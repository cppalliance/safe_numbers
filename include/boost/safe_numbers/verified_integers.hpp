// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_VERIFIED_INTEGERS_HPP
#define BOOST_SAFE_NUMBERS_VERIFIED_INTEGERS_HPP

#include <boost/safe_numbers/detail/verified_type_basis.hpp>
#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/bounded_integers.hpp>

namespace boost::safe_numbers {

BOOST_SAFE_NUMBERS_EXPORT using verified_u8 = detail::verified_type_basis<u8>;

BOOST_SAFE_NUMBERS_EXPORT using verified_u16 = detail::verified_type_basis<u16>;

BOOST_SAFE_NUMBERS_EXPORT using verified_u32 = detail::verified_type_basis<u32>;

BOOST_SAFE_NUMBERS_EXPORT using verified_u64 = detail::verified_type_basis<u64>;

BOOST_SAFE_NUMBERS_EXPORT using verified_u128 = detail::verified_type_basis<u128>;

BOOST_SAFE_NUMBERS_EXPORT template <auto Max, auto Min>
using verified_bounded_integer = detail::verified_type_basis<bounded_uint<Max, Min>>;

} // boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_VERIFIED_INTEGERS_HPP

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_FLOATS_HPP
#define BOOST_SAFE_NUMBERS_FLOATS_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/float_basis.hpp>

namespace boost::safe_numbers {

BOOST_SAFE_NUMBERS_EXPORT using f32 = detail::float_basis<float>;

BOOST_SAFE_NUMBERS_EXPORT using f64 = detail::float_basis<double>;

// Saturating counterparts with raw IEEE 754 semantics: overflow saturates to
// infinity, NaN propagates, and division by zero yields infinity. No checks run.

BOOST_SAFE_NUMBERS_EXPORT using sat_f32 = detail::float_basis<float, overflow_policy::saturate>;

BOOST_SAFE_NUMBERS_EXPORT using sat_f64 = detail::float_basis<double, overflow_policy::saturate>;

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_FLOATS_HPP

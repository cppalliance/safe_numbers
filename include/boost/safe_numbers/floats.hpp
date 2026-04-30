// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_FLOATS_HPP
#define BOOST_SAFE_NUMBERS_FLOATS_HPP

#include <boost/safe_numbers/detail/float_basis.hpp>

namespace boost::safe_numbers {

using f32 = detail::float_basis<float>;
using f64 = detail::float_basis<double>;

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_FLOATS_HPP

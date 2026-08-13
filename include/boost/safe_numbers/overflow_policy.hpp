// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_OVERFLOW_POLICY_HPP
#define BOOST_SAFE_NUMBERS_OVERFLOW_POLICY_HPP

#include <boost/safe_numbers/detail/config.hpp>

namespace boost::safe_numbers {

BOOST_SAFE_NUMBERS_EXPORT enum class overflow_policy
{
    throw_exception,
    saturate,
    overflow_tuple,
    checked,
    strict,
    widen,
};

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_OVERFLOW_POLICY_HPP

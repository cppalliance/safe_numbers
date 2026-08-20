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

// The kind of error being reported to a user defined handler.
// divide_by_zero covers modulo by zero as well; the message distinguishes them.
// invalid_operation and nan_operation are produced by the floating point types only.
BOOST_SAFE_NUMBERS_EXPORT enum class error_kind
{
    overflow,
    underflow,
    divide_by_zero,
    invalid_operation,
    nan_operation,
};

// Tag types selecting the built-in policies through the basic_* alias templates,
// e.g. basic_u8<saturating>. A user defined handler type is used the same way:
// basic_u8<my_handler>.

BOOST_SAFE_NUMBERS_EXPORT struct throwing {};

BOOST_SAFE_NUMBERS_EXPORT struct saturating {};

BOOST_SAFE_NUMBERS_EXPORT struct strict {};

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_OVERFLOW_POLICY_HPP

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_OVERFLOW_POLICY_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_OVERFLOW_POLICY_HPP

namespace boost::safe_numbers::detail {

enum class overflow_policy
{
    throw_exception,
    saturate,
    overflow_tuple,
    checked,
};

} // namespace boost::safe_numbers::detail

#endif // BOOST_SAFE_NUMBERS_DETAIL_OVERFLOW_POLICY_HPP

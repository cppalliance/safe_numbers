// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// This test MUST COMPILE even with the compile_assert feature enabled in an
// optimized build. checked is a value policy: a constant divide-by-zero yields
// nullopt, not an error, so the compile-time check must not fire here.

#include <boost/safe_numbers/unsigned_integers.hpp>

using namespace boost::safe_numbers;

bool f()
{
    return checked_div(u32{10U}, u32{0U}).has_value();
}

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// This test MUST FAIL to compile: div_ceil divides by its second argument and is
// noexcept, so a compile-time-constant zero divisor is a build error when the
// compile_assert feature is enabled in an optimized build.

#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/integer_utilities.hpp>

using namespace boost::safe_numbers;

u32 f()
{
    return div_ceil(u32{10U}, u32{0U});
}

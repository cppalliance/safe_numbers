// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// This test MUST COMPILE even with the compile_assert feature enabled in an
// optimized build. saturate is a value policy: a constant overflow yields a defined
// result (the clamp), not an error, so the compile-time check must not fire here.

#include <boost/safe_numbers/unsigned_integers.hpp>

using namespace boost::safe_numbers;

u8 f()
{
    return saturating_add(u8{200U}, u8{100U});
}

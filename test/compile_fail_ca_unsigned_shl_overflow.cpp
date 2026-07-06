// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// This test MUST FAIL to compile: a constant left shift past the type width is a
// build error when the compile_assert feature is enabled in an optimized build.

#include <boost/safe_numbers/unsigned_integers.hpp>

using namespace boost::safe_numbers;

u32 f()
{
    return u32{1U} << u32{40U};
}

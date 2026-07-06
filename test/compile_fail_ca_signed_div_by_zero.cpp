// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// This test MUST FAIL to compile: a compile-time-constant signed division by zero
// is a build error when the compile_assert feature is enabled in an optimized build.

#include <boost/safe_numbers/signed_integers.hpp>

using namespace boost::safe_numbers;

i32 f()
{
    return i32{10} / i32{0};
}

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// This test MUST FAIL to compile: a constant addition that overflows is a build
// error under the default (throw_exception) policy when the compile_assert feature
// is enabled in an optimized build. The check reuses the same overflow result the
// runtime path computes (__builtin_add_overflow on gcc/clang), so it fires at both
// -O2 and -O3.

#include <boost/safe_numbers/unsigned_integers.hpp>

using namespace boost::safe_numbers;

u8 f()
{
    return u8{200U} + u8{100U};
}

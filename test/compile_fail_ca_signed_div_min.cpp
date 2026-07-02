// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// This test MUST FAIL to compile: the constant signed division min / -1 overflows
// (since -min > max in two's complement) and is a build error under the default
// (throw_exception) policy when the compile_assert feature is enabled in an
// optimized build.

#include <boost/safe_numbers/signed_integers.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE
#include <limits>
#include <cstdint>
#endif

using namespace boost::safe_numbers;

i32 f()
{
    return i32{std::numeric_limits<std::int32_t>::min()} / i32{-1};
}

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// This test MUST FAIL to compile. It performs a compile-time-constant division by
// zero, which boost.safe_numbers turns into a build error when the compile_assert
// feature is enabled. The Jamfile supplies BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT
// and forces optimization on (the mechanism is inactive at -O0).

#include <boost/safe_numbers/unsigned_integers.hpp>

using namespace boost::safe_numbers;

u32 f()
{
    return u32{10U} / u32{0U};
}

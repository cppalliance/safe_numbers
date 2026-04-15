// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Signed safe integers deliberately do not provide bitwise operations
// (see the Ada programming language for the rationale). This test
// verifies that any attempted use of a bitwise operator on a signed
// safe integer is a compile-time error.

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers/signed_integers.hpp>

#endif

int main()
{
    using namespace boost::safe_numbers;

    const auto a {i32{0x1234}};
    const auto b {i32{0x00FF}};

    const auto result {a & b}; // Should fail to compile
    static_cast<void>(result);

    return 0;
}

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// This example demonstrates that verified types catch overflow at compile time.
// This file is expected to fail compilation.

#include <boost/safe_numbers/verified_integers.hpp>

int main()
{
    using namespace boost::safe_numbers;

    // u8 can hold 0-255. Adding 1 to 255 overflows.
    // With a verified type this is a compile error, not a runtime exception.
    constexpr auto result {verified_u8{u8{255}} + verified_u8{u8{1}}};

    static_cast<void>(result);

    return 0;
}

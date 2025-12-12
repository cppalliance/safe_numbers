// Copyright 2024 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Import the STL if we can
#if defined(__cpp_lib_modules) && __cpp_lib_modules >= 202207L
import std;
#else
#include <iostream>
#endif

import boost.safe_numbers;

int main()
{
    using namespace boost::safe_numbers;

    u32 a {5U};
    std::cout << a << std::endl;

    return 0;
}

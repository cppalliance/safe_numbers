// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers/unsigned_integers.hpp>     // For safe_numbers types
#include <boost/safe_numbers/iostream.hpp>              // For safe_numbers <iostream> support
#include <iostream>
#include <cstdint>
#include <climits>

int main()
{
    using boost::safe_numbers::u8;

    try
    {
        const u8 x {UINT8_MAX};
        const u8 y {2};
        const u8 z {x + y};

        std::cout << "Value of z: " << z << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error Detected: " << e.what() << std::endl;
    }

    return 0;
}

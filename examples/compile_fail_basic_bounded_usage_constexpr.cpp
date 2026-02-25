// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers/bounded_integers.hpp>      // For safe_numbers types
#include <boost/safe_numbers/iostream.hpp>              // For safe_numbers <iostream> support
#include <iostream>
#include <cstdint>

int main()
{
    using boost::safe_numbers::bounded_uint;

    try
    {
        constexpr bounded_uint<0u, 100u> x {100};
        constexpr bounded_uint<0u, 100u> y {2};
        constexpr bounded_uint<0u, 100u> z {x + y};

        std::cout << "Value of z: " << z << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error Detected: " << e.what() << std::endl;
    }

    return 0;
}

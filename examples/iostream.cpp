// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

//[iostream_example
//` This example demonstrates stream I/O support for safe integer types.
//` Safe integers can be written to and read from standard streams using
//` the familiar << and >> operators. The u8 type displays as a number
//` rather than as a character.

#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/iostream.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdint>

int main()
{
    using namespace boost::safe_numbers;

    // Output: safe integers work with std::cout
    {
        const u8  a {42U};
        const u16 b {1000U};
        const u32 c {100000U};
        const u64 d {9999999999ULL};

        std::cout << "u8:  " << a << std::endl;
        std::cout << "u16: " << b << std::endl;
        std::cout << "u32: " << c << std::endl;
        std::cout << "u64: " << d << std::endl;
    }

    // u8 displays as a number, not a character
    {
        const u8 newline {10U};
        const u8 space {32U};

        std::cout << "u8(10) = " << newline << " (not a newline character)" << std::endl;
        std::cout << "u8(32) = " << space << " (not a space character)" << std::endl;
    }

    // Input: safe integers work with std::istringstream
    {
        u32 value;
        std::istringstream input {"12345"};
        input >> value;

        std::cout << "Read from stream: " << value << std::endl;
    }

    // Works with stream formatting
    {
        const u32 val {255U};
        std::cout << "Decimal:     " << std::dec << val << std::endl;
        std::cout << "Hexadecimal: " << std::hex << val << std::endl;
        std::cout << "Octal:       " << std::oct << val << std::endl;
        std::cout << std::dec; // Reset to decimal
    }

    // Roundtrip through a stringstream
    {
        const u64 original {18446744073709551615ULL};
        std::stringstream ss;
        ss << original;

        u64 restored;
        ss >> restored;

        std::cout << "Roundtrip: " << original << " -> \"" << original << "\" -> " << restored << std::endl;
    }

    return 0;
}
//]

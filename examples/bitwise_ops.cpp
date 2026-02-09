// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/iostream.hpp>
#include <iostream>

int main()
{
    using namespace boost::safe_numbers;

    const auto a = u32{0xFF00FF00u};
    const auto b = u32{0x0F0F0F0Fu};

    // Bitwise NOT
    std::cout << std::hex;
    std::cout << "~a             = 0x" << ~a << '\n';

    // Bitwise AND
    std::cout << "a & b          = 0x" << (a & b) << '\n';

    // Bitwise OR
    std::cout << "a | b          = 0x" << (a | b) << '\n';

    // Bitwise XOR
    std::cout << "a ^ b          = 0x" << (a ^ b) << '\n';

    std::cout << std::dec << '\n';

    // Left shift (throws if bits would be shifted past the type width)
    const auto one = u8{1};
    const auto shift = u8{4};
    std::cout << "u8(1) << 4     = " << static_cast<unsigned>(one << shift) << '\n';

    // Right shift (throws if the shift amount >= type width)
    const auto val = u8{0x80};
    std::cout << "u8(128) >> 4   = " << static_cast<unsigned>(val >> shift) << '\n';

    std::cout << '\n';

    // Compound assignment operators
    auto x = u32{0xFF00u};
    x &= u32{0x0F0Fu};
    std::cout << std::hex;
    std::cout << "x &= 0x0F0F   -> 0x" << x << '\n';

    x |= u32{0xF000u};
    std::cout << "x |= 0xF000   -> 0x" << x << '\n';

    x ^= u32{0x00FFu};
    std::cout << "x ^= 0x00FF   -> 0x" << x << '\n';

    std::cout << std::dec;
    auto y = u32{1};
    y <<= u32{8};
    std::cout << "y <<= 8       -> " << y << '\n';

    y >>= u32{4};
    std::cout << "y >>= 4       -> " << y << '\n';

    std::cout << '\n';

    // Shift overflow detection
    try
    {
        const auto big = u8{0xFF};
        const auto result = big << u8{1};  // Would shift bits past width
        std::cout << "Should not reach here: " << static_cast<unsigned>(result) << '\n';
    }
    catch (const std::overflow_error& e)
    {
        std::cerr << "Left shift error: " << e.what() << '\n';
    }

    try
    {
        const auto val2 = u8{1};
        const auto result = val2 >> u8{8};  // Shift amount >= type width
        std::cout << "Should not reach here: " << static_cast<unsigned>(result) << '\n';
    }
    catch (const std::overflow_error& e)
    {
        std::cerr << "Right shift error: " << e.what() << '\n';
    }

    return 0;
}

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

    std::cout << '\n';

    // ---- Policy-based shift operations ----

    // Saturating shifts: clamp to max (left) or zero (right) on overflow
    std::cout << "saturating_shl(u8(1), 4)    = "
              << static_cast<unsigned>(saturating_shl(u8{1}, u8{4})) << '\n';
    std::cout << "saturating_shl(u8(255), 1)  = "
              << static_cast<unsigned>(saturating_shl(u8{0xFF}, u8{1})) << '\n';
    std::cout << "saturating_shr(u8(128), 4)  = "
              << static_cast<unsigned>(saturating_shr(u8{0x80}, u8{4})) << '\n';
    std::cout << "saturating_shr(u8(1), 8)    = "
              << static_cast<unsigned>(saturating_shr(u8{1}, u8{8})) << '\n';

    std::cout << '\n';

    // Overflowing shifts: return the result and an overflow flag
    {
        const auto [result, flag] = overflowing_shl(u8{0xFF}, u8{1});
        std::cout << "overflowing_shl(u8(255), 1) = "
                  << static_cast<unsigned>(result) << " (overflow: "
                  << std::boolalpha << flag << ")\n";
    }
    {
        const auto [result, flag] = overflowing_shr(u8{1}, u8{8});
        std::cout << "overflowing_shr(u8(1), 8)   = "
                  << static_cast<unsigned>(result) << " (overflow: "
                  << std::boolalpha << flag << ")\n";
    }

    std::cout << '\n';

    // Checked shifts: return std::optional (nullopt on overflow)
    {
        const auto result = checked_shl(u8{1}, u8{4});
        if (result)
        {
            std::cout << "checked_shl(u8(1), 4)       = "
                      << static_cast<unsigned>(*result) << '\n';
        }
    }
    {
        const auto result = checked_shl(u8{0xFF}, u8{1});
        std::cout << "checked_shl(u8(255), 1)     = "
                  << (result ? "value" : "nullopt (overflow)") << '\n';
    }
    {
        const auto result = checked_shr(u8{1}, u8{8});
        std::cout << "checked_shr(u8(1), 8)       = "
                  << (result ? "value" : "nullopt (overflow)") << '\n';
    }

    std::cout << '\n';

    // Wrapping shifts: perform the shift ignoring overflow
    std::cout << "wrapping_shl(u8(255), 1)    = "
              << static_cast<unsigned>(wrapping_shl(u8{0xFF}, u8{1})) << '\n';
    std::cout << "wrapping_shr(u8(1), 8)      = "
              << static_cast<unsigned>(wrapping_shr(u8{1}, u8{8})) << '\n';

    std::cout << '\n';

    // Generic policy-parameterized shifts
    std::cout << "shl<saturate>(u32(1), 30)   = "
              << shl<overflow_policy::saturate>(u32{1}, u32{30}) << '\n';
    std::cout << "shl<saturate>(u32(max), 1)  = "
              << shl<overflow_policy::saturate>(u32{0xFFFFFFFFu}, u32{1}) << '\n';
    std::cout << "shr<checked>(u32(8), 1)     = ";
    {
        const auto result = shr<overflow_policy::checked>(u32{8}, u32{1});
        if (result)
        {
            std::cout << *result << '\n';
        }
    }

    return 0;
}

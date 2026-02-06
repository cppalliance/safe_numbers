// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/bit.hpp>
#include <iostream>

int main()
{
    using namespace boost::safe_numbers;

    const u32 x {0b0000'0000'0000'0000'0000'0000'0010'1000};  // 40

    // Power-of-two queries
    std::cout << "has_single_bit(40) = " << has_single_bit(x) << '\n';
    std::cout << "has_single_bit(32) = " << has_single_bit(u32{32}) << '\n';
    std::cout << "bit_ceil(40)       = " << static_cast<std::uint32_t>(bit_ceil(x)) << '\n';
    std::cout << "bit_floor(40)      = " << static_cast<std::uint32_t>(bit_floor(x)) << '\n';
    std::cout << "bit_width(40)      = " << bit_width(x) << '\n';

    std::cout << '\n';

    // Rotation
    const u8 y {0b1011'0001};  // 177
    std::cout << "rotl(0b10110001, 2) = " << static_cast<unsigned>(rotl(y, 2)) << '\n';
    std::cout << "rotr(0b10110001, 2) = " << static_cast<unsigned>(rotr(y, 2)) << '\n';

    std::cout << '\n';

    // Counting
    const u16 z {0b0000'1111'0000'0000};  // 3840
    std::cout << "countl_zero(0x0F00) = " << countl_zero(z) << '\n';
    std::cout << "countl_one(0x0F00)  = " << countl_one(z) << '\n';
    std::cout << "countr_zero(0x0F00) = " << countr_zero(z) << '\n';
    std::cout << "countr_one(0x0F00)  = " << countr_one(z) << '\n';
    std::cout << "popcount(0x0F00)    = " << popcount(z) << '\n';

    std::cout << '\n';

    // Byteswap
    const u32 w {0x12345678};
    std::cout << std::hex;
    std::cout << "byteswap(0x12345678) = 0x" << static_cast<std::uint32_t>(byteswap(w)) << '\n';

    return 0;
}

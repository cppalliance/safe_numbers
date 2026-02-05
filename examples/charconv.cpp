// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/charconv.hpp>
#include <iostream>
#include <cstring>

int main()
{
    using namespace boost::safe_numbers;

    // to_chars: Convert safe integer to string
    u32 value {12345};
    char buffer[32];

    auto result = to_chars(buffer, buffer + sizeof(buffer), value);
    if (result)
    {
        *result.ptr = '\0';  // Null-terminate
        std::cout << "to_chars (base 10): " << buffer << '\n';
    }

    // to_chars with different bases
    result = to_chars(buffer, buffer + sizeof(buffer), value, 16);
    if (result)
    {
        *result.ptr = '\0';
        std::cout << "to_chars (base 16): " << buffer << '\n';
    }

    result = to_chars(buffer, buffer + sizeof(buffer), value, 2);
    if (result)
    {
        *result.ptr = '\0';
        std::cout << "to_chars (base 2):  " << buffer << '\n';
    }

    std::cout << '\n';

    // from_chars: Convert string to safe integer
    const char* str = "98765";
    u32 parsed_value {};

    auto parse_result = from_chars(str, str + std::strlen(str), parsed_value);
    if (parse_result)
    {
        std::cout << "from_chars (base 10): " << static_cast<std::uint32_t>(parsed_value) << '\n';
    }

    // from_chars with hexadecimal
    const char* hex_str = "1a2b";
    u32 hex_value {};

    parse_result = from_chars(hex_str, hex_str + std::strlen(hex_str), hex_value, 16);
    if (parse_result)
    {
        std::cout << "from_chars (base 16): " << static_cast<std::uint32_t>(hex_value) << '\n';
    }

    // from_chars with binary
    const char* bin_str = "11010";
    u8 bin_value {};

    parse_result = from_chars(bin_str, bin_str + std::strlen(bin_str), bin_value, 2);
    if (parse_result)
    {
        std::cout << "from_chars (base 2):  " << static_cast<unsigned>(bin_value) << '\n';
    }

    return 0;
}

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#define FMT_HEADER_ONLY

#if __has_include(<fmt/format.h>)

#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/fmt_format.hpp>
#include <fmt/format.h>
#include <iostream>

int main()
{
    using namespace boost::safe_numbers;

    const u32 val1 {12345};
    const u64 val2 {9876543210};

    // Default format (decimal)
    std::cout << "Default Format:\n";
    std::cout << fmt::format("{}", val1) << '\n';
    std::cout << fmt::format("{}", val2) << "\n\n";

    // Hexadecimal format
    std::cout << "Hexadecimal Format:\n";
    std::cout << fmt::format("{:x}", val1) << '\n';
    std::cout << fmt::format("{:#x}", val2) << "\n\n";

    // Binary format
    std::cout << "Binary Format:\n";
    std::cout << fmt::format("{:b}", val1) << '\n';
    std::cout << fmt::format("{:#b}", u8{42}) << "\n\n";

    // Octal format
    std::cout << "Octal Format:\n";
    std::cout << fmt::format("{:o}", val1) << '\n';
    std::cout << fmt::format("{:#o}", val1) << "\n\n";

    // Padding and alignment
    std::cout << "Padding and Alignment:\n";
    std::cout << fmt::format("{:>10}", val1) << '\n';   // Right align
    std::cout << fmt::format("{:<10}", val1) << '\n';   // Left align
    std::cout << fmt::format("{:^10}", val1) << '\n';   // Center align
    std::cout << fmt::format("{:0>10}", val1) << "\n\n"; // Zero-padded

    // Fill character
    std::cout << "Fill Character:\n";
    std::cout << fmt::format("{:*>10}", val1) << '\n';
    std::cout << fmt::format("{:_<10}", val1) << '\n';

    return 0;
}

#else

#include <iostream>

int main()
{
    std::cout << "{fmt} headers are required to run this example." << std::endl;
}

#endif

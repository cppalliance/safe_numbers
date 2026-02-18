// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// This test verifies that from_be_bytes produces a compile error when
// the span's fixed extent does not match the target type's size.

#include <boost/safe_numbers/byte_conversions.hpp>
#include <array>
#include <cstddef>
#include <span>

using namespace boost::safe_numbers;

int main()
{
    // u32 requires 4 bytes, but we provide a 2-byte span
    const std::array<std::byte, 2> bytes {std::byte{0x01}, std::byte{0x02}};
    auto val = from_be_bytes<u32>(std::span<const std::byte, 2>{bytes});

    return 0;
}

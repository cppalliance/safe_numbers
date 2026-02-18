// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers/byte_conversions.hpp>
#include <boost/safe_numbers/verified_integers.hpp>
#include <iostream>
#include <iomanip>
#include <cstddef>
#include <cstdint>
#include <span>

int main()
{
    using namespace boost::safe_numbers;

    // ---- to_be_bytes: convert to big-endian byte array ----
    std::cout << "=== to_be_bytes ===\n";
    {
        const auto bytes = to_be_bytes(u32{0x01020304U});
        std::cout << "u32(0x01020304) -> BE bytes: ";
        for (const auto& b : bytes)
        {
            std::cout << std::hex << std::setfill('0') << std::setw(2)
                      << static_cast<unsigned>(b) << ' ';
        }
        std::cout << '\n';
    }
    {
        const auto bytes = to_be_bytes(u16{static_cast<std::uint16_t>(0xABCD)});
        std::cout << "u16(0xABCD)     -> BE bytes: ";
        for (const auto& b : bytes)
        {
            std::cout << std::hex << std::setfill('0') << std::setw(2)
                      << static_cast<unsigned>(b) << ' ';
        }
        std::cout << '\n';
    }

    // ---- from_be_bytes: reconstruct from big-endian bytes ----
    std::cout << "\n=== from_be_bytes ===\n";
    {
        const std::array<std::byte, 4> bytes {
            std::byte{0x01}, std::byte{0x02}, std::byte{0x03}, std::byte{0x04}
        };
        const auto val = from_be_bytes<u32>(std::span<const std::byte, 4>{bytes});
        std::cout << "BE bytes {01,02,03,04} -> u32: 0x"
                  << std::hex << static_cast<std::uint32_t>(val) << '\n';
    }

    // ---- to_le_bytes: convert to little-endian byte array ----
    std::cout << "\n=== to_le_bytes ===\n";
    {
        const auto bytes = to_le_bytes(u32{0x01020304U});
        std::cout << "u32(0x01020304) -> LE bytes: ";
        for (const auto& b : bytes)
        {
            std::cout << std::hex << std::setfill('0') << std::setw(2)
                      << static_cast<unsigned>(b) << ' ';
        }
        std::cout << '\n';
    }
    {
        const auto bytes = to_le_bytes(u64{0x0102030405060708ULL});
        std::cout << "u64(0x01..08)   -> LE bytes: ";
        for (const auto& b : bytes)
        {
            std::cout << std::hex << std::setfill('0') << std::setw(2)
                      << static_cast<unsigned>(b) << ' ';
        }
        std::cout << '\n';
    }

    // ---- from_le_bytes: reconstruct from little-endian bytes ----
    std::cout << "\n=== from_le_bytes ===\n";
    {
        const std::array<std::byte, 4> bytes {
            std::byte{0x04}, std::byte{0x03}, std::byte{0x02}, std::byte{0x01}
        };
        const auto val = from_le_bytes<u32>(std::span<const std::byte, 4>{bytes});
        std::cout << "LE bytes {04,03,02,01} -> u32: 0x"
                  << std::hex << static_cast<std::uint32_t>(val) << '\n';
    }

    // ---- to_ne_bytes / from_ne_bytes: native endian round-trip ----
    std::cout << "\n=== to_ne_bytes / from_ne_bytes (native endian) ===\n";
    {
        const auto original = u32{0xDEADBEEFU};
        const auto bytes = to_ne_bytes(original);
        std::cout << "u32(0xDEADBEEF) -> NE bytes: ";
        for (const auto& b : bytes)
        {
            std::cout << std::hex << std::setfill('0') << std::setw(2)
                      << static_cast<unsigned>(b) << ' ';
        }
        std::cout << '\n';

        const auto reconstructed = from_ne_bytes<u32>(std::span<const std::byte, 4>{bytes});
        std::cout << "Round-trip:      -> u32: 0x"
                  << std::hex << static_cast<std::uint32_t>(reconstructed) << '\n';
    }

    // ---- BE round-trip with u8 ----
    std::cout << "\n=== u8 round-trip ===\n";
    {
        const auto original = u8{0x42};
        const auto be = to_be_bytes(original);
        const auto le = to_le_bytes(original);
        std::cout << "u8(0x42) -> BE: " << std::hex << std::setfill('0') << std::setw(2)
                  << static_cast<unsigned>(be[0]) << '\n';
        std::cout << "u8(0x42) -> LE: " << std::hex << std::setfill('0') << std::setw(2)
                  << static_cast<unsigned>(le[0]) << '\n';
    }

    // ---- Verified types (compile-time) ----
    std::cout << "\n=== Verified types (compile-time) ===\n";
    {
        constexpr auto bytes = to_be_bytes(verified_u32{u32{0x01020304U}});
        static_assert(bytes[0] == std::byte{0x01});
        static_assert(bytes[3] == std::byte{0x04});
        std::cout << "verified_u32 to_be_bytes: ";
        for (const auto& b : bytes)
        {
            std::cout << std::hex << std::setfill('0') << std::setw(2)
                      << static_cast<unsigned>(b) << ' ';
        }
        std::cout << '\n';
    }
    {
        constexpr auto original = verified_u32{u32{0xDEADBEEFU}};
        constexpr auto bytes = to_be_bytes(original);
        constexpr auto reconstructed = from_be_bytes<verified_u32>(std::span<const std::byte, 4>{bytes});
        static_assert(reconstructed == original);
        std::cout << "verified_u32 BE round-trip: 0x" << std::hex
                  << static_cast<std::uint32_t>(static_cast<u32>(reconstructed)) << '\n';
    }
    {
        constexpr auto original = verified_u64{u64{0x0123456789ABCDEFULL}};
        constexpr auto bytes = to_le_bytes(original);
        constexpr auto reconstructed = from_le_bytes<verified_u64>(std::span<const std::byte, 8>{bytes});
        static_assert(reconstructed == original);
        std::cout << "verified_u64 LE round-trip: 0x" << std::hex
                  << static_cast<std::uint64_t>(static_cast<u64>(reconstructed)) << '\n';
    }
    {
        constexpr auto original = verified_u32{u32{0xCAFEBABEU}};
        constexpr auto bytes = to_ne_bytes(original);
        constexpr auto reconstructed = from_ne_bytes<verified_u32>(std::span<const std::byte, 4>{bytes});
        static_assert(reconstructed == original);
        std::cout << "verified_u32 NE round-trip: 0x" << std::hex
                  << static_cast<std::uint32_t>(static_cast<u32>(reconstructed)) << '\n';
    }

    return 0;
}

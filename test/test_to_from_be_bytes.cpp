// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/lightweight_test.hpp>

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers/byte_conversions.hpp>
#include <boost/safe_numbers/verified_integers.hpp>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>

#endif

using namespace boost::safe_numbers;

// =============================================================================
// to_be_bytes: known byte patterns
// =============================================================================

void test_to_be_bytes_u8()
{
    const auto bytes = to_be_bytes(u8{0x42});
    BOOST_TEST_EQ(bytes.size(), std::size_t{1});
    BOOST_TEST(bytes[0] == std::byte{0x42});
}

void test_to_be_bytes_u16()
{
    const auto bytes = to_be_bytes(u16{static_cast<std::uint16_t>(0x0102)});
    BOOST_TEST_EQ(bytes.size(), std::size_t{2});
    // Big-endian: most significant byte first
    BOOST_TEST(bytes[0] == std::byte{0x01});
    BOOST_TEST(bytes[1] == std::byte{0x02});
}

void test_to_be_bytes_u32()
{
    const auto bytes = to_be_bytes(u32{0x01020304U});
    BOOST_TEST_EQ(bytes.size(), std::size_t{4});
    BOOST_TEST(bytes[0] == std::byte{0x01});
    BOOST_TEST(bytes[1] == std::byte{0x02});
    BOOST_TEST(bytes[2] == std::byte{0x03});
    BOOST_TEST(bytes[3] == std::byte{0x04});
}

void test_to_be_bytes_u64()
{
    const auto bytes = to_be_bytes(u64{0x0102030405060708ULL});
    BOOST_TEST_EQ(bytes.size(), std::size_t{8});
    BOOST_TEST(bytes[0] == std::byte{0x01});
    BOOST_TEST(bytes[1] == std::byte{0x02});
    BOOST_TEST(bytes[2] == std::byte{0x03});
    BOOST_TEST(bytes[3] == std::byte{0x04});
    BOOST_TEST(bytes[4] == std::byte{0x05});
    BOOST_TEST(bytes[5] == std::byte{0x06});
    BOOST_TEST(bytes[6] == std::byte{0x07});
    BOOST_TEST(bytes[7] == std::byte{0x08});
}

void test_to_be_bytes_zero()
{
    {
        const auto bytes = to_be_bytes(u8{0});
        BOOST_TEST(bytes[0] == std::byte{0x00});
    }
    {
        const auto bytes = to_be_bytes(u32{0U});
        for (const auto& b : bytes)
        {
            BOOST_TEST(b == std::byte{0x00});
        }
    }
}

// =============================================================================
// from_be_bytes: reconstruct from known byte patterns
// =============================================================================

void test_from_be_bytes_u8()
{
    const std::array<std::byte, 1> bytes {std::byte{0x42}};
    const auto val = from_be_bytes<u8>(std::span<const std::byte, 1>{bytes});
    BOOST_TEST(val == u8{0x42});
}

void test_from_be_bytes_u16()
{
    const std::array<std::byte, 2> bytes {std::byte{0x01}, std::byte{0x02}};
    const auto val = from_be_bytes<u16>(std::span<const std::byte, 2>{bytes});
    BOOST_TEST(val == u16{static_cast<std::uint16_t>(0x0102)});
}

void test_from_be_bytes_u32()
{
    const std::array<std::byte, 4> bytes {std::byte{0x01}, std::byte{0x02}, std::byte{0x03}, std::byte{0x04}};
    const auto val = from_be_bytes<u32>(std::span<const std::byte, 4>{bytes});
    BOOST_TEST(val == u32{0x01020304U});
}

void test_from_be_bytes_u64()
{
    const std::array<std::byte, 8> bytes {
        std::byte{0x01}, std::byte{0x02}, std::byte{0x03}, std::byte{0x04},
        std::byte{0x05}, std::byte{0x06}, std::byte{0x07}, std::byte{0x08}
    };
    const auto val = from_be_bytes<u64>(std::span<const std::byte, 8>{bytes});
    BOOST_TEST(val == u64{0x0102030405060708ULL});
}

void test_from_be_bytes_zero()
{
    const std::array<std::byte, 4> bytes {};
    const auto val = from_be_bytes<u32>(std::span<const std::byte, 4>{bytes});
    BOOST_TEST(val == u32{0U});
}

// =============================================================================
// Round-trip: to_be_bytes -> from_be_bytes == identity
// =============================================================================

template <typename T>
void test_round_trip(T value)
{
    const auto bytes = to_be_bytes(value);
    const auto result = from_be_bytes<T>(std::span<const std::byte, sizeof(T)>{bytes});
    BOOST_TEST(result == value);
}

void test_round_trips()
{
    test_round_trip(u8{0});
    test_round_trip(u8{0xFF});
    test_round_trip(u8{0x42});

    test_round_trip(u16{0});
    test_round_trip(u16{static_cast<std::uint16_t>(0xFFFF)});
    test_round_trip(u16{static_cast<std::uint16_t>(0xABCD)});

    test_round_trip(u32{0U});
    test_round_trip(u32{0xFFFFFFFFU});
    test_round_trip(u32{0xDEADBEEFU});

    test_round_trip(u64{0ULL});
    test_round_trip(u64{0xFFFFFFFFFFFFFFFFULL});
    test_round_trip(u64{0x0123456789ABCDEFULL});
}

// =============================================================================
// from_be_bytes with dynamic extent: runtime size mismatch throws
// =============================================================================

// =============================================================================
// Verified overloads: to_be_bytes and from_be_bytes at compile time
// =============================================================================

void test_verified_to_be_bytes()
{
    {
        constexpr auto bytes = to_be_bytes(verified_u8{u8{0x42}});
        static_assert(bytes[0] == std::byte{0x42});
    }
    {
        constexpr auto bytes = to_be_bytes(verified_u16{u16{static_cast<std::uint16_t>(0x0102)}});
        static_assert(bytes[0] == std::byte{0x01});
        static_assert(bytes[1] == std::byte{0x02});
    }
    {
        constexpr auto bytes = to_be_bytes(verified_u32{u32{0x01020304U}});
        static_assert(bytes[0] == std::byte{0x01});
        static_assert(bytes[1] == std::byte{0x02});
        static_assert(bytes[2] == std::byte{0x03});
        static_assert(bytes[3] == std::byte{0x04});
    }
    {
        constexpr auto bytes = to_be_bytes(verified_u64{u64{0x0102030405060708ULL}});
        static_assert(bytes[0] == std::byte{0x01});
        static_assert(bytes[1] == std::byte{0x02});
        static_assert(bytes[2] == std::byte{0x03});
        static_assert(bytes[3] == std::byte{0x04});
        static_assert(bytes[4] == std::byte{0x05});
        static_assert(bytes[5] == std::byte{0x06});
        static_assert(bytes[6] == std::byte{0x07});
        static_assert(bytes[7] == std::byte{0x08});
    }
}

void test_verified_from_be_bytes()
{
    {
        constexpr std::array<std::byte, 1> bytes {std::byte{0x42}};
        constexpr auto val = from_be_bytes<verified_u8>(std::span<const std::byte, 1>{bytes});
        static_assert(static_cast<u8>(val) == u8{0x42});
    }
    {
        constexpr std::array<std::byte, 4> bytes {std::byte{0x01}, std::byte{0x02}, std::byte{0x03}, std::byte{0x04}};
        constexpr auto val = from_be_bytes<verified_u32>(std::span<const std::byte, 4>{bytes});
        static_assert(static_cast<u32>(val) == u32{0x01020304U});
    }
    {
        constexpr std::array<std::byte, 8> bytes {
            std::byte{0x01}, std::byte{0x02}, std::byte{0x03}, std::byte{0x04},
            std::byte{0x05}, std::byte{0x06}, std::byte{0x07}, std::byte{0x08}
        };
        constexpr auto val = from_be_bytes<verified_u64>(std::span<const std::byte, 8>{bytes});
        static_assert(static_cast<u64>(val) == u64{0x0102030405060708ULL});
    }
}

void test_verified_be_bytes_round_trip()
{
    {
        constexpr auto original = verified_u32{u32{0xDEADBEEFU}};
        constexpr auto bytes = to_be_bytes(original);
        constexpr auto reconstructed = from_be_bytes<verified_u32>(std::span<const std::byte, 4>{bytes});
        static_assert(reconstructed == original);
    }
    {
        constexpr auto original = verified_u64{u64{0x0123456789ABCDEFULL}};
        constexpr auto bytes = to_be_bytes(original);
        constexpr auto reconstructed = from_be_bytes<verified_u64>(std::span<const std::byte, 8>{bytes});
        static_assert(reconstructed == original);
    }
}

// =============================================================================
// from_be_bytes with dynamic extent: runtime size mismatch throws
// =============================================================================

void test_from_be_bytes_dynamic_size_mismatch()
{
    const std::array<std::byte, 2> bytes {std::byte{0x01}, std::byte{0x02}};
    std::span<const std::byte> dynamic_span {bytes};

    BOOST_TEST_THROWS(from_be_bytes<u32>(dynamic_span), std::domain_error);
}

void test_from_be_bytes_dynamic_size_match()
{
    const std::array<std::byte, 4> bytes {std::byte{0x01}, std::byte{0x02}, std::byte{0x03}, std::byte{0x04}};
    std::span<const std::byte> dynamic_span {bytes};

    const auto val = from_be_bytes<u32>(dynamic_span);
    BOOST_TEST(val == u32{0x01020304U});
}

int main()
{
    test_to_be_bytes_u8();
    test_to_be_bytes_u16();
    test_to_be_bytes_u32();
    test_to_be_bytes_u64();
    test_to_be_bytes_zero();

    test_from_be_bytes_u8();
    test_from_be_bytes_u16();
    test_from_be_bytes_u32();
    test_from_be_bytes_u64();
    test_from_be_bytes_zero();

    test_round_trips();

    test_verified_to_be_bytes();
    test_verified_from_be_bytes();
    test_verified_be_bytes_round_trip();

    test_from_be_bytes_dynamic_size_mismatch();
    test_from_be_bytes_dynamic_size_match();

    return boost::report_errors();
}

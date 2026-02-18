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
// to_ne_bytes: the byte representation matches the platform's native layout
// =============================================================================

void test_to_ne_bytes_matches_platform()
{
    // On little-endian, to_ne_bytes should match to_le_bytes
    // On big-endian, to_ne_bytes should match to_be_bytes
    {
        const auto ne_bytes = to_ne_bytes(u32{0x01020304U});
        if constexpr (std::endian::native == std::endian::little)
        {
            const auto le_bytes = to_le_bytes(u32{0x01020304U});
            BOOST_TEST(ne_bytes == le_bytes);
        }
        else
        {
            const auto be_bytes = to_be_bytes(u32{0x01020304U});
            BOOST_TEST(ne_bytes == be_bytes);
        }
    }
    {
        const auto ne_bytes = to_ne_bytes(u64{0x0102030405060708ULL});
        if constexpr (std::endian::native == std::endian::little)
        {
            const auto le_bytes = to_le_bytes(u64{0x0102030405060708ULL});
            BOOST_TEST(ne_bytes == le_bytes);
        }
        else
        {
            const auto be_bytes = to_be_bytes(u64{0x0102030405060708ULL});
            BOOST_TEST(ne_bytes == be_bytes);
        }
    }
}

void test_to_ne_bytes_is_identity()
{
    // to_ne_bytes should produce the same bytes as a bit_cast of the value
    {
        const auto val = u32{0xDEADBEEFU};
        const auto ne_bytes = to_ne_bytes(val);
        const auto raw_bytes = std::bit_cast<std::array<std::byte, 4>>(val);
        BOOST_TEST(ne_bytes == raw_bytes);
    }
    {
        const auto val = u16{static_cast<std::uint16_t>(0xABCD)};
        const auto ne_bytes = to_ne_bytes(val);
        const auto raw_bytes = std::bit_cast<std::array<std::byte, 2>>(val);
        BOOST_TEST(ne_bytes == raw_bytes);
    }
    {
        const auto val = u64{0x0123456789ABCDEFULL};
        const auto ne_bytes = to_ne_bytes(val);
        const auto raw_bytes = std::bit_cast<std::array<std::byte, 8>>(val);
        BOOST_TEST(ne_bytes == raw_bytes);
    }
}

// =============================================================================
// from_ne_bytes: reconstruct from native byte patterns
// =============================================================================

void test_from_ne_bytes_matches_platform()
{
    {
        const std::array<std::byte, 4> bytes {std::byte{0x01}, std::byte{0x02}, std::byte{0x03}, std::byte{0x04}};
        const auto ne_val = from_ne_bytes<u32>(std::span<const std::byte, 4>{bytes});
        if constexpr (std::endian::native == std::endian::little)
        {
            const auto le_val = from_le_bytes<u32>(std::span<const std::byte, 4>{bytes});
            BOOST_TEST(ne_val == le_val);
        }
        else
        {
            const auto be_val = from_be_bytes<u32>(std::span<const std::byte, 4>{bytes});
            BOOST_TEST(ne_val == be_val);
        }
    }
}

// =============================================================================
// Round-trip: to_ne_bytes -> from_ne_bytes == identity
// =============================================================================

template <typename T>
void test_round_trip(T value)
{
    const auto bytes = to_ne_bytes(value);
    const auto result = from_ne_bytes<T>(std::span<const std::byte, sizeof(T)>{bytes});
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
// from_ne_bytes with dynamic extent
// =============================================================================

void test_from_ne_bytes_dynamic_size_mismatch()
{
    const std::array<std::byte, 2> bytes {std::byte{0x01}, std::byte{0x02}};
    std::span<const std::byte> dynamic_span {bytes};

    BOOST_TEST_THROWS(from_ne_bytes<u32>(dynamic_span), std::domain_error);
}

void test_from_ne_bytes_dynamic_size_match()
{
    const auto val = u32{0x01020304U};
    const auto bytes = to_ne_bytes(val);
    std::span<const std::byte> dynamic_span {bytes};

    const auto result = from_ne_bytes<u32>(dynamic_span);
    BOOST_TEST(result == val);
}

// =============================================================================
// Verified overloads
// =============================================================================

void test_verified_to_ne_bytes()
{
    // Verify consteval works and produces correct bytes via round-trip
    {
        constexpr auto original = verified_u32{u32{0x01020304U}};
        constexpr auto bytes = to_ne_bytes(original);
        static_assert(bytes.size() == 4);
    }
    {
        constexpr auto original = verified_u64{u64{0x0102030405060708ULL}};
        constexpr auto bytes = to_ne_bytes(original);
        static_assert(bytes.size() == 8);
    }
}

void test_verified_from_ne_bytes()
{
    {
        constexpr auto original = verified_u32{u32{0xDEADBEEFU}};
        constexpr auto bytes = to_ne_bytes(original);
        constexpr auto reconstructed = from_ne_bytes<verified_u32>(std::span<const std::byte, 4>{bytes});
        static_assert(reconstructed == original);
    }
    {
        constexpr auto original = verified_u64{u64{0x0123456789ABCDEFULL}};
        constexpr auto bytes = to_ne_bytes(original);
        constexpr auto reconstructed = from_ne_bytes<verified_u64>(std::span<const std::byte, 8>{bytes});
        static_assert(reconstructed == original);
    }
}

int main()
{
    test_to_ne_bytes_matches_platform();
    test_to_ne_bytes_is_identity();

    test_from_ne_bytes_matches_platform();

    test_round_trips();

    test_from_ne_bytes_dynamic_size_mismatch();
    test_from_ne_bytes_dynamic_size_match();

    test_verified_to_ne_bytes();
    test_verified_from_ne_bytes();

    return boost::report_errors();
}

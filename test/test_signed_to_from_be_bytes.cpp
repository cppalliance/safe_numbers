// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/lightweight_test.hpp>

// Ignore [[nodiscard]] on the test that we know are going to throw
#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunused-result"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wunused-result"
#elif defined(_MSC_VER)
#  pragma warning (push)
#  pragma warning (disable: 4834)
#endif

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE
import boost.safe_numbers;
#else
#include <boost/safe_numbers.hpp>
#include <limits>
#include <type_traits>
#endif

using namespace boost::safe_numbers;

// =============================================================================
// to_be_bytes: known byte patterns for signed types
// =============================================================================

void test_to_be_bytes_i8()
{
    // Positive value
    {
        const auto bytes = to_be_bytes(i8{static_cast<std::int8_t>(0x42)});
        BOOST_TEST_EQ(bytes.size(), std::size_t{1});
        BOOST_TEST(bytes[0] == std::byte{0x42});
    }

    // Negative value: -1 is 0xFF in two's complement
    {
        const auto bytes = to_be_bytes(i8{static_cast<std::int8_t>(-1)});
        BOOST_TEST(bytes[0] == std::byte{0xFF});
    }

    // Negative value: -42 is 0xD6
    {
        const auto bytes = to_be_bytes(i8{static_cast<std::int8_t>(-42)});
        BOOST_TEST(bytes[0] == std::byte{0xD6});
    }
}

void test_to_be_bytes_i16()
{
    // Positive value: big-endian most significant byte first
    {
        const auto bytes = to_be_bytes(i16{static_cast<std::int16_t>(0x0102)});
        BOOST_TEST_EQ(bytes.size(), std::size_t{2});
        BOOST_TEST(bytes[0] == std::byte{0x01});
        BOOST_TEST(bytes[1] == std::byte{0x02});
    }

    // -1 is 0xFFFF
    {
        const auto bytes = to_be_bytes(i16{static_cast<std::int16_t>(-1)});
        BOOST_TEST(bytes[0] == std::byte{0xFF});
        BOOST_TEST(bytes[1] == std::byte{0xFF});
    }

    // -42 is 0xFFD6
    {
        const auto bytes = to_be_bytes(i16{static_cast<std::int16_t>(-42)});
        BOOST_TEST(bytes[0] == std::byte{0xFF});
        BOOST_TEST(bytes[1] == std::byte{0xD6});
    }
}

void test_to_be_bytes_i32()
{
    {
        const auto bytes = to_be_bytes(i32{static_cast<std::int32_t>(0x01020304)});
        BOOST_TEST_EQ(bytes.size(), std::size_t{4});
        BOOST_TEST(bytes[0] == std::byte{0x01});
        BOOST_TEST(bytes[1] == std::byte{0x02});
        BOOST_TEST(bytes[2] == std::byte{0x03});
        BOOST_TEST(bytes[3] == std::byte{0x04});
    }

    // -1 is 0xFFFFFFFF
    {
        const auto bytes = to_be_bytes(i32{static_cast<std::int32_t>(-1)});
        for (const auto& b : bytes)
        {
            BOOST_TEST(b == std::byte{0xFF});
        }
    }

    // -42 is 0xFFFFFFD6
    {
        const auto bytes = to_be_bytes(i32{static_cast<std::int32_t>(-42)});
        BOOST_TEST(bytes[0] == std::byte{0xFF});
        BOOST_TEST(bytes[1] == std::byte{0xFF});
        BOOST_TEST(bytes[2] == std::byte{0xFF});
        BOOST_TEST(bytes[3] == std::byte{0xD6});
    }
}

void test_to_be_bytes_i64()
{
    {
        const auto bytes = to_be_bytes(i64{static_cast<std::int64_t>(0x0102030405060708LL)});
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

    // -1 is all 0xFF
    {
        const auto bytes = to_be_bytes(i64{static_cast<std::int64_t>(-1)});
        for (const auto& b : bytes)
        {
            BOOST_TEST(b == std::byte{0xFF});
        }
    }
}

void test_to_be_bytes_zero()
{
    {
        const auto bytes = to_be_bytes(i8{static_cast<std::int8_t>(0)});
        BOOST_TEST(bytes[0] == std::byte{0x00});
    }
    {
        const auto bytes = to_be_bytes(i32{static_cast<std::int32_t>(0)});
        for (const auto& b : bytes)
        {
            BOOST_TEST(b == std::byte{0x00});
        }
    }
}

// =============================================================================
// from_be_bytes: reconstruct from known byte patterns
// =============================================================================

void test_from_be_bytes_i8()
{
    const std::array<std::byte, 1> bytes{std::byte{0xD6}};
    const auto val = from_be_bytes<i8>(std::span<const std::byte, 1>{bytes});
    BOOST_TEST(val == i8{static_cast<std::int8_t>(-42)});
}

void test_from_be_bytes_i16()
{
    // -42 in big-endian: 0xFF 0xD6
    const std::array<std::byte, 2> bytes{std::byte{0xFF}, std::byte{0xD6}};
    const auto val = from_be_bytes<i16>(std::span<const std::byte, 2>{bytes});
    BOOST_TEST(val == i16{static_cast<std::int16_t>(-42)});
}

void test_from_be_bytes_i32()
{
    const std::array<std::byte, 4> bytes{std::byte{0x01}, std::byte{0x02}, std::byte{0x03}, std::byte{0x04}};
    const auto val = from_be_bytes<i32>(std::span<const std::byte, 4>{bytes});
    BOOST_TEST(val == i32{static_cast<std::int32_t>(0x01020304)});
}

void test_from_be_bytes_i64()
{
    const std::array<std::byte, 8> bytes{
        std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF},
        std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF}
    };
    const auto val = from_be_bytes<i64>(std::span<const std::byte, 8>{bytes});
    BOOST_TEST(val == i64{static_cast<std::int64_t>(-1)});
}

void test_from_be_bytes_zero()
{
    const std::array<std::byte, 4> bytes{};
    const auto val = from_be_bytes<i32>(std::span<const std::byte, 4>{bytes});
    BOOST_TEST(val == i32{static_cast<std::int32_t>(0)});
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
    test_round_trip(i8{static_cast<std::int8_t>(0)});
    test_round_trip(i8{static_cast<std::int8_t>(-1)});
    test_round_trip(i8{static_cast<std::int8_t>(-42)});
    test_round_trip(i8{static_cast<std::int8_t>(127)});
    test_round_trip(i8{static_cast<std::int8_t>(-128)});

    test_round_trip(i16{static_cast<std::int16_t>(0)});
    test_round_trip(i16{static_cast<std::int16_t>(-1)});
    test_round_trip(i16{static_cast<std::int16_t>(-42)});
    test_round_trip(i16{std::numeric_limits<std::int16_t>::min()});
    test_round_trip(i16{std::numeric_limits<std::int16_t>::max()});

    test_round_trip(i32{static_cast<std::int32_t>(0)});
    test_round_trip(i32{static_cast<std::int32_t>(-1)});
    test_round_trip(i32{static_cast<std::int32_t>(-100000)});
    test_round_trip(i32{std::numeric_limits<std::int32_t>::min()});
    test_round_trip(i32{std::numeric_limits<std::int32_t>::max()});

    test_round_trip(i64{static_cast<std::int64_t>(0)});
    test_round_trip(i64{static_cast<std::int64_t>(-1)});
    test_round_trip(i64{static_cast<std::int64_t>(-1000000000000LL)});
    test_round_trip(i64{std::numeric_limits<std::int64_t>::min()});
    test_round_trip(i64{std::numeric_limits<std::int64_t>::max()});
}

// =============================================================================
// from_be_bytes with dynamic extent: runtime size mismatch throws
// =============================================================================

void test_from_be_bytes_dynamic_size_mismatch()
{
    const std::array<std::byte, 2> bytes{std::byte{0x01}, std::byte{0x02}};
    std::span<const std::byte> dynamic_span{bytes};

    BOOST_TEST_THROWS(from_be_bytes<i32>(dynamic_span), std::domain_error);
}

void test_from_be_bytes_dynamic_size_match()
{
    const std::array<std::byte, 4> bytes{std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF}, std::byte{0xD6}};
    std::span<const std::byte> dynamic_span{bytes};

    const auto val = from_be_bytes<i32>(dynamic_span);
    BOOST_TEST(val == i32{static_cast<std::int32_t>(-42)});
}

int main()
{
    test_to_be_bytes_i8();
    test_to_be_bytes_i16();
    test_to_be_bytes_i32();
    test_to_be_bytes_i64();
    test_to_be_bytes_zero();

    test_from_be_bytes_i8();
    test_from_be_bytes_i16();
    test_from_be_bytes_i32();
    test_from_be_bytes_i64();
    test_from_be_bytes_zero();

    test_round_trips();

    test_from_be_bytes_dynamic_size_mismatch();
    test_from_be_bytes_dynamic_size_match();

    return boost::report_errors();
}

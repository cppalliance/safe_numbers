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
// to_ne_bytes: the byte representation matches the platform's native layout
// =============================================================================

void test_to_ne_bytes_matches_platform()
{
    // On little-endian, to_ne_bytes should match to_le_bytes
    // On big-endian, to_ne_bytes should match to_be_bytes
    {
        const auto ne_bytes = to_ne_bytes(i32{static_cast<std::int32_t>(-42)});
        if constexpr (std::endian::native == std::endian::little)
        {
            const auto le_bytes = to_le_bytes(i32{static_cast<std::int32_t>(-42)});
            BOOST_TEST(ne_bytes == le_bytes);
        }
        else
        {
            const auto be_bytes = to_be_bytes(i32{static_cast<std::int32_t>(-42)});
            BOOST_TEST(ne_bytes == be_bytes);
        }
    }
    {
        const auto ne_bytes = to_ne_bytes(i64{static_cast<std::int64_t>(-1000000000000LL)});
        if constexpr (std::endian::native == std::endian::little)
        {
            const auto le_bytes = to_le_bytes(i64{static_cast<std::int64_t>(-1000000000000LL)});
            BOOST_TEST(ne_bytes == le_bytes);
        }
        else
        {
            const auto be_bytes = to_be_bytes(i64{static_cast<std::int64_t>(-1000000000000LL)});
            BOOST_TEST(ne_bytes == be_bytes);
        }
    }
}

void test_to_ne_bytes_is_identity()
{
    // to_ne_bytes should produce the same bytes as a bit_cast of the value
    {
        const auto val = i32{static_cast<std::int32_t>(-42)};
        const auto ne_bytes = to_ne_bytes(val);
        const auto raw_bytes = std::bit_cast<std::array<std::byte, 4>>(val);
        BOOST_TEST(ne_bytes == raw_bytes);
    }
    {
        const auto val = i16{static_cast<std::int16_t>(-1)};
        const auto ne_bytes = to_ne_bytes(val);
        const auto raw_bytes = std::bit_cast<std::array<std::byte, 2>>(val);
        BOOST_TEST(ne_bytes == raw_bytes);
    }
    {
        const auto val = i64{static_cast<std::int64_t>(-1000000000000LL)};
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
        const std::array<std::byte, 4> bytes{std::byte{0x01}, std::byte{0x02}, std::byte{0x03}, std::byte{0x04}};
        const auto ne_val = from_ne_bytes<i32>(std::span<const std::byte, 4>{bytes});
        if constexpr (std::endian::native == std::endian::little)
        {
            const auto le_val = from_le_bytes<i32>(std::span<const std::byte, 4>{bytes});
            BOOST_TEST(ne_val == le_val);
        }
        else
        {
            const auto be_val = from_be_bytes<i32>(std::span<const std::byte, 4>{bytes});
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
    test_round_trip(i8{static_cast<std::int8_t>(0)});
    test_round_trip(i8{static_cast<std::int8_t>(-1)});
    test_round_trip(i8{static_cast<std::int8_t>(-42)});
    test_round_trip(i8{static_cast<std::int8_t>(127)});
    test_round_trip(i8{static_cast<std::int8_t>(-128)});

    test_round_trip(i16{static_cast<std::int16_t>(0)});
    test_round_trip(i16{static_cast<std::int16_t>(-1)});
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
// from_ne_bytes with dynamic extent
// =============================================================================

void test_from_ne_bytes_dynamic_size_mismatch()
{
    const std::array<std::byte, 2> bytes{std::byte{0x01}, std::byte{0x02}};
    std::span<const std::byte> dynamic_span{bytes};

    BOOST_TEST_THROWS(from_ne_bytes<i32>(dynamic_span), std::domain_error);
}

void test_from_ne_bytes_dynamic_size_match()
{
    const auto val = i32{static_cast<std::int32_t>(-42)};
    const auto bytes = to_ne_bytes(val);
    std::span<const std::byte> dynamic_span{bytes};

    const auto result = from_ne_bytes<i32>(dynamic_span);
    BOOST_TEST(result == val);
}

int main()
{
    test_to_ne_bytes_matches_platform();
    test_to_ne_bytes_is_identity();

    test_from_ne_bytes_matches_platform();

    test_round_trips();

    test_from_ne_bytes_dynamic_size_mismatch();
    test_from_ne_bytes_dynamic_size_match();

    return boost::report_errors();
}

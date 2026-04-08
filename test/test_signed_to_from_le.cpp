// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE
import boost.safe_numbers;
#else
#include <boost/safe_numbers.hpp>
#include <limits>
#include <type_traits>
#endif

#include <boost/core/lightweight_test.hpp>

using namespace boost::safe_numbers;

// =============================================================================
// to_le followed by from_le should round-trip to the original value
// =============================================================================

template <typename T>
void test_round_trip()
{
    using basis_type = detail::underlying_type_t<T>;

    // Zero
    {
        const auto val = T{static_cast<basis_type>(0)};
        BOOST_TEST(from_le(to_le(val)) == val);
    }

    // One
    {
        const auto val = T{static_cast<basis_type>(1)};
        BOOST_TEST(from_le(to_le(val)) == val);
    }

    // Negative one
    {
        const auto val = T{static_cast<basis_type>(-1)};
        BOOST_TEST(from_le(to_le(val)) == val);
    }

    // Negative forty-two
    {
        const auto val = T{static_cast<basis_type>(-42)};
        BOOST_TEST(from_le(to_le(val)) == val);
    }

    // Min
    {
        const auto val = T{std::numeric_limits<basis_type>::min()};
        BOOST_TEST(from_le(to_le(val)) == val);
    }

    // Max
    {
        const auto val = T{std::numeric_limits<basis_type>::max()};
        BOOST_TEST(from_le(to_le(val)) == val);
    }

    // Arbitrary positive value
    {
        const auto val = T{static_cast<basis_type>(0x42)};
        BOOST_TEST(from_le(to_le(val)) == val);
    }

    if constexpr (sizeof(basis_type) >= 2)
    {
        const auto val = T{static_cast<basis_type>(-0x1234)};
        BOOST_TEST(from_le(to_le(val)) == val);
    }

    if constexpr (sizeof(basis_type) >= 4)
    {
        const auto val = T{static_cast<basis_type>(-100000)};
        BOOST_TEST(from_le(to_le(val)) == val);
    }

    if constexpr (sizeof(basis_type) >= 8)
    {
        const auto val = T{static_cast<basis_type>(-1000000000000LL)};
        BOOST_TEST(from_le(to_le(val)) == val);
    }
}

// =============================================================================
// to_le should produce the same result as manually byteswapping on big-endian,
// or be a no-op on little-endian
// =============================================================================

template <typename T>
void test_to_le_matches_byteswap()
{
    using basis_type = detail::underlying_type_t<T>;

    const auto val = T{static_cast<basis_type>(-1)};
    const auto le_val = to_le(val);

    if constexpr (std::endian::native == std::endian::little)
    {
        BOOST_TEST(le_val == val);
    }
    else
    {
        BOOST_TEST(le_val == byteswap(val));
    }

    if constexpr (sizeof(basis_type) >= 2)
    {
        const auto val2 = T{static_cast<basis_type>(-42)};
        const auto le_val2 = to_le(val2);

        if constexpr (std::endian::native == std::endian::little)
        {
            BOOST_TEST(le_val2 == val2);
        }
        else
        {
            BOOST_TEST(le_val2 == byteswap(val2));
        }
    }

    if constexpr (sizeof(basis_type) >= 4)
    {
        const auto val4 = T{static_cast<basis_type>(-100000)};
        const auto le_val4 = to_le(val4);

        if constexpr (std::endian::native == std::endian::little)
        {
            BOOST_TEST(le_val4 == val4);
        }
        else
        {
            BOOST_TEST(le_val4 == byteswap(val4));
        }
    }

    if constexpr (sizeof(basis_type) >= 8)
    {
        const auto val8 = T{static_cast<basis_type>(-1000000000000LL)};
        const auto le_val8 = to_le(val8);

        if constexpr (std::endian::native == std::endian::little)
        {
            BOOST_TEST(le_val8 == val8);
        }
        else
        {
            BOOST_TEST(le_val8 == byteswap(val8));
        }
    }
}

// =============================================================================
// from_le is the same operation as to_le (self-inverse)
// =============================================================================

template <typename T>
void test_from_le_equals_to_le()
{
    using basis_type = detail::underlying_type_t<T>;

    const auto val = T{static_cast<basis_type>(-1)};
    BOOST_TEST(from_le(val) == to_le(val));

    if constexpr (sizeof(basis_type) >= 2)
    {
        const auto val2 = T{static_cast<basis_type>(-42)};
        BOOST_TEST(from_le(val2) == to_le(val2));
    }

    if constexpr (sizeof(basis_type) >= 4)
    {
        const auto val4 = T{static_cast<basis_type>(-100000)};
        BOOST_TEST(from_le(val4) == to_le(val4));
    }
}

// =============================================================================
// Double application of to_le should return the original value
// =============================================================================

template <typename T>
void test_double_to_le_is_identity()
{
    using basis_type = detail::underlying_type_t<T>;

    {
        const auto val = T{static_cast<basis_type>(0)};
        BOOST_TEST(to_le(to_le(val)) == val);
    }

    {
        const auto val = T{static_cast<basis_type>(-1)};
        BOOST_TEST(to_le(to_le(val)) == val);
    }

    {
        const auto val = T{static_cast<basis_type>(-42)};
        BOOST_TEST(to_le(to_le(val)) == val);
    }

    {
        const auto val = T{std::numeric_limits<basis_type>::min()};
        BOOST_TEST(to_le(to_le(val)) == val);
    }

    {
        const auto val = T{std::numeric_limits<basis_type>::max()};
        BOOST_TEST(to_le(to_le(val)) == val);
    }
}

// =============================================================================
// Verify known byte patterns
// =============================================================================

void test_known_byte_patterns()
{
    if constexpr (std::endian::native == std::endian::little)
    {
        // On little-endian, to_le is identity
        {
            const auto val = i16{static_cast<std::int16_t>(-1)};
            BOOST_TEST(to_le(val) == val);
        }

        {
            const auto val = i32{static_cast<std::int32_t>(-42)};
            BOOST_TEST(to_le(val) == val);
        }

        {
            const auto val = i64{static_cast<std::int64_t>(-1000000000000LL)};
            BOOST_TEST(to_le(val) == val);
        }

        {
            const auto val = i8{static_cast<std::int8_t>(-1)};
            BOOST_TEST(to_le(val) == val);
        }
    }
    else
    {
        // On big-endian, to_le swaps bytes
        // i16: 0x0102 should become 0x0201
        {
            const auto val = i16{static_cast<std::int16_t>(0x0102)};
            const auto expected = i16{static_cast<std::int16_t>(0x0201)};
            BOOST_TEST(to_le(val) == expected);
        }

        // i32: 0x01020304 should become 0x04030201
        {
            const auto val = i32{static_cast<std::int32_t>(0x01020304)};
            const auto expected = i32{static_cast<std::int32_t>(0x04030201)};
            BOOST_TEST(to_le(val) == expected);
        }

        // i8: should be identity (single byte)
        {
            const auto val = i8{static_cast<std::int8_t>(-42)};
            BOOST_TEST(to_le(val) == val);
        }
    }
}

// =============================================================================
// to_le and to_be should both round-trip correctly
// =============================================================================

template <typename T>
void test_le_be_relationship()
{
    using basis_type = detail::underlying_type_t<T>;

    {
        const auto val = T{static_cast<basis_type>(-42)};
        BOOST_TEST(from_le(to_le(val)) == val);
        BOOST_TEST(from_be(to_be(val)) == val);
    }

    if constexpr (sizeof(basis_type) >= 4)
    {
        const auto val = T{static_cast<basis_type>(-100000)};
        BOOST_TEST(from_le(to_le(val)) == val);
        BOOST_TEST(from_be(to_be(val)) == val);
    }
}

int main()
{
    // Round-trip tests for all signed types
    test_round_trip<i8>();
    test_round_trip<i16>();
    test_round_trip<i32>();
    test_round_trip<i64>();
    // i128 skipped: boost::core::byteswap does not support int128_t

    // to_le matches byteswap on big-endian, identity on little-endian
    test_to_le_matches_byteswap<i8>();
    test_to_le_matches_byteswap<i16>();
    test_to_le_matches_byteswap<i32>();
    test_to_le_matches_byteswap<i64>();

    // from_le == to_le (self-inverse property)
    test_from_le_equals_to_le<i8>();
    test_from_le_equals_to_le<i16>();
    test_from_le_equals_to_le<i32>();
    test_from_le_equals_to_le<i64>();

    // Double application is identity
    test_double_to_le_is_identity<i8>();
    test_double_to_le_is_identity<i16>();
    test_double_to_le_is_identity<i32>();
    test_double_to_le_is_identity<i64>();
    // i128 skipped: boost::core::byteswap does not support int128_t

    // Known byte patterns
    test_known_byte_patterns();

    // Relationship between to_le and to_be
    test_le_be_relationship<i8>();
    test_le_be_relationship<i16>();
    test_le_be_relationship<i32>();
    test_le_be_relationship<i64>();

    return boost::report_errors();
}

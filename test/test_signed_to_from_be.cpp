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
// to_be followed by from_be should round-trip to the original value
// =============================================================================

template <typename T>
void test_round_trip()
{
    using basis_type = detail::underlying_type_t<T>;

    // Zero
    {
        const auto val = T{static_cast<basis_type>(0)};
        BOOST_TEST(from_be(to_be(val)) == val);
    }

    // One
    {
        const auto val = T{static_cast<basis_type>(1)};
        BOOST_TEST(from_be(to_be(val)) == val);
    }

    // Negative one
    {
        const auto val = T{static_cast<basis_type>(-1)};
        BOOST_TEST(from_be(to_be(val)) == val);
    }

    // Negative forty-two
    {
        const auto val = T{static_cast<basis_type>(-42)};
        BOOST_TEST(from_be(to_be(val)) == val);
    }

    // Min
    {
        const auto val = T{std::numeric_limits<basis_type>::min()};
        BOOST_TEST(from_be(to_be(val)) == val);
    }

    // Max
    {
        const auto val = T{std::numeric_limits<basis_type>::max()};
        BOOST_TEST(from_be(to_be(val)) == val);
    }

    // Arbitrary positive value
    {
        const auto val = T{static_cast<basis_type>(0x42)};
        BOOST_TEST(from_be(to_be(val)) == val);
    }

    if constexpr (sizeof(basis_type) >= 2)
    {
        const auto val = T{static_cast<basis_type>(-0x1234)};
        BOOST_TEST(from_be(to_be(val)) == val);
    }

    if constexpr (sizeof(basis_type) >= 4)
    {
        const auto val = T{static_cast<basis_type>(-100000)};
        BOOST_TEST(from_be(to_be(val)) == val);
    }

    if constexpr (sizeof(basis_type) >= 8)
    {
        const auto val = T{static_cast<basis_type>(-1000000000000LL)};
        BOOST_TEST(from_be(to_be(val)) == val);
    }
}

// =============================================================================
// to_be should produce the same result as manually byteswapping on little-endian,
// or be a no-op on big-endian
// =============================================================================

template <typename T>
void test_to_be_matches_byteswap()
{
    using basis_type = detail::underlying_type_t<T>;

    const auto val = T{static_cast<basis_type>(-1)};
    const auto be_val = to_be(val);

    if constexpr (std::endian::native == std::endian::big)
    {
        BOOST_TEST(be_val == val);
    }
    else
    {
        BOOST_TEST(be_val == byteswap(val));
    }

    if constexpr (sizeof(basis_type) >= 2)
    {
        const auto val2 = T{static_cast<basis_type>(-42)};
        const auto be_val2 = to_be(val2);

        if constexpr (std::endian::native == std::endian::big)
        {
            BOOST_TEST(be_val2 == val2);
        }
        else
        {
            BOOST_TEST(be_val2 == byteswap(val2));
        }
    }

    if constexpr (sizeof(basis_type) >= 4)
    {
        const auto val4 = T{static_cast<basis_type>(-100000)};
        const auto be_val4 = to_be(val4);

        if constexpr (std::endian::native == std::endian::big)
        {
            BOOST_TEST(be_val4 == val4);
        }
        else
        {
            BOOST_TEST(be_val4 == byteswap(val4));
        }
    }

    if constexpr (sizeof(basis_type) >= 8)
    {
        const auto val8 = T{static_cast<basis_type>(-1000000000000LL)};
        const auto be_val8 = to_be(val8);

        if constexpr (std::endian::native == std::endian::big)
        {
            BOOST_TEST(be_val8 == val8);
        }
        else
        {
            BOOST_TEST(be_val8 == byteswap(val8));
        }
    }
}

// =============================================================================
// from_be is the same operation as to_be (self-inverse)
// =============================================================================

template <typename T>
void test_from_be_equals_to_be()
{
    using basis_type = detail::underlying_type_t<T>;

    const auto val = T{static_cast<basis_type>(-1)};
    BOOST_TEST(from_be(val) == to_be(val));

    if constexpr (sizeof(basis_type) >= 2)
    {
        const auto val2 = T{static_cast<basis_type>(-42)};
        BOOST_TEST(from_be(val2) == to_be(val2));
    }

    if constexpr (sizeof(basis_type) >= 4)
    {
        const auto val4 = T{static_cast<basis_type>(-100000)};
        BOOST_TEST(from_be(val4) == to_be(val4));
    }
}

// =============================================================================
// Double application of to_be should return the original value
// =============================================================================

template <typename T>
void test_double_to_be_is_identity()
{
    using basis_type = detail::underlying_type_t<T>;

    {
        const auto val = T{static_cast<basis_type>(0)};
        BOOST_TEST(to_be(to_be(val)) == val);
    }

    {
        const auto val = T{static_cast<basis_type>(-1)};
        BOOST_TEST(to_be(to_be(val)) == val);
    }

    {
        const auto val = T{static_cast<basis_type>(-42)};
        BOOST_TEST(to_be(to_be(val)) == val);
    }

    {
        const auto val = T{std::numeric_limits<basis_type>::min()};
        BOOST_TEST(to_be(to_be(val)) == val);
    }

    {
        const auto val = T{std::numeric_limits<basis_type>::max()};
        BOOST_TEST(to_be(to_be(val)) == val);
    }
}

// =============================================================================
// Verify known byte patterns on little-endian systems for negative values
// =============================================================================

void test_known_byte_patterns()
{
    if constexpr (std::endian::native == std::endian::little)
    {
        // i8: single byte, should be identity
        {
            const auto val = i8{static_cast<std::int8_t>(-1)};
            BOOST_TEST(to_be(val) == val);
        }

        // i16: -1 is 0xFFFF in both byte orders, so to_be(-1) == -1
        {
            const auto val = i16{static_cast<std::int16_t>(-1)};
            BOOST_TEST(to_be(val) == val);
        }

        // i16: 0x0102 should become 0x0201
        {
            const auto val = i16{static_cast<std::int16_t>(0x0102)};
            const auto expected = i16{static_cast<std::int16_t>(0x0201)};
            BOOST_TEST(to_be(val) == expected);
        }

        // i32: 0x01020304 should become 0x04030201
        {
            const auto val = i32{static_cast<std::int32_t>(0x01020304)};
            const auto expected = i32{static_cast<std::int32_t>(0x04030201)};
            BOOST_TEST(to_be(val) == expected);
        }

        // i32: -1 is 0xFFFFFFFF, symmetric under byte swap
        {
            const auto val = i32{static_cast<std::int32_t>(-1)};
            BOOST_TEST(to_be(val) == val);
        }
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

    // to_be matches byteswap
    test_to_be_matches_byteswap<i8>();
    test_to_be_matches_byteswap<i16>();
    test_to_be_matches_byteswap<i32>();
    test_to_be_matches_byteswap<i64>();

    // from_be == to_be (self-inverse property)
    test_from_be_equals_to_be<i8>();
    test_from_be_equals_to_be<i16>();
    test_from_be_equals_to_be<i32>();
    test_from_be_equals_to_be<i64>();

    // Double application is identity
    test_double_to_be_is_identity<i8>();
    test_double_to_be_is_identity<i16>();
    test_double_to_be_is_identity<i32>();
    test_double_to_be_is_identity<i64>();
    // i128 skipped: boost::core::byteswap does not support int128_t

    // Known byte patterns on little-endian
    test_known_byte_patterns();

    return boost::report_errors();
}

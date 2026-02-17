// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/lightweight_test.hpp>

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers/byte_conversions.hpp>
#include <boost/safe_numbers/verified_integers.hpp>
#include <boost/safe_numbers/iostream.hpp>
#include <bit>
#include <cstdint>
#include <limits>

#endif

using namespace boost::safe_numbers;

// to_be followed by from_be should round-trip to the original value
template <typename T>
void test_round_trip()
{
    using basis_type = detail::underlying_type_t<T>;

    // Zero
    {
        const auto val = T{0U};
        BOOST_TEST(from_be(to_be(val)) == val);
    }

    // One
    {
        const auto val = T{1U};
        BOOST_TEST(from_be(to_be(val)) == val);
    }

    // Max
    {
        const auto val = T{std::numeric_limits<basis_type>::max()};
        BOOST_TEST(from_be(to_be(val)) == val);
    }

    // Arbitrary values
    {
        const auto val = T{static_cast<basis_type>(0x42)};
        BOOST_TEST(from_be(to_be(val)) == val);
    }

    if constexpr (sizeof(basis_type) >= 2)
    {
        const auto val = T{static_cast<basis_type>(0xABCD)};
        BOOST_TEST(from_be(to_be(val)) == val);
    }

    if constexpr (sizeof(basis_type) >= 4)
    {
        const auto val = T{static_cast<basis_type>(0xDEADBEEF)};
        BOOST_TEST(from_be(to_be(val)) == val);
    }

    if constexpr (sizeof(basis_type) >= 8)
    {
        const auto val = T{static_cast<basis_type>(0x0123456789ABCDEFULL)};
        BOOST_TEST(from_be(to_be(val)) == val);
    }
}

// to_be should produce the same result as manually byteswapping on little-endian,
// or be a no-op on big-endian
template <typename T>
void test_to_be_matches_byteswap()
{
    using basis_type = detail::underlying_type_t<T>;

    const auto val = T{static_cast<basis_type>(0x42)};
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
        const auto val2 = T{static_cast<basis_type>(0xABCD)};
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
        const auto val4 = T{static_cast<basis_type>(0xDEADBEEF)};
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
        const auto val8 = T{static_cast<basis_type>(0x0123456789ABCDEFULL)};
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

// from_be is the same operation as to_be (self-inverse)
template <typename T>
void test_from_be_equals_to_be()
{
    using basis_type = detail::underlying_type_t<T>;

    const auto val = T{static_cast<basis_type>(0x42)};
    BOOST_TEST(from_be(val) == to_be(val));

    if constexpr (sizeof(basis_type) >= 2)
    {
        const auto val2 = T{static_cast<basis_type>(0xABCD)};
        BOOST_TEST(from_be(val2) == to_be(val2));
    }

    if constexpr (sizeof(basis_type) >= 4)
    {
        const auto val4 = T{static_cast<basis_type>(0xDEADBEEF)};
        BOOST_TEST(from_be(val4) == to_be(val4));
    }
}

// Double application of to_be should return the original value
template <typename T>
void test_double_to_be_is_identity()
{
    using basis_type = detail::underlying_type_t<T>;

    {
        const auto val = T{0U};
        BOOST_TEST(to_be(to_be(val)) == val);
    }

    {
        const auto val = T{std::numeric_limits<basis_type>::max()};
        BOOST_TEST(to_be(to_be(val)) == val);
    }

    {
        const auto val = T{static_cast<basis_type>(0x42)};
        BOOST_TEST(to_be(to_be(val)) == val);
    }
}

// Verify known byte patterns on little-endian systems
void test_known_byte_patterns()
{
    if constexpr (std::endian::native == std::endian::little)
    {
        // u16: 0x0102 should become 0x0201
        {
            const auto val = u16{static_cast<std::uint16_t>(0x0102)};
            const auto expected = u16{static_cast<std::uint16_t>(0x0201)};
            BOOST_TEST(to_be(val) == expected);
        }

        // u32: 0x01020304 should become 0x04030201
        {
            const auto val = u32{0x01020304U};
            const auto expected = u32{0x04030201U};
            BOOST_TEST(to_be(val) == expected);
        }

        // u64: 0x0102030405060708 should become 0x0807060504030201
        {
            const auto val = u64{0x0102030405060708ULL};
            const auto expected = u64{0x0807060504030201ULL};
            BOOST_TEST(to_be(val) == expected);
        }

        // u8: should be identity (single byte)
        {
            const auto val = u8{0xAB};
            BOOST_TEST(to_be(val) == val);
        }
    }
}

// Verified types: to_be and from_be should work at consteval
void test_verified_consteval()
{
    // verified_u8 round-trip
    {
        constexpr auto val = verified_u8{u8{0x42}};
        constexpr auto be_val = to_be(val);
        constexpr auto rt_val = from_be(be_val);
        BOOST_TEST(rt_val == val);
    }

    // verified_u16 round-trip
    {
        constexpr auto val = verified_u16{u16{static_cast<std::uint16_t>(0xABCD)}};
        constexpr auto be_val = to_be(val);
        constexpr auto rt_val = from_be(be_val);
        BOOST_TEST(rt_val == val);
    }

    // verified_u32 round-trip
    {
        constexpr auto val = verified_u32{u32{0xDEADBEEFU}};
        constexpr auto be_val = to_be(val);
        constexpr auto rt_val = from_be(be_val);
        BOOST_TEST(rt_val == val);
    }

    // verified_u64 round-trip
    {
        constexpr auto val = verified_u64{u64{0x0123456789ABCDEFULL}};
        constexpr auto be_val = to_be(val);
        constexpr auto rt_val = from_be(be_val);
        BOOST_TEST(rt_val == val);
    }

    // Verify known pattern for verified_u32 on little-endian
    if constexpr (std::endian::native == std::endian::little)
    {
        constexpr auto val = verified_u32{u32{0x01020304U}};
        constexpr auto expected = verified_u32{u32{0x04030201U}};
        constexpr auto be_val = to_be(val);
        BOOST_TEST(be_val == expected);
    }
}

int main()
{
    // Round-trip tests for all unsigned types
    test_round_trip<u8>();
    test_round_trip<u16>();
    test_round_trip<u32>();
    test_round_trip<u64>();
    test_round_trip<u128>();

    // to_be matches byteswap
    test_to_be_matches_byteswap<u8>();
    test_to_be_matches_byteswap<u16>();
    test_to_be_matches_byteswap<u32>();
    test_to_be_matches_byteswap<u64>();

    // from_be == to_be (self-inverse property)
    test_from_be_equals_to_be<u8>();
    test_from_be_equals_to_be<u16>();
    test_from_be_equals_to_be<u32>();
    test_from_be_equals_to_be<u64>();

    // Double application is identity
    test_double_to_be_is_identity<u8>();
    test_double_to_be_is_identity<u16>();
    test_double_to_be_is_identity<u32>();
    test_double_to_be_is_identity<u64>();
    test_double_to_be_is_identity<u128>();

    // Known byte patterns on little-endian
    test_known_byte_patterns();

    // Verified types (consteval)
    test_verified_consteval();

    return boost::report_errors();
}

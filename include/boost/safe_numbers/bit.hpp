// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_BIT_HPP
#define BOOST_SAFE_NUMBERS_BIT_HPP

#include <boost/safe_numbers/detail/unsigned_integer_basis.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>
#include <boost/safe_numbers/detail/int128/bit.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/core/bit.hpp>
#include <array>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers {

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
[[nodiscard]] constexpr auto has_single_bit(const UnsignedInt x) noexcept -> bool
{
    using boost::core::has_single_bit;
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    return has_single_bit(static_cast<underlying_type>(x));
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
[[nodiscard]] constexpr auto bit_ceil(const UnsignedInt x) noexcept -> UnsignedInt
{
    using boost::core::bit_ceil;
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    return UnsignedInt{bit_ceil(static_cast<underlying_type>(x))};
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
[[nodiscard]] constexpr auto bit_floor(const UnsignedInt x) noexcept -> UnsignedInt
{
    using boost::core::bit_floor;
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    return UnsignedInt{bit_floor(static_cast<underlying_type>(x))};
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
[[nodiscard]] constexpr auto bit_width(const UnsignedInt x) noexcept -> int
{
    using boost::core::bit_width;
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    return static_cast<int>(bit_width(static_cast<underlying_type>(x)));
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::non_bounded_unsigned_library_type UnsignedInt>
[[nodiscard]] constexpr auto rotl(const UnsignedInt x, const int s) noexcept -> UnsignedInt
{
    using boost::core::rotl;
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    return UnsignedInt{rotl(static_cast<underlying_type>(x), s)};
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::non_bounded_unsigned_library_type UnsignedInt>
[[nodiscard]] constexpr auto rotr(const UnsignedInt x, const int s) noexcept -> UnsignedInt
{
    using boost::core::rotr;
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    return UnsignedInt{rotr(static_cast<underlying_type>(x), s)};
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
[[nodiscard]] constexpr auto countl_zero(const UnsignedInt x) noexcept -> int
{
    using boost::core::countl_zero;
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    return countl_zero(static_cast<underlying_type>(x));
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
[[nodiscard]] constexpr auto countl_one(const UnsignedInt x) noexcept -> int
{
    using boost::core::countl_one;
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    return countl_one(static_cast<underlying_type>(x));
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
[[nodiscard]] constexpr auto countr_zero(const UnsignedInt x) noexcept -> int
{
    using boost::core::countr_zero;
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    return countr_zero(static_cast<underlying_type>(x));
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
[[nodiscard]] constexpr auto countr_one(const UnsignedInt x) noexcept -> int
{
    using boost::core::countr_one;
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    return countr_one(static_cast<underlying_type>(x));
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
[[nodiscard]] constexpr auto popcount(const UnsignedInt x) noexcept -> int
{
    using boost::core::popcount;
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    return popcount(static_cast<underlying_type>(x));
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::non_bounded_integral_library_type Int>
[[nodiscard]] constexpr auto byteswap(const Int x) noexcept -> Int
{
    using boost::core::byteswap;
    using underlying_type = detail::underlying_type_t<Int>;

    return Int{byteswap(static_cast<underlying_type>(x))};
}

namespace detail {

consteval auto reverse_byte(std::uint8_t b) noexcept -> std::uint8_t
{
    b = static_cast<std::uint8_t>(((b & UINT8_C(0x55)) << 1u) | ((b & UINT8_C(0xAA)) >> 1u));
    b = static_cast<std::uint8_t>(((b & UINT8_C(0x33)) << 2u) | ((b & UINT8_C(0xCC)) >> 2u));
    b = static_cast<std::uint8_t>(((b & UINT8_C(0x0F)) << 4u) | ((b & UINT8_C(0xF0)) >> 4u));

    return b;
}

consteval auto make_byte_reverse_table() -> std::array<std::uint8_t, 256>
{
    std::array<std::uint8_t, 256> table {};

    for (int i {}; i < 256; ++i)
    {
        table[i] = reverse_byte(static_cast<std::uint8_t>(i));
    }

    return table;
}

inline constexpr auto reverse_table {make_byte_reverse_table()};

template <fundamental_unsigned_integral UnsignedInt>
[[nodiscard]] constexpr auto bitswap_impl(UnsignedInt x) noexcept -> UnsignedInt
{
    if constexpr (sizeof(UnsignedInt) == 1)
    {
        return static_cast<UnsignedInt>(reverse_table[static_cast<std::uint8_t>(x)]);
    }
    else
    {
        constexpr auto n {sizeof(UnsignedInt)};

        UnsignedInt result {};
        for (std::size_t i {}; i < n; ++i)
        {
            result = static_cast<UnsignedInt>(static_cast<UnsignedInt>(result << 8U) |
                     static_cast<UnsignedInt>(reverse_table[static_cast<std::uint8_t>(x & 0xFFU)]));
            x >>= 8U;
        }

        return result;
    }
}

} // namespace detail

BOOST_SAFE_NUMBERS_EXPORT template <detail::non_bounded_integral_library_type Int>
[[nodiscard]] constexpr auto bitswap(Int x) noexcept -> Int
{
    using underlying_type = detail::underlying_type_t<Int>;
    return static_cast<Int>(detail::bitswap_impl(static_cast<underlying_type>(x)));
}

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_BIT_HPP

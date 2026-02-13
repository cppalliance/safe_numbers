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
    requires (!detail::is_verified_type_v<UnsignedInt>)
[[nodiscard]] constexpr auto bit_ceil(const UnsignedInt x) noexcept -> UnsignedInt
{
    using boost::core::bit_ceil;
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    return UnsignedInt{bit_ceil(static_cast<underlying_type>(x))};
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
    requires detail::is_verified_type_v<UnsignedInt>
[[nodiscard]] consteval auto bit_ceil(const UnsignedInt x) noexcept -> UnsignedInt
{
    using boost::core::bit_ceil;
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    return UnsignedInt{bit_ceil(static_cast<underlying_type>(x))};
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
    requires (!detail::is_verified_type_v<UnsignedInt>)
[[nodiscard]] constexpr auto bit_floor(const UnsignedInt x) noexcept -> UnsignedInt
{
    using boost::core::bit_floor;
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    return UnsignedInt{bit_floor(static_cast<underlying_type>(x))};
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
    requires detail::is_verified_type_v<UnsignedInt>
[[nodiscard]] consteval auto bit_floor(const UnsignedInt x) noexcept -> UnsignedInt
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
    requires (!detail::is_verified_type_v<UnsignedInt>)
[[nodiscard]] constexpr auto rotl(const UnsignedInt x, const int s) noexcept -> UnsignedInt
{
    using boost::core::rotl;
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    return UnsignedInt{rotl(static_cast<underlying_type>(x), s)};
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::non_bounded_unsigned_library_type UnsignedInt>
    requires detail::is_verified_type_v<UnsignedInt>
[[nodiscard]] consteval auto rotl(const UnsignedInt x, const int s) noexcept -> UnsignedInt
{
    using boost::core::rotl;
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    return UnsignedInt{rotl(static_cast<underlying_type>(x), s)};
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::non_bounded_unsigned_library_type UnsignedInt>
    requires (!detail::is_verified_type_v<UnsignedInt>)
[[nodiscard]] constexpr auto rotr(const UnsignedInt x, const int s) noexcept -> UnsignedInt
{
    using boost::core::rotr;
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    return UnsignedInt{rotr(static_cast<underlying_type>(x), s)};
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::non_bounded_unsigned_library_type UnsignedInt>
    requires detail::is_verified_type_v<UnsignedInt>
[[nodiscard]] consteval auto rotr(const UnsignedInt x, const int s) noexcept -> UnsignedInt
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

BOOST_SAFE_NUMBERS_EXPORT template <detail::non_bounded_unsigned_library_type UnsignedInt>
    requires (!detail::is_verified_type_v<UnsignedInt>)
[[nodiscard]] constexpr auto byteswap(const UnsignedInt x) noexcept -> UnsignedInt
{
    using boost::core::byteswap;
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    return UnsignedInt{byteswap(static_cast<underlying_type>(x))};
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::non_bounded_unsigned_library_type UnsignedInt>
    requires detail::is_verified_type_v<UnsignedInt>
[[nodiscard]] consteval auto byteswap(const UnsignedInt x) noexcept -> UnsignedInt
{
    using boost::core::byteswap;
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    return UnsignedInt{byteswap(static_cast<underlying_type>(x))};
}

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_BIT_HPP

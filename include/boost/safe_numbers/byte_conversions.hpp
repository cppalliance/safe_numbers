// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_BYTE_CONVERSIONS_HPP
#define BOOST_SAFE_NUMBERS_BYTE_CONVERSIONS_HPP

#include <boost/safe_numbers/bit.hpp>
#include <boost/safe_numbers/verified_integers.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <array>
#include <bit>
#include <cstddef>
#include <span>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers {

template <detail::non_bounded_integral_library_type T>
    requires (!detail::is_verified_type_v<T>)
constexpr auto to_be(const T value) noexcept -> T
{
    if constexpr (std::endian::native == std::endian::big)
    {
        return value;
    }
    else
    {
        return byteswap(value);
    }
}

template <detail::non_bounded_integral_library_type T>
consteval auto to_be(const detail::verified_type_basis<T> value) noexcept -> detail::verified_type_basis<T>
{
    if constexpr (std::endian::native == std::endian::big)
    {
        return value;
    }
    else
    {
        return byteswap(value);
    }
}

template <detail::non_bounded_integral_library_type T>
    requires (!detail::is_verified_type_v<T>)
constexpr auto from_be(const T value) noexcept -> T
{
    // Self-inverse
    return to_be(value);
}

template <detail::non_bounded_integral_library_type T>
consteval auto from_be(const detail::verified_type_basis<T> value) noexcept -> detail::verified_type_basis<T>
{
    // Self-inverse
    return to_be(value);
}

template <detail::non_bounded_integral_library_type T>
    requires (!detail::is_verified_type_v<T>)
constexpr auto to_le(const T value) noexcept -> T
{
    if constexpr (std::endian::native == std::endian::little)
    {
        return value;
    }
    else
    {
        return byteswap(value);
    }
}

template <detail::non_bounded_integral_library_type T>
consteval auto to_le(const detail::verified_type_basis<T> value) noexcept -> detail::verified_type_basis<T>
{
    if constexpr (std::endian::native == std::endian::little)
    {
        return value;
    }
    else
    {
        return byteswap(value);
    }
}

template <detail::non_bounded_integral_library_type T>
    requires (!detail::is_verified_type_v<T>)
constexpr auto from_le(const T value) noexcept -> T
{
    // Self-inverse
    return to_le(value);
}

template <detail::non_bounded_integral_library_type T>
consteval auto from_le(const detail::verified_type_basis<T> value) noexcept -> detail::verified_type_basis<T>
{
    // Self-inverse
    return to_le(value);
}

template <detail::non_bounded_integral_library_type T>
    requires (!detail::is_verified_type_v<T>)
constexpr auto to_be_bytes(const T value) noexcept -> std::array<std::byte, sizeof(T)>
{
    const auto be_value {to_be(value)};
    return std::bit_cast<std::array<std::byte, sizeof(T)>>(be_value);
}

template <detail::non_bounded_integral_library_type T>
consteval auto to_be_bytes(const detail::verified_type_basis<T> value) noexcept -> std::array<std::byte, sizeof(T)>
{
    const auto be_value {to_be(value)};
    return std::bit_cast<std::array<std::byte, sizeof(T)>>(be_value);
}

template <detail::non_bounded_integral_library_type T, std::size_t N>
    requires (!detail::is_verified_type_v<T>)
constexpr auto from_be_bytes(const std::span<const std::byte, N> bytes) -> T
{
    using underlying_type = detail::underlying_type_t<T>;

    if constexpr (N == sizeof(T))
    {
        std::array<std::byte, sizeof(T)> arr {};
        for (std::size_t i {}; i < N; ++i)
        {
            arr[i] = bytes[i];
        }
        return from_be(T{std::bit_cast<underlying_type>(arr)});
    }
    else if constexpr (N != std::dynamic_extent)
    {
        static_assert(detail::dependent_false<T>, "The number of bytes provided, and the target type number of bytes do not match");
        return T{}; // LCOV_EXCL_LINE
    }
    else
    {
        if (bytes.size_bytes() != sizeof(T))
        {
            BOOST_THROW_EXCEPTION(std::domain_error("The number of bytes provided, and the target type number of bytes do not match"));
        }

        std::array<std::byte, sizeof(T)> arr {};
        for (std::size_t i {}; i < sizeof(T); ++i)
        {
            arr[i] = bytes[i];
        }
        return from_be(T{std::bit_cast<underlying_type>(arr)});
    }
}

template <detail::verified_type T, std::size_t N>
consteval auto from_be_bytes(const std::span<const std::byte, N> bytes) -> T
{
    using underlying_type = detail::underlying_type_t<T>;
    static_assert(N == sizeof(T), "The number of bytes provided, and the target type number of bytes do not match");

    std::array<std::byte, sizeof(T)> arr {};
    for (std::size_t i {}; i < N; ++i)
    {
        arr[i] = bytes[i];
    }
    return from_be(T{std::bit_cast<underlying_type>(arr)});
}

template <detail::non_bounded_integral_library_type T>
    requires (!detail::is_verified_type_v<T>)
constexpr auto to_le_bytes(const T value) noexcept -> std::array<std::byte, sizeof(T)>
{
    const auto le_value {to_le(value)};
    return std::bit_cast<std::array<std::byte, sizeof(T)>>(le_value);
}

template <detail::non_bounded_integral_library_type T>
consteval auto to_le_bytes(const detail::verified_type_basis<T> value) noexcept -> std::array<std::byte, sizeof(T)>
{
    const auto le_value {to_le(value)};
    return std::bit_cast<std::array<std::byte, sizeof(T)>>(le_value);
}

template <detail::non_bounded_integral_library_type T, std::size_t N>
    requires (!detail::is_verified_type_v<T>)
constexpr auto from_le_bytes(const std::span<const std::byte, N> bytes) -> T
{
    using underlying_type = detail::underlying_type_t<T>;

    if constexpr (N == sizeof(T))
    {
        std::array<std::byte, sizeof(T)> arr {};
        for (std::size_t i {}; i < N; ++i)
        {
            arr[i] = bytes[i];
        }
        return from_le(T{std::bit_cast<underlying_type>(arr)});
    }
    else if constexpr (N != std::dynamic_extent)
    {
        static_assert(detail::dependent_false<T>, "The number of bytes provided, and the target type number of bytes do not match");
        return T{}; // LCOV_EXCL_LINE
    }
    else
    {
        if (bytes.size_bytes() != sizeof(T))
        {
            BOOST_THROW_EXCEPTION(std::domain_error("The number of bytes provided, and the target type number of bytes do not match"));
        }

        std::array<std::byte, sizeof(T)> arr {};
        for (std::size_t i {}; i < sizeof(T); ++i)
        {
            arr[i] = bytes[i];
        }
        return from_le(T{std::bit_cast<underlying_type>(arr)});
    }
}

template <detail::verified_type T, std::size_t N>
consteval auto from_le_bytes(const std::span<const std::byte, N> bytes) -> T
{
    using underlying_type = detail::underlying_type_t<T>;
    static_assert(N == sizeof(T), "The number of bytes provided, and the target type number of bytes do not match");

    std::array<std::byte, sizeof(T)> arr {};
    for (std::size_t i {}; i < N; ++i)
    {
        arr[i] = bytes[i];
    }
    return from_le(T{std::bit_cast<underlying_type>(arr)});
}

template <detail::non_bounded_integral_library_type T>
    requires (!detail::is_verified_type_v<T>)
constexpr auto to_ne_bytes(const T value) noexcept -> std::array<std::byte, sizeof(T)>
{
    if constexpr (std::endian::native == std::endian::little)
    {
        return to_le_bytes(value);
    }
    else
    {
        return to_be_bytes(value);
    }
}

template <detail::non_bounded_integral_library_type T>
consteval auto to_ne_bytes(const detail::verified_type_basis<T> value) noexcept -> std::array<std::byte, sizeof(T)>
{
    if constexpr (std::endian::native == std::endian::little)
    {
        return to_le_bytes(value);
    }
    else
    {
        return to_be_bytes(value);
    }
}

template <detail::non_bounded_integral_library_type T, std::size_t N>
    requires (!detail::is_verified_type_v<T>)
constexpr auto from_ne_bytes(const std::span<const std::byte, N> bytes) -> T
{
    if constexpr (std::endian::native == std::endian::little)
    {
        return from_le_bytes<T>(bytes);
    }
    else
    {
        return from_be_bytes<T>(bytes);
    }
}

template <detail::verified_type T, std::size_t N>
consteval auto from_ne_bytes(const std::span<const std::byte, N> bytes) -> T
{
    if constexpr (std::endian::native == std::endian::little)
    {
        return from_le_bytes<T>(bytes);
    }
    else
    {
        return from_be_bytes<T>(bytes);
    }
}

} // namespace boost::safe_numbers

#endif //BOOST_SAFE_NUMBERS_BYTE_CONVERSIONS_HPP

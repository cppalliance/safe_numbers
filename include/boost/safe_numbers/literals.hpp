// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_LITERALS_HPP
#define BOOST_SAFE_NUMBERS_LITERALS_HPP

#include <boost/safe_numbers/detail/int128/literals.hpp>
#include <boost/safe_numbers/detail/throw_exception.hpp>
#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/signed_integers.hpp>
#include <boost/safe_numbers/floats.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <limits>
#include <cstdint>

#endif

namespace boost::safe_numbers::literals {

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable:4702) // Unreachable code is only true with exceptions enabled
#endif

BOOST_SAFE_NUMBERS_EXPORT constexpr auto operator ""_u8(const unsigned long long int val) -> u8
{
    if (constexpr unsigned long long int max_value {std::numeric_limits<std::uint8_t>::max()}; val > max_value)
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "Overflow detected in literal construction");
    }

    return static_cast<u8>(static_cast<std::uint8_t>(val));
}

BOOST_SAFE_NUMBERS_EXPORT constexpr auto operator ""_u16(const unsigned long long int val) -> u16
{
    if (constexpr unsigned long long int max_value {std::numeric_limits<std::uint16_t>::max()}; val > max_value)
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "Overflow detected in literal construction");
    }

    return static_cast<u16>(static_cast<std::uint16_t>(val));
}

BOOST_SAFE_NUMBERS_EXPORT constexpr auto operator ""_u32(const unsigned long long int val) -> u32
{
    if (constexpr unsigned long long int max_value {std::numeric_limits<std::uint32_t>::max()}; val > max_value)
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "Overflow detected in literal construction");
    }

    return static_cast<u32>(static_cast<std::uint32_t>(val));
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

BOOST_SAFE_NUMBERS_EXPORT constexpr auto operator ""_u64(const unsigned long long int val) noexcept -> u64
{
    static_assert(std::numeric_limits<std::uint64_t>::max() == std::numeric_limits<unsigned long long int>::max());
    return static_cast<u64>(static_cast<std::uint64_t>(val));
}

BOOST_SAFE_NUMBERS_EXPORT constexpr auto operator ""_u128(const char* str) -> u128
{
    int128::uint128_t result;
    const auto r {int128::detail::from_chars(str, str + int128::detail::strlen(str), result)};

    if (r == EDOM)
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "Overflow detected in literal construction");
    }
    else if (r == EINVAL)
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::invalid_argument, "Invalid conversion from literal");
    }

    return u128{result};
}

BOOST_SAFE_NUMBERS_EXPORT constexpr auto operator ""_i8(const unsigned long long int val) -> i8
{
    if (constexpr unsigned long long int max_value {std::numeric_limits<std::int8_t>::max()}; val > max_value)
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "Overflow detected in literal construction");
    }

    return static_cast<i8>(static_cast<std::int8_t>(val));
}

BOOST_SAFE_NUMBERS_EXPORT constexpr auto operator ""_i16(const unsigned long long int val) -> i16
{
    if (constexpr unsigned long long int max_value {std::numeric_limits<std::int16_t>::max()}; val > max_value)
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "Overflow detected in literal construction");
    }

    return static_cast<i16>(static_cast<std::int16_t>(val));
}

BOOST_SAFE_NUMBERS_EXPORT constexpr auto operator ""_i32(const unsigned long long int val) -> i32
{
    if (constexpr unsigned long long int max_value {std::numeric_limits<std::int32_t>::max()}; val > max_value)
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "Overflow detected in literal construction");
    }

    return static_cast<i32>(static_cast<std::int32_t>(val));
}

BOOST_SAFE_NUMBERS_EXPORT constexpr auto operator ""_i64(const unsigned long long int val) -> i64
{
    if (constexpr unsigned long long int max_value {std::numeric_limits<std::int64_t>::max()}; val > max_value)
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "Overflow detected in literal construction");
    }

    return static_cast<i64>(static_cast<std::int64_t>(val));
}

BOOST_SAFE_NUMBERS_EXPORT constexpr auto operator ""_i128(const char* str) -> i128
{
    int128::int128_t result;
    const auto r {int128::detail::from_chars(str, str + int128::detail::strlen(str), result)};

    if (r == EDOM)
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "Overflow detected in literal construction");
    }
    else if (r == EINVAL)
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::invalid_argument, "Invalid conversion from literal");
    }

    return i128{result};
}

BOOST_SAFE_NUMBERS_EXPORT constexpr auto operator ""_f32(const long double val) -> f32
{
    if (constexpr auto max_value {static_cast<long double>(std::numeric_limits<float>::max())}; val > max_value)
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "Overflow detected in literal construction");
    }

    return static_cast<f32>(static_cast<float>(val));
}

BOOST_SAFE_NUMBERS_EXPORT constexpr auto operator ""_f64(const long double val) -> f64
{
    if (constexpr auto max_value {static_cast<long double>(std::numeric_limits<double>::max())}; val > max_value)
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "Overflow detected in literal construction");
    }

    return static_cast<f64>(static_cast<double>(val));
}

}  // boost::safe_numbers::literals

#endif // BOOST_SAFE_NUMBERS_LITERALS_HPP

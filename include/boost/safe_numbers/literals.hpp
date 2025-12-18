// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_LITERALS_HPP
#define BOOST_SAFE_NUMBERS_LITERALS_HPP

#include <boost/safe_numbers/unsigned_integers.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/throw_exception.hpp>
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
        BOOST_THROW_EXCEPTION(std::overflow_error("Overflow detected in literal construction"));
        return std::numeric_limits<u8>::max();
    }

    return static_cast<u8>(static_cast<std::uint8_t>(val));
}

BOOST_SAFE_NUMBERS_EXPORT constexpr auto operator ""_u16(const unsigned long long int val) -> u16
{
    if (constexpr unsigned long long int max_value {std::numeric_limits<std::uint16_t>::max()}; val > max_value)
    {
        BOOST_THROW_EXCEPTION(std::overflow_error("Overflow detected in literal construction"));
        return std::numeric_limits<u16>::max();
    }

    return static_cast<u16>(static_cast<std::uint16_t>(val));
}

BOOST_SAFE_NUMBERS_EXPORT constexpr auto operator ""_u32(const unsigned long long int val) -> u32
{
    if (constexpr unsigned long long int max_value {std::numeric_limits<std::uint32_t>::max()}; val > max_value)
    {
        BOOST_THROW_EXCEPTION(std::overflow_error("Overflow detected in literal construction"));
        return std::numeric_limits<u32>::max();
    }

    return static_cast<u32>(static_cast<std::uint32_t>(val));
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

BOOST_SAFE_NUMBERS_EXPORT constexpr auto operator ""_u64(const unsigned long long int val) -> u64
{
    return static_cast<u64>(static_cast<std::uint64_t>(val));
}

}  // boost::safe_numbers::literals

#endif // BOOST_SAFE_NUMBERS_LITERALS_HPP

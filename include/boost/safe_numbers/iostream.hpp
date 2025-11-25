// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFENUMBERS_IOSTREAM_HPP
#define BOOST_SAFENUMBERS_IOSTREAM_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/unsigned_integers.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <concepts>
#include <type_traits>

#endif // ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers {

namespace detail {

template <typename T>
struct is_library_type : std::false_type {};

template <>
struct is_library_type<u32> : std::true_type {};

template <>
struct is_library_type<u64> : std::true_type {};

template <typename T>
inline constexpr bool is_library_type_v = is_library_type<T>::value;

template <typename T>
concept library_type = is_library_type_v<T>;

template <typename T>
struct underlying;

template <>
struct underlying<u32>
{
    using type = std::uint32_t;
};

template <>
struct underlying<u64>
{
    using type = std::uint64_t;
};

template <typename T>
using underlying_type_t = underlying<T>::type;

} // namespace detail

BOOST_SAFE_NUMBERS_EXPORT template <typename charT, typename traits, detail::library_type LibType>
auto operator>>(std::basic_istream<charT, traits>& is, LibType& v)
{
    using underlying_type = detail::underlying_type_t<charT>::type;

    underlying_type temp;
    is >> temp;
    v = temp;

    return is;
}

BOOST_SAFE_NUMBERS_EXPORT template <typename charT, typename traits, detail::library_type LibType>
auto operator<<(std::basic_ostream<charT, traits>& os, const LibType& v)
{
    using underlying_type = detail::underlying_type_t<charT>::type;

    const auto temp {static_cast<underlying_type>(v)};
    os << temp;

    return os;
}

}  // namespace boost::safe_numbers

#endif // BOOST_SAFENUMBERS_IOSTREAM_HPP

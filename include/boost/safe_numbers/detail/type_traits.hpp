// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_TYPE_TRAITS_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_TYPE_TRAITS_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/concepts.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <concepts>
#include <type_traits>

#endif // ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers::detail {

template <unsigned_integral BasisType>
class unsigned_integer_basis;

template <auto Min, auto Max>
class bounded_uint;

namespace impl {

template <typename>
struct is_library_type : std::false_type {};

template <typename T>
struct is_library_type<unsigned_integer_basis<T>> : std::true_type {};

template <auto Min, auto Max>
struct is_library_type<bounded_uint<Min, Max>> : std::true_type {};

} // namespace impl

template <typename T>
inline constexpr bool is_library_type_v = impl::is_library_type<T>::value;

template <typename T>
concept library_type = is_library_type_v<T>;

namespace impl {

template <typename>
struct is_unsigned_library_type : std::false_type {};

template <typename T>
struct is_unsigned_library_type<unsigned_integer_basis<T>> : std::true_type {};

template <auto Min, auto Max>
struct is_unsigned_library_type<bounded_uint<Min, Max>> : std::true_type {};

} // namespace impl

template <typename T>
inline constexpr bool is_unsigned_library_type_v = impl::is_unsigned_library_type<T>::value;

template <typename T>
concept unsigned_library_type = is_unsigned_library_type_v<T>;

namespace impl {

template <typename T>
struct is_fundamental_unsigned_integral : std::bool_constant<std::is_unsigned_v<T> || std::is_same_v<T, int128::uint128_t>> {};

} // namespace impl

template <typename T>
inline constexpr bool is_fundamental_unsigned_integral_v = impl::is_fundamental_unsigned_integral<T>::value;

namespace impl {

template <typename T>
struct underlying
{
    using type = std::remove_cv_t<std::remove_reference_t<T>>;
};

template <typename T>
struct underlying<unsigned_integer_basis<T>>
{
    using type = T;
};

template <auto Min, auto Max>
struct underlying<bounded_uint<Min, Max>>
{
    using type = underlying<typename bounded_uint<Min, Max>::basis_type>;
};

} // namespace impl

template <typename T>
using underlying_type_t = typename impl::underlying<T>::type;

} // namespace boost::safe_numbers::detail

#endif // BOOST_SAFE_NUMBERS_DETAIL_TYPE_TRAITS_HPP

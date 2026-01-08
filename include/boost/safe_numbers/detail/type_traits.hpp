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

namespace impl {

template <typename>
struct is_library_type : std::false_type {};

template <typename T>
struct is_library_type<unsigned_integer_basis<T>> : std::true_type {};

} // namespace impl

template <typename T>
inline constexpr bool is_library_type_v = impl::is_library_type<T>::value;

template <typename T>
concept library_type = is_library_type_v<T>;

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

} // namespace impl

template <typename T>
using underlying_type_t = impl::underlying<T>::type;

} // namespace boost::safe_numbers::detail

#endif // BOOST_SAFE_NUMBERS_DETAIL_TYPE_TRAITS_HPP

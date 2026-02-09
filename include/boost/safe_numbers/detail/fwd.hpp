// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_FWD_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_FWD_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/concepts.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <concepts>
#include <type_traits>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers::detail {

// Forward declaration of unsigned_integer_basis
template <unsigned_integral BasisType>
class unsigned_integer_basis;

// is_fundamental_unsigned_integral

namespace impl {

template <typename T>
struct is_fundamental_unsigned_integral : std::bool_constant<std::is_unsigned_v<T> || std::is_same_v<T, int128::uint128_t>> {};

} // namespace impl

template <typename T>
inline constexpr bool is_fundamental_unsigned_integral_v = impl::is_fundamental_unsigned_integral<T>::value;

// is_unsigned_library_type (base + unsigned_integer_basis specialization)

namespace impl {

template <typename>
struct is_unsigned_library_type : std::false_type {};

template <typename T>
struct is_unsigned_library_type<unsigned_integer_basis<T>> : std::true_type {};

} // namespace impl

template <typename T>
inline constexpr bool is_unsigned_library_type_v = impl::is_unsigned_library_type<T>::value;

// underlying type trait (base + unsigned_integer_basis specialization)

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
using underlying_type_t = typename impl::underlying<T>::type;

// valid_bound concept

template <typename T>
concept valid_bound = !std::is_same_v<T, bool> && (is_unsigned_library_type_v<T> || is_fundamental_unsigned_integral_v<T>);

// raw_value function

template <typename T>
    requires valid_bound<T>
consteval auto raw_value(T val) noexcept
{
    if constexpr (is_unsigned_library_type_v<T>)
    {
        return static_cast<underlying_type_t<T>>(val);
    }
    else
    {
        return val;
    }
}

} // namespace boost::safe_numbers::detail

// Constrained forward declaration of bounded_uint
namespace boost::safe_numbers {

template <auto Min, auto Max>
    requires (detail::valid_bound<decltype(Min)> &&
              detail::valid_bound<decltype(Max)> &&
              detail::raw_value(Max) > detail::raw_value(Min))
class bounded_uint;

} // namespace boost::safe_numbers

// bounded_uint specialization of is_unsigned_library_type
namespace boost::safe_numbers::detail::impl {

template <auto Min, auto Max>
struct is_unsigned_library_type<bounded_uint<Min, Max>> : std::true_type {};

} // namespace boost::safe_numbers::detail::impl

#endif // BOOST_SAFE_NUMBERS_DETAIL_FWD_HPP

// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_TYPE_TRAITS_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_TYPE_TRAITS_HPP

#include <boost/safe_numbers/detail/fwd.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <concepts>
#include <type_traits>

#endif // ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers::detail {

// is_fundamental_unsigned_integral

namespace impl {

template <typename T>
struct is_fundamental_unsigned_integral : std::bool_constant<std::is_unsigned_v<T> || std::is_same_v<std::remove_cv_t<T>, int128::uint128_t>> {};

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
constexpr auto raw_value(T val) noexcept
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

// is_bounded_type trait
namespace boost::safe_numbers::detail {

namespace impl {

template <typename>
struct is_bounded_type : std::false_type {};

template <auto Min, auto Max>
struct is_bounded_type<bounded_uint<Min, Max>> : std::true_type {};

} // namespace impl

template <typename T>
inline constexpr bool is_bounded_type_v = impl::is_bounded_type<T>::value;

// is_library_type

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

template <typename T>
concept unsigned_library_type = is_unsigned_library_type_v<T>;

template <typename T>
concept non_bounded_unsigned_library_type = is_unsigned_library_type_v<T> && !is_bounded_type_v<T>;

// Forward declaration of verified_type_basis
template <library_type BasisType>
class verified_type_basis;

// is_verified_type trait

namespace impl {

template <typename>
struct is_verified_type : std::false_type {};

template <library_type BasisType>
struct is_verified_type<verified_type_basis<BasisType>> : std::true_type {};

template <library_type BasisType>
struct is_library_type<verified_type_basis<BasisType>> : std::true_type {};

template <library_type BasisType>
    requires is_unsigned_library_type_v<BasisType>
struct is_unsigned_library_type<verified_type_basis<BasisType>> : std::true_type {};

template <library_type BasisType>
    requires is_bounded_type_v<BasisType>
struct is_bounded_type<verified_type_basis<BasisType>> : std::true_type {};

} // namespace impl

template <typename T>
inline constexpr bool is_verified_type_v = impl::is_verified_type<T>::value;

template <typename T>
concept verified_type = is_verified_type_v<T>;

// underlying specialization for bounded_uint

namespace impl {

template <auto Min, auto Max>
struct underlying<bounded_uint<Min, Max>>
{
    using type = typename underlying<typename bounded_uint<Min, Max>::basis_type>::type;
};

template <library_type BasisType>
struct underlying<verified_type_basis<BasisType>>
{
    using type = typename underlying<BasisType>::type;
};

} // namespace impl

// Promotes an unsigned integer to the next higher type
// uint128_t becomes bool so that we can static_assert on bool check that we can't widen uint128_t
template <unsigned_integral T>
using promoted_type = std::conditional_t<std::is_same_v<T, std::uint8_t>, std::uint16_t,
                          std::conditional_t<std::is_same_v<T, std::uint16_t>, std::uint32_t,
                              std::conditional_t<std::is_same_v<T, std::uint32_t>, std::uint64_t,
                                  std::conditional_t<std::is_same_v<T, std::uint64_t>, int128::uint128_t, bool>>>>;

} // namespace boost::safe_numbers::detail

#endif // BOOST_SAFE_NUMBERS_DETAIL_TYPE_TRAITS_HPP

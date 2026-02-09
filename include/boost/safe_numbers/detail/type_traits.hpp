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

// underlying specialization for bounded_uint

namespace impl {

template <auto Min, auto Max>
struct underlying<bounded_uint<Min, Max>>
{
    using type = typename underlying<typename bounded_uint<Min, Max>::basis_type>::type;
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

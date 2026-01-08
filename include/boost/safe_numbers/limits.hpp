// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_LIMITS_HPP
#define BOOST_SAFE_NUMBERS_LIMITS_HPP

#include <boost/safe_numbers/detail/type_traits.hpp>
#include <boost/safe_numbers/unsigned_integers.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <limits>

#endif // ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers::detail {

template <typename T>
struct numeric_limits_impl
{
    using basis_type = underlying_type_t<T>;

    static constexpr bool is_specialized = std::numeric_limits<basis_type>::is_specialized;
    static constexpr bool is_signed = std::numeric_limits<basis_type>::is_signed;
    static constexpr bool is_integer = std::numeric_limits<basis_type>::is_integer;
    static constexpr bool is_exact = std::numeric_limits<basis_type>::is_exact;
    static constexpr bool has_infinity = std::numeric_limits<basis_type>::has_infinity;
    static constexpr bool has_quiet_NaN = std::numeric_limits<basis_type>::has_quiet_NaN;
    static constexpr bool has_signaling_NaN = std::numeric_limits<basis_type>::has_signaling_NaN;

    #if ((!defined(_MSC_VER) && (__cplusplus <= 202002L)) || (defined(_MSC_VER) && (_MSVC_LANG <= 202002L)))
    static constexpr std::float_denorm_style has_denorm = std::numeric_limits<basis_type>::has_denorm;
    static constexpr bool has_denorm_loss = std::numeric_limits<basis_type>::has_denorm_loss;
    #endif

    static constexpr std::float_round_style round_style = std::numeric_limits<basis_type>::round_style;
    static constexpr bool is_iec559 = std::numeric_limits<basis_type>::is_iec559;
    static constexpr bool is_bounded = std::numeric_limits<basis_type>::is_bounded;
    static constexpr bool is_modulo = std::numeric_limits<basis_type>::is_modulo;
    static constexpr int digits = std::numeric_limits<basis_type>::digits;
    static constexpr int digits10 = std::numeric_limits<basis_type>::digits10;
    static constexpr int max_digits10 = std::numeric_limits<basis_type>::max_digits10;
    static constexpr int radix = std::numeric_limits<basis_type>::radix;
    static constexpr int min_exponent = std::numeric_limits<basis_type>::min_exponent;
    static constexpr int min_exponent10 = std::numeric_limits<basis_type>::min_exponent10;
    static constexpr int max_exponent = std::numeric_limits<basis_type>::max_exponent;
    static constexpr int max_exponent10 = std::numeric_limits<basis_type>::max_exponent10;
    static constexpr bool traps = std::numeric_limits<basis_type>::traps;
    static constexpr bool tinyness_before = std::numeric_limits<basis_type>::tinyness_before;

    static constexpr T min() { return T{std::numeric_limits<basis_type>::min()}; }
    static constexpr T max() { return T{std::numeric_limits<basis_type>::max()}; }
    static constexpr T lowest() { return T{std::numeric_limits<basis_type>::lowest()}; }
    static constexpr T epsilon() { return T{std::numeric_limits<basis_type>::epsilon()}; }
    static constexpr T round_error() { return T{std::numeric_limits<basis_type>::round_error() }; }
    static constexpr T infinity() { return T{std::numeric_limits<basis_type>::infinity()}; }
    static constexpr T quiet_NaN() { return T{std::numeric_limits<basis_type>::quiet_NaN()}; }
    static constexpr T signaling_NaN() { return T{std::numeric_limits<basis_type>::signaling_NaN()}; }
    static constexpr T denorm_min() { return T{std::numeric_limits<basis_type>::denorm_min()}; }
};

} // namespace boost::safe_numbers::detail

namespace std {

#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wmismatched-tags"
#endif

template <>
class numeric_limits<boost::safe_numbers::u8> :
    public boost::safe_numbers::detail::numeric_limits_impl<boost::safe_numbers::u8> {};

template <>
class numeric_limits<boost::safe_numbers::u16> :
    public boost::safe_numbers::detail::numeric_limits_impl<boost::safe_numbers::u16> {};

template <>
class numeric_limits<boost::safe_numbers::u32> :
    public boost::safe_numbers::detail::numeric_limits_impl<boost::safe_numbers::u32> {};

template <>
class numeric_limits<boost::safe_numbers::u64> :
    public boost::safe_numbers::detail::numeric_limits_impl<boost::safe_numbers::u64> {};

template <>
class numeric_limits<boost::safe_numbers::u128> :
    public boost::safe_numbers::detail::numeric_limits_impl<boost::safe_numbers::u128> {};

#ifdef __clang__
#  pragma clang diagnostic pop
#endif

} // namespace std

#endif // BOOST_SAFE_NUMBERS_LIMITS_HPP

// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_LIMITS_HPP
#define BOOST_SAFE_NUMBERS_LIMITS_HPP

#include <boost/safe_numbers/detail/type_traits.hpp>
#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/bounded_integers.hpp>
#include <boost/safe_numbers/verified_integers.hpp>

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

template <auto Min, auto Max>
class numeric_limits<boost::safe_numbers::bounded_uint<Min, Max>>
{
    using type = boost::safe_numbers::bounded_uint<Min, Max>;
    using underlying_type = boost::safe_numbers::detail::underlying_type_t<type>;

public:
    static constexpr bool is_specialized = std::numeric_limits<underlying_type>::is_specialized;
    static constexpr bool is_signed = std::numeric_limits<underlying_type>::is_signed;
    static constexpr bool is_integer = std::numeric_limits<underlying_type>::is_integer;
    static constexpr bool is_exact = std::numeric_limits<underlying_type>::is_exact;
    static constexpr bool has_infinity = std::numeric_limits<underlying_type>::has_infinity;
    static constexpr bool has_quiet_NaN = std::numeric_limits<underlying_type>::has_quiet_NaN;
    static constexpr bool has_signaling_NaN = std::numeric_limits<underlying_type>::has_signaling_NaN;

    #if ((!defined(_MSC_VER) && (__cplusplus <= 202002L)) || (defined(_MSC_VER) && (_MSVC_LANG <= 202002L)))
    static constexpr std::float_denorm_style has_denorm = std::numeric_limits<underlying_type>::has_denorm;
    static constexpr bool has_denorm_loss = std::numeric_limits<underlying_type>::has_denorm_loss;
    #endif

    static constexpr std::float_round_style round_style = std::numeric_limits<underlying_type>::round_style;
    static constexpr bool is_iec559 = std::numeric_limits<underlying_type>::is_iec559;
    static constexpr bool is_bounded = std::numeric_limits<underlying_type>::is_bounded;
    static constexpr bool is_modulo = std::numeric_limits<underlying_type>::is_modulo;
    static constexpr int digits = std::numeric_limits<underlying_type>::digits;
    static constexpr int digits10 = std::numeric_limits<underlying_type>::digits10;
    static constexpr int max_digits10 = std::numeric_limits<underlying_type>::max_digits10;
    static constexpr int radix = std::numeric_limits<underlying_type>::radix;
    static constexpr int min_exponent = std::numeric_limits<underlying_type>::min_exponent;
    static constexpr int min_exponent10 = std::numeric_limits<underlying_type>::min_exponent10;
    static constexpr int max_exponent = std::numeric_limits<underlying_type>::max_exponent;
    static constexpr int max_exponent10 = std::numeric_limits<underlying_type>::max_exponent10;
    static constexpr bool traps = std::numeric_limits<underlying_type>::traps;
    static constexpr bool tinyness_before = std::numeric_limits<underlying_type>::tinyness_before;

    static constexpr type min() { return type{static_cast<underlying_type>(boost::safe_numbers::detail::raw_value(Min))}; }
    static constexpr type max() { return type{static_cast<underlying_type>(boost::safe_numbers::detail::raw_value(Max))}; }
    static constexpr type lowest() { return min(); }
    static constexpr type epsilon() { return min(); }
    static constexpr type round_error() { return min(); }
    static constexpr type infinity() { return min(); }
    static constexpr type quiet_NaN() { return min(); }
    static constexpr type signaling_NaN() { return min(); }
    static constexpr type denorm_min() { return min(); }
};

template <boost::safe_numbers::detail::library_type BasisType>
class numeric_limits<boost::safe_numbers::detail::verified_type_basis<BasisType>>
{
    using type = boost::safe_numbers::detail::verified_type_basis<BasisType>;
    using basis_limits = std::numeric_limits<BasisType>;

public:
    static constexpr bool is_specialized = basis_limits::is_specialized;
    static constexpr bool is_signed = basis_limits::is_signed;
    static constexpr bool is_integer = basis_limits::is_integer;
    static constexpr bool is_exact = basis_limits::is_exact;
    static constexpr bool has_infinity = basis_limits::has_infinity;
    static constexpr bool has_quiet_NaN = basis_limits::has_quiet_NaN;
    static constexpr bool has_signaling_NaN = basis_limits::has_signaling_NaN;

    #if ((!defined(_MSC_VER) && (__cplusplus <= 202002L)) || (defined(_MSC_VER) && (_MSVC_LANG <= 202002L)))
    static constexpr std::float_denorm_style has_denorm = basis_limits::has_denorm;
    static constexpr bool has_denorm_loss = basis_limits::has_denorm_loss;
    #endif

    static constexpr std::float_round_style round_style = basis_limits::round_style;
    static constexpr bool is_iec559 = basis_limits::is_iec559;
    static constexpr bool is_bounded = basis_limits::is_bounded;
    static constexpr bool is_modulo = basis_limits::is_modulo;
    static constexpr int digits = basis_limits::digits;
    static constexpr int digits10 = basis_limits::digits10;
    static constexpr int max_digits10 = basis_limits::max_digits10;
    static constexpr int radix = basis_limits::radix;
    static constexpr int min_exponent = basis_limits::min_exponent;
    static constexpr int min_exponent10 = basis_limits::min_exponent10;
    static constexpr int max_exponent = basis_limits::max_exponent;
    static constexpr int max_exponent10 = basis_limits::max_exponent10;
    static constexpr bool traps = basis_limits::traps;
    static constexpr bool tinyness_before = basis_limits::tinyness_before;

    static constexpr type min() { return type{basis_limits::min()}; }
    static constexpr type max() { return type{basis_limits::max()}; }
    static constexpr type lowest() { return type{basis_limits::lowest()}; }
    static constexpr type epsilon() { return type{basis_limits::epsilon()}; }
    static constexpr type round_error() { return type{basis_limits::round_error()}; }
    static constexpr type infinity() { return type{basis_limits::infinity()}; }
    static constexpr type quiet_NaN() { return type{basis_limits::quiet_NaN()}; }
    static constexpr type signaling_NaN() { return type{basis_limits::signaling_NaN()}; }
    static constexpr type denorm_min() { return type{basis_limits::denorm_min()}; }
};

#ifdef __clang__
#  pragma clang diagnostic pop
#endif

} // namespace std

#endif // BOOST_SAFE_NUMBERS_LIMITS_HPP

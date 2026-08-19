// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_TYPE_TRAITS_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_TYPE_TRAITS_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/int128/int128.hpp>
#include <boost/safe_numbers/overflow_policy.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <concepts>
#include <type_traits>

#endif // ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers::detail {

// is_fundamental_unsigned_integral

namespace impl {

template <typename T>
struct is_fundamental_unsigned_integral : std::bool_constant<std::is_unsigned_v<T> || std::is_same_v<std::remove_cv_t<T>, int128::uint128_t>> {};

template <typename T>
struct is_fundamental_signed_integral : std::bool_constant<std::is_signed_v<T> || std::is_same_v<std::remove_cv_t<T>, int128::int128_t>> {};

template <typename T>
struct is_compatible_float_type : std::bool_constant<std::is_same_v<float, std::remove_cv_t<T>> || std::is_same_v<double, std::remove_cv_t<T>>> {};

} // namespace impl

template <typename T>
inline constexpr bool is_fundamental_unsigned_integral_v = impl::is_fundamental_unsigned_integral<T>::value;

template <typename T>
concept fundamental_unsigned_integral = is_fundamental_unsigned_integral_v<T>;

template <typename T>
inline constexpr bool is_fundamental_signed_integral_v = impl::is_fundamental_signed_integral<T>::value;

template <typename T>
concept fundamental_signed_integral = is_fundamental_signed_integral_v<T>;

template <typename T>
inline constexpr bool is_compatible_float_type = impl::is_compatible_float_type<T>::value;

template <typename T>
concept compatible_float_type = is_compatible_float_type<T>;

// Which family of basis template a type-level error policy is validated against
enum class basis_kind
{
    unsigned_integer,
    signed_integer,
    floating_point
};

// True when the NTTP is an overflow_policy enumerator. Every comparison against
// an enumerator must sit behind this check because comparing a non-enum NTTP
// against overflow_policy would be ill-formed rather than false.
template <auto Policy>
inline constexpr bool is_overflow_policy_v = std::is_same_v<std::remove_cv_t<decltype(Policy)>, overflow_policy>;

// True when the NTTP is a user defined handler object rather than an
// overflow_policy enumerator. The handler interface is validated separately
// by the error_handler_for concept.
template <auto Policy>
inline constexpr bool is_user_handler_v = std::is_class_v<std::remove_cv_t<decltype(Policy)>>;

// A user defined handler is a stateless class whose on_error is callable on a
// const object with the error kind, a defined fallback value (the wrapped
// integer result, the dividend, or the raw IEEE 754 result), and the
// diagnostic message. Whatever it returns becomes the operation's result.
template <typename Handler, typename T>
concept error_handler_for = std::is_empty_v<Handler> &&
    requires(const Handler handler, const T value, const char* msg)
    {
        { handler.on_error(error_kind::overflow, value, msg) } -> std::same_as<T>;
    };

// Compares a policy NTTP against an enumerator, funneled so that user handler
// objects compare unequal instead of making the comparison ill-formed.
// consteval, so uses in runtime conditions fold to a constant.
template <auto Policy>
consteval auto policy_equals(const overflow_policy policy) noexcept -> bool
{
    if constexpr (is_overflow_policy_v<Policy>)
    {
        return Policy == policy;
    }
    else
    {
        return false;
    }
}

// noexcept specification for operations whose only error is overflow or
// underflow (add, sub, mul, shifts, increment, decrement, unary minus, and
// every float operator): throwing throws, saturate and strict do not, and a
// user handler propagates the noexcept of its on_error.
template <auto Policy, typename T>
consteval auto policy_is_nothrow_arith() noexcept -> bool
{
    if constexpr (is_overflow_policy_v<Policy>)
    {
        return Policy != overflow_policy::throw_exception;
    }
    else
    {
        return noexcept(Policy.on_error(error_kind::overflow, T{}, static_cast<const char*>(nullptr)));
    }
}

// noexcept specification for integer division and modulo, where division by
// zero throws under both throw_exception and saturate.
template <auto Policy, typename T>
consteval auto policy_is_nothrow_div() noexcept -> bool
{
    if constexpr (is_overflow_policy_v<Policy>)
    {
        return Policy == overflow_policy::strict;
    }
    else
    {
        return noexcept(Policy.on_error(error_kind::divide_by_zero, T{}, static_cast<const char*>(nullptr)));
    }
}

// True when two policy NTTPs differ, without requiring comparability between
// unrelated handler types. Handlers are stateless, so same type means equal.
template <auto LHSPolicy, auto RHSPolicy>
consteval auto policies_differ() noexcept -> bool
{
    if constexpr (!std::is_same_v<decltype(LHSPolicy), decltype(RHSPolicy)>)
    {
        return true;
    }
    else if constexpr (is_overflow_policy_v<LHSPolicy>)
    {
        return LHSPolicy != RHSPolicy;
    }
    else
    {
        return false;
    }
}

// Policies whose result is not the operand type (pair, optional, or a wider type)
// can never live in the type itself; they remain free functions.
template <auto Policy>
consteval auto is_value_returning_policy() noexcept -> bool
{
    if constexpr (is_overflow_policy_v<Policy>)
    {
        return Policy == overflow_policy::overflow_tuple ||
               Policy == overflow_policy::checked ||
               Policy == overflow_policy::widen;
    }
    else
    {
        return false;
    }
}

// The subset of overflow_policy values allowed as a type-level policy for the
// given basis kind: throw_exception and saturate everywhere, strict only for integers.
template <auto Policy>
consteval auto is_valid_type_policy(const basis_kind kind) noexcept -> bool
{
    if constexpr (is_overflow_policy_v<Policy>)
    {
        // One expression rather than an if on a constant condition, which MSVC rejects
        // under /W4 /WX (C4127)
        return Policy == overflow_policy::throw_exception ||
               Policy == overflow_policy::saturate ||
               (Policy == overflow_policy::strict && kind != basis_kind::floating_point);
    }
    else
    {
        return false;
    }
}

template <fundamental_unsigned_integral BasisType, auto ErrorPolicy = overflow_policy::throw_exception>
class unsigned_integer_basis;

template <fundamental_signed_integral BasisType, auto ErrorPolicy = overflow_policy::throw_exception>
class signed_integer_basis;

template <compatible_float_type BasisType, auto ErrorPolicy = overflow_policy::throw_exception>
class float_basis;

// Maps the type argument of the basic_* alias templates onto the basis NTTP:
// the tag types select the built-in enum policies and any other type is a user
// defined handler passed by value.
template <typename ErrorHandler>
inline constexpr auto type_policy_v = ErrorHandler{};

template <>
inline constexpr auto type_policy_v<throwing> = overflow_policy::throw_exception;

template <>
inline constexpr auto type_policy_v<saturating> = overflow_policy::saturate;

template <>
inline constexpr auto type_policy_v<strict> = overflow_policy::strict;

// is_unsigned_library_type (base + unsigned_integer_basis specialization)

namespace impl {

template <typename>
struct is_unsigned_library_type : std::false_type {};

template <typename>
struct is_signed_library_type : std::false_type {};

template <typename>
struct is_float_library_type : std::false_type {};

template <typename T, auto ErrorPolicy>
struct is_unsigned_library_type<unsigned_integer_basis<T, ErrorPolicy>> : std::true_type {};

template <typename T, auto ErrorPolicy>
struct is_signed_library_type<signed_integer_basis<T, ErrorPolicy>> : std::true_type {};

template <typename T, auto ErrorPolicy>
struct is_float_library_type<float_basis<T, ErrorPolicy>> : std::true_type {};

} // namespace impl

template <typename T>
inline constexpr bool is_unsigned_library_type_v = impl::is_unsigned_library_type<T>::value;

template <typename T>
inline constexpr bool is_signed_library_type_v = impl::is_signed_library_type<T>::value;

template <typename T>
inline constexpr bool is_float_library_type_v = impl::is_float_library_type<T>::value;

// underlying type trait (base + unsigned_integer_basis specialization)

namespace impl {

template <typename T>
struct underlying
{
    using type = std::remove_cv_t<std::remove_reference_t<T>>;
};

template <typename T, auto ErrorPolicy>
struct underlying<unsigned_integer_basis<T, ErrorPolicy>>
{
    using type = T;
};

template <typename T, auto ErrorPolicy>
struct underlying<signed_integer_basis<T, ErrorPolicy>>
{
    using type = T;
};

template <typename T, auto ErrorPolicy>
struct underlying<float_basis<T, ErrorPolicy>>
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

// valid_signed_bound concept

template <typename T>
concept valid_signed_bound = !std::is_same_v<T, bool> && (is_signed_library_type_v<T> || is_fundamental_signed_integral_v<T>);

// signed_raw_value function

template <typename T>
    requires valid_signed_bound<T>
constexpr auto signed_raw_value(T val) noexcept
{
    if constexpr (is_signed_library_type_v<T>)
    {
        return static_cast<underlying_type_t<T>>(val);
    }
    else
    {
        return val;
    }
}

// valid_float_bound concept

template <typename T>
concept valid_float_bound = is_compatible_float_type<T> || is_float_library_type_v<T>;

// float_raw_value function

template <typename T>
    requires valid_float_bound<T>
constexpr auto float_raw_value(T val) noexcept
{
    if constexpr (is_float_library_type_v<T>)
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

// Constrained forward declaration of bounded_int
namespace boost::safe_numbers {

template <auto Min, auto Max>
    requires (detail::valid_signed_bound<decltype(Min)> &&
              detail::valid_signed_bound<decltype(Max)> &&
              detail::signed_raw_value(Max) > detail::signed_raw_value(Min))
class bounded_int;

} // namespace boost::safe_numbers

// Constrained forward declaration of bounded_float (requires float NTTP support)
#if BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT

namespace boost::safe_numbers {

template <auto Min, auto Max>
    requires (detail::valid_float_bound<decltype(Min)> &&
              detail::valid_float_bound<decltype(Max)> &&
              std::is_same_v<decltype(Min), decltype(Max)> &&
              detail::float_raw_value(Min) == detail::float_raw_value(Min) &&
              detail::float_raw_value(Max) == detail::float_raw_value(Max) &&
              detail::float_raw_value(Max) > detail::float_raw_value(Min))
class bounded_float;

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT

// bounded_uint specialization of is_unsigned_library_type
namespace boost::safe_numbers::detail::impl {

template <auto Min, auto Max>
struct is_unsigned_library_type<bounded_uint<Min, Max>> : std::true_type {};

} // namespace boost::safe_numbers::detail::impl

// bounded_int specialization of is_signed_library_type
namespace boost::safe_numbers::detail::impl {

template <auto Min, auto Max>
struct is_signed_library_type<bounded_int<Min, Max>> : std::true_type {};

} // namespace boost::safe_numbers::detail::impl

// bounded_float specialization of is_float_library_type
#if BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT

namespace boost::safe_numbers::detail::impl {

template <auto Min, auto Max>
struct is_float_library_type<bounded_float<Min, Max>> : std::true_type {};

} // namespace boost::safe_numbers::detail::impl

#endif // BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT

// is_bounded_type trait
namespace boost::safe_numbers::detail {

namespace impl {

template <typename>
struct is_bounded_type : std::false_type {};

template <auto Min, auto Max>
struct is_bounded_type<bounded_uint<Min, Max>> : std::true_type {};

template <auto Min, auto Max>
struct is_bounded_type<bounded_int<Min, Max>> : std::true_type {};

#if BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT
template <auto Min, auto Max>
struct is_bounded_type<bounded_float<Min, Max>> : std::true_type {};
#endif

} // namespace impl

template <typename T>
inline constexpr bool is_bounded_type_v = impl::is_bounded_type<T>::value;

// is_library_type

namespace impl {

template <typename>
struct is_library_type : std::false_type {};

template <typename T, auto ErrorPolicy>
struct is_library_type<unsigned_integer_basis<T, ErrorPolicy>> : std::true_type {};

template <typename T, auto ErrorPolicy>
struct is_library_type<signed_integer_basis<T, ErrorPolicy>> : std::true_type {};

template <typename T, auto ErrorPolicy>
struct is_library_type<float_basis<T, ErrorPolicy>> : std::true_type {};

template <auto Min, auto Max>
struct is_library_type<bounded_uint<Min, Max>> : std::true_type {};

template <auto Min, auto Max>
struct is_library_type<bounded_int<Min, Max>> : std::true_type {};

#if BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT
template <auto Min, auto Max>
struct is_library_type<bounded_float<Min, Max>> : std::true_type {};
#endif

template <typename>
struct is_integral_library_type : std::false_type {};

template <typename T, auto ErrorPolicy>
struct is_integral_library_type<unsigned_integer_basis<T, ErrorPolicy>> : std::true_type {};

template <typename T, auto ErrorPolicy>
struct is_integral_library_type<signed_integer_basis<T, ErrorPolicy>> : std::true_type {};

template <auto Min, auto Max>
struct is_integral_library_type<bounded_uint<Min, Max>> : std::true_type {};

template <auto Min, auto Max>
struct is_integral_library_type<bounded_int<Min, Max>> : std::true_type {};

} // namespace impl

template <typename T>
inline constexpr bool is_library_type_v = impl::is_library_type<T>::value;

template <typename T>
inline constexpr bool is_integral_library_type_v = impl::is_integral_library_type<T>::value;

template <typename T>
concept library_type = is_library_type_v<T>;

template <typename T>
concept integral_library_type = is_integral_library_type_v<T>;

template <typename T>
concept unsigned_library_type = is_unsigned_library_type_v<T>;

template <typename T>
concept non_bounded_unsigned_library_type = is_unsigned_library_type_v<T> && !is_bounded_type_v<T>;

template <typename T>
concept non_bounded_integral_library_type = is_integral_library_type_v<T> && !is_bounded_type_v<T>;

template <typename T>
concept float_library_type = is_float_library_type_v<T>;

template <typename T>
concept non_bounded_float_library_type = is_float_library_type_v<T> && !is_bounded_type_v<T>;

// underlying specialization for bounded_uint

namespace impl {

template <auto Min, auto Max>
struct underlying<bounded_uint<Min, Max>>
{
    using type = typename underlying<typename bounded_uint<Min, Max>::basis_type>::type;
};

template <auto Min, auto Max>
struct underlying<bounded_int<Min, Max>>
{
    using type = typename underlying<typename bounded_int<Min, Max>::basis_type>::type;
};

#if BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT
template <auto Min, auto Max>
struct underlying<bounded_float<Min, Max>>
{
    using type = typename underlying<typename bounded_float<Min, Max>::basis_type>::type;
};
#endif

} // namespace impl

// Promotes an unsigned integer to the next higher type
// uint128_t becomes bool so that we can static_assert on bool check that we can't widen uint128_t
template <fundamental_unsigned_integral T>
using promoted_type = std::conditional_t<std::is_same_v<T, std::uint8_t>, std::uint16_t,
                          std::conditional_t<std::is_same_v<T, std::uint16_t>, std::uint32_t,
                              std::conditional_t<std::is_same_v<T, std::uint32_t>, std::uint64_t,
                                  std::conditional_t<std::is_same_v<T, std::uint64_t>, int128::uint128_t, bool>>>>;

// Promotes a signed integer to the next higher type
// int128_t becomes bool so that we can static_assert on bool check that we can't widen int128_t
template <fundamental_signed_integral T>
using signed_promoted_type = std::conditional_t<std::is_same_v<T, std::int8_t>, std::int16_t,
                                 std::conditional_t<std::is_same_v<T, std::int16_t>, std::int32_t,
                                     std::conditional_t<std::is_same_v<T, std::int32_t>, std::int64_t,
                                         std::conditional_t<std::is_same_v<T, std::int64_t>, int128::int128_t, bool>>>>;

} // namespace boost::safe_numbers::detail

#endif // BOOST_SAFE_NUMBERS_DETAIL_TYPE_TRAITS_HPP

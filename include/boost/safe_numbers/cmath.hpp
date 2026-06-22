// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_CMATH_HPP
#define BOOST_SAFE_NUMBERS_CMATH_HPP

#include <boost/safe_numbers/floats.hpp>
#include <boost/safe_numbers/signed_integers.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#if (defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))
#include <cuda/std/cmath>
#else
#include <cmath>
#endif

#include <cstdint>
#include <type_traits>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

// Namespace that actually provides the <cmath> functions. The wrappers delegate
// to it with an explicit qualification so there is no ADL or self-recursion.
#if (defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))
#  define BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS cuda::std
#else
#  define BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS std
#endif

namespace boost::safe_numbers {

#define BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(fn)                                                       \
template <detail::non_bounded_float_library_type T>                                                     \
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto fn(const T x) -> T                           \
{                                                                                                       \
    using underlying_type = detail::underlying_type_t<T>;                                               \
    return T{detail::impl::check_cmath_result<underlying_type>(                                          \
        BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS::fn(static_cast<underlying_type>(x)), #fn)};                  \
}

#define BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BINARY(fn)                                                      \
template <detail::non_bounded_float_library_type T>                                                     \
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto fn(const T x, const T y) -> T                \
{                                                                                                       \
    using underlying_type = detail::underlying_type_t<T>;                                               \
    return T{detail::impl::check_cmath_result<underlying_type>(                                          \
        BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS::fn(static_cast<underlying_type>(x),                          \
                                               static_cast<underlying_type>(y)), #fn)};                  \
}

// Trigonometric
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(sin)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(cos)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(tan)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(asin)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(acos)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(atan)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BINARY(atan2)

// Hyperbolic
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(sinh)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(cosh)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(tanh)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(asinh)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(acosh)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(atanh)

// Exponential and logarithmic
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(exp)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(exp2)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(expm1)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(log)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(log2)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(log10)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(log1p)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(logb)

// Power and root
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(sqrt)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(cbrt)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BINARY(pow)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BINARY(hypot)

// Error and gamma
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(erf)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(erfc)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(tgamma)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(lgamma)

// Nearest integer (floating-point result)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(floor)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(ceil)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(trunc)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(round)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(nearbyint)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(rint)

// Absolute value
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(abs)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(fabs)

// Remainder, difference, min/max, sign manipulation
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BINARY(fmod)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BINARY(remainder)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BINARY(fdim)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BINARY(fmin)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BINARY(fmax)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BINARY(copysign)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BINARY(nextafter)

// Fused multiply-add
template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto fma(const T x, const T y, const T z) -> T
{
    using underlying_type = detail::underlying_type_t<T>;
    return T{detail::impl::check_cmath_result<underlying_type>(
        BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS::fma(static_cast<underlying_type>(x),
                                                static_cast<underlying_type>(y),
                                                static_cast<underlying_type>(z)), "fma")};
}

// Scaling by a power of two (integer exponent input, floating-point result)
template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto ldexp(const T x, const int exp) -> T
{
    using underlying_type = detail::underlying_type_t<T>;
    return T{detail::impl::check_cmath_result<underlying_type>(
        BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS::ldexp(static_cast<underlying_type>(x), exp), "ldexp")};
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto scalbn(const T x, const int exp) -> T
{
    using underlying_type = detail::underlying_type_t<T>;
    return T{detail::impl::check_cmath_result<underlying_type>(
        BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS::scalbn(static_cast<underlying_type>(x), exp), "scalbn")};
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto scalbln(const T x, const long exp) -> T
{
    using underlying_type = detail::underlying_type_t<T>;
    return T{detail::impl::check_cmath_result<underlying_type>(
        BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS::scalbln(static_cast<underlying_type>(x), exp), "scalbln")};
}

// ------------------------------
// Classification predicates
// ------------------------------
// These inspect a value and never throw, so they are the escape hatch for code
// that needs to reason about infinities and NANs. They are genuinely constexpr
// because they delegate to the library's own constexpr classification helpers.

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto isnan(const T x) noexcept -> bool
{
    return detail::impl::constexpr_isnan(static_cast<detail::underlying_type_t<T>>(x));
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto isinf(const T x) noexcept -> bool
{
    return detail::impl::constexpr_isinf(static_cast<detail::underlying_type_t<T>>(x));
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto isfinite(const T x) noexcept -> bool
{
    const auto v {static_cast<detail::underlying_type_t<T>>(x)};
    return !detail::impl::constexpr_isinf(v) && !detail::impl::constexpr_isnan(v);
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto isnormal(const T x) noexcept -> bool
{
    return detail::impl::constexpr_isnormal(static_cast<detail::underlying_type_t<T>>(x));
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto signbit(const T x) noexcept -> bool
{
    return detail::impl::constexpr_signbit(static_cast<detail::underlying_type_t<T>>(x));
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto fpclassify(const T x) noexcept -> int
{
    return detail::impl::constexpr_fpclassify(static_cast<detail::underlying_type_t<T>>(x));
}

// Non-signaling comparison predicates: ordered comparisons that return false
// (rather than raising) when an operand is NAN.

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto isunordered(const T x, const T y) noexcept -> bool
{
    using underlying_type = detail::underlying_type_t<T>;
    return detail::impl::constexpr_isnan(static_cast<underlying_type>(x)) ||
           detail::impl::constexpr_isnan(static_cast<underlying_type>(y));
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto isgreater(const T x, const T y) noexcept -> bool
{
    using underlying_type = detail::underlying_type_t<T>;
    return !isunordered(x, y) && static_cast<underlying_type>(x) > static_cast<underlying_type>(y);
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto isgreaterequal(const T x, const T y) noexcept -> bool
{
    using underlying_type = detail::underlying_type_t<T>;
    return !isunordered(x, y) && static_cast<underlying_type>(x) >= static_cast<underlying_type>(y);
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto isless(const T x, const T y) noexcept -> bool
{
    using underlying_type = detail::underlying_type_t<T>;
    return !isunordered(x, y) && static_cast<underlying_type>(x) < static_cast<underlying_type>(y);
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto islessequal(const T x, const T y) noexcept -> bool
{
    using underlying_type = detail::underlying_type_t<T>;
    return !isunordered(x, y) && static_cast<underlying_type>(x) <= static_cast<underlying_type>(y);
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto islessgreater(const T x, const T y) noexcept -> bool
{
    using underlying_type = detail::underlying_type_t<T>;
    const auto a {static_cast<underlying_type>(x)};
    const auto b {static_cast<underlying_type>(y)};
    return !isunordered(x, y) && (a < b || a > b);
}

// ------------------------------
// Integer-returning functions
// ------------------------------
// These return the safe integer type that matches the width of the std return
// type. A non-finite input has an unspecified std result (and raises FE_INVALID),
// so it is reported as a domain error instead. Not constexpr: the underlying std
// functions are not constexpr in C++20/23.

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] auto lround(const T x) -> i64
{
    using underlying_type = detail::underlying_type_t<T>;
    const auto v {static_cast<underlying_type>(x)};
    if (detail::impl::constexpr_isnan(v) || detail::impl::constexpr_isinf(v))
    {
        detail::impl::throw_cmath_domain<underlying_type>("lround");
    }
    return i64{static_cast<std::int64_t>(BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS::lround(v))};
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] auto llround(const T x) -> i64
{
    using underlying_type = detail::underlying_type_t<T>;
    const auto v {static_cast<underlying_type>(x)};
    if (detail::impl::constexpr_isnan(v) || detail::impl::constexpr_isinf(v))
    {
        detail::impl::throw_cmath_domain<underlying_type>("llround");
    }
    return i64{static_cast<std::int64_t>(BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS::llround(v))};
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] auto lrint(const T x) -> i64
{
    using underlying_type = detail::underlying_type_t<T>;
    const auto v {static_cast<underlying_type>(x)};
    if (detail::impl::constexpr_isnan(v) || detail::impl::constexpr_isinf(v))
    {
        detail::impl::throw_cmath_domain<underlying_type>("lrint");
    }
    return i64{static_cast<std::int64_t>(BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS::lrint(v))};
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] auto llrint(const T x) -> i64
{
    using underlying_type = detail::underlying_type_t<T>;
    const auto v {static_cast<underlying_type>(x)};
    if (detail::impl::constexpr_isnan(v) || detail::impl::constexpr_isinf(v))
    {
        detail::impl::throw_cmath_domain<underlying_type>("llrint");
    }
    return i64{static_cast<std::int64_t>(BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS::llrint(v))};
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] auto ilogb(const T x) -> i32
{
    using underlying_type = detail::underlying_type_t<T>;
    const auto v {static_cast<underlying_type>(x)};
    const auto cls {detail::impl::constexpr_fpclassify(v)};
    if (cls == FP_NAN || cls == FP_INFINITE || cls == FP_ZERO)
    {
        detail::impl::throw_cmath_domain<underlying_type>("ilogb");
    }
    return i32{static_cast<std::int32_t>(BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS::ilogb(v))};
}

#undef BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY
#undef BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BINARY

} // namespace boost::safe_numbers

#undef BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS

#endif // BOOST_SAFE_NUMBERS_CMATH_HPP

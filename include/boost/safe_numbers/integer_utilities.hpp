// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_INTEGER_UTILITIES_HPP
#define BOOST_SAFE_NUMBERS_INTEGER_UTILITIES_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>
#include <boost/safe_numbers/detail/rtz.hpp>
#include <boost/safe_numbers/bit.hpp>

namespace boost::safe_numbers {

// Newton's method as it can't possibly overflow, and converges rapidly
template <detail::non_bounded_unsigned_library_type T>
    requires (!detail::is_verified_type_v<T>)
constexpr auto isqrt(const T val) -> T
{
    using underlying = typename detail::underlying_type_t<T>;

    constexpr auto one {static_cast<underlying>(1)};
    constexpr auto two {static_cast<underlying>(2)};

    auto n {static_cast<underlying>(val)};

    if (n < two)
    {
        return val;
    }

    auto x {n};
    auto y {static_cast<underlying>(x / two + (x & one))}; // (x + 1) / 2 without overflow

    while (y < x)
    {
        x = y;
        y = static_cast<underlying>(x + n / x) / two;
    }

    return T{x};
}

template <detail::non_bounded_unsigned_library_type T>
consteval auto isqrt(const detail::verified_type_basis<T> val) -> detail::verified_type_basis<T>
{
    return detail::verified_type_basis<T>{isqrt(static_cast<T>(val))};
}

template <detail::non_bounded_unsigned_library_type T>
    requires (!detail::is_verified_type_v<T>)
constexpr auto remove_trailing_zeros(const T n)
{
    using underlying = typename detail::underlying_type_t<T>;

    if (static_cast<underlying>(n) == static_cast<underlying>(0))
    {
        return detail::remove_trailing_zeros_return<underlying>{static_cast<underlying>(0), static_cast<std::size_t>(0)};
    }

    return detail::remove_trailing_zeros(static_cast<underlying>(n));
}

template <detail::non_bounded_unsigned_library_type T>
consteval auto remove_trailing_zeros(const detail::verified_type_basis<T> val)
{
    using underlying = typename detail::underlying_type_t<T>;

    if (static_cast<underlying>(val) == static_cast<underlying>(0))
    {
        return detail::remove_trailing_zeros_return<underlying>{static_cast<underlying>(0), static_cast<std::size_t>(0)};
    }

    return detail::remove_trailing_zeros(static_cast<underlying>(val));
}

template <detail::non_bounded_unsigned_library_type T>
    requires (!detail::is_verified_type_v<T>)
constexpr auto is_power_10(const T n) -> bool
{
    using underlying = typename detail::underlying_type_t<T>;

    const auto [trimmed_number, _] = detail::remove_trailing_zeros(static_cast<underlying>(n));
    return trimmed_number == static_cast<underlying>(1);
}

template <detail::non_bounded_unsigned_library_type T>
consteval auto is_power_10(const detail::verified_type_basis<T> n) -> bool
{
    using underlying = typename detail::underlying_type_t<T>;

    const auto [trimmed_number, _] = detail::remove_trailing_zeros(static_cast<underlying>(n));
    return trimmed_number == static_cast<underlying>(1);
}

template <detail::non_bounded_unsigned_library_type T>
    requires (!detail::is_verified_type_v<T>)
constexpr auto is_power_2(const T n) noexcept -> bool
{
    return has_single_bit(n);
}

template <detail::non_bounded_unsigned_library_type T>
consteval auto is_power_2(const detail::verified_type_basis<T> n) noexcept -> bool
{
    return has_single_bit(n);
}

namespace detail {

// Uses simple exponentiation by squaring, which is normally quite performant
// Power (a, b) = 1                     if b = 0
// Power (a, b) = a * Power(a, b - 1)   if b is odd
// Power (a, b) = Power(a, b/2)^2       if b is even
template <non_bounded_unsigned_library_type T>
constexpr auto ipow_impl(T a, T b) -> T
{
    using underlying = underlying_type_t<T>;

    if (b == T{})
    {
        return T{static_cast<underlying>(1)};
    }
    else if (static_cast<underlying>(b) & underlying{1})
    {
        return a * ipow_impl(a, b - T{static_cast<underlying>(1)});
    }
    else
    {
        const auto p {ipow_impl(a, b / T{static_cast<underlying>(2)})};
        return p * p;
    }
}

} // namespace detail

template <detail::non_bounded_unsigned_library_type T>
    requires (!detail::is_verified_type_v<T>)
constexpr auto ipow(const T a, const T b) -> T
{
    return detail::ipow_impl(a, b);
}

template <detail::non_bounded_unsigned_library_type T>
consteval auto ipow(const detail::verified_type_basis<T> a,
                    const detail::verified_type_basis<T> b) -> detail::verified_type_basis<T>
{
    return detail::verified_type_basis<T>{detail::ipow_impl(static_cast<T>(a), static_cast<T>(b))};
}

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_INTEGER_UTILITIES_HPP

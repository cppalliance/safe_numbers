// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_INTEGER_UTILITIES_HPP
#define BOOST_SAFE_NUMBERS_INTEGER_UTILITIES_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>
#include <boost/safe_numbers/detail/rtz.hpp>
#include <boost/safe_numbers/detail/num_digits.hpp>
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

// Integer log base 2: floor(log2(n)) == bit_width(n) - 1
template <detail::non_bounded_unsigned_library_type T>
    requires (!detail::is_verified_type_v<T>)
constexpr auto log2(const T n) noexcept -> int
{
    return bit_width(n) - 1;
}

template <detail::non_bounded_unsigned_library_type T>
consteval auto log2(const detail::verified_type_basis<T> n) noexcept -> int
{
    return bit_width(n) - 1;
}

// Integer log base 10: floor(log10(n)) == num_digits(n) - 1
// Uses MSB-based approximation with power-of-10 table lookup (O(1))
template <detail::non_bounded_unsigned_library_type T>
    requires (!detail::is_verified_type_v<T>)
constexpr auto log10(const T n) noexcept -> int
{
    using underlying = detail::underlying_type_t<T>;
    return detail::num_digits(static_cast<underlying>(n)) - 1;
}

template <detail::non_bounded_unsigned_library_type T>
consteval auto log10(const detail::verified_type_basis<T> n) noexcept -> int
{
    using underlying = detail::underlying_type_t<T>;
    return detail::num_digits(static_cast<underlying>(n)) - 1;
}

namespace detail {

// Iterative exponentiation by squaring: O(log b) multiplications
template <non_bounded_unsigned_library_type T>
constexpr auto ipow_impl(T base, T exp) -> T
{
    using underlying = underlying_type_t<T>;

    auto result = T{static_cast<underlying>(1)};

    while (exp != T{static_cast<underlying>(0)})
    {
        if (static_cast<underlying>(exp) & underlying{1})
        {
            result = result * base;
        }
        exp = exp / T{static_cast<underlying>(2)};
        if (exp != T{static_cast<underlying>(0)})
        {
            base = base * base;
        }
    }

    return result;
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
    // This is a workaround for P2564R3 "Consteval should propagate up"
    // Operate on the underlying type rather than on the verfied type directly
    return detail::verified_type_basis<T>{detail::ipow_impl(static_cast<T>(a), static_cast<T>(b))};
}

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_INTEGER_UTILITIES_HPP

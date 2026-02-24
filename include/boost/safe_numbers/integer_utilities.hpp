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

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/throw_exception.hpp>
#include <stdexcept>

#endif

namespace boost::safe_numbers {

// Newton's method as it can't possibly overflow, and converges rapidly
template <detail::non_bounded_unsigned_library_type T>
[[nodiscard]] constexpr auto isqrt(const T val) -> T
{
    using underlying_type = detail::underlying_type_t<T>;

    constexpr auto one {static_cast<underlying_type>(1)};
    constexpr auto two {static_cast<underlying_type>(2)};

    auto n {static_cast<underlying_type>(val)};

    if (n < two)
    {
        return val;
    }

    auto x {n};
    auto y {static_cast<underlying_type>(x / two + (x & one))}; // (x + 1) / 2 without overflow

    while (y < x)
    {
        x = y;
        y = static_cast<underlying_type>(x + n / x) / two;
    }

    return T{x};
}

template <detail::non_bounded_unsigned_library_type T>
[[nodiscard]] constexpr auto remove_trailing_zeros(const T n)
{
    using underlying_type = detail::underlying_type_t<T>;

    if (static_cast<underlying_type>(n) == static_cast<underlying_type>(0))
    {
        return detail::remove_trailing_zeros_return<underlying_type>{static_cast<underlying_type>(0), static_cast<std::size_t>(0)};
    }

    return detail::remove_trailing_zeros(static_cast<underlying_type>(n));
}

template <detail::non_bounded_unsigned_library_type T>
[[nodiscard]] constexpr auto is_power_10(const T n) -> bool
{
    using underlying_type = detail::underlying_type_t<T>;

    const auto [trimmed_number, _] = detail::remove_trailing_zeros(static_cast<underlying_type>(n));
    return trimmed_number == static_cast<underlying_type>(1);
}

template <detail::non_bounded_unsigned_library_type T>
[[nodiscard]] constexpr auto is_power_2(const T n) noexcept -> bool
{
    return has_single_bit(n);
}

// Integer log base 2: floor(log2(n)) == bit_width(n) - 1
template <detail::non_bounded_unsigned_library_type T>
[[nodiscard]] constexpr auto log2(const T n) -> int
{
    using underlying_type = detail::underlying_type_t<T>;

    if (static_cast<underlying_type>(n) == underlying_type{0})
    {
        BOOST_THROW_EXCEPTION(std::domain_error("log2(0) is undefined"));
    }

    return bit_width(n) - 1;
}

// Integer log base 10: floor(log10(n)) == num_digits(n) - 1
// Uses MSB-based approximation with power-of-10 table lookup (O(1))
template <detail::non_bounded_unsigned_library_type T>
[[nodiscard]] constexpr auto log10(const T n) -> int
{
    using underlying_type = detail::underlying_type_t<T>;

    if (static_cast<underlying_type>(n) == underlying_type{0})
    {
        BOOST_THROW_EXCEPTION(std::domain_error("log10(0) is undefined"));
    }

    return detail::num_digits(static_cast<underlying_type>(n)) - 1;
}

// Integer log arbitrary base: floor(log_base(n))
// Repeated division: O(log_base(n)) divisions
template <detail::non_bounded_unsigned_library_type T>
[[nodiscard]] constexpr auto ilog(const T n, const T base) -> int
{
    using underlying_type = detail::underlying_type_t<T>;

    if (static_cast<underlying_type>(n) == underlying_type{0})
    {
        BOOST_THROW_EXCEPTION(std::domain_error("ilog(0, base) is undefined"));
    }

    if (static_cast<underlying_type>(base) < underlying_type{2})
    {
        BOOST_THROW_EXCEPTION(std::domain_error("ilog(n, base) requires base >= 2"));
    }

    auto result {0};
    auto val {static_cast<underlying_type>(n)};
    const auto b {static_cast<underlying_type>(base)};

    while (val >= b)
    {
        val /= b;
        ++result;
    }

    return result;
}

namespace detail {

// Iterative exponentiation by squaring: O(log b) multiplications
template <non_bounded_unsigned_library_type T>
[[nodiscard]] constexpr auto ipow_impl(T base, T exp) -> T
{
    using underlying_type = underlying_type_t<T>;

    auto result = T{static_cast<underlying_type>(1)};

    while (exp != T{static_cast<underlying_type>(0)})
    {
        if (static_cast<underlying_type>(exp) & underlying_type{1})
        {
            result = result * base;
        }
        exp = exp / T{static_cast<underlying_type>(2)};
        if (exp != T{static_cast<underlying_type>(0)})
        {
            base = base * base;
        }
    }

    return result;
}

} // namespace detail

template <detail::non_bounded_unsigned_library_type T>
[[nodiscard]] constexpr auto ipow(const T a, const T b) -> T
{
    return detail::ipow_impl(a, b);
}

template <detail::integral_library_type T>
[[nodiscard]] constexpr auto abs_diff(const T a, const T b) noexcept -> T
{
    return a > b ? a - b : b - a;
}

template <detail::integral_library_type T>
[[nodiscard]] constexpr auto div_ceil(const T a, const T b) noexcept -> T
{
    using underlying_type = detail::underlying_type_t<T>;

    const auto d {a / b};
    const auto r {a % b};

    if constexpr (std::numeric_limits<T>::is_signed)
    {
        return d + T{static_cast<underlying_type>(r != static_cast<T>(0) && (r ^ b) > static_cast<T>(0) ? 1 : 0)};
    }
    else
    {
        return d + T{static_cast<underlying_type>(r != static_cast<T>(0) ? 1 : 0)};
    }
}

template <detail::integral_library_type T>
[[nodiscard]] constexpr auto next_multiple_of(const T a, const T b) noexcept -> T
{
    return div_ceil(a, b) * b;
}

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_INTEGER_UTILITIES_HPP

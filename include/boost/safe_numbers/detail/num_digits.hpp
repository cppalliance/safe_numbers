// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_NUM_DIGITS_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_NUM_DIGITS_HPP

// https://stackoverflow.com/questions/1489830/efficient-way-to-determine-number-of-digits-in-an-integer
// https://graphics.stanford.edu/~seander/bithacks.html

#include <boost/safe_numbers/detail/int128/bit.hpp>
#include <boost/safe_numbers/detail/int128/detail/config.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/core/bit.hpp>
#include <array>
#include <cstdint>
#include <limits>

#endif

namespace boost::safe_numbers::detail {

// ============================================================================
// Power-of-10 lookup tables (generated at compile time)
// ============================================================================

template <typename T>
consteval auto make_powers_of_10() noexcept
{
    constexpr auto N {static_cast<std::size_t>(std::numeric_limits<T>::digits10 + 1)};

    std::array<T, N> table {};
    table[0] = T{1};
    for (std::size_t i {1}; i < N; ++i)
    {
        table[i] = static_cast<T>(table[i - 1] * T{10});
    }

    return table;
}

inline constexpr auto powers_of_10_u32 {make_powers_of_10<std::uint32_t>()};
inline constexpr auto powers_of_10_u64 {make_powers_of_10<std::uint64_t>()};
inline constexpr auto powers_of_10_u128 {make_powers_of_10<int128::uint128_t>()};

// ============================================================================
// num_digits: counts the number of decimal digits using MSB approximation
//
// Algorithm: log10(x) ~= log2(x) / log2(10) ~= log2(x) / 3.322
// Use (msb * 1000) / 3322 + 1 as an estimate, then refine with a
// table lookup (at most one comparison in each direction).
// ============================================================================

// Overload for types fitting in uint32_t (uint8_t, uint16_t, uint32_t)
template <typename T>
    requires (std::numeric_limits<T>::digits <= 32 && std::is_unsigned_v<T>)
constexpr auto num_digits(const T init_x) noexcept -> int
{
    const auto x {static_cast<std::uint32_t>(init_x)};

    if (x == 0)
    {
        return 1;
    }

    const auto msb {32 - boost::core::countl_zero(x)};

    // Approximate log10
    const auto estimated_digits {(msb * 1000) / 3322 + 1};

    if (estimated_digits < 10 && x >= powers_of_10_u32[static_cast<std::size_t>(estimated_digits)])
    {
        return estimated_digits + 1;
    }

    if (estimated_digits > 1 && x < powers_of_10_u32[static_cast<std::size_t>(estimated_digits - 1)])
    {
        return estimated_digits - 1;
    }

    return estimated_digits;
}

// Overload for uint64_t
constexpr auto num_digits(const std::uint64_t x) noexcept -> int
{
    if (x <= UINT32_MAX)
    {
        return num_digits(static_cast<std::uint32_t>(x));
    }

    const auto msb {64 - boost::core::countl_zero(x)};

    // Approximate log10
    const auto estimated_digits {(msb * 1000) / 3322 + 1};

    if (estimated_digits < 20 && x >= powers_of_10_u64[static_cast<std::size_t>(estimated_digits)])
    {
        return estimated_digits + 1;
    }

    if (estimated_digits > 1 && x < powers_of_10_u64[static_cast<std::size_t>(estimated_digits - 1)])
    {
        return estimated_digits - 1;
    }

    return estimated_digits;
}

// Overload for uint128_t
constexpr auto num_digits(const boost::int128::uint128_t& x) noexcept -> int
{
    if (x.high == UINT64_C(0))
    {
        return num_digits(x.low);
    }

    const auto msb {64 + (64 - boost::int128::detail::countl_zero(x.high))};

    // Approximate log10
    const auto estimated_digits {(msb * 1000) / 3322 + 1};

    if (estimated_digits < 39 && x >= powers_of_10_u128[static_cast<std::size_t>(estimated_digits)])
    {
        return estimated_digits + 1;
    }

    // Estimated digits can't be less than 20 (65+ bits)
    if (x < powers_of_10_u128[static_cast<std::size_t>(estimated_digits - 1)])
    {
        return estimated_digits - 1;
    }

    return estimated_digits;
}

} // namespace boost::safe_numbers::detail

#endif // BOOST_SAFE_NUMBERS_DETAIL_NUM_DIGITS_HPP

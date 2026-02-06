// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_BOUNDED_INTEGERS_HPP
#define BOOST_SAFE_NUMBERS_BOUNDED_INTEGERS_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/concepts.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>
#include <boost/safe_numbers/overflow_policy.hpp>
#include <boost/safe_numbers/unsigned_integers.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/throw_exception.hpp>
#include <concepts>
#include <compare>
#include <limits>
#include <stdexcept>
#include <cstdint>
#include <cstdlib>
#include <utility>
#include <optional>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers {

namespace detail {

template <typename T>
concept valid_bound = is_unsigned_library_type_v<T> || is_fundamental_unsigned_integral_v<T>;

template <typename T>
    requires valid_bound<T>
consteval auto raw_value(T val) noexcept
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

} // namespace detail

template <auto Min, auto Max>
    requires (detail::valid_bound<decltype(Min)> &&
              detail::valid_bound<decltype(Max)> &&
              detail::raw_value(Max) > detail::raw_value(Min))
class bounded_uint
{
public:

    using basis_type = std::conditional_t<(std::numeric_limits<std::uint8_t>::max() >= detail::raw_value(Max)), u8,
                           std::conditional_t<(std::numeric_limits<std::uint16_t>::max() >= detail::raw_value(Max)), u16,
                               std::conditional_t<(std::numeric_limits<std::uint32_t>::max() >= detail::raw_value(Max)), u32,
                                   std::conditional_t<(std::numeric_limits<std::uint64_t>::max() >= detail::raw_value(Max)), u64, u128>>>>;

private:

    basis_type basis_ {static_cast<basis_type>(detail::raw_value(Min))};

public:

    explicit constexpr bounded_uint(const basis_type val)
    {
        constexpr auto min_val {static_cast<basis_type>(detail::raw_value(Min))};
        constexpr auto max_val {static_cast<basis_type>(detail::raw_value(Max))};

        if (val < min_val || val > max_val)
        {
            BOOST_THROW_EXCEPTION(std::domain_error("Construction from value outside the bounds"));
        }

        basis_ = val;
    }

    template <typename OtherBasis>
        requires (detail::is_unsigned_library_type_v<OtherBasis> || detail::is_fundamental_unsigned_integral_v<OtherBasis>)
    [[nodiscard]] explicit constexpr operator OtherBasis() const noexcept
    {
        if constexpr (sizeof(OtherBasis) < sizeof(basis_type))
        {
            static_assert(detail::dependent_false<OtherBasis>, "Narrowing conversions are not allowed");
        }

        return static_cast<OtherBasis>(static_cast<detail::underlying_type_t<basis_type>>(basis_));
    }

    template <auto Min2, auto Max2>
    [[nodiscard]] explicit constexpr operator bounded_uint<Min2, Max2>() const
    {
        using target_basis = typename bounded_uint<Min2, Max2>::basis_type;
        const auto raw {static_cast<detail::underlying_type_t<basis_type>>(basis_)};
        return bounded_uint<Min2, Max2>{static_cast<target_basis>(raw)};
    }

    [[nodiscard]] friend constexpr auto operator<=>(bounded_uint lhs, bounded_uint rhs) noexcept
        -> std::strong_ordering = default;

};

template <auto Min, auto Max>
[[nodiscard]] constexpr auto operator+(const bounded_uint<Min, Max> lhs,
                                       const bounded_uint<Min, Max> rhs) -> bounded_uint<Min, Max>
{
    using basis = typename bounded_uint<Min, Max>::basis_type;
    return bounded_uint<Min, Max>{static_cast<basis>(lhs) + static_cast<basis>(rhs)};
}

template <auto Min, auto Max>
[[nodiscard]] constexpr auto operator-(const bounded_uint<Min, Max> lhs,
                                       const bounded_uint<Min, Max> rhs) -> bounded_uint<Min, Max>
{
    using basis = typename bounded_uint<Min, Max>::basis_type;
    return bounded_uint<Min, Max>{static_cast<basis>(lhs) - static_cast<basis>(rhs)};
}

template <auto Min, auto Max>
[[nodiscard]] constexpr auto operator*(const bounded_uint<Min, Max> lhs,
                                       const bounded_uint<Min, Max> rhs) -> bounded_uint<Min, Max>
{
    using basis = typename bounded_uint<Min, Max>::basis_type;
    return bounded_uint<Min, Max>{static_cast<basis>(lhs) * static_cast<basis>(rhs)};
}

template <auto Min, auto Max>
[[nodiscard]] constexpr auto operator/(const bounded_uint<Min, Max> lhs,
                                       const bounded_uint<Min, Max> rhs) -> bounded_uint<Min, Max>
{
    using basis = typename bounded_uint<Min, Max>::basis_type;
    return bounded_uint<Min, Max>{static_cast<basis>(lhs) / static_cast<basis>(rhs)};
}

template <auto Min, auto Max>
[[nodiscard]] constexpr auto operator%(const bounded_uint<Min, Max> lhs,
                                       const bounded_uint<Min, Max> rhs) -> bounded_uint<Min, Max>
{
    using basis = typename bounded_uint<Min, Max>::basis_type;
    return bounded_uint<Min, Max>{static_cast<basis>(lhs) % static_cast<basis>(rhs)};
}

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_BOUNDED_INTEGERS_HPP

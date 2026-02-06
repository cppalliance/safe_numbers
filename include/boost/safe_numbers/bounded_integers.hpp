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

template <auto Min, auto Max>
    requires (Max > Min &&
        Max >= static_cast<decltype(Max)>(0) &&
        Min >= static_cast<decltype(Min)>(0) &&
        std::numeric_limits<decltype(Min)>::is_specialized &&
        std::numeric_limits<decltype(Max)>::is_specialized &&
        Max <= std::numeric_limits<int128::uint128_t>::max())
class bounded_uint
{
public:

    using basis_type = std::conditional_t<std::numeric_limits<std::uint8_t>::max() >= Max, u8,
                           std::conditional_t<std::numeric_limits<std::uint16_t>::max() >= Max, u16,
                               std::conditional_t<std::numeric_limits<std::uint32_t>::max() >= Max, u32,
                                   std::conditional_t<std::numeric_limits<std::uint64_t>::max() >= Max, u64, u128>>>>;

    explicit constexpr bounded_uint(const basis_type val) noexcept : basis_(val) {}

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

    [[nodiscard]] friend constexpr auto operator<=>(bounded_uint lhs, bounded_uint rhs) noexcept
        -> std::strong_ordering = default;

private:

    basis_type basis_ {static_cast<basis_type>(Min)};
};

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_BOUNDED_INTEGERS_HPP

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
concept valid_bound = !std::is_same_v<T, bool> && (is_unsigned_library_type_v<T> || is_fundamental_unsigned_integral_v<T>);

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
    [[nodiscard]] explicit constexpr operator OtherBasis() const
    {
        const auto raw {static_cast<detail::underlying_type_t<basis_type>>(basis_)};

        if constexpr (sizeof(OtherBasis) < sizeof(basis_type))
        {
            using raw_other = detail::underlying_type_t<OtherBasis>;
            if (raw > static_cast<detail::underlying_type_t<basis_type>>(std::numeric_limits<raw_other>::max()))
            {
                BOOST_THROW_EXCEPTION(std::domain_error("Overflow in conversion to smaller type"));
            }

            return static_cast<OtherBasis>(static_cast<raw_other>(raw));
        }
        else
        {
            return static_cast<OtherBasis>(raw);
        }
    }

    template <auto Min2, auto Max2>
    [[nodiscard]] explicit constexpr operator bounded_uint<Min2, Max2>() const
    {
        using target_basis = typename bounded_uint<Min2, Max2>::basis_type;
        const auto raw {static_cast<detail::underlying_type_t<basis_type>>(basis_)};
        return bounded_uint<Min2, Max2>{static_cast<target_basis>(raw)};
    }

    [[nodiscard]] explicit constexpr operator basis_type() const noexcept { return basis_; }

    [[nodiscard]] friend constexpr auto operator<=>(bounded_uint lhs, bounded_uint rhs) noexcept
        -> std::strong_ordering = default;


    constexpr auto operator+=(bounded_uint<Min, Max> rhs) -> bounded_uint&;

    constexpr auto operator-=(bounded_uint<Min, Max> rhs) -> bounded_uint&;

    constexpr auto operator*=(bounded_uint<Min, Max> rhs) -> bounded_uint&;

    constexpr auto operator/=(bounded_uint<Min, Max> rhs) -> bounded_uint&;

    constexpr auto operator++() -> bounded_uint&;

    constexpr auto operator++(int) -> bounded_uint;

    constexpr auto operator--() -> bounded_uint&;

    constexpr auto operator--(int) -> bounded_uint;
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

template <auto Min, auto Max>
    requires (detail::valid_bound<decltype(Min)> &&
              detail::valid_bound<decltype(Max)> &&
              detail::raw_value(Max) > detail::raw_value(Min))
constexpr auto bounded_uint<Min, Max>::operator+=(bounded_uint<Min, Max> rhs) -> bounded_uint&
{
    *this = *this + rhs;
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_bound<decltype(Min)> &&
              detail::valid_bound<decltype(Max)> &&
              detail::raw_value(Max) > detail::raw_value(Min))
constexpr auto bounded_uint<Min, Max>::operator-=(bounded_uint<Min, Max> rhs) -> bounded_uint&
{
    *this = *this - rhs;
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_bound<decltype(Min)> &&
              detail::valid_bound<decltype(Max)> &&
              detail::raw_value(Max) > detail::raw_value(Min))
constexpr auto bounded_uint<Min, Max>::operator*=(bounded_uint<Min, Max> rhs) -> bounded_uint&
{
    *this = *this * rhs;
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_bound<decltype(Min)> &&
              detail::valid_bound<decltype(Max)> &&
              detail::raw_value(Max) > detail::raw_value(Min))
constexpr auto bounded_uint<Min, Max>::operator/=(bounded_uint<Min, Max> rhs) -> bounded_uint&
{
    *this = *this / rhs;
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_bound<decltype(Min)> &&
              detail::valid_bound<decltype(Max)> &&
              detail::raw_value(Max) > detail::raw_value(Min))
constexpr auto bounded_uint<Min, Max>::operator++() -> bounded_uint&
{
    auto val {static_cast<basis_type>(*this) + basis_type{1}};
    *this = bounded_uint{val};
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_bound<decltype(Min)> &&
              detail::valid_bound<decltype(Max)> &&
              detail::raw_value(Max) > detail::raw_value(Min))
constexpr auto bounded_uint<Min, Max>::operator++(int) -> bounded_uint
{
    auto tmp {*this};
    ++(*this);
    return tmp;
}

template <auto Min, auto Max>
    requires (detail::valid_bound<decltype(Min)> &&
              detail::valid_bound<decltype(Max)> &&
              detail::raw_value(Max) > detail::raw_value(Min))
constexpr auto bounded_uint<Min, Max>::operator--() -> bounded_uint&
{
    auto val {static_cast<basis_type>(*this) - basis_type{1}};
    *this = bounded_uint{val};
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_bound<decltype(Min)> &&
              detail::valid_bound<decltype(Max)> &&
              detail::raw_value(Max) > detail::raw_value(Min))
constexpr auto bounded_uint<Min, Max>::operator--(int) -> bounded_uint
{
    auto tmp {*this};
    --(*this);
    return tmp;
}

} // namespace boost::safe_numbers

#define BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP(OP_NAME, OP_SYMBOL)                                                             \
template <auto LHSMin, auto LHSMax, auto RHSMin, auto RHSMax>                                                                          \
    requires (LHSMin != RHSMin || LHSMax != RHSMax)                                                                                     \
constexpr auto OP_SYMBOL(const boost::safe_numbers::bounded_uint<LHSMin, LHSMax>,                                                      \
                         const boost::safe_numbers::bounded_uint<RHSMin, RHSMax>)                                                       \
{                                                                                                                                       \
    static_assert(boost::safe_numbers::detail::dependent_false<                                                                         \
                      boost::safe_numbers::bounded_uint<LHSMin, LHSMax>,                                                                \
                      boost::safe_numbers::bounded_uint<RHSMin, RHSMax>>,                                                               \
                  "Can not perform " OP_NAME " between bounded_uint types with different bounds. "                                       \
                  "Both operands must have the same Min and Max.");                                                                      \
                                                                                                                                        \
    return boost::safe_numbers::bounded_uint<LHSMin, LHSMax>(                                                                           \
        typename boost::safe_numbers::bounded_uint<LHSMin, LHSMax>::basis_type{0});                                                     \
}

namespace boost::safe_numbers {

BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("addition", operator+)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("subtraction", operator-)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("multiplication", operator*)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("division", operator/)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("modulo", operator%)

} // namespace boost::safe_numbers

#undef BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP

#endif // BOOST_SAFE_NUMBERS_BOUNDED_INTEGERS_HPP

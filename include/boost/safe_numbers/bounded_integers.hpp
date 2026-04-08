// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_BOUNDED_INTEGERS_HPP
#define BOOST_SAFE_NUMBERS_BOUNDED_INTEGERS_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>
#include <boost/safe_numbers/detail/throw_exception.hpp>
#include <boost/safe_numbers/detail/int128/string.hpp>
#include <boost/safe_numbers/overflow_policy.hpp>
#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/signed_integers.hpp>

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
#include <string>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers {

namespace detail {

template <fundamental_unsigned_integral T>
auto to_string_val(T val) -> std::string
{
    using std::to_string;
    using boost::int128::to_string;
    return to_string(val);
}

template <fundamental_signed_integral T>
auto to_string_val(T val) -> std::string
{
    using std::to_string;
    using boost::int128::to_string;
    return to_string(val);
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

    using underlying_type = detail::underlying_type_t<basis_type>;
    basis_type basis_ {};

public:

    explicit constexpr bounded_uint(const basis_type val)
    {
        constexpr auto min_raw {static_cast<underlying_type>(detail::raw_value(Min))};
        constexpr auto max_raw {static_cast<underlying_type>(detail::raw_value(Max))};
        constexpr auto min_val {basis_type{min_raw}};
        constexpr auto max_val {basis_type{max_raw}};

        if (val < min_val || val > max_val)
        {
            if (std::is_constant_evaluated())
            {
                throw std::domain_error("bounded_uint value out of range");
            }
            else
            {
                BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_uint value out of range");
            }
        }

        basis_ = val;
    }

    explicit constexpr bounded_uint(const underlying_type val) : bounded_uint{basis_type{val}} {}

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
                if (std::is_constant_evaluated())
                {
                    throw std::domain_error("bounded_uint conversion overflow");
                }
                else
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_uint conversion overflow");
                }
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

    [[nodiscard]] explicit constexpr operator underlying_type() const noexcept { return static_cast<underlying_type>(basis_); }

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
    using underlying = detail::underlying_type_t<basis>;
    constexpr auto min_raw {static_cast<underlying>(detail::raw_value(Min))};
    constexpr auto max_raw {static_cast<underlying>(detail::raw_value(Max))};
    const auto lhs_raw {static_cast<underlying>(static_cast<basis>(lhs))};
    const auto rhs_raw {static_cast<underlying>(static_cast<basis>(rhs))};

    underlying res {};
    if (detail::impl::unsigned_no_intrin_add(lhs_raw, rhs_raw, res))
    {
        if (std::is_constant_evaluated())
        {
            throw std::overflow_error("bounded_uint addition overflow");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "bounded_uint addition overflow");
        }
    }

    if (res < min_raw || res > max_raw)
    {
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_uint addition result out of range");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_uint addition result out of range");
        }
    }

    return bounded_uint<Min, Max>{basis{res}};
}

template <auto Min, auto Max>
[[nodiscard]] constexpr auto operator-(const bounded_uint<Min, Max> lhs,
                                       const bounded_uint<Min, Max> rhs) -> bounded_uint<Min, Max>
{
    using basis = typename bounded_uint<Min, Max>::basis_type;
    using underlying = detail::underlying_type_t<basis>;
    constexpr auto min_raw {static_cast<underlying>(detail::raw_value(Min))};
    constexpr auto max_raw {static_cast<underlying>(detail::raw_value(Max))};
    const auto lhs_raw {static_cast<underlying>(static_cast<basis>(lhs))};
    const auto rhs_raw {static_cast<underlying>(static_cast<basis>(rhs))};

    underlying res {};
    if (detail::impl::unsigned_no_intrin_sub(lhs_raw, rhs_raw, res))
    {
        if (std::is_constant_evaluated())
        {
            throw std::underflow_error("bounded_uint subtraction underflow");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, "bounded_uint subtraction underflow");
        }
    }

    if (res < min_raw || res > max_raw)
    {
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_uint subtraction result out of range");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_uint subtraction result out of range");
        }
    }

    return bounded_uint<Min, Max>{basis{res}};
}

template <auto Min, auto Max>
[[nodiscard]] constexpr auto operator*(const bounded_uint<Min, Max> lhs,
                                       const bounded_uint<Min, Max> rhs) -> bounded_uint<Min, Max>
{
    using basis = typename bounded_uint<Min, Max>::basis_type;
    using underlying = detail::underlying_type_t<basis>;
    constexpr auto min_raw {static_cast<underlying>(detail::raw_value(Min))};
    constexpr auto max_raw {static_cast<underlying>(detail::raw_value(Max))};
    const auto lhs_raw {static_cast<underlying>(static_cast<basis>(lhs))};
    const auto rhs_raw {static_cast<underlying>(static_cast<basis>(rhs))};

    underlying res {};
    if (detail::impl::no_intrin_mul(lhs_raw, rhs_raw, res))
    {
        if (std::is_constant_evaluated())
        {
            throw std::overflow_error("bounded_uint multiplication overflow");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "bounded_uint multiplication overflow");
        }
    }

    if (res < min_raw || res > max_raw)
    {
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_uint multiplication result out of range");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_uint multiplication result out of range");
        }
    }

    return bounded_uint<Min, Max>{basis{res}};
}

template <auto Min, auto Max>
[[nodiscard]] constexpr auto operator/(const bounded_uint<Min, Max> lhs,
                                       const bounded_uint<Min, Max> rhs) -> bounded_uint<Min, Max>
{
    using basis = typename bounded_uint<Min, Max>::basis_type;
    using underlying = detail::underlying_type_t<basis>;
    constexpr auto min_raw {static_cast<underlying>(detail::raw_value(Min))};
    constexpr auto max_raw {static_cast<underlying>(detail::raw_value(Max))};
    const auto lhs_raw {static_cast<underlying>(static_cast<basis>(lhs))};
    const auto rhs_raw {static_cast<underlying>(static_cast<basis>(rhs))};

    if (rhs_raw == 0U) [[unlikely]]
    {
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_uint division by zero");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_uint division by zero");
        }
    }

    underlying res {};
    if constexpr (std::is_same_v<underlying, std::uint8_t> || std::is_same_v<underlying, std::uint16_t>)
    {
        res = static_cast<underlying>(lhs_raw / rhs_raw);
    }
    else
    {
        res = lhs_raw / rhs_raw;
    }

    if (res < min_raw || res > max_raw)
    {
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_uint division result out of range");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_uint division result out of range");
        }
    }

    return bounded_uint<Min, Max>{basis{res}};
}

template <auto Min, auto Max>
[[nodiscard]] constexpr auto operator%(const bounded_uint<Min, Max> lhs,
                                       const bounded_uint<Min, Max> rhs) -> bounded_uint<Min, Max>
{
    using basis = typename bounded_uint<Min, Max>::basis_type;
    using underlying = detail::underlying_type_t<basis>;
    constexpr auto min_raw {static_cast<underlying>(detail::raw_value(Min))};
    constexpr auto max_raw {static_cast<underlying>(detail::raw_value(Max))};
    const auto lhs_raw {static_cast<underlying>(static_cast<basis>(lhs))};
    const auto rhs_raw {static_cast<underlying>(static_cast<basis>(rhs))};

    if (rhs_raw == 0U) [[unlikely]]
    {
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_uint modulo by zero");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_uint modulo by zero");
        }
    }

    underlying res {};
    if constexpr (std::is_same_v<underlying, std::uint8_t> || std::is_same_v<underlying, std::uint16_t>)
    {
        res = static_cast<underlying>(lhs_raw % rhs_raw);
    }
    else
    {
        res = lhs_raw % rhs_raw;
    }

    if (res < min_raw || res > max_raw)
    {
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_uint modulo result out of range");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_uint modulo result out of range");
        }
    }

    return bounded_uint<Min, Max>{basis{res}};
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
    using underlying = detail::underlying_type_t<basis_type>;
    constexpr auto min_raw {static_cast<underlying>(detail::raw_value(Min))};
    constexpr auto max_raw {static_cast<underlying>(detail::raw_value(Max))};
    const auto raw {static_cast<underlying>(static_cast<basis_type>(*this))};

    underlying res {};
    if (detail::impl::unsigned_no_intrin_add(raw, static_cast<underlying>(1U), res))
    {
        if (std::is_constant_evaluated())
        {
            throw std::overflow_error("bounded_uint increment overflow");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "bounded_uint increment overflow");
        }
    }

    if (res > max_raw)
    {
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_uint increment result out of range");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_uint increment result out of range");
        }
    }

    *this = bounded_uint{basis_type{res}};
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
    using underlying = detail::underlying_type_t<basis_type>;
    constexpr auto min_raw {static_cast<underlying>(detail::raw_value(Min))};
    constexpr auto max_raw {static_cast<underlying>(detail::raw_value(Max))};
    const auto raw {static_cast<underlying>(static_cast<basis_type>(*this))};

    underlying res {};
    if (detail::impl::unsigned_no_intrin_sub(raw, static_cast<underlying>(1U), res))
    {
        if (std::is_constant_evaluated())
        {
            throw std::underflow_error("bounded_uint decrement underflow");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, "bounded_uint decrement underflow");
        }
    }

    if (res < min_raw)
    {
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_uint decrement result out of range");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_uint decrement result out of range");
        }
    }

    *this = bounded_uint{basis_type{res}};
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

// ============================================================
// bounded_int
// ============================================================

namespace boost::safe_numbers {

template <auto Min, auto Max>
    requires (detail::valid_signed_bound<decltype(Min)> &&
              detail::valid_signed_bound<decltype(Max)> &&
              detail::signed_raw_value(Max) > detail::signed_raw_value(Min))
class bounded_int
{
public:

    using basis_type = std::conditional_t<
        (detail::signed_raw_value(Min) >= std::numeric_limits<std::int8_t>::min() &&
         detail::signed_raw_value(Max) <= std::numeric_limits<std::int8_t>::max()), i8,
        std::conditional_t<
            (detail::signed_raw_value(Min) >= std::numeric_limits<std::int16_t>::min() &&
             detail::signed_raw_value(Max) <= std::numeric_limits<std::int16_t>::max()), i16,
            std::conditional_t<
                (detail::signed_raw_value(Min) >= std::numeric_limits<std::int32_t>::min() &&
                 detail::signed_raw_value(Max) <= std::numeric_limits<std::int32_t>::max()), i32,
                std::conditional_t<
                    (detail::signed_raw_value(Min) >= std::numeric_limits<std::int64_t>::min() &&
                     detail::signed_raw_value(Max) <= std::numeric_limits<std::int64_t>::max()), i64, i128>>>>;

private:

    using underlying_type = detail::underlying_type_t<basis_type>;
    basis_type basis_ {};

public:

    explicit constexpr bounded_int(const basis_type val)
    {
        constexpr auto min_raw {static_cast<underlying_type>(detail::signed_raw_value(Min))};
        constexpr auto max_raw {static_cast<underlying_type>(detail::signed_raw_value(Max))};
        constexpr auto min_val {basis_type{min_raw}};
        constexpr auto max_val {basis_type{max_raw}};

        if (val < min_val || val > max_val)
        {
            if (std::is_constant_evaluated())
            {
                throw std::domain_error("bounded_int value out of range");
            }
            else
            {
                BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_int value out of range");
            }
        }

        basis_ = val;
    }

    explicit constexpr bounded_int(const underlying_type val) : bounded_int{basis_type{val}} {}

    template <typename OtherBasis>
        requires (detail::is_signed_library_type_v<OtherBasis> || detail::is_fundamental_signed_integral_v<OtherBasis>)
    [[nodiscard]] explicit constexpr operator OtherBasis() const
    {
        const auto raw {static_cast<detail::underlying_type_t<basis_type>>(basis_)};

        if constexpr (sizeof(OtherBasis) < sizeof(basis_type))
        {
            using raw_other = detail::underlying_type_t<OtherBasis>;
            if (raw > static_cast<detail::underlying_type_t<basis_type>>(std::numeric_limits<raw_other>::max()) ||
                raw < static_cast<detail::underlying_type_t<basis_type>>(std::numeric_limits<raw_other>::min()))
            {
                if (std::is_constant_evaluated())
                {
                    throw std::domain_error("bounded_int conversion overflow");
                }
                else
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_int conversion overflow");
                }
            }

            return static_cast<OtherBasis>(static_cast<raw_other>(raw));
        }
        else
        {
            return static_cast<OtherBasis>(raw);
        }
    }

    template <auto Min2, auto Max2>
    [[nodiscard]] explicit constexpr operator bounded_int<Min2, Max2>() const
    {
        using target_basis = typename bounded_int<Min2, Max2>::basis_type;
        using target_underlying = detail::underlying_type_t<target_basis>;
        const auto raw {static_cast<detail::underlying_type_t<basis_type>>(basis_)};
        return bounded_int<Min2, Max2>{target_basis{static_cast<target_underlying>(raw)}};
    }

    [[nodiscard]] explicit constexpr operator basis_type() const noexcept { return basis_; }

    [[nodiscard]] explicit constexpr operator underlying_type() const noexcept { return static_cast<underlying_type>(basis_); }

    [[nodiscard]] friend constexpr auto operator<=>(bounded_int lhs, bounded_int rhs) noexcept
        -> std::strong_ordering = default;

    [[nodiscard]] constexpr auto operator+() const noexcept -> bounded_int { return *this; }

    [[nodiscard]] constexpr auto operator-() const -> bounded_int;

    constexpr auto operator+=(bounded_int<Min, Max> rhs) -> bounded_int&;

    constexpr auto operator-=(bounded_int<Min, Max> rhs) -> bounded_int&;

    constexpr auto operator*=(bounded_int<Min, Max> rhs) -> bounded_int&;

    constexpr auto operator/=(bounded_int<Min, Max> rhs) -> bounded_int&;

    constexpr auto operator++() -> bounded_int&;

    constexpr auto operator++(int) -> bounded_int;

    constexpr auto operator--() -> bounded_int&;

    constexpr auto operator--(int) -> bounded_int;
};

// ------------------------------
// Unary negation
// ------------------------------

template <auto Min, auto Max>
    requires (detail::valid_signed_bound<decltype(Min)> &&
              detail::valid_signed_bound<decltype(Max)> &&
              detail::signed_raw_value(Max) > detail::signed_raw_value(Min))
constexpr auto bounded_int<Min, Max>::operator-() const -> bounded_int
{
    using underlying = detail::underlying_type_t<basis_type>;
    const auto raw {static_cast<underlying>(basis_)};

    if (raw == std::numeric_limits<underlying>::min()) [[unlikely]]
    {
        if (std::is_constant_evaluated())
        {
            throw std::overflow_error("bounded_int negation overflow");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "bounded_int negation overflow");
        }
    }

    const auto negated {static_cast<underlying>(-raw)};
    return bounded_int{basis_type{negated}};
}

// ------------------------------
// Addition
// ------------------------------

template <auto Min, auto Max>
[[nodiscard]] constexpr auto operator+(const bounded_int<Min, Max> lhs,
                                       const bounded_int<Min, Max> rhs) -> bounded_int<Min, Max>
{
    using basis = typename bounded_int<Min, Max>::basis_type;
    using underlying = detail::underlying_type_t<basis>;
    constexpr auto min_raw {static_cast<underlying>(detail::signed_raw_value(Min))};
    constexpr auto max_raw {static_cast<underlying>(detail::signed_raw_value(Max))};
    const auto lhs_raw {static_cast<underlying>(static_cast<basis>(lhs))};
    const auto rhs_raw {static_cast<underlying>(static_cast<basis>(rhs))};

    underlying res {};
    const auto status {detail::impl::signed_no_intrin_add(lhs_raw, rhs_raw, res)};
    if (status == detail::impl::signed_overflow_status::overflow)
    {
        if (std::is_constant_evaluated())
        {
            throw std::overflow_error("bounded_int addition overflow");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "bounded_int addition overflow");
        }
    }
    else if (status == detail::impl::signed_overflow_status::underflow)
    {
        if (std::is_constant_evaluated())
        {
            throw std::underflow_error("bounded_int addition underflow");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, "bounded_int addition underflow");
        }
    }

    if (res < min_raw || res > max_raw)
    {
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_int addition result out of range");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_int addition result out of range");
        }
    }

    return bounded_int<Min, Max>{basis{res}};
}

// ------------------------------
// Subtraction
// ------------------------------

template <auto Min, auto Max>
[[nodiscard]] constexpr auto operator-(const bounded_int<Min, Max> lhs,
                                       const bounded_int<Min, Max> rhs) -> bounded_int<Min, Max>
{
    using basis = typename bounded_int<Min, Max>::basis_type;
    using underlying = detail::underlying_type_t<basis>;
    constexpr auto min_raw {static_cast<underlying>(detail::signed_raw_value(Min))};
    constexpr auto max_raw {static_cast<underlying>(detail::signed_raw_value(Max))};
    const auto lhs_raw {static_cast<underlying>(static_cast<basis>(lhs))};
    const auto rhs_raw {static_cast<underlying>(static_cast<basis>(rhs))};

    underlying res {};
    const auto status {detail::impl::signed_no_intrin_sub(lhs_raw, rhs_raw, res)};
    if (status == detail::impl::signed_overflow_status::overflow)
    {
        if (std::is_constant_evaluated())
        {
            throw std::overflow_error("bounded_int subtraction overflow");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "bounded_int subtraction overflow");
        }
    }
    else if (status == detail::impl::signed_overflow_status::underflow)
    {
        if (std::is_constant_evaluated())
        {
            throw std::underflow_error("bounded_int subtraction underflow");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, "bounded_int subtraction underflow");
        }
    }

    if (res < min_raw || res > max_raw)
    {
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_int subtraction result out of range");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_int subtraction result out of range");
        }
    }

    return bounded_int<Min, Max>{basis{res}};
}

// ------------------------------
// Multiplication
// ------------------------------

template <auto Min, auto Max>
[[nodiscard]] constexpr auto operator*(const bounded_int<Min, Max> lhs,
                                       const bounded_int<Min, Max> rhs) -> bounded_int<Min, Max>
{
    using basis = typename bounded_int<Min, Max>::basis_type;
    using underlying = detail::underlying_type_t<basis>;
    constexpr auto min_raw {static_cast<underlying>(detail::signed_raw_value(Min))};
    constexpr auto max_raw {static_cast<underlying>(detail::signed_raw_value(Max))};
    const auto lhs_raw {static_cast<underlying>(static_cast<basis>(lhs))};
    const auto rhs_raw {static_cast<underlying>(static_cast<basis>(rhs))};

    underlying res {};
    const auto status {detail::impl::signed_no_intrin_mul(lhs_raw, rhs_raw, res)};
    if (status == detail::impl::signed_overflow_status::overflow)
    {
        if (std::is_constant_evaluated())
        {
            throw std::overflow_error("bounded_int multiplication overflow");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "bounded_int multiplication overflow");
        }
    }
    else if (status == detail::impl::signed_overflow_status::underflow)
    {
        if (std::is_constant_evaluated())
        {
            throw std::underflow_error("bounded_int multiplication underflow");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, "bounded_int multiplication underflow");
        }
    }

    if (res < min_raw || res > max_raw)
    {
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_int multiplication result out of range");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_int multiplication result out of range");
        }
    }

    return bounded_int<Min, Max>{basis{res}};
}

// ------------------------------
// Division
// ------------------------------

template <auto Min, auto Max>
[[nodiscard]] constexpr auto operator/(const bounded_int<Min, Max> lhs,
                                       const bounded_int<Min, Max> rhs) -> bounded_int<Min, Max>
{
    using basis = typename bounded_int<Min, Max>::basis_type;
    using underlying = detail::underlying_type_t<basis>;
    constexpr auto min_raw {static_cast<underlying>(detail::signed_raw_value(Min))};
    constexpr auto max_raw {static_cast<underlying>(detail::signed_raw_value(Max))};
    const auto lhs_raw {static_cast<underlying>(static_cast<basis>(lhs))};
    const auto rhs_raw {static_cast<underlying>(static_cast<basis>(rhs))};

    if (rhs_raw == static_cast<underlying>(0)) [[unlikely]]
    {
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_int division by zero");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_int division by zero");
        }
    }

    if (lhs_raw == std::numeric_limits<underlying>::min() &&
        rhs_raw == static_cast<underlying>(-1)) [[unlikely]]
    {
        if (std::is_constant_evaluated())
        {
            throw std::overflow_error("bounded_int division overflow");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "bounded_int division overflow");
        }
    }

    // Fast path: 0 / x = 0
    if (lhs_raw == static_cast<underlying>(0))
    {
        return bounded_int<Min, Max>{basis{static_cast<underlying>(0)}};
    }

    underlying res {};
    if constexpr (std::is_same_v<underlying, std::int8_t> || std::is_same_v<underlying, std::int16_t>)
    {
        res = static_cast<underlying>(lhs_raw / rhs_raw);
    }
    else
    {
        res = lhs_raw / rhs_raw;
    }

    if (res < min_raw || res > max_raw)
    {
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_int division result out of range");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_int division result out of range");
        }
    }

    return bounded_int<Min, Max>{basis{res}};
}

// ------------------------------
// Modulo
// ------------------------------

template <auto Min, auto Max>
[[nodiscard]] constexpr auto operator%(const bounded_int<Min, Max> lhs,
                                       const bounded_int<Min, Max> rhs) -> bounded_int<Min, Max>
{
    using basis = typename bounded_int<Min, Max>::basis_type;
    using underlying = detail::underlying_type_t<basis>;
    constexpr auto min_raw {static_cast<underlying>(detail::signed_raw_value(Min))};
    constexpr auto max_raw {static_cast<underlying>(detail::signed_raw_value(Max))};
    const auto lhs_raw {static_cast<underlying>(static_cast<basis>(lhs))};
    const auto rhs_raw {static_cast<underlying>(static_cast<basis>(rhs))};

    if (rhs_raw == static_cast<underlying>(0)) [[unlikely]]
    {
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_int modulo by zero");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_int modulo by zero");
        }
    }

    if (lhs_raw == std::numeric_limits<underlying>::min() &&
        rhs_raw == static_cast<underlying>(-1)) [[unlikely]]
    {
        if (std::is_constant_evaluated())
        {
            throw std::overflow_error("bounded_int modulo overflow");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "bounded_int modulo overflow");
        }
    }

    // Fast path: 0 % x = 0
    if (lhs_raw == static_cast<underlying>(0))
    {
        return bounded_int<Min, Max>{basis{static_cast<underlying>(0)}};
    }

    underlying res {};
    if constexpr (std::is_same_v<underlying, std::int8_t> || std::is_same_v<underlying, std::int16_t>)
    {
        res = static_cast<underlying>(lhs_raw % rhs_raw);
    }
    else
    {
        res = lhs_raw % rhs_raw;
    }

    if (res < min_raw || res > max_raw)
    {
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_int modulo result out of range");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_int modulo result out of range");
        }
    }

    return bounded_int<Min, Max>{basis{res}};
}

// ------------------------------
// Compound assignment
// ------------------------------

template <auto Min, auto Max>
    requires (detail::valid_signed_bound<decltype(Min)> &&
              detail::valid_signed_bound<decltype(Max)> &&
              detail::signed_raw_value(Max) > detail::signed_raw_value(Min))
constexpr auto bounded_int<Min, Max>::operator+=(bounded_int<Min, Max> rhs) -> bounded_int&
{
    *this = *this + rhs;
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_signed_bound<decltype(Min)> &&
              detail::valid_signed_bound<decltype(Max)> &&
              detail::signed_raw_value(Max) > detail::signed_raw_value(Min))
constexpr auto bounded_int<Min, Max>::operator-=(bounded_int<Min, Max> rhs) -> bounded_int&
{
    *this = *this - rhs;
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_signed_bound<decltype(Min)> &&
              detail::valid_signed_bound<decltype(Max)> &&
              detail::signed_raw_value(Max) > detail::signed_raw_value(Min))
constexpr auto bounded_int<Min, Max>::operator*=(bounded_int<Min, Max> rhs) -> bounded_int&
{
    *this = *this * rhs;
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_signed_bound<decltype(Min)> &&
              detail::valid_signed_bound<decltype(Max)> &&
              detail::signed_raw_value(Max) > detail::signed_raw_value(Min))
constexpr auto bounded_int<Min, Max>::operator/=(bounded_int<Min, Max> rhs) -> bounded_int&
{
    *this = *this / rhs;
    return *this;
}

// ------------------------------
// Increment / Decrement
// ------------------------------

template <auto Min, auto Max>
    requires (detail::valid_signed_bound<decltype(Min)> &&
              detail::valid_signed_bound<decltype(Max)> &&
              detail::signed_raw_value(Max) > detail::signed_raw_value(Min))
constexpr auto bounded_int<Min, Max>::operator++() -> bounded_int&
{
    using underlying = detail::underlying_type_t<basis_type>;
    constexpr auto max_raw {static_cast<underlying>(detail::signed_raw_value(Max))};
    const auto raw {static_cast<underlying>(static_cast<basis_type>(*this))};

    underlying res {};
    const auto status {detail::impl::signed_no_intrin_add(raw, static_cast<underlying>(1), res)};
    if (status != detail::impl::signed_overflow_status::no_error)
    {
        if (std::is_constant_evaluated())
        {
            throw std::overflow_error("bounded_int increment overflow");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "bounded_int increment overflow");
        }
    }

    if (res > max_raw)
    {
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_int increment result out of range");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_int increment result out of range");
        }
    }

    *this = bounded_int{basis_type{res}};
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_signed_bound<decltype(Min)> &&
              detail::valid_signed_bound<decltype(Max)> &&
              detail::signed_raw_value(Max) > detail::signed_raw_value(Min))
constexpr auto bounded_int<Min, Max>::operator++(int) -> bounded_int
{
    auto tmp {*this};
    ++(*this);
    return tmp;
}

template <auto Min, auto Max>
    requires (detail::valid_signed_bound<decltype(Min)> &&
              detail::valid_signed_bound<decltype(Max)> &&
              detail::signed_raw_value(Max) > detail::signed_raw_value(Min))
constexpr auto bounded_int<Min, Max>::operator--() -> bounded_int&
{
    using underlying = detail::underlying_type_t<basis_type>;
    constexpr auto min_raw {static_cast<underlying>(detail::signed_raw_value(Min))};
    const auto raw {static_cast<underlying>(static_cast<basis_type>(*this))};

    underlying res {};
    const auto status {detail::impl::signed_no_intrin_sub(raw, static_cast<underlying>(1), res)};
    if (status != detail::impl::signed_overflow_status::no_error)
    {
        if (std::is_constant_evaluated())
        {
            throw std::underflow_error("bounded_int decrement underflow");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, "bounded_int decrement underflow");
        }
    }

    if (res < min_raw)
    {
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_int decrement result out of range");
        }
        else
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_int decrement result out of range");
        }
    }

    *this = bounded_int{basis_type{res}};
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_signed_bound<decltype(Min)> &&
              detail::valid_signed_bound<decltype(Max)> &&
              detail::signed_raw_value(Max) > detail::signed_raw_value(Min))
constexpr auto bounded_int<Min, Max>::operator--(int) -> bounded_int
{
    auto tmp {*this};
    --(*this);
    return tmp;
}

} // namespace boost::safe_numbers

// Mixed-bounds blocking for bounded_int

#define BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_INT_OP(OP_NAME, OP_SYMBOL)                                                              \
template <auto LHSMin, auto LHSMax, auto RHSMin, auto RHSMax>                                                                          \
    requires (LHSMin != RHSMin || LHSMax != RHSMax)                                                                                     \
constexpr auto OP_SYMBOL(const boost::safe_numbers::bounded_int<LHSMin, LHSMax>,                                                       \
                         const boost::safe_numbers::bounded_int<RHSMin, RHSMax>)                                                        \
{                                                                                                                                       \
    static_assert(boost::safe_numbers::detail::dependent_false<                                                                         \
                      boost::safe_numbers::bounded_int<LHSMin, LHSMax>,                                                                 \
                      boost::safe_numbers::bounded_int<RHSMin, RHSMax>>,                                                                \
                  "Can not perform " OP_NAME " between bounded_int types with different bounds. "                                        \
                  "Both operands must have the same Min and Max.");                                                                      \
                                                                                                                                        \
    return boost::safe_numbers::bounded_int<LHSMin, LHSMax>(                                                                            \
        typename boost::safe_numbers::bounded_int<LHSMin, LHSMax>::basis_type{0});                                                      \
}

namespace boost::safe_numbers {

BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_INT_OP("comparison", operator<=>)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_INT_OP("equality", operator==)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_INT_OP("addition", operator+)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_INT_OP("subtraction", operator-)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_INT_OP("multiplication", operator*)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_INT_OP("division", operator/)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_INT_OP("modulo", operator%)

} // namespace boost::safe_numbers

#undef BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_INT_OP

// Bitwise operations blocking for bounded_int

#define BOOST_SAFE_NUMBERS_DEFINE_BOUNDED_INT_BITWISE_OP(OP_NAME, OP_SYMBOL)                                                   \
template <auto Min, auto Max>                                                                                                  \
constexpr auto OP_SYMBOL(const boost::safe_numbers::bounded_int<Min, Max>,                                                     \
                         const boost::safe_numbers::bounded_int<Min, Max>)                                                     \
{                                                                                                                              \
    static_assert(boost::safe_numbers::detail::dependent_false<boost::safe_numbers::bounded_int<Min, Max>>,                    \
                  "Can not perform " OP_NAME " between bounded_int types as the results would be non-sensical");               \
                                                                                                                               \
    return boost::safe_numbers::bounded_int<Min, Max>(                                                                         \
        typename boost::safe_numbers::bounded_int<Min, Max>::basis_type{0});                                                   \
}

namespace boost::safe_numbers {

BOOST_SAFE_NUMBERS_DEFINE_BOUNDED_INT_BITWISE_OP("and", operator&)
BOOST_SAFE_NUMBERS_DEFINE_BOUNDED_INT_BITWISE_OP("or", operator|)
BOOST_SAFE_NUMBERS_DEFINE_BOUNDED_INT_BITWISE_OP("xor", operator^)
BOOST_SAFE_NUMBERS_DEFINE_BOUNDED_INT_BITWISE_OP("right shift", operator>>)
BOOST_SAFE_NUMBERS_DEFINE_BOUNDED_INT_BITWISE_OP("left shift", operator<<)

} // namespace boost::safe_numbers

#undef BOOST_SAFE_NUMBERS_DEFINE_BOUNDED_INT_BITWISE_OP

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

BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("comparison", operator<=>)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("equality", operator==)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("addition", operator+)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("subtraction", operator-)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("multiplication", operator*)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("division", operator/)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("modulo", operator%)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("and", operator&)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("or", operator|)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("xor", operator^)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("right shift", operator>>)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("left shift", operator<<)

} // namespace boost::safe_numbers

#undef BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP

#define BOOST_SAFE_NUMBERS_DEFINE_BITWISE_OP(OP_NAME, OP_SYMBOL)                                                    \
template <auto Min, auto Max>                                                                                       \
constexpr auto OP_SYMBOL(const boost::safe_numbers::bounded_uint<Min, Max>,                                         \
                         const boost::safe_numbers::bounded_uint<Min, Max>)                                         \
{                                                                                                                   \
    static_assert(boost::safe_numbers::detail::dependent_false<boost::safe_numbers::bounded_uint<Min, Max>>,        \
                  "Can not perform " OP_NAME "between bounded_uint types as the results would be non-sensical");    \
                                                                                                                    \
    return boost::safe_numbers::bounded_uint<Min, Max>(                                                             \
        typename boost::safe_numbers::bounded_uint<Min, Max>::basis_type{0});                                       \
}                                                                                                                   \

namespace boost::safe_numbers {

BOOST_SAFE_NUMBERS_DEFINE_BITWISE_OP("and", operator&)
BOOST_SAFE_NUMBERS_DEFINE_BITWISE_OP("or", operator|)
BOOST_SAFE_NUMBERS_DEFINE_BITWISE_OP("xor", operator^)
BOOST_SAFE_NUMBERS_DEFINE_BITWISE_OP("right shift", operator>>)
BOOST_SAFE_NUMBERS_DEFINE_BITWISE_OP("left shift", operator<<)

}

#undef BOOST_SAFE_NUMBERS_DEFINE_BITWISE_OP

#endif // BOOST_SAFE_NUMBERS_BOUNDED_INTEGERS_HPP

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_BOUNDED_INTEGERS_HPP
#define BOOST_SAFE_NUMBERS_BOUNDED_INTEGERS_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>
#include <boost/safe_numbers/detail/int128/string.hpp>
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
#include <string>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers {

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
        constexpr auto min_val {basis_type{static_cast<underlying_type>(detail::raw_value(Min))}};
        constexpr auto max_val {basis_type{static_cast<underlying_type>(detail::raw_value(Max))}};

        if (val < min_val || val > max_val)
        {
            if (std::is_constant_evaluated())
            {
                throw std::domain_error("bounded_uint value out of range");
            }
            else
            {
                BOOST_THROW_EXCEPTION(std::domain_error(
                    std::string("bounded_uint<") + std::to_string(detail::raw_value(Min)) + ", " +
                    std::to_string(detail::raw_value(Max)) + "> value out of range"));
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
                    BOOST_THROW_EXCEPTION(std::domain_error(
                        std::string("Overflow in bounded_uint<") + std::to_string(detail::raw_value(Min)) + ", " +
                        std::to_string(detail::raw_value(Max)) + "> conversion to smaller type"));
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
            BOOST_THROW_EXCEPTION(std::overflow_error(
                std::string("Overflow in bounded_uint<") + std::to_string(detail::raw_value(Min)) + ", " +
                std::to_string(detail::raw_value(Max)) + "> addition"));
        }
    }

    constexpr auto min_raw {static_cast<underlying>(detail::raw_value(Min))};
    constexpr auto max_raw {static_cast<underlying>(detail::raw_value(Max))};
    if (res < min_raw || res > max_raw)
    {
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_uint addition result out of range");
        }
        else
        {
            using std::to_string;

            BOOST_THROW_EXCEPTION(std::domain_error(
                std::string("bounded_uint<") + to_string(detail::raw_value(Min)) + ", " +
                to_string(detail::raw_value(Max)) + "> addition result out of range"));
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
            BOOST_THROW_EXCEPTION(std::underflow_error(
                std::string("Underflow in bounded_uint<") + std::to_string(detail::raw_value(Min)) + ", " +
                std::to_string(detail::raw_value(Max)) + "> subtraction"));
        }
    }

    constexpr auto min_raw {static_cast<underlying>(detail::raw_value(Min))};
    constexpr auto max_raw {static_cast<underlying>(detail::raw_value(Max))};
    if (res < min_raw || res > max_raw)
    {
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_uint subtraction result out of range");
        }
        else
        {
            BOOST_THROW_EXCEPTION(std::domain_error(
                std::string("bounded_uint<") + std::to_string(detail::raw_value(Min)) + ", " +
                std::to_string(detail::raw_value(Max)) + "> subtraction result out of range"));
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
            BOOST_THROW_EXCEPTION(std::overflow_error(
                std::string("Overflow in bounded_uint<") + std::to_string(detail::raw_value(Min)) + ", " +
                std::to_string(detail::raw_value(Max)) + "> multiplication"));
        }
    }

    constexpr auto min_raw {static_cast<underlying>(detail::raw_value(Min))};
    constexpr auto max_raw {static_cast<underlying>(detail::raw_value(Max))};
    if (res < min_raw || res > max_raw)
    {
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_uint multiplication result out of range");
        }
        else
        {
            BOOST_THROW_EXCEPTION(std::domain_error(
                std::string("bounded_uint<") + std::to_string(detail::raw_value(Min)) + ", " +
                std::to_string(detail::raw_value(Max)) + "> multiplication result out of range"));
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
            BOOST_THROW_EXCEPTION(std::domain_error(
                std::string("Division by zero in bounded_uint<") + std::to_string(detail::raw_value(Min)) + ", " +
                std::to_string(detail::raw_value(Max)) + "> division"));
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

    constexpr auto min_raw {static_cast<underlying>(detail::raw_value(Min))};
    constexpr auto max_raw {static_cast<underlying>(detail::raw_value(Max))};
    if (res < min_raw || res > max_raw)
    {
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_uint division result out of range");
        }
        else
        {
            BOOST_THROW_EXCEPTION(std::domain_error(
                std::string("bounded_uint<") + std::to_string(detail::raw_value(Min)) + ", " +
                std::to_string(detail::raw_value(Max)) + "> division result out of range"));
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
            BOOST_THROW_EXCEPTION(std::domain_error(
                std::string("Division by zero in bounded_uint<") + std::to_string(detail::raw_value(Min)) + ", " +
                std::to_string(detail::raw_value(Max)) + "> modulo"));
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

    constexpr auto min_raw {static_cast<underlying>(detail::raw_value(Min))};
    constexpr auto max_raw {static_cast<underlying>(detail::raw_value(Max))};
    if (res < min_raw || res > max_raw)
    {
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_uint modulo result out of range");
        }
        else
        {
            BOOST_THROW_EXCEPTION(std::domain_error(
                std::string("bounded_uint<") + std::to_string(detail::raw_value(Min)) + ", " +
                std::to_string(detail::raw_value(Max)) + "> modulo result out of range"));
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
            BOOST_THROW_EXCEPTION(std::overflow_error(
                std::string("Overflow in bounded_uint<") + std::to_string(detail::raw_value(Min)) + ", " +
                std::to_string(detail::raw_value(Max)) + "> increment"));
        }
    }

    constexpr auto max_raw {static_cast<underlying>(detail::raw_value(Max))};
    if (res > max_raw)
    {
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_uint increment result out of range");
        }
        else
        {
            BOOST_THROW_EXCEPTION(std::domain_error(
                std::string("bounded_uint<") + std::to_string(detail::raw_value(Min)) + ", " +
                std::to_string(detail::raw_value(Max)) + "> increment result out of range"));
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
            BOOST_THROW_EXCEPTION(std::underflow_error(
                std::string("Underflow in bounded_uint<") + std::to_string(detail::raw_value(Min)) + ", " +
                std::to_string(detail::raw_value(Max)) + "> decrement"));
        }
    }

    constexpr auto min_raw {static_cast<underlying>(detail::raw_value(Min))};
    if (res < min_raw)
    {
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_uint decrement result out of range");
        }
        else
        {
            BOOST_THROW_EXCEPTION(std::domain_error(
                std::string("bounded_uint<") + std::to_string(detail::raw_value(Min)) + ", " +
                std::to_string(detail::raw_value(Max)) + "> decrement result out of range"));
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

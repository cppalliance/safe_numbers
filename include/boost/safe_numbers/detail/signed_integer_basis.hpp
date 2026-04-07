// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_SIGNED_INTEGER_BASIS_HPP
#define BOOST_SAFE_NUMBERS_SIGNED_INTEGER_BASIS_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>
#include <boost/safe_numbers/detail/int128/bit.hpp>
#include <boost/safe_numbers/detail/throw_exception.hpp>
#include <boost/safe_numbers/overflow_policy.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/core/bit.hpp>
#include <concepts>
#include <compare>
#include <limits>
#include <stdexcept>
#include <cstdint>
#include <cstdlib>
#include <utility>
#include <optional>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers::detail {

template <fundamental_signed_integral BasisType>
class signed_integer_basis
{
public:

    // This is exposed to the user so that they can convert back to built-in
    using basis_type = BasisType;

private:

    BasisType basis_ {0U};

public:

    constexpr signed_integer_basis() noexcept = default;

    explicit constexpr signed_integer_basis(const BasisType val) noexcept : basis_{val} {}

    template <typename T>
        requires std::is_same_v<T, bool>
    explicit constexpr signed_integer_basis(T) noexcept
    {
        static_assert(dependent_false<T>, "Construction from bool is not allowed");
    }

    template <fundamental_signed_integral OtherBasis>
    [[nodiscard]] explicit constexpr operator OtherBasis() const;

    [[nodiscard]] explicit constexpr operator BasisType() const noexcept { return basis_; }

    [[nodiscard]] friend constexpr auto operator<=>(signed_integer_basis lhs, signed_integer_basis rhs) noexcept
        -> std::strong_ordering = default;

    [[nodiscard]] constexpr auto operator+() const noexcept -> signed_integer_basis;

    [[nodiscard]] constexpr auto operator-() const -> signed_integer_basis;

    template <fundamental_signed_integral OtherBasis>
    constexpr auto operator+=(signed_integer_basis<OtherBasis> rhs) -> signed_integer_basis&;

    template <fundamental_signed_integral OtherBasis>
    constexpr auto operator-=(signed_integer_basis<OtherBasis> rhs) -> signed_integer_basis&;

    template <fundamental_signed_integral OtherBasis>
    constexpr auto operator*=(signed_integer_basis<OtherBasis> rhs) -> signed_integer_basis&;

    template <fundamental_signed_integral OtherBasis>
    constexpr auto operator/=(signed_integer_basis<OtherBasis> rhs) -> signed_integer_basis&;

    template <fundamental_signed_integral OtherBasis>
    constexpr auto operator%=(signed_integer_basis<OtherBasis> rhs) -> signed_integer_basis&;

    constexpr auto operator++() -> signed_integer_basis&;

    constexpr auto operator++(int) -> signed_integer_basis;

    constexpr auto operator--() -> signed_integer_basis&;

    constexpr auto operator--(int) -> signed_integer_basis;
};

// Helper for diagnostic messages
template <typename BasisType>
constexpr auto signed_type_name() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::int8_t>)
    {
        return "i8";
    }
    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
    {
        return "i16";
    }
    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
    {
        return "i32";
    }
    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
    {
        return "i64";
    }
    else
    {
        return "i128";
    }
}

// Device-friendly error message helpers returning const char* string literals

template <fundamental_signed_integral BasisType>
constexpr auto signed_overflow_add_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::int8_t>)
    {
        return "Overflow detected in i8 addition";
    }
    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
    {
        return "Overflow detected in i16 addition";
    }
    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
    {
        return "Overflow detected in i32 addition";
    }
    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
    {
        return "Overflow detected in i64 addition";
    }
    else
    {
        return "Overflow detected in i128 addition";
    }
}

template <fundamental_signed_integral BasisType>
constexpr auto signed_underflow_add_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::int8_t>)
    {
        return "Underflow detected in i8 addition";
    }
    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
    {
        return "Underflow detected in i16 addition";
    }
    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
    {
        return "Underflow detected in i32 addition";
    }
    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
    {
        return "Underflow detected in i64 addition";
    }
    else
    {
        return "Underflow detected in i128 addition";
    }
}

template <fundamental_signed_integral BasisType>
constexpr auto signed_unary_minus_overflow_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::int8_t>)
    {
        return "Overflow in i8 unary minus operator";
    }
    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
    {
        return "Overflow in i16 unary minus operator";
    }
    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
    {
        return "Overflow in i32 unary minus operator";
    }
    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
    {
        return "Overflow in i64 unary minus operator";
    }
    else
    {
        return "Overflow in i128 unary minus operator";
    }
}

template <fundamental_signed_integral BasisType, fundamental_signed_integral OtherBasis>
constexpr auto signed_overflow_conversion_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::int16_t> && std::is_same_v<OtherBasis, std::int8_t>)
    {
        return "Overflow in i16 to i8 conversion";
    }
    else if constexpr (std::is_same_v<BasisType, std::int32_t> && std::is_same_v<OtherBasis, std::int8_t>)
    {
        return "Overflow in i32 to i8 conversion";
    }
    else if constexpr (std::is_same_v<BasisType, std::int32_t> && std::is_same_v<OtherBasis, std::int16_t>)
    {
        return "Overflow in i32 to i16 conversion";
    }
    else if constexpr (std::is_same_v<BasisType, std::int64_t> && std::is_same_v<OtherBasis, std::int8_t>)
    {
        return "Overflow in i64 to i8 conversion";
    }
    else if constexpr (std::is_same_v<BasisType, std::int64_t> && std::is_same_v<OtherBasis, std::int16_t>)
    {
        return "Overflow in i64 to i16 conversion";
    }
    else if constexpr (std::is_same_v<BasisType, std::int64_t> && std::is_same_v<OtherBasis, std::int32_t>)
    {
        return "Overflow in i64 to i32 conversion";
    }
    else if constexpr (std::is_same_v<BasisType, int128::int128_t> && std::is_same_v<OtherBasis, std::int8_t>)
    {
        return "Overflow in i128 to i8 conversion";
    }
    else if constexpr (std::is_same_v<BasisType, int128::int128_t> && std::is_same_v<OtherBasis, std::int16_t>)
    {
        return "Overflow in i128 to i16 conversion";
    }
    else if constexpr (std::is_same_v<BasisType, int128::int128_t> && std::is_same_v<OtherBasis, std::int32_t>)
    {
        return "Overflow in i128 to i32 conversion";
    }
    else if constexpr (std::is_same_v<BasisType, int128::int128_t> && std::is_same_v<OtherBasis, std::int64_t>)
    {
        return "Overflow in i128 to i64 conversion";
    }
    else
    {
        return "Overflow in signed integer conversion";
    }
}

template <fundamental_signed_integral BasisType, fundamental_signed_integral OtherBasis>
constexpr auto signed_underflow_conversion_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::int16_t> && std::is_same_v<OtherBasis, std::int8_t>)
    {
        return "Underflow in i16 to i8 conversion";
    }
    else if constexpr (std::is_same_v<BasisType, std::int32_t> && std::is_same_v<OtherBasis, std::int8_t>)
    {
        return "Underflow in i32 to i8 conversion";
    }
    else if constexpr (std::is_same_v<BasisType, std::int32_t> && std::is_same_v<OtherBasis, std::int16_t>)
    {
        return "Underflow in i32 to i16 conversion";
    }
    else if constexpr (std::is_same_v<BasisType, std::int64_t> && std::is_same_v<OtherBasis, std::int8_t>)
    {
        return "Underflow in i64 to i8 conversion";
    }
    else if constexpr (std::is_same_v<BasisType, std::int64_t> && std::is_same_v<OtherBasis, std::int16_t>)
    {
        return "Underflow in i64 to i16 conversion";
    }
    else if constexpr (std::is_same_v<BasisType, std::int64_t> && std::is_same_v<OtherBasis, std::int32_t>)
    {
        return "Underflow in i64 to i32 conversion";
    }
    else if constexpr (std::is_same_v<BasisType, int128::int128_t> && std::is_same_v<OtherBasis, std::int8_t>)
    {
        return "Underflow in i128 to i8 conversion";
    }
    else if constexpr (std::is_same_v<BasisType, int128::int128_t> && std::is_same_v<OtherBasis, std::int16_t>)
    {
        return "Underflow in i128 to i16 conversion";
    }
    else if constexpr (std::is_same_v<BasisType, int128::int128_t> && std::is_same_v<OtherBasis, std::int32_t>)
    {
        return "Underflow in i128 to i32 conversion";
    }
    else if constexpr (std::is_same_v<BasisType, int128::int128_t> && std::is_same_v<OtherBasis, std::int64_t>)
    {
        return "Underflow in i128 to i64 conversion";
    }
    else
    {
        return "Underflow in signed integer conversion";
    }
}

template <fundamental_signed_integral BasisType>
template <fundamental_signed_integral OtherBasis>
constexpr signed_integer_basis<BasisType>::operator OtherBasis() const
{
    if constexpr (sizeof(OtherBasis) < sizeof(BasisType))
    {
        if (basis_ > static_cast<BasisType>(std::numeric_limits<OtherBasis>::max()))
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, (signed_overflow_conversion_msg<BasisType, OtherBasis>()));
        }
        else if (basis_ < static_cast<BasisType>(std::numeric_limits<OtherBasis>::min()))
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, (signed_underflow_conversion_msg<BasisType, OtherBasis>()));
        }
    }

    return static_cast<OtherBasis>(basis_);
}

template <fundamental_signed_integral BasisType>
constexpr auto signed_integer_basis<BasisType>::operator+() const noexcept -> signed_integer_basis
{
    return signed_integer_basis{basis_};
}

template <fundamental_signed_integral BasisType>
constexpr auto signed_integer_basis<BasisType>::operator-() const -> signed_integer_basis
{
    if (basis_ == std::numeric_limits<BasisType>::min()) [[unlikely]]
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, signed_unary_minus_overflow_msg<BasisType>());
    }

    return signed_integer_basis{static_cast<BasisType>(-basis_)};
}

// ------------------------------
// Addition
// ------------------------------

namespace impl {

enum class signed_overflow_status
{
    no_error,
    overflow,
    underflow
};

template <fundamental_signed_integral T>
struct make_unsigned_helper
{
    using type = std::make_unsigned_t<T>;
};

template <>
struct make_unsigned_helper<int128::int128_t>
{
    using type = int128::uint128_t;
};

template <fundamental_signed_integral T>
using make_unsigned_helper_t = typename make_unsigned_helper<T>::type;

// Determines the direction of signed overflow from the lhs operand.
// Signed addition overflow only occurs when both operands share the same sign,
// so the sign of lhs is sufficient to determine direction.
template <fundamental_signed_integral T>
constexpr auto classify_signed_overflow(const T lhs) noexcept -> signed_overflow_status
{
    return lhs >= 0 ? signed_overflow_status::overflow : signed_overflow_status::underflow;
}

#if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_add_overflow)

template <std::signed_integral T>
auto signed_intrin_add(const T lhs, const T rhs, T& result) -> signed_overflow_status
{
    if (__builtin_add_overflow(lhs, rhs, &result))
    {
        return classify_signed_overflow(lhs);
    }

    return signed_overflow_status::no_error;
}

#elif defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

template <std::signed_integral T>
auto signed_intrin_add(const T lhs, const T rhs, T& result) -> signed_overflow_status
{
    using unsigned_t = std::make_unsigned_t<T>;

    unsigned_t temp {};

    if constexpr (std::is_same_v<T, std::int8_t>)
    {
        _addcarry_u8(0, static_cast<std::uint8_t>(lhs), static_cast<std::uint8_t>(rhs), &temp);
    }
    else if constexpr (std::is_same_v<T, std::int16_t>)
    {
        _addcarry_u16(0, static_cast<std::uint16_t>(lhs), static_cast<std::uint16_t>(rhs), &temp);
    }
    else if constexpr (std::is_same_v<T, std::int32_t>)
    {
        _addcarry_u32(0, static_cast<std::uint32_t>(lhs), static_cast<std::uint32_t>(rhs), &temp);
    }
    #if defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)
    else if constexpr (std::is_same_v<T, std::int64_t>)
    {
        // x86 does not provide the _addcarry_u64 intrinsic
        temp = static_cast<std::uint64_t>(lhs) + static_cast<std::uint64_t>(rhs);
    }
    #else
    else if constexpr (std::is_same_v<T, std::int64_t>)
    {
        _addcarry_u64(0, static_cast<std::uint64_t>(lhs), static_cast<std::uint64_t>(rhs), &temp);
    }
    #endif

    result = static_cast<T>(temp);

    // _addcarry detects unsigned carry, not signed overflow
    // Signed overflow: both operands have the same sign but the result has a different sign
    const auto ulhs {static_cast<unsigned_t>(lhs)};
    const auto urhs {static_cast<unsigned_t>(rhs)};
    const auto has_overflow {static_cast<bool>(((~(ulhs ^ urhs)) & (ulhs ^ temp)) >> std::numeric_limits<T>::digits)};

    if (has_overflow)
    {
        return classify_signed_overflow(lhs);
    }

    return signed_overflow_status::no_error;
}

#endif

template <fundamental_signed_integral T>
constexpr auto signed_no_intrin_add(const T lhs, const T rhs, T& result) noexcept -> signed_overflow_status
{
    using unsigned_t = make_unsigned_helper_t<T>;
    unsigned_t temp {};

    if constexpr (std::is_same_v<T, std::int8_t> || std::is_same_v<T, std::int16_t>)
    {
        temp = static_cast<unsigned_t>(static_cast<std::uint32_t>(lhs) + static_cast<std::uint32_t>(rhs));
    }
    else
    {
        temp = static_cast<unsigned_t>(lhs) + static_cast<unsigned_t>(rhs);
    }

    result = static_cast<T>(temp);

    // Signed overflow: both operands have the same sign but the result has a different sign
    const auto ulhs {static_cast<unsigned_t>(lhs)};
    const auto urhs {static_cast<unsigned_t>(rhs)};
    const auto has_overflow {static_cast<bool>(((~(ulhs ^ urhs)) & (ulhs ^ temp)) >> std::numeric_limits<T>::digits)};

    if (has_overflow)
    {
        return classify_signed_overflow(lhs);
    }

    return signed_overflow_status::no_error;
}

template <overflow_policy Policy, fundamental_signed_integral BasisType>
struct signed_add_helper
{
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType> lhs,
                                              const signed_integer_basis<BasisType> rhs)
        noexcept(Policy != overflow_policy::throw_exception)
        -> signed_integer_basis<BasisType>
    {
        using result_type = signed_integer_basis<BasisType>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType result {};

        auto handle_error = [&result](signed_overflow_status status)
        {
            if (std::is_constant_evaluated())
            {
                if (status == signed_overflow_status::overflow)
                {
                    if constexpr (std::is_same_v<BasisType, std::int8_t>)
                    {
                        throw std::overflow_error("Overflow detected in i8 addition");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
                    {
                        throw std::overflow_error("Overflow detected in i16 addition");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
                    {
                        throw std::overflow_error("Overflow detected in i32 addition");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
                    {
                        throw std::overflow_error("Overflow detected in i64 addition");
                    }
                    else
                    {
                        throw std::overflow_error("Overflow detected in i128 addition");
                    }
                }
                else
                {
                    if constexpr (std::is_same_v<BasisType, std::int8_t>)
                    {
                        throw std::underflow_error("Underflow detected in i8 addition");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
                    {
                        throw std::underflow_error("Underflow detected in i16 addition");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
                    {
                        throw std::underflow_error("Underflow detected in i32 addition");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
                    {
                        throw std::underflow_error("Underflow detected in i64 addition");
                    }
                    else
                    {
                        throw std::underflow_error("Underflow detected in i128 addition");
                    }
                }
            }
            else
            {
                if constexpr (Policy == overflow_policy::throw_exception)
                {
                    static_cast<void>(result);

                    if (status == signed_overflow_status::overflow)
                    {
                        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, signed_overflow_add_msg<BasisType>());
                    }
                    else
                    {
                        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, signed_underflow_add_msg<BasisType>());
                    }
                }
                else if constexpr (Policy == overflow_policy::saturate)
                {
                    if (status == signed_overflow_status::overflow)
                    {
                        result = std::numeric_limits<BasisType>::max();
                    }
                    else
                    {
                        result = std::numeric_limits<BasisType>::min();
                    }
                }
                else if constexpr (Policy == overflow_policy::strict)
                {
                    static_cast<void>(result);
                    std::exit(EXIT_FAILURE);
                }
                else
                {
                    static_cast<void>(result);
                    BOOST_SAFE_NUMBERS_UNREACHABLE;
                }
            }
        };

        #if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_add_overflow) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

        if constexpr (!std::is_same_v<BasisType, int128::int128_t>)
        {
            if (!std::is_constant_evaluated())
            {
                const auto status {impl::signed_intrin_add(lhs_basis, rhs_basis, result)};
                if (status != signed_overflow_status::no_error)
                {
                    handle_error(status);
                }

                return result_type{result};
            }
        }

        #endif // BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_add_overflow) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

        const auto status {impl::signed_no_intrin_add(lhs_basis, rhs_basis, result)};
        if (status != signed_overflow_status::no_error)
        {
            handle_error(status);
        }

        return result_type{result};
    }
};

// Partial specialization for overflow_tuple policy
template <fundamental_signed_integral BasisType>
struct signed_add_helper<overflow_policy::overflow_tuple, BasisType>
{
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType> lhs,
                                              const signed_integer_basis<BasisType> rhs) noexcept
        -> std::pair<signed_integer_basis<BasisType>, bool>
    {
        using result_type = signed_integer_basis<BasisType>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType result {};

        #if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_add_overflow) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

        if constexpr (!std::is_same_v<BasisType, int128::int128_t>)
        {
            if (!std::is_constant_evaluated())
            {
                const auto status {impl::signed_intrin_add(lhs_basis, rhs_basis, result)};
                return std::make_pair(result_type{result}, status != signed_overflow_status::no_error);
            }
        }

        #endif

        const auto status {impl::signed_no_intrin_add(lhs_basis, rhs_basis, result)};
        return std::make_pair(result_type{result}, status != signed_overflow_status::no_error);
    }
};

// Partial specialization for checked policy
template <fundamental_signed_integral BasisType>
struct signed_add_helper<overflow_policy::checked, BasisType>
{
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType> lhs,
                                              const signed_integer_basis<BasisType> rhs) noexcept
        -> std::optional<signed_integer_basis<BasisType>>
    {
        using result_type = signed_integer_basis<BasisType>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType result {};

        #if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_add_overflow) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

        if constexpr (!std::is_same_v<BasisType, int128::int128_t>)
        {
            if (!std::is_constant_evaluated())
            {
                const auto status {impl::signed_intrin_add(lhs_basis, rhs_basis, result)};
                return status != signed_overflow_status::no_error
                    ? std::nullopt
                    : std::make_optional(result_type{result});
            }
        }

        #endif

        const auto status {impl::signed_no_intrin_add(lhs_basis, rhs_basis, result)};
        return status != signed_overflow_status::no_error
            ? std::nullopt
            : std::make_optional(result_type{result});
    }
};

// Partial specialization for widening policy
template <fundamental_signed_integral BasisType>
struct signed_add_helper<overflow_policy::widen, BasisType>
{
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType> lhs,
                                              const signed_integer_basis<BasisType> rhs) noexcept
    {
        using promoted = signed_promoted_type<BasisType>;
        static_assert(!std::is_same_v<promoted, bool>, "Widening policy with int128_t is not supported");

        using result_type = signed_integer_basis<promoted>;
        return result_type{static_cast<promoted>(static_cast<promoted>(static_cast<BasisType>(lhs)) + static_cast<BasisType>(rhs))};
    }
};

template <overflow_policy Policy, fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto add_impl(const signed_integer_basis<BasisType> lhs,
                                      const signed_integer_basis<BasisType> rhs)
    noexcept(Policy == overflow_policy::saturate || Policy == overflow_policy::overflow_tuple ||
             Policy == overflow_policy::checked || Policy == overflow_policy::strict ||
             Policy == overflow_policy::widen)
{
    return signed_add_helper<Policy, BasisType>::apply(lhs,rhs);
}

} // namespace impl

template <fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto operator+(const signed_integer_basis<BasisType> lhs,
                                       const signed_integer_basis<BasisType> rhs) -> signed_integer_basis<BasisType>
{
    if (std::is_constant_evaluated())
    {
        BasisType res {};
        const auto status {impl::signed_no_intrin_add(static_cast<BasisType>(lhs), static_cast<BasisType>(rhs), res)};
        if (status == impl::signed_overflow_status::overflow)
        {
            if constexpr (std::is_same_v<BasisType, std::int8_t>)
            {
                throw std::overflow_error("Overflow detected in i8 addition");
            }
            else if constexpr (std::is_same_v<BasisType, std::int16_t>)
            {
                throw std::overflow_error("Overflow detected in i16 addition");
            }
            else if constexpr (std::is_same_v<BasisType, std::int32_t>)
            {
                throw std::overflow_error("Overflow detected in i32 addition");
            }
            else if constexpr (std::is_same_v<BasisType, std::int64_t>)
            {
                throw std::overflow_error("Overflow detected in i64 addition");
            }
            else
            {
                throw std::overflow_error("Overflow detected in i128 addition");
            }
        }
        else if (status == impl::signed_overflow_status::underflow)
        {
            if constexpr (std::is_same_v<BasisType, std::int8_t>)
            {
                throw std::underflow_error("Underflow detected in i8 addition");
            }
            else if constexpr (std::is_same_v<BasisType, std::int16_t>)
            {
                throw std::underflow_error("Underflow detected in i16 addition");
            }
            else if constexpr (std::is_same_v<BasisType, std::int32_t>)
            {
                throw std::underflow_error("Underflow detected in i32 addition");
            }
            else if constexpr (std::is_same_v<BasisType, std::int64_t>)
            {
                throw std::underflow_error("Underflow detected in i64 addition");
            }
            else
            {
                throw std::underflow_error("Underflow detected in i128 addition");
            }
        }

        return signed_integer_basis<BasisType>{res};
    }

    return impl::signed_add_helper<overflow_policy::throw_exception, BasisType>::apply(lhs, rhs);
}

} // namespace boost::safe_numbers::detail

#define BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP(OP_NAME, OP_SYMBOL)                                                                                \
template <boost::safe_numbers::detail::fundamental_signed_integral LHSBasis,                                                                                    \
          boost::safe_numbers::detail::fundamental_signed_integral RHSBasis>                                                                                    \
    requires (!std::is_same_v<LHSBasis, RHSBasis>)                                                                                                             \
constexpr auto OP_SYMBOL(const boost::safe_numbers::detail::signed_integer_basis<LHSBasis>,                                                                    \
                         const boost::safe_numbers::detail::signed_integer_basis<RHSBasis>)                                                                    \
{                                                                                                                                                              \
    if constexpr (std::is_same_v<LHSBasis, std::int8_t>)                                                                                                      \
    {                                                                                                                                                          \
        if constexpr (std::is_same_v<RHSBasis, std::int16_t>)                                                                                                 \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between i8 and i16");                 \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, std::int32_t>)                                                                                            \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between i8 and i32");                 \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, std::int64_t>)                                                                                            \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between i8 and i64");                 \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, boost::int128::int128_t>)                                                                                 \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between i8 and i128");                \
        }                                                                                                                                                      \
        else                                                                                                                                                   \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between i8 and unknown type");        \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
    else if constexpr (std::is_same_v<LHSBasis, std::int16_t>)                                                                                                \
    {                                                                                                                                                          \
        if constexpr (std::is_same_v<RHSBasis, std::int8_t>)                                                                                                  \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between i16 and i8");                 \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, std::int32_t>)                                                                                            \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between i16 and i32");                \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, std::int64_t>)                                                                                            \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between i16 and i64");                \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, boost::int128::int128_t>)                                                                                 \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between i16 and i128");               \
        }                                                                                                                                                      \
        else                                                                                                                                                   \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between i16 and unknown type");       \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
    else if constexpr (std::is_same_v<LHSBasis, std::int32_t>)                                                                                                \
    {                                                                                                                                                          \
        if constexpr (std::is_same_v<RHSBasis, std::int8_t>)                                                                                                  \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between i32 and i8");                 \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, std::int16_t>)                                                                                            \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between i32 and i16");                \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, std::int64_t>)                                                                                            \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between i32 and i64");                \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, boost::int128::int128_t>)                                                                                 \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between i32 and i128");               \
        }                                                                                                                                                      \
        else                                                                                                                                                   \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between i32 and unknown type");       \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
    else if constexpr (std::is_same_v<LHSBasis, std::int64_t>)                                                                                                \
    {                                                                                                                                                          \
        if constexpr (std::is_same_v<RHSBasis, std::int8_t>)                                                                                                  \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between i64 and i8");                 \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, std::int16_t>)                                                                                            \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between i64 and i16");                \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, std::int32_t>)                                                                                            \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between i64 and i32");                \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, boost::int128::int128_t>)                                                                                 \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between i64 and i128");               \
        }                                                                                                                                                      \
        else                                                                                                                                                   \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between i64 and unknown type");       \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
    else if constexpr (std::is_same_v<LHSBasis, boost::int128::int128_t>)                                                                                     \
    {                                                                                                                                                          \
        if constexpr (std::is_same_v<RHSBasis, std::int8_t>)                                                                                                  \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between i128 and i8");                \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, std::int16_t>)                                                                                            \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between i128 and i16");               \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, std::int32_t>)                                                                                            \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between i128 and i32");               \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, std::int64_t>)                                                                                            \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between i128 and i64");               \
        }                                                                                                                                                      \
        else                                                                                                                                                   \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between i128 and unknown type");      \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
    else                                                                                                                                                       \
    {                                                                                                                                                          \
        static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " on mixed width signed integer types");    \
    }                                                                                                                                                          \
                                                                                                                                                               \
    return boost::safe_numbers::detail::signed_integer_basis<LHSBasis>(0);                                                                                    \
}


namespace boost::safe_numbers::detail {

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("comparison", operator<=>)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("equality", operator==)

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("addition", operator+)

template <fundamental_signed_integral BasisType>
template <fundamental_signed_integral OtherBasisType>
constexpr auto signed_integer_basis<BasisType>::operator+=(const signed_integer_basis<OtherBasisType> rhs)
    -> signed_integer_basis&
{
    *this = *this + rhs;
    return *this;
}

// ------------------------------
// Subtraction
// ------------------------------

namespace impl {

template <fundamental_signed_integral BasisType>
constexpr auto signed_overflow_sub_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::int8_t>)
    {
        return "Overflow detected in i8 subtraction";
    }
    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
    {
        return "Overflow detected in i16 subtraction";
    }
    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
    {
        return "Overflow detected in i32 subtraction";
    }
    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
    {
        return "Overflow detected in i64 subtraction";
    }
    else
    {
        return "Overflow detected in i128 subtraction";
    }
}

template <fundamental_signed_integral BasisType>
constexpr auto signed_underflow_sub_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::int8_t>)
    {
        return "Underflow detected in i8 subtraction";
    }
    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
    {
        return "Underflow detected in i16 subtraction";
    }
    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
    {
        return "Underflow detected in i32 subtraction";
    }
    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
    {
        return "Underflow detected in i64 subtraction";
    }
    else
    {
        return "Underflow detected in i128 subtraction";
    }
}

#if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_sub_overflow)

template <std::signed_integral T>
auto signed_intrin_sub(const T lhs, const T rhs, T& result) -> signed_overflow_status
{
    if (__builtin_sub_overflow(lhs, rhs, &result))
    {
        return classify_signed_overflow(lhs);
    }

    return signed_overflow_status::no_error;
}

#elif defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

template <std::signed_integral T>
auto signed_intrin_sub(const T lhs, const T rhs, T& result) -> signed_overflow_status
{
    using unsigned_t = std::make_unsigned_t<T>;

    unsigned_t temp {};

    if constexpr (std::is_same_v<T, std::int8_t>)
    {
        _subborrow_u8(0, static_cast<std::uint8_t>(lhs), static_cast<std::uint8_t>(rhs), &temp);
    }
    else if constexpr (std::is_same_v<T, std::int16_t>)
    {
        _subborrow_u16(0, static_cast<std::uint16_t>(lhs), static_cast<std::uint16_t>(rhs), &temp);
    }
    else if constexpr (std::is_same_v<T, std::int32_t>)
    {
        _subborrow_u32(0, static_cast<std::uint32_t>(lhs), static_cast<std::uint32_t>(rhs), &temp);
    }
    #if defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)
    else if constexpr (std::is_same_v<T, std::int64_t>)
    {
        // x86 does not provide the _subborrow_u64 intrinsic
        temp = static_cast<std::uint64_t>(lhs) - static_cast<std::uint64_t>(rhs);
    }
    #else
    else if constexpr (std::is_same_v<T, std::int64_t>)
    {
        _subborrow_u64(0, static_cast<std::uint64_t>(lhs), static_cast<std::uint64_t>(rhs), &temp);
    }
    #endif

    result = static_cast<T>(temp);

    // Signed subtraction overflow: operands have different signs and result sign differs from lhs
    const auto ulhs {static_cast<unsigned_t>(lhs)};
    const auto urhs {static_cast<unsigned_t>(rhs)};
    const auto has_overflow {static_cast<bool>(((ulhs ^ urhs) & (ulhs ^ temp)) >> std::numeric_limits<T>::digits)};

    if (has_overflow)
    {
        return classify_signed_overflow(lhs);
    }

    return signed_overflow_status::no_error;
}

#endif

template <fundamental_signed_integral T>
constexpr auto signed_no_intrin_sub(const T lhs, const T rhs, T& result) noexcept -> signed_overflow_status
{
    using unsigned_t = make_unsigned_helper_t<T>;
    unsigned_t temp {};

    if constexpr (std::is_same_v<T, std::int8_t> || std::is_same_v<T, std::int16_t>)
    {
        temp = static_cast<unsigned_t>(static_cast<std::uint32_t>(lhs) - static_cast<std::uint32_t>(rhs));
    }
    else
    {
        temp = static_cast<unsigned_t>(lhs) - static_cast<unsigned_t>(rhs);
    }

    result = static_cast<T>(temp);

    // Signed subtraction overflow: operands have different signs and result sign differs from lhs
    const auto ulhs {static_cast<unsigned_t>(lhs)};
    const auto urhs {static_cast<unsigned_t>(rhs)};
    const auto has_overflow {static_cast<bool>(((ulhs ^ urhs) & (ulhs ^ temp)) >> std::numeric_limits<T>::digits)};

    if (has_overflow)
    {
        return classify_signed_overflow(lhs);
    }

    return signed_overflow_status::no_error;
}

template <overflow_policy Policy, fundamental_signed_integral BasisType>
struct signed_sub_helper
{
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType> lhs,
                                              const signed_integer_basis<BasisType> rhs)
        noexcept(Policy != overflow_policy::throw_exception)
        -> signed_integer_basis<BasisType>
    {
        using result_type = signed_integer_basis<BasisType>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType result {};

        auto handle_error = [&result](signed_overflow_status status)
        {
            if (std::is_constant_evaluated())
            {
                if (status == signed_overflow_status::overflow)
                {
                    if constexpr (std::is_same_v<BasisType, std::int8_t>)
                    {
                        throw std::overflow_error("Overflow detected in i8 subtraction");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
                    {
                        throw std::overflow_error("Overflow detected in i16 subtraction");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
                    {
                        throw std::overflow_error("Overflow detected in i32 subtraction");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
                    {
                        throw std::overflow_error("Overflow detected in i64 subtraction");
                    }
                    else
                    {
                        throw std::overflow_error("Overflow detected in i128 subtraction");
                    }
                }
                else
                {
                    if constexpr (std::is_same_v<BasisType, std::int8_t>)
                    {
                        throw std::underflow_error("Underflow detected in i8 subtraction");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
                    {
                        throw std::underflow_error("Underflow detected in i16 subtraction");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
                    {
                        throw std::underflow_error("Underflow detected in i32 subtraction");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
                    {
                        throw std::underflow_error("Underflow detected in i64 subtraction");
                    }
                    else
                    {
                        throw std::underflow_error("Underflow detected in i128 subtraction");
                    }
                }
            }
            else
            {
                if constexpr (Policy == overflow_policy::throw_exception)
                {
                    static_cast<void>(result);

                    if (status == signed_overflow_status::overflow)
                    {
                        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, signed_overflow_sub_msg<BasisType>());
                    }
                    else
                    {
                        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, signed_underflow_sub_msg<BasisType>());
                    }
                }
                else if constexpr (Policy == overflow_policy::saturate)
                {
                    if (status == signed_overflow_status::overflow)
                    {
                        result = std::numeric_limits<BasisType>::max();
                    }
                    else
                    {
                        result = std::numeric_limits<BasisType>::min();
                    }
                }
                else if constexpr (Policy == overflow_policy::strict)
                {
                    static_cast<void>(result);
                    std::exit(EXIT_FAILURE);
                }
                else
                {
                    static_cast<void>(result);
                    BOOST_SAFE_NUMBERS_UNREACHABLE;
                }
            }
        };

        #if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_sub_overflow) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

        if constexpr (!std::is_same_v<BasisType, int128::int128_t>)
        {
            if (!std::is_constant_evaluated())
            {
                const auto status {impl::signed_intrin_sub(lhs_basis, rhs_basis, result)};
                if (status != signed_overflow_status::no_error)
                {
                    handle_error(status);
                }

                return result_type{result};
            }
        }

        #endif // BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_sub_overflow) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

        const auto status {impl::signed_no_intrin_sub(lhs_basis, rhs_basis, result)};
        if (status != signed_overflow_status::no_error)
        {
            handle_error(status);
        }

        return result_type{result};
    }
};

// Partial specialization for overflow_tuple policy
template <fundamental_signed_integral BasisType>
struct signed_sub_helper<overflow_policy::overflow_tuple, BasisType>
{
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType> lhs,
                                              const signed_integer_basis<BasisType> rhs) noexcept
        -> std::pair<signed_integer_basis<BasisType>, bool>
    {
        using result_type = signed_integer_basis<BasisType>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType result {};

        #if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_sub_overflow) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

        if constexpr (!std::is_same_v<BasisType, int128::int128_t>)
        {
            if (!std::is_constant_evaluated())
            {
                const auto status {impl::signed_intrin_sub(lhs_basis, rhs_basis, result)};
                return std::make_pair(result_type{result}, status != signed_overflow_status::no_error);
            }
        }

        #endif

        const auto status {impl::signed_no_intrin_sub(lhs_basis, rhs_basis, result)};
        return std::make_pair(result_type{result}, status != signed_overflow_status::no_error);
    }
};

// Partial specialization for checked policy
template <fundamental_signed_integral BasisType>
struct signed_sub_helper<overflow_policy::checked, BasisType>
{
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType> lhs,
                                              const signed_integer_basis<BasisType> rhs) noexcept
        -> std::optional<signed_integer_basis<BasisType>>
    {
        using result_type = signed_integer_basis<BasisType>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType result {};

        #if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_sub_overflow) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

        if constexpr (!std::is_same_v<BasisType, int128::int128_t>)
        {
            if (!std::is_constant_evaluated())
            {
                const auto status {impl::signed_intrin_sub(lhs_basis, rhs_basis, result)};
                return status != signed_overflow_status::no_error
                    ? std::nullopt
                    : std::make_optional(result_type{result});
            }
        }

        #endif

        const auto status {impl::signed_no_intrin_sub(lhs_basis, rhs_basis, result)};
        return status != signed_overflow_status::no_error
            ? std::nullopt
            : std::make_optional(result_type{result});
    }
};

template <overflow_policy Policy, fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto sub_impl(const signed_integer_basis<BasisType> lhs,
                                      const signed_integer_basis<BasisType> rhs)
    noexcept(Policy == overflow_policy::saturate || Policy == overflow_policy::overflow_tuple ||
             Policy == overflow_policy::checked || Policy == overflow_policy::strict)
{
    return signed_sub_helper<Policy, BasisType>::apply(lhs, rhs);
}

} // namespace impl

template <fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto operator-(const signed_integer_basis<BasisType> lhs,
                                       const signed_integer_basis<BasisType> rhs) -> signed_integer_basis<BasisType>
{
    if (std::is_constant_evaluated())
    {
        BasisType res {};
        const auto status {impl::signed_no_intrin_sub(static_cast<BasisType>(lhs), static_cast<BasisType>(rhs), res)};
        if (status == impl::signed_overflow_status::overflow)
        {
            if constexpr (std::is_same_v<BasisType, std::int8_t>)
            {
                throw std::overflow_error("Overflow detected in i8 subtraction");
            }
            else if constexpr (std::is_same_v<BasisType, std::int16_t>)
            {
                throw std::overflow_error("Overflow detected in i16 subtraction");
            }
            else if constexpr (std::is_same_v<BasisType, std::int32_t>)
            {
                throw std::overflow_error("Overflow detected in i32 subtraction");
            }
            else if constexpr (std::is_same_v<BasisType, std::int64_t>)
            {
                throw std::overflow_error("Overflow detected in i64 subtraction");
            }
            else
            {
                throw std::overflow_error("Overflow detected in i128 subtraction");
            }
        }
        else if (status == impl::signed_overflow_status::underflow)
        {
            if constexpr (std::is_same_v<BasisType, std::int8_t>)
            {
                throw std::underflow_error("Underflow detected in i8 subtraction");
            }
            else if constexpr (std::is_same_v<BasisType, std::int16_t>)
            {
                throw std::underflow_error("Underflow detected in i16 subtraction");
            }
            else if constexpr (std::is_same_v<BasisType, std::int32_t>)
            {
                throw std::underflow_error("Underflow detected in i32 subtraction");
            }
            else if constexpr (std::is_same_v<BasisType, std::int64_t>)
            {
                throw std::underflow_error("Underflow detected in i64 subtraction");
            }
            else
            {
                throw std::underflow_error("Underflow detected in i128 subtraction");
            }
        }

        return signed_integer_basis<BasisType>{res};
    }

    return impl::signed_sub_helper<overflow_policy::throw_exception, BasisType>::apply(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("subtraction", operator-)

template <fundamental_signed_integral BasisType>
template <fundamental_signed_integral OtherBasisType>
constexpr auto signed_integer_basis<BasisType>::operator-=(const signed_integer_basis<OtherBasisType> rhs)
    -> signed_integer_basis&
{
    *this = *this - rhs;
    return *this;
}

// ------------------------------
// Multiplication
// ------------------------------

namespace impl {

template <fundamental_signed_integral BasisType>
constexpr auto signed_overflow_mul_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::int8_t>)
    {
        return "Overflow detected in i8 multiplication";
    }
    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
    {
        return "Overflow detected in i16 multiplication";
    }
    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
    {
        return "Overflow detected in i32 multiplication";
    }
    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
    {
        return "Overflow detected in i64 multiplication";
    }
    else
    {
        return "Overflow detected in i128 multiplication";
    }
}

template <fundamental_signed_integral BasisType>
constexpr auto signed_underflow_mul_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::int8_t>)
    {
        return "Underflow detected in i8 multiplication";
    }
    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
    {
        return "Underflow detected in i16 multiplication";
    }
    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
    {
        return "Underflow detected in i32 multiplication";
    }
    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
    {
        return "Underflow detected in i64 multiplication";
    }
    else
    {
        return "Underflow detected in i128 multiplication";
    }
}

#if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_mul_overflow)

template <std::signed_integral T>
auto signed_intrin_mul(const T lhs, const T rhs, T& result) -> signed_overflow_status
{
    if (__builtin_mul_overflow(lhs, rhs, &result))
    {
        return (lhs >= 0) == (rhs >= 0) ? signed_overflow_status::overflow : signed_overflow_status::underflow;
    }

    return signed_overflow_status::no_error;
}

#elif defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

template <std::signed_integral T>
auto signed_intrin_mul(const T lhs, const T rhs, T& result) -> signed_overflow_status
{
    if constexpr (std::is_same_v<T, std::int8_t> || std::is_same_v<T, std::int16_t>)
    {
        const auto wide {static_cast<std::int32_t>(lhs) * static_cast<std::int32_t>(rhs)};
        result = static_cast<T>(wide);

        if (wide > static_cast<std::int32_t>(std::numeric_limits<T>::max()))
        {
            return signed_overflow_status::overflow;
        }
        if (wide < static_cast<std::int32_t>(std::numeric_limits<T>::min()))
        {
            return signed_overflow_status::underflow;
        }

        return signed_overflow_status::no_error;
    }
    else if constexpr (std::is_same_v<T, std::int32_t>)
    {
        const auto wide {static_cast<std::int64_t>(lhs) * static_cast<std::int64_t>(rhs)};
        result = static_cast<T>(wide);

        if (wide > static_cast<std::int64_t>(std::numeric_limits<T>::max()))
        {
            return signed_overflow_status::overflow;
        }
        if (wide < static_cast<std::int64_t>(std::numeric_limits<T>::min()))
        {
            return signed_overflow_status::underflow;
        }

        return signed_overflow_status::no_error;
    }
    else if constexpr (std::is_same_v<T, std::int64_t>)
    {
        if (lhs == 0 || rhs == 0)
        {
            result = 0;
            return signed_overflow_status::no_error;
        }

        if (lhs > 0)
        {
            if (rhs > 0)
            {
                if (lhs > std::numeric_limits<T>::max() / rhs)
                {
                    result = 0;
                    return signed_overflow_status::overflow;
                }
            }
            else
            {
                if (rhs < std::numeric_limits<T>::min() / lhs)
                {
                    result = 0;
                    return signed_overflow_status::underflow;
                }
            }
        }
        else
        {
            if (rhs > 0)
            {
                if (lhs < std::numeric_limits<T>::min() / rhs)
                {
                    result = 0;
                    return signed_overflow_status::underflow;
                }
            }
            else
            {
                if (lhs < std::numeric_limits<T>::max() / rhs)
                {
                    result = 0;
                    return signed_overflow_status::overflow;
                }
            }
        }

        result = static_cast<T>(lhs * rhs);
        return signed_overflow_status::no_error;
    }
}

#endif

template <fundamental_signed_integral T>
constexpr auto signed_no_intrin_mul(const T lhs, const T rhs, T& result) noexcept -> signed_overflow_status
{
    if constexpr (std::is_same_v<T, int128::int128_t>)
    {
        if (lhs == 0 || rhs == 0)
        {
            result = 0;
            return signed_overflow_status::no_error;
        }

        // Use unsigned arithmetic for overflow detection (avoids int128 signed division issues)
        using unsigned_t = make_unsigned_helper_t<T>;

        // Compute absolute values in unsigned domain (two's complement negation)
        const auto abs_lhs = lhs < 0
            ? static_cast<unsigned_t>(~static_cast<unsigned_t>(lhs)) + unsigned_t{1}
            : static_cast<unsigned_t>(lhs);
        const auto abs_rhs = rhs < 0
            ? static_cast<unsigned_t>(~static_cast<unsigned_t>(rhs)) + unsigned_t{1}
            : static_cast<unsigned_t>(rhs);

        const auto result_negative = (lhs < 0) != (rhs < 0);

        // Maximum magnitude the result can have
        const auto max_magnitude = result_negative
            ? static_cast<unsigned_t>(std::numeric_limits<T>::max()) + unsigned_t{1}  // |min|
            : static_cast<unsigned_t>(std::numeric_limits<T>::max());

        // Unsigned overflow check (well-tested for uint128)
        // Always compute the wrapped product so overflow_tuple policy gets the correct value
        result = static_cast<T>(lhs * rhs);
        if (abs_rhs != unsigned_t{0} && abs_lhs > max_magnitude / abs_rhs)
        {
            return result_negative ? signed_overflow_status::underflow : signed_overflow_status::overflow;
        }

        return signed_overflow_status::no_error;
    }
    else if constexpr (std::is_same_v<T, std::int8_t> || std::is_same_v<T, std::int16_t>)
    {
        const auto wide {static_cast<std::int32_t>(lhs) * static_cast<std::int32_t>(rhs)};
        result = static_cast<T>(wide);

        if (wide > static_cast<std::int32_t>(std::numeric_limits<T>::max()))
        {
            return signed_overflow_status::overflow;
        }
        if (wide < static_cast<std::int32_t>(std::numeric_limits<T>::min()))
        {
            return signed_overflow_status::underflow;
        }

        return signed_overflow_status::no_error;
    }
    else if constexpr (std::is_same_v<T, std::int32_t>)
    {
        const auto wide {static_cast<std::int64_t>(lhs) * static_cast<std::int64_t>(rhs)};
        result = static_cast<T>(wide);

        if (wide > static_cast<std::int64_t>(std::numeric_limits<T>::max()))
        {
            return signed_overflow_status::overflow;
        }
        if (wide < static_cast<std::int64_t>(std::numeric_limits<T>::min()))
        {
            return signed_overflow_status::underflow;
        }

        return signed_overflow_status::no_error;
    }
    else // std::int64_t
    {
        const auto wide {static_cast<int128::int128_t>(lhs) * static_cast<int128::int128_t>(rhs)};
        result = static_cast<T>(wide);

        if (wide > static_cast<int128::int128_t>(std::numeric_limits<T>::max()))
        {
            return signed_overflow_status::overflow;
        }
        if (wide < static_cast<int128::int128_t>(std::numeric_limits<T>::min()))
        {
            return signed_overflow_status::underflow;
        }

        return signed_overflow_status::no_error;
    }
}

template <overflow_policy Policy, fundamental_signed_integral BasisType>
struct signed_mul_helper
{
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType> lhs,
                                              const signed_integer_basis<BasisType> rhs)
        noexcept(Policy != overflow_policy::throw_exception)
        -> signed_integer_basis<BasisType>
    {
        using result_type = signed_integer_basis<BasisType>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType result {};

        auto handle_error = [&result](signed_overflow_status status)
        {
            if (std::is_constant_evaluated())
            {
                if (status == signed_overflow_status::overflow)
                {
                    if constexpr (std::is_same_v<BasisType, std::int8_t>)
                    {
                        throw std::overflow_error("Overflow detected in i8 multiplication");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
                    {
                        throw std::overflow_error("Overflow detected in i16 multiplication");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
                    {
                        throw std::overflow_error("Overflow detected in i32 multiplication");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
                    {
                        throw std::overflow_error("Overflow detected in i64 multiplication");
                    }
                    else
                    {
                        throw std::overflow_error("Overflow detected in i128 multiplication");
                    }
                }
                else
                {
                    if constexpr (std::is_same_v<BasisType, std::int8_t>)
                    {
                        throw std::underflow_error("Underflow detected in i8 multiplication");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
                    {
                        throw std::underflow_error("Underflow detected in i16 multiplication");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
                    {
                        throw std::underflow_error("Underflow detected in i32 multiplication");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
                    {
                        throw std::underflow_error("Underflow detected in i64 multiplication");
                    }
                    else
                    {
                        throw std::underflow_error("Underflow detected in i128 multiplication");
                    }
                }
            }
            else
            {
                if constexpr (Policy == overflow_policy::throw_exception)
                {
                    static_cast<void>(result);

                    if (status == signed_overflow_status::overflow)
                    {
                        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, signed_overflow_mul_msg<BasisType>());
                    }
                    else
                    {
                        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, signed_underflow_mul_msg<BasisType>());
                    }
                }
                else if constexpr (Policy == overflow_policy::saturate)
                {
                    if (status == signed_overflow_status::overflow)
                    {
                        result = std::numeric_limits<BasisType>::max();
                    }
                    else
                    {
                        result = std::numeric_limits<BasisType>::min();
                    }
                }
                else if constexpr (Policy == overflow_policy::strict)
                {
                    static_cast<void>(result);
                    std::exit(EXIT_FAILURE);
                }
                else
                {
                    static_cast<void>(result);
                    BOOST_SAFE_NUMBERS_UNREACHABLE;
                }
            }
        };

        #if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_mul_overflow) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

        if constexpr (!std::is_same_v<BasisType, int128::int128_t>)
        {
            if (!std::is_constant_evaluated())
            {
                const auto status {impl::signed_intrin_mul(lhs_basis, rhs_basis, result)};
                if (status != signed_overflow_status::no_error)
                {
                    handle_error(status);
                }

                return result_type{result};
            }
        }

        #endif // BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_mul_overflow) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

        const auto status {impl::signed_no_intrin_mul(lhs_basis, rhs_basis, result)};
        if (status != signed_overflow_status::no_error)
        {
            handle_error(status);
        }

        return result_type{result};
    }
};

// Partial specialization for overflow_tuple policy
template <fundamental_signed_integral BasisType>
struct signed_mul_helper<overflow_policy::overflow_tuple, BasisType>
{
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType> lhs,
                                              const signed_integer_basis<BasisType> rhs) noexcept
        -> std::pair<signed_integer_basis<BasisType>, bool>
    {
        using result_type = signed_integer_basis<BasisType>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType result {};

        #if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_mul_overflow) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

        if constexpr (!std::is_same_v<BasisType, int128::int128_t>)
        {
            if (!std::is_constant_evaluated())
            {
                const auto status {impl::signed_intrin_mul(lhs_basis, rhs_basis, result)};
                return std::make_pair(result_type{result}, status != signed_overflow_status::no_error);
            }
        }

        #endif

        const auto status {impl::signed_no_intrin_mul(lhs_basis, rhs_basis, result)};
        return std::make_pair(result_type{result}, status != signed_overflow_status::no_error);
    }
};

// Partial specialization for checked policy
template <fundamental_signed_integral BasisType>
struct signed_mul_helper<overflow_policy::checked, BasisType>
{
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType> lhs,
                                              const signed_integer_basis<BasisType> rhs) noexcept
        -> std::optional<signed_integer_basis<BasisType>>
    {
        using result_type = signed_integer_basis<BasisType>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType result {};

        #if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_mul_overflow) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

        if constexpr (!std::is_same_v<BasisType, int128::int128_t>)
        {
            if (!std::is_constant_evaluated())
            {
                const auto status {impl::signed_intrin_mul(lhs_basis, rhs_basis, result)};
                return status != signed_overflow_status::no_error
                    ? std::nullopt
                    : std::make_optional(result_type{result});
            }
        }

        #endif

        const auto status {impl::signed_no_intrin_mul(lhs_basis, rhs_basis, result)};
        return status != signed_overflow_status::no_error
            ? std::nullopt
            : std::make_optional(result_type{result});
    }
};

// Partial specialization for widening policy
template <fundamental_signed_integral BasisType>
struct signed_mul_helper<overflow_policy::widen, BasisType>
{
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType> lhs,
                                              const signed_integer_basis<BasisType> rhs) noexcept
    {
        using promoted = signed_promoted_type<BasisType>;
        static_assert(!std::is_same_v<promoted, bool>, "Widening policy with int128_t is not supported");

        using result_type = signed_integer_basis<promoted>;
        return result_type{static_cast<promoted>(static_cast<promoted>(static_cast<BasisType>(lhs)) * static_cast<BasisType>(rhs))};
    }
};

template <overflow_policy Policy, fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto mul_impl(const signed_integer_basis<BasisType> lhs,
                                      const signed_integer_basis<BasisType> rhs)
    noexcept(Policy == overflow_policy::saturate || Policy == overflow_policy::overflow_tuple ||
             Policy == overflow_policy::checked || Policy == overflow_policy::strict ||
             Policy == overflow_policy::widen)
{
    return signed_mul_helper<Policy, BasisType>::apply(lhs, rhs);
}

} // namespace impl

template <fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto operator*(const signed_integer_basis<BasisType> lhs,
                                       const signed_integer_basis<BasisType> rhs) -> signed_integer_basis<BasisType>
{
    if (std::is_constant_evaluated())
    {
        BasisType res {};
        const auto status {impl::signed_no_intrin_mul(static_cast<BasisType>(lhs), static_cast<BasisType>(rhs), res)};
        if (status == impl::signed_overflow_status::overflow)
        {
            if constexpr (std::is_same_v<BasisType, std::int8_t>)
            {
                throw std::overflow_error("Overflow detected in i8 multiplication");
            }
            else if constexpr (std::is_same_v<BasisType, std::int16_t>)
            {
                throw std::overflow_error("Overflow detected in i16 multiplication");
            }
            else if constexpr (std::is_same_v<BasisType, std::int32_t>)
            {
                throw std::overflow_error("Overflow detected in i32 multiplication");
            }
            else if constexpr (std::is_same_v<BasisType, std::int64_t>)
            {
                throw std::overflow_error("Overflow detected in i64 multiplication");
            }
            else
            {
                throw std::overflow_error("Overflow detected in i128 multiplication");
            }
        }
        else if (status == impl::signed_overflow_status::underflow)
        {
            if constexpr (std::is_same_v<BasisType, std::int8_t>)
            {
                throw std::underflow_error("Underflow detected in i8 multiplication");
            }
            else if constexpr (std::is_same_v<BasisType, std::int16_t>)
            {
                throw std::underflow_error("Underflow detected in i16 multiplication");
            }
            else if constexpr (std::is_same_v<BasisType, std::int32_t>)
            {
                throw std::underflow_error("Underflow detected in i32 multiplication");
            }
            else if constexpr (std::is_same_v<BasisType, std::int64_t>)
            {
                throw std::underflow_error("Underflow detected in i64 multiplication");
            }
            else
            {
                throw std::underflow_error("Underflow detected in i128 multiplication");
            }
        }

        return signed_integer_basis<BasisType>{res};
    }

    return impl::signed_mul_helper<overflow_policy::throw_exception, BasisType>::apply(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("multiplication", operator*)

template <fundamental_signed_integral BasisType>
template <fundamental_signed_integral OtherBasisType>
constexpr auto signed_integer_basis<BasisType>::operator*=(const signed_integer_basis<OtherBasisType> rhs)
    -> signed_integer_basis&
{
    *this = *this * rhs;
    return *this;
}

// ------------------------------
// Division
// ------------------------------

namespace impl {

template <fundamental_signed_integral BasisType>
constexpr auto signed_div_by_zero_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::int8_t>)
    {
        return "Division by zero in i8 division";
    }
    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
    {
        return "Division by zero in i16 division";
    }
    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
    {
        return "Division by zero in i32 division";
    }
    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
    {
        return "Division by zero in i64 division";
    }
    else
    {
        return "Division by zero in i128 division";
    }
}

template <fundamental_signed_integral BasisType>
constexpr auto signed_overflow_div_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::int8_t>)
    {
        return "Overflow detected in i8 division";
    }
    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
    {
        return "Overflow detected in i16 division";
    }
    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
    {
        return "Overflow detected in i32 division";
    }
    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
    {
        return "Overflow detected in i64 division";
    }
    else
    {
        return "Overflow detected in i128 division";
    }
}

template <overflow_policy Policy, fundamental_signed_integral BasisType>
struct signed_div_helper
{
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType> lhs,
                                              const signed_integer_basis<BasisType> rhs)
        noexcept(Policy == overflow_policy::strict)
        -> signed_integer_basis<BasisType>
    {
        using result_type = signed_integer_basis<BasisType>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};

        if (rhs_basis == BasisType{0}) [[unlikely]]
        {
            if constexpr (Policy == overflow_policy::strict)
            {
                std::exit(EXIT_FAILURE);
            }
            else
            {
                if (std::is_constant_evaluated())
                {
                    if constexpr (std::is_same_v<BasisType, std::int8_t>)
                    {
                        throw std::domain_error("Division by zero in i8 division");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
                    {
                        throw std::domain_error("Division by zero in i16 division");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
                    {
                        throw std::domain_error("Division by zero in i32 division");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
                    {
                        throw std::domain_error("Division by zero in i64 division");
                    }
                    else
                    {
                        throw std::domain_error("Division by zero in i128 division");
                    }
                }
                else
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, signed_div_by_zero_msg<BasisType>());
                }
            }
        }

        // Fast path: 0 / x = 0 for any non-zero x
        if (lhs_basis == BasisType{0})
        {
            return result_type{BasisType{0}};
        }

        // min / -1 is the only division that overflows (since -min > max in two's complement)
        if (lhs_basis == std::numeric_limits<BasisType>::min() &&
            rhs_basis == static_cast<BasisType>(-1)) [[unlikely]]
        {
            if constexpr (Policy == overflow_policy::strict)
            {
                std::exit(EXIT_FAILURE);
            }
            else if constexpr (Policy == overflow_policy::saturate)
            {
                return result_type{std::numeric_limits<BasisType>::max()};
            }
            else
            {
                if (std::is_constant_evaluated())
                {
                    if constexpr (std::is_same_v<BasisType, std::int8_t>)
                    {
                        throw std::overflow_error("Overflow detected in i8 division");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
                    {
                        throw std::overflow_error("Overflow detected in i16 division");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
                    {
                        throw std::overflow_error("Overflow detected in i32 division");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
                    {
                        throw std::overflow_error("Overflow detected in i64 division");
                    }
                    else
                    {
                        throw std::overflow_error("Overflow detected in i128 division");
                    }
                }
                else
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, signed_overflow_div_msg<BasisType>());
                }
            }
        }

        return result_type{static_cast<BasisType>(lhs_basis / rhs_basis)};
    }
};

// Partial specialization for overflow_tuple policy
template <fundamental_signed_integral BasisType>
struct signed_div_helper<overflow_policy::overflow_tuple, BasisType>
{
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType> lhs,
                                              const signed_integer_basis<BasisType> rhs)
        -> std::pair<signed_integer_basis<BasisType>, bool>
    {
        using result_type = signed_integer_basis<BasisType>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};

        if (rhs_basis == BasisType{0}) [[unlikely]]
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, signed_div_by_zero_msg<BasisType>());
        }

        if (lhs_basis == BasisType{0})
        {
            return std::make_pair(result_type{BasisType{0}}, false);
        }

        if (lhs_basis == std::numeric_limits<BasisType>::min() &&
            rhs_basis == static_cast<BasisType>(-1)) [[unlikely]]
        {
            // The wrapped result is min (since -min overflows back to min in two's complement)
            return std::make_pair(result_type{std::numeric_limits<BasisType>::min()}, true);
        }

        return std::make_pair(result_type{static_cast<BasisType>(lhs_basis / rhs_basis)}, false);
    }
};

// Partial specialization for checked policy
template <fundamental_signed_integral BasisType>
struct signed_div_helper<overflow_policy::checked, BasisType>
{
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType> lhs,
                                              const signed_integer_basis<BasisType> rhs) noexcept
        -> std::optional<signed_integer_basis<BasisType>>
    {
        using result_type = signed_integer_basis<BasisType>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};

        if (rhs_basis == BasisType{0}) [[unlikely]]
        {
            return std::nullopt;
        }

        if (lhs_basis == BasisType{0})
        {
            return std::make_optional(result_type{BasisType{0}});
        }

        if (lhs_basis == std::numeric_limits<BasisType>::min() &&
            rhs_basis == static_cast<BasisType>(-1)) [[unlikely]]
        {
            return std::nullopt;
        }

        return std::make_optional(result_type{static_cast<BasisType>(lhs_basis / rhs_basis)});
    }
};

template <overflow_policy Policy, fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto div_impl(const signed_integer_basis<BasisType> lhs,
                                      const signed_integer_basis<BasisType> rhs)
    noexcept(Policy == overflow_policy::checked || Policy == overflow_policy::strict)
{
    return signed_div_helper<Policy, BasisType>::apply(lhs, rhs);
}

} // namespace impl

template <fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto operator/(const signed_integer_basis<BasisType> lhs,
                                       const signed_integer_basis<BasisType> rhs) -> signed_integer_basis<BasisType>
{
    return impl::signed_div_helper<overflow_policy::throw_exception, BasisType>::apply(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("division", operator/)

template <fundamental_signed_integral BasisType>
template <fundamental_signed_integral OtherBasisType>
constexpr auto signed_integer_basis<BasisType>::operator/=(const signed_integer_basis<OtherBasisType> rhs)
    -> signed_integer_basis&
{
    *this = *this / rhs;
    return *this;
}

// ------------------------------
// Modulo
// ------------------------------

namespace impl {

template <fundamental_signed_integral BasisType>
constexpr auto signed_mod_by_zero_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::int8_t>)
    {
        return "Division by zero in i8 modulo";
    }
    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
    {
        return "Division by zero in i16 modulo";
    }
    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
    {
        return "Division by zero in i32 modulo";
    }
    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
    {
        return "Division by zero in i64 modulo";
    }
    else
    {
        return "Division by zero in i128 modulo";
    }
}

template <fundamental_signed_integral BasisType>
constexpr auto signed_overflow_mod_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::int8_t>)
    {
        return "Overflow detected in i8 modulo";
    }
    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
    {
        return "Overflow detected in i16 modulo";
    }
    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
    {
        return "Overflow detected in i32 modulo";
    }
    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
    {
        return "Overflow detected in i64 modulo";
    }
    else
    {
        return "Overflow detected in i128 modulo";
    }
}

template <overflow_policy Policy, fundamental_signed_integral BasisType>
struct signed_mod_helper
{
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType> lhs,
                                              const signed_integer_basis<BasisType> rhs)
        noexcept(Policy == overflow_policy::strict)
        -> signed_integer_basis<BasisType>
    {
        using result_type = signed_integer_basis<BasisType>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};

        if (rhs_basis == BasisType{0}) [[unlikely]]
        {
            if constexpr (Policy == overflow_policy::strict)
            {
                std::exit(EXIT_FAILURE);
            }
            else
            {
                if (std::is_constant_evaluated())
                {
                    if constexpr (std::is_same_v<BasisType, std::int8_t>)
                    {
                        throw std::domain_error("Division by zero in i8 modulo");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
                    {
                        throw std::domain_error("Division by zero in i16 modulo");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
                    {
                        throw std::domain_error("Division by zero in i32 modulo");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
                    {
                        throw std::domain_error("Division by zero in i64 modulo");
                    }
                    else
                    {
                        throw std::domain_error("Division by zero in i128 modulo");
                    }
                }
                else
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, signed_mod_by_zero_msg<BasisType>());
                }
            }
        }

        // Fast path: 0 % x = 0 for any non-zero x
        // Also avoids int128 software division assertion on zero dividend
        if (lhs_basis == BasisType{0})
        {
            return result_type{BasisType{0}};
        }

        // min % -1 is UB for built-in signed types (the implicit division overflows)
        // The mathematical result is 0, but we treat this as overflow consistent with min / -1
        if (lhs_basis == std::numeric_limits<BasisType>::min() &&
            rhs_basis == static_cast<BasisType>(-1)) [[unlikely]]
        {
            if constexpr (Policy == overflow_policy::strict)
            {
                std::exit(EXIT_FAILURE);
            }
            else if constexpr (Policy == overflow_policy::saturate)
            {
                // The mathematical result of min % -1 is 0
                return result_type{BasisType{0}};
            }
            else
            {
                if (std::is_constant_evaluated())
                {
                    if constexpr (std::is_same_v<BasisType, std::int8_t>)
                    {
                        throw std::overflow_error("Overflow detected in i8 modulo");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
                    {
                        throw std::overflow_error("Overflow detected in i16 modulo");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
                    {
                        throw std::overflow_error("Overflow detected in i32 modulo");
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
                    {
                        throw std::overflow_error("Overflow detected in i64 modulo");
                    }
                    else
                    {
                        throw std::overflow_error("Overflow detected in i128 modulo");
                    }
                }
                else
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, signed_overflow_mod_msg<BasisType>());
                }
            }
        }

        return result_type{static_cast<BasisType>(lhs_basis % rhs_basis)};
    }
};

// Partial specialization for overflow_tuple policy
template <fundamental_signed_integral BasisType>
struct signed_mod_helper<overflow_policy::overflow_tuple, BasisType>
{
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType> lhs,
                                              const signed_integer_basis<BasisType> rhs)
        -> std::pair<signed_integer_basis<BasisType>, bool>
    {
        using result_type = signed_integer_basis<BasisType>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};

        if (rhs_basis == BasisType{0}) [[unlikely]]
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, signed_mod_by_zero_msg<BasisType>());
        }

        if (lhs_basis == BasisType{0})
        {
            return std::make_pair(result_type{BasisType{0}}, false);
        }

        if (lhs_basis == std::numeric_limits<BasisType>::min() &&
            rhs_basis == static_cast<BasisType>(-1)) [[unlikely]]
        {
            // The mathematical result is 0, but the implied division overflows
            return std::make_pair(result_type{BasisType{0}}, true);
        }

        return std::make_pair(result_type{static_cast<BasisType>(lhs_basis % rhs_basis)}, false);
    }
};

// Partial specialization for checked policy
template <fundamental_signed_integral BasisType>
struct signed_mod_helper<overflow_policy::checked, BasisType>
{
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType> lhs,
                                              const signed_integer_basis<BasisType> rhs) noexcept
        -> std::optional<signed_integer_basis<BasisType>>
    {
        using result_type = signed_integer_basis<BasisType>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};

        if (rhs_basis == BasisType{0}) [[unlikely]]
        {
            return std::nullopt;
        }

        if (lhs_basis == BasisType{0})
        {
            return std::make_optional(result_type{BasisType{0}});
        }

        if (lhs_basis == std::numeric_limits<BasisType>::min() &&
            rhs_basis == static_cast<BasisType>(-1)) [[unlikely]]
        {
            return std::nullopt;
        }

        return std::make_optional(result_type{static_cast<BasisType>(lhs_basis % rhs_basis)});
    }
};

template <overflow_policy Policy, fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto mod_impl(const signed_integer_basis<BasisType> lhs,
                                      const signed_integer_basis<BasisType> rhs)
    noexcept(Policy == overflow_policy::checked || Policy == overflow_policy::strict)
{
    return signed_mod_helper<Policy, BasisType>::apply(lhs, rhs);
}

} // namespace impl

template <fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto operator%(const signed_integer_basis<BasisType> lhs,
                                       const signed_integer_basis<BasisType> rhs) -> signed_integer_basis<BasisType>
{
    return impl::signed_mod_helper<overflow_policy::throw_exception, BasisType>::apply(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("modulo", operator%)

template <fundamental_signed_integral BasisType>
template <fundamental_signed_integral OtherBasisType>
constexpr auto signed_integer_basis<BasisType>::operator%=(const signed_integer_basis<OtherBasisType> rhs)
    -> signed_integer_basis&
{
    *this = *this % rhs;
    return *this;
}

// ------------------------------
// Increment / Decrement error messages
// ------------------------------

template <fundamental_signed_integral BasisType>
constexpr auto signed_overflow_inc_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::int8_t>)
    {
        return "Overflow detected in i8 increment";
    }
    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
    {
        return "Overflow detected in i16 increment";
    }
    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
    {
        return "Overflow detected in i32 increment";
    }
    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
    {
        return "Overflow detected in i64 increment";
    }
    else
    {
        return "Overflow detected in i128 increment";
    }
}

template <fundamental_signed_integral BasisType>
constexpr auto signed_underflow_dec_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::int8_t>)
    {
        return "Underflow detected in i8 decrement";
    }
    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
    {
        return "Underflow detected in i16 decrement";
    }
    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
    {
        return "Underflow detected in i32 decrement";
    }
    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
    {
        return "Underflow detected in i64 decrement";
    }
    else
    {
        return "Underflow detected in i128 decrement";
    }
}

// ------------------------------
// Pre and post increment
// ------------------------------

template <fundamental_signed_integral BasisType>
constexpr auto signed_integer_basis<BasisType>::operator++()
    -> signed_integer_basis&
{
    if (this->basis_ == std::numeric_limits<BasisType>::max()) [[unlikely]]
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, signed_overflow_inc_msg<BasisType>());
    }

    ++this->basis_;
    return *this;
}

template <fundamental_signed_integral BasisType>
constexpr auto signed_integer_basis<BasisType>::operator++(int)
    -> signed_integer_basis
{
    if (this->basis_ == std::numeric_limits<BasisType>::max()) [[unlikely]]
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, signed_overflow_inc_msg<BasisType>());
    }

    const auto temp {*this};
    ++this->basis_;
    return temp;
}

// ------------------------------
// Pre and post decrement
// ------------------------------

template <fundamental_signed_integral BasisType>
constexpr auto signed_integer_basis<BasisType>::operator--()
    -> signed_integer_basis&
{
    if (this->basis_ == std::numeric_limits<BasisType>::min()) [[unlikely]]
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, signed_underflow_dec_msg<BasisType>());
    }

    --this->basis_;
    return *this;
}

template <fundamental_signed_integral BasisType>
constexpr auto signed_integer_basis<BasisType>::operator--(int)
    -> signed_integer_basis
{
    if (this->basis_ == std::numeric_limits<BasisType>::min()) [[unlikely]]
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, signed_underflow_dec_msg<BasisType>());
    }

    const auto temp {*this};
    --this->basis_;
    return temp;
}

} // namespace boost::safe_numbers::detail

// ------------------------------
// Saturating Math
// ------------------------------

namespace boost::safe_numbers {

template <detail::fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto saturating_add(const detail::signed_integer_basis<BasisType> lhs,
                                            const detail::signed_integer_basis<BasisType> rhs) noexcept
    -> detail::signed_integer_basis<BasisType>
{
    return detail::impl::add_impl<overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("saturating addition", saturating_add)

template <detail::fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto saturating_sub(const detail::signed_integer_basis<BasisType> lhs,
                                            const detail::signed_integer_basis<BasisType> rhs) noexcept
    -> detail::signed_integer_basis<BasisType>
{
    return detail::impl::sub_impl<overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("saturating subtraction", saturating_sub)

template <detail::fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto saturating_mul(const detail::signed_integer_basis<BasisType> lhs,
                                            const detail::signed_integer_basis<BasisType> rhs) noexcept
    -> detail::signed_integer_basis<BasisType>
{
    return detail::impl::mul_impl<overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("saturating multiplication", saturating_mul)

template <detail::fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto saturating_div(const detail::signed_integer_basis<BasisType> lhs,
                                            const detail::signed_integer_basis<BasisType> rhs)
    -> detail::signed_integer_basis<BasisType>
{
    return detail::impl::div_impl<overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("saturating division", saturating_div)

template <detail::fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto saturating_mod(const detail::signed_integer_basis<BasisType> lhs,
                                            const detail::signed_integer_basis<BasisType> rhs)
    -> detail::signed_integer_basis<BasisType>
{
    return detail::impl::mod_impl<overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("saturating modulo", saturating_mod)

// ------------------------------
// Overflowing Math
// ------------------------------

template <detail::fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto overflowing_add(const detail::signed_integer_basis<BasisType> lhs,
                                             const detail::signed_integer_basis<BasisType> rhs) noexcept
    -> std::pair<detail::signed_integer_basis<BasisType>, bool>
{
    return detail::impl::add_impl<overflow_policy::overflow_tuple>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("overflowing addition", overflowing_add)

template <detail::fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto overflowing_sub(const detail::signed_integer_basis<BasisType> lhs,
                                             const detail::signed_integer_basis<BasisType> rhs) noexcept
    -> std::pair<detail::signed_integer_basis<BasisType>, bool>
{
    return detail::impl::sub_impl<overflow_policy::overflow_tuple>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("overflowing subtraction", overflowing_sub)

template <detail::fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto overflowing_mul(const detail::signed_integer_basis<BasisType> lhs,
                                             const detail::signed_integer_basis<BasisType> rhs) noexcept
    -> std::pair<detail::signed_integer_basis<BasisType>, bool>
{
    return detail::impl::mul_impl<overflow_policy::overflow_tuple>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("overflowing multiplication", overflowing_mul)

template <detail::fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto overflowing_div(const detail::signed_integer_basis<BasisType> lhs,
                                             const detail::signed_integer_basis<BasisType> rhs)
    -> std::pair<detail::signed_integer_basis<BasisType>, bool>
{
    return detail::impl::div_impl<overflow_policy::overflow_tuple>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("overflowing division", overflowing_div)

template <detail::fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto overflowing_mod(const detail::signed_integer_basis<BasisType> lhs,
                                             const detail::signed_integer_basis<BasisType> rhs)
    -> std::pair<detail::signed_integer_basis<BasisType>, bool>
{
    return detail::impl::mod_impl<overflow_policy::overflow_tuple>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("overflowing modulo", overflowing_mod)

// ------------------------------
// Checked Math
// ------------------------------

template <detail::fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto checked_add(const detail::signed_integer_basis<BasisType> lhs,
                                         const detail::signed_integer_basis<BasisType> rhs) noexcept
    -> std::optional<detail::signed_integer_basis<BasisType>>
{
    return detail::impl::add_impl<overflow_policy::checked>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("checked addition", checked_add)

template <detail::fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto checked_sub(const detail::signed_integer_basis<BasisType> lhs,
                                         const detail::signed_integer_basis<BasisType> rhs) noexcept
    -> std::optional<detail::signed_integer_basis<BasisType>>
{
    return detail::impl::sub_impl<overflow_policy::checked>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("checked subtraction", checked_sub)

template <detail::fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto checked_mul(const detail::signed_integer_basis<BasisType> lhs,
                                         const detail::signed_integer_basis<BasisType> rhs) noexcept
    -> std::optional<detail::signed_integer_basis<BasisType>>
{
    return detail::impl::mul_impl<overflow_policy::checked>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("checked multiplication", checked_mul)

template <detail::fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto checked_div(const detail::signed_integer_basis<BasisType> lhs,
                                         const detail::signed_integer_basis<BasisType> rhs) noexcept
    -> std::optional<detail::signed_integer_basis<BasisType>>
{
    return detail::impl::div_impl<overflow_policy::checked>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("checked division", checked_div)

template <detail::fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto checked_mod(const detail::signed_integer_basis<BasisType> lhs,
                                         const detail::signed_integer_basis<BasisType> rhs) noexcept
    -> std::optional<detail::signed_integer_basis<BasisType>>
{
    return detail::impl::mod_impl<overflow_policy::checked>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("checked modulo", checked_mod)

// ------------------------------
// Strict Math
// ------------------------------

template <detail::fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto strict_add(const detail::signed_integer_basis<BasisType> lhs,
                                        const detail::signed_integer_basis<BasisType> rhs) noexcept
    -> detail::signed_integer_basis<BasisType>
{
    return detail::impl::add_impl<overflow_policy::strict>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("strict addition", strict_add)

template <detail::fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto strict_sub(const detail::signed_integer_basis<BasisType> lhs,
                                        const detail::signed_integer_basis<BasisType> rhs) noexcept
    -> detail::signed_integer_basis<BasisType>
{
    return detail::impl::sub_impl<overflow_policy::strict>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("strict subtraction", strict_sub)

template <detail::fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto strict_mul(const detail::signed_integer_basis<BasisType> lhs,
                                        const detail::signed_integer_basis<BasisType> rhs) noexcept
    -> detail::signed_integer_basis<BasisType>
{
    return detail::impl::mul_impl<overflow_policy::strict>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("strict multiplication", strict_mul)

template <detail::fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto strict_div(const detail::signed_integer_basis<BasisType> lhs,
                                        const detail::signed_integer_basis<BasisType> rhs) noexcept
    -> detail::signed_integer_basis<BasisType>
{
    return detail::impl::div_impl<overflow_policy::strict>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("strict division", strict_div)

template <detail::fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto strict_mod(const detail::signed_integer_basis<BasisType> lhs,
                                        const detail::signed_integer_basis<BasisType> rhs) noexcept
    -> detail::signed_integer_basis<BasisType>
{
    return detail::impl::mod_impl<overflow_policy::strict>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("strict modulo", strict_mod)

// ------------------------------
// Widening Math
// ------------------------------

template <detail::fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto widening_add(const detail::signed_integer_basis<BasisType> lhs,
                                          const detail::signed_integer_basis<BasisType> rhs) noexcept
{
    return detail::impl::add_impl<overflow_policy::widen>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("widening add", widening_add)

template <detail::fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto widening_mul(const detail::signed_integer_basis<BasisType> lhs,
                                          const detail::signed_integer_basis<BasisType> rhs) noexcept
{
    return detail::impl::mul_impl<overflow_policy::widen>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("widening mul", widening_mul)

// ------------------------------
// Generic policy-parameterized functions
// ------------------------------

template <overflow_policy Policy, detail::fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto add(const detail::signed_integer_basis<BasisType> lhs,
                                 const detail::signed_integer_basis<BasisType> rhs)
    noexcept(Policy != overflow_policy::throw_exception)
{
    if constexpr (Policy == overflow_policy::throw_exception)
    {
        return lhs + rhs;
    }
    else if constexpr (Policy == overflow_policy::saturate)
    {
        return saturating_add(lhs, rhs);
    }
    else if constexpr (Policy == overflow_policy::overflow_tuple)
    {
        return overflowing_add(lhs, rhs);
    }
    else if constexpr (Policy == overflow_policy::checked)
    {
        return checked_add(lhs, rhs);
    }
    else if constexpr (Policy == overflow_policy::strict)
    {
        return strict_add(lhs, rhs);
    }
    else if constexpr (Policy == overflow_policy::widen)
    {
        return widening_add(lhs, rhs);
    }
    else
    {
        static_assert(detail::dependent_false<BasisType>, "Policy is not supported for addition");
    }
}

template <overflow_policy Policy, detail::fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto sub(const detail::signed_integer_basis<BasisType> lhs,
                                 const detail::signed_integer_basis<BasisType> rhs)
    noexcept(Policy != overflow_policy::throw_exception)
{
    if constexpr (Policy == overflow_policy::throw_exception)
    {
        return lhs - rhs;
    }
    else if constexpr (Policy == overflow_policy::saturate)
    {
        return saturating_sub(lhs, rhs);
    }
    else if constexpr (Policy == overflow_policy::overflow_tuple)
    {
        return overflowing_sub(lhs, rhs);
    }
    else if constexpr (Policy == overflow_policy::checked)
    {
        return checked_sub(lhs, rhs);
    }
    else if constexpr (Policy == overflow_policy::strict)
    {
        return strict_sub(lhs, rhs);
    }
    else
    {
        static_assert(detail::dependent_false<BasisType>, "Policy is not supported for subtraction");
    }
}

template <overflow_policy Policy, detail::fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto mul(const detail::signed_integer_basis<BasisType> lhs,
                                 const detail::signed_integer_basis<BasisType> rhs)
    noexcept(Policy != overflow_policy::throw_exception)
{
    if constexpr (Policy == overflow_policy::throw_exception)
    {
        return lhs * rhs;
    }
    else if constexpr (Policy == overflow_policy::saturate)
    {
        return saturating_mul(lhs, rhs);
    }
    else if constexpr (Policy == overflow_policy::overflow_tuple)
    {
        return overflowing_mul(lhs, rhs);
    }
    else if constexpr (Policy == overflow_policy::checked)
    {
        return checked_mul(lhs, rhs);
    }
    else if constexpr (Policy == overflow_policy::strict)
    {
        return strict_mul(lhs, rhs);
    }
    else if constexpr (Policy == overflow_policy::widen)
    {
        return widening_mul(lhs, rhs);
    }
    else
    {
        static_assert(detail::dependent_false<BasisType>, "Policy is not supported for multiplication");
    }
}

template <overflow_policy Policy, detail::fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto div(const detail::signed_integer_basis<BasisType> lhs,
                                 const detail::signed_integer_basis<BasisType> rhs)
    noexcept(Policy == overflow_policy::checked || Policy == overflow_policy::strict)
{
    if constexpr (Policy == overflow_policy::throw_exception)
    {
        return lhs / rhs;
    }
    else if constexpr (Policy == overflow_policy::saturate)
    {
        return saturating_div(lhs, rhs);
    }
    else if constexpr (Policy == overflow_policy::overflow_tuple)
    {
        return overflowing_div(lhs, rhs);
    }
    else if constexpr (Policy == overflow_policy::checked)
    {
        return checked_div(lhs, rhs);
    }
    else if constexpr (Policy == overflow_policy::strict)
    {
        return strict_div(lhs, rhs);
    }
    else
    {
        static_assert(detail::dependent_false<BasisType>, "Policy is not supported for division");
    }
}

template <overflow_policy Policy, detail::fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto mod(const detail::signed_integer_basis<BasisType> lhs,
                                 const detail::signed_integer_basis<BasisType> rhs)
    noexcept(Policy == overflow_policy::checked || Policy == overflow_policy::strict)
{
    if constexpr (Policy == overflow_policy::throw_exception)
    {
        return lhs % rhs;
    }
    else if constexpr (Policy == overflow_policy::saturate)
    {
        return saturating_mod(lhs, rhs);
    }
    else if constexpr (Policy == overflow_policy::overflow_tuple)
    {
        return overflowing_mod(lhs, rhs);
    }
    else if constexpr (Policy == overflow_policy::checked)
    {
        return checked_mod(lhs, rhs);
    }
    else if constexpr (Policy == overflow_policy::strict)
    {
        return strict_mod(lhs, rhs);
    }
    else
    {
        static_assert(detail::dependent_false<BasisType>, "Policy is not supported for modulo");
    }
}

} // namespace boost::safe_numbers

#undef BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP

#endif //BOOST_SAFE_NUMBERS_SIGNED_INTEGER_BASIS_HPP

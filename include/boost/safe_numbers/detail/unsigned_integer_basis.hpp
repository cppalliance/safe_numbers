// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_UNSIGNED_INTEGER_BASIS_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_UNSIGNED_INTEGER_BASIS_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>
#include <boost/safe_numbers/detail/throw_exception.hpp>
#include <boost/safe_numbers/detail/int128/bit.hpp>
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

template <fundamental_unsigned_integral BasisType>
class unsigned_integer_basis
{
public:

    // This is exposed to the user so that they can convert back to built-in
    using basis_type = BasisType;

private:

    BasisType basis_ {0U};

public:

    constexpr unsigned_integer_basis() noexcept = default;

    BOOST_SAFE_NUMBERS_HOST_DEVICE explicit constexpr unsigned_integer_basis(const BasisType val) noexcept : basis_{val} {}

    template <typename T>
        requires std::is_same_v<T, bool>
    BOOST_SAFE_NUMBERS_HOST_DEVICE explicit constexpr unsigned_integer_basis(T) noexcept
    {
        static_assert(dependent_false<T>, "Construction from bool is not allowed");
    }

    template <fundamental_unsigned_integral OtherBasis>
    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] explicit constexpr operator OtherBasis() const;

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] explicit constexpr operator BasisType() const noexcept { return basis_;}

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] friend constexpr auto operator<=>(unsigned_integer_basis lhs, unsigned_integer_basis rhs) noexcept
        -> std::strong_ordering = default;

    template <fundamental_unsigned_integral OtherBasis>
    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator+=(unsigned_integer_basis<OtherBasis> rhs) -> unsigned_integer_basis&;

    template <fundamental_unsigned_integral OtherBasis>
    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator-=(unsigned_integer_basis<OtherBasis> rhs) -> unsigned_integer_basis&;

    template <fundamental_unsigned_integral OtherBasis>
    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator*=(unsigned_integer_basis<OtherBasis> rhs) -> unsigned_integer_basis&;

    template <fundamental_unsigned_integral OtherBasis>
    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator/=(unsigned_integer_basis<OtherBasis> rhs) -> unsigned_integer_basis&;

    template <fundamental_unsigned_integral OtherBasis>
    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator%=(unsigned_integer_basis<OtherBasis> rhs) -> unsigned_integer_basis&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator&=(unsigned_integer_basis rhs) noexcept -> unsigned_integer_basis&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator|=(unsigned_integer_basis rhs) noexcept -> unsigned_integer_basis&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator^=(unsigned_integer_basis rhs) noexcept -> unsigned_integer_basis&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator<<=(unsigned_integer_basis rhs) -> unsigned_integer_basis&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator>>=(unsigned_integer_basis rhs) -> unsigned_integer_basis&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator++() -> unsigned_integer_basis&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator++(int) -> unsigned_integer_basis;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator--() -> unsigned_integer_basis&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator--(int) -> unsigned_integer_basis;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator+() const noexcept -> unsigned_integer_basis { return *this; }

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator-() const noexcept
    {
        static_assert(dependent_false<BasisType>, "Unary minus is deliberately disabled for unsigned safe integers");
        return *this; // LCOV_EXCL_LINE : deliberately unreachable
    }
};

// Helper to map BasisType to a short name for diagnostic messages.
template <typename BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto unsigned_type_name() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::uint8_t>)
    {
        return "u8";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint16_t>)
    {
        return "u16";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint32_t>)
    {
        return "u32";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint64_t>)
    {
        return "u64";
    }
    else
    {
        return "u128";
    }
}

// Device-friendly error message helpers returning const char* string literals
// These avoid std::string concatenation which is not available on CUDA device

template <fundamental_unsigned_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto overflow_add_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::uint8_t>)
    {
        return "Overflow detected in u8 addition";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint16_t>)
    {
        return "Overflow detected in u16 addition";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint32_t>)
    {
        return "Overflow detected in u32 addition";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint64_t>)
    {
        return "Overflow detected in u64 addition";
    }
    else
    {
        return "Overflow detected in u128 addition";
    }
}

template <fundamental_unsigned_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto underflow_sub_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::uint8_t>)
    {
        return "Underflow detected in u8 subtraction";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint16_t>)
    {
        return "Underflow detected in u16 subtraction";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint32_t>)
    {
        return "Underflow detected in u32 subtraction";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint64_t>)
    {
        return "Underflow detected in u64 subtraction";
    }
    else
    {
        return "Underflow detected in u128 subtraction";
    }
}

template <fundamental_unsigned_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto overflow_mul_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::uint8_t>)
    {
        return "Overflow detected in u8 multiplication";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint16_t>)
    {
        return "Overflow detected in u16 multiplication";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint32_t>)
    {
        return "Overflow detected in u32 multiplication";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint64_t>)
    {
        return "Overflow detected in u64 multiplication";
    }
    else
    {
        return "Overflow detected in u128 multiplication";
    }
}

template <fundamental_unsigned_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto div_by_zero_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::uint8_t>)
    {
        return "Unsigned u8 division by zero";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint16_t>)
    {
        return "Unsigned u16 division by zero";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint32_t>)
    {
        return "Unsigned u32 division by zero";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint64_t>)
    {
        return "Unsigned u64 division by zero";
    }
    else
    {
        return "Unsigned u128 division by zero";
    }
}

template <fundamental_unsigned_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto mod_by_zero_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::uint8_t>)
    {
        return "Unsigned u8 modulo by zero";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint16_t>)
    {
        return "Unsigned u16 modulo by zero";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint32_t>)
    {
        return "Unsigned u32 modulo by zero";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint64_t>)
    {
        return "Unsigned u64 modulo by zero";
    }
    else
    {
        return "Unsigned u128 modulo by zero";
    }
}

template <fundamental_unsigned_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto overflow_inc_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::uint8_t>)
    {
        return "Overflow detected in u8 increment";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint16_t>)
    {
        return "Overflow detected in u16 increment";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint32_t>)
    {
        return "Overflow detected in u32 increment";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint64_t>)
    {
        return "Overflow detected in u64 increment";
    }
    else
    {
        return "Overflow detected in u128 increment";
    }
}

template <fundamental_unsigned_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto underflow_dec_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::uint8_t>)
    {
        return "Underflow detected in u8 decrement";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint16_t>)
    {
        return "Underflow detected in u16 decrement";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint32_t>)
    {
        return "Underflow detected in u32 decrement";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint64_t>)
    {
        return "Underflow detected in u64 decrement";
    }
    else
    {
        return "Underflow detected in u128 decrement";
    }
}

template <fundamental_unsigned_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto left_shift_overflow_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::uint8_t>)
    {
        return "Left shift past the end of u8 type width";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint16_t>)
    {
        return "Left shift past the end of u16 type width";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint32_t>)
    {
        return "Left shift past the end of u32 type width";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint64_t>)
    {
        return "Left shift past the end of u64 type width";
    }
    else
    {
        return "Left shift past the end of u128 type width";
    }
}

template <fundamental_unsigned_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto right_shift_overflow_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::uint8_t>)
    {
        return "Right shift past the end of u8 type width";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint16_t>)
    {
        return "Right shift past the end of u16 type width";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint32_t>)
    {
        return "Right shift past the end of u32 type width";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint64_t>)
    {
        return "Right shift past the end of u64 type width";
    }
    else
    {
        return "Right shift past the end of u128 type width";
    }
}

template <fundamental_unsigned_integral BasisType, fundamental_unsigned_integral OtherBasis>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto overflow_conversion_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, std::uint16_t> && std::is_same_v<OtherBasis, std::uint8_t>)
    {
        return "Overflow in u16 to u8 conversion";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint32_t> && std::is_same_v<OtherBasis, std::uint8_t>)
    {
        return "Overflow in u32 to u8 conversion";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint32_t> && std::is_same_v<OtherBasis, std::uint16_t>)
    {
        return "Overflow in u32 to u16 conversion";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint64_t> && std::is_same_v<OtherBasis, std::uint8_t>)
    {
        return "Overflow in u64 to u8 conversion";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint64_t> && std::is_same_v<OtherBasis, std::uint16_t>)
    {
        return "Overflow in u64 to u16 conversion";
    }
    else if constexpr (std::is_same_v<BasisType, std::uint64_t> && std::is_same_v<OtherBasis, std::uint32_t>)
    {
        return "Overflow in u64 to u32 conversion";
    }
    else if constexpr (std::is_same_v<BasisType, int128::uint128_t> && std::is_same_v<OtherBasis, std::uint8_t>)
    {
        return "Overflow in u128 to u8 conversion";
    }
    else if constexpr (std::is_same_v<BasisType, int128::uint128_t> && std::is_same_v<OtherBasis, std::uint16_t>)
    {
        return "Overflow in u128 to u16 conversion";
    }
    else if constexpr (std::is_same_v<BasisType, int128::uint128_t> && std::is_same_v<OtherBasis, std::uint32_t>)
    {
        return "Overflow in u128 to u32 conversion";
    }
    else if constexpr (std::is_same_v<BasisType, int128::uint128_t> && std::is_same_v<OtherBasis, std::uint64_t>)
    {
        return "Overflow in u128 to u64 conversion";
    }
    else
    {
        return "Overflow in unsigned integer conversion";
    }
}

template <fundamental_unsigned_integral BasisType>
template <fundamental_unsigned_integral OtherBasis>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr unsigned_integer_basis<BasisType>::operator OtherBasis() const
{
    if constexpr (sizeof(OtherBasis) < sizeof(BasisType))
    {
        if (basis_ > static_cast<BasisType>(std::numeric_limits<OtherBasis>::max()))
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, (overflow_conversion_msg<BasisType, OtherBasis>()));
        }
    }

    return static_cast<OtherBasis>(basis_);
}

// ------------------------------
// Addition
// ------------------------------

namespace impl {

#if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_add_overflow) && !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))

template <std::unsigned_integral T>
bool unsigned_intrin_add(const T lhs, const T rhs, T& result)
{
    return __builtin_add_overflow(lhs, rhs, &result);
}

#elif defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) && !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))

template <std::unsigned_integral T>
bool unsigned_intrin_add(const T lhs, const T rhs, T& result)
{
    if constexpr (std::is_same_v<T, std::uint8_t>)
    {
        return _addcarry_u8(0, lhs, rhs, &result);
    }
    else if constexpr (std::is_same_v<T, std::uint16_t>)
    {
        return _addcarry_u16(0, lhs, rhs, &result);
    }
    else if constexpr (std::is_same_v<T, std::uint32_t>)
    {
        return _addcarry_u32(0, lhs, rhs, &result);
    }
    else if constexpr (std::is_same_v<T, std::uint64_t>)
    {
        return _addcarry_u64(0, lhs, rhs, &result);
    }
}

#elif defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN) && !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))

template <std::unsigned_integral T>
bool unsigned_intrin_add(const T lhs, const T rhs, T& result)
{
    if constexpr (std::is_same_v<T, std::uint8_t>)
    {
        return _addcarry_u8(0, lhs, rhs, &result);
    }
    else if constexpr (std::is_same_v<T, std::uint16_t>)
    {
        return _addcarry_u16(0, lhs, rhs, &result);
    }
    else if constexpr (std::is_same_v<T, std::uint32_t>)
    {
        return _addcarry_u32(0, lhs, rhs, &result);
    }
    else
    {
        // x86 windows does not provide _addcarry_u64 so fall back to normal impl
        result = lhs + rhs;
        return result < lhs;
    }
}

#endif

template <std::unsigned_integral T>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr bool unsigned_no_intrin_add(const T lhs, const T rhs, T& result) noexcept
{
    if constexpr (std::is_same_v<T, std::uint8_t> || std::is_same_v<T, std::uint16_t>)
    {
        result = static_cast<T>(static_cast<std::uint32_t>(lhs + rhs));
    }
    else
    {
        result = static_cast<T>(lhs + rhs);
    }

    return result < lhs;
}

BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr bool unsigned_no_intrin_add(const int128::uint128_t& lhs, const int128::uint128_t& rhs, int128::uint128_t& result) noexcept
{
    result = lhs + rhs;
    return result < lhs;
}

} // namespace impl

// Primary template for non-tuple policies
template <overflow_policy Policy, fundamental_unsigned_integral BasisType>
struct add_helper
{
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType> lhs,
                                              const unsigned_integer_basis<BasisType> rhs)
        noexcept(Policy != overflow_policy::throw_exception)
        -> unsigned_integer_basis<BasisType>
    {
        using result_type = unsigned_integer_basis<BasisType>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType res {};

        auto handle_overflow = [&res]
        {
            #if !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))
            if (std::is_constant_evaluated())
            {
                if constexpr (std::is_same_v<BasisType, std::uint8_t>)
                {
                    throw std::overflow_error("Overflow detected in u8 addition");
                }
                else if constexpr (std::is_same_v<BasisType, std::uint16_t>)
                {
                    throw std::overflow_error("Overflow detected in u16 addition");
                }
                else if constexpr (std::is_same_v<BasisType, std::uint32_t>)
                {
                    throw std::overflow_error("Overflow detected in u32 addition");
                }
                else if constexpr (std::is_same_v<BasisType, std::uint64_t>)
                {
                    throw std::overflow_error("Overflow detected in u64 addition");
                }
                else
                {
                    throw std::overflow_error("Overflow detected in u128 addition");
                }
            }
            else
            #endif
            {
                if constexpr (Policy == overflow_policy::throw_exception)
                {
                    static_cast<void>(res);
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, overflow_add_msg<BasisType>());
                }
                else if constexpr (Policy == overflow_policy::saturate)
                {
                    res = std::numeric_limits<BasisType>::max();
                }
                else if constexpr (Policy == overflow_policy::strict)
                {
                    static_cast<void>(res);
                    std::exit(EXIT_FAILURE);
                }
                else
                {
                    static_cast<void>(res);
                    BOOST_SAFE_NUMBERS_UNREACHABLE;
                }
            }
        };

        #if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_add_overflow) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

        if constexpr (!std::is_same_v<BasisType, int128::uint128_t>)
        {
            #if !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))

            if (!std::is_constant_evaluated())
            {
                if (impl::unsigned_intrin_add(lhs_basis, rhs_basis, res))
                {
                    handle_overflow();
                }

                return result_type{res};
            }

            #endif
        }

        #endif // BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_add_overflow) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

        if (impl::unsigned_no_intrin_add(lhs_basis, rhs_basis, res))
        {
            handle_overflow();
        }

        return result_type{res};
    }
};

// Partial specialization for overflow_tuple policy
template <fundamental_unsigned_integral BasisType>
struct add_helper<overflow_policy::overflow_tuple, BasisType>
{
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType> lhs,
                                              const unsigned_integer_basis<BasisType> rhs) noexcept
        -> std::pair<unsigned_integer_basis<BasisType>, bool>
    {
        using result_type = unsigned_integer_basis<BasisType>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType res {};

        if constexpr (!std::is_same_v<BasisType, int128::uint128_t>)
        {
            #if (BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_add_overflow) || BOOST_SAFE_NUMBERS_HAS_BUILTIN(_addcarry_u64) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)) && !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))

            if (!std::is_constant_evaluated())
            {
                const auto overflowed {impl::unsigned_intrin_add(lhs_basis, rhs_basis, res)};
                return std::make_pair(result_type{res}, overflowed);
            }

            #endif
        }

        const auto overflowed {impl::unsigned_no_intrin_add(lhs_basis, rhs_basis, res)};
        return std::make_pair(result_type{res}, overflowed);
    }
};

// Partial specialization for overflow_tuple policy
template <fundamental_unsigned_integral BasisType>
struct add_helper<overflow_policy::checked, BasisType>
{
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType> lhs,
                                              const unsigned_integer_basis<BasisType> rhs) noexcept
        -> std::optional<unsigned_integer_basis<BasisType>>
    {
        using result_type = unsigned_integer_basis<BasisType>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType res {};

        if constexpr (!std::is_same_v<BasisType, int128::uint128_t>)
        {
            #if (BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_add_overflow) || BOOST_SAFE_NUMBERS_HAS_BUILTIN(_addcarry_u64) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)) && !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))

            if (!std::is_constant_evaluated())
            {
                const auto overflowed {impl::unsigned_intrin_add(lhs_basis, rhs_basis, res)};
                return overflowed ? std::nullopt : std::make_optional(result_type{res});
            }

            #endif
        }

        const auto overflowed {impl::unsigned_no_intrin_add(lhs_basis, rhs_basis, res)};
        return overflowed ? std::nullopt : std::make_optional(result_type{res});
    }
};

// Partial specialization for widening policy
template <fundamental_unsigned_integral BasisType>
struct add_helper<overflow_policy::widen, BasisType>
{
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType> lhs,
                                              const unsigned_integer_basis<BasisType> rhs) noexcept
    {
        using promoted_type = promoted_type<BasisType>;
        static_assert(!std::is_same_v<promoted_type, bool>, "Widening policy with uint128_t is not supported");

        using result_type = unsigned_integer_basis<promoted_type>;
        return result_type{static_cast<promoted_type>(static_cast<promoted_type>(lhs) + rhs)};
    }
};

template <overflow_policy Policy, fundamental_unsigned_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto add_impl(const unsigned_integer_basis<BasisType> lhs,
                                      const unsigned_integer_basis<BasisType> rhs)
    noexcept(Policy == overflow_policy::saturate || Policy == overflow_policy::overflow_tuple || Policy == overflow_policy::checked || Policy == overflow_policy::strict || Policy == overflow_policy::widen)
{
    return add_helper<Policy, BasisType>::apply(lhs, rhs);
}

template <fundamental_unsigned_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto operator+(const unsigned_integer_basis<BasisType> lhs,
                                       const unsigned_integer_basis<BasisType> rhs) -> unsigned_integer_basis<BasisType>
{
    // Here we do repeat some logic in the above add_impls
    // The reason for this is to significantly improve constexpr error messages.
    // We go from this:
    /*
    clang-darwin.compile.c++ ../../../bin.v2/libs/safe_numbers/test/compile_fail_basic_usage_constexpr.test/clang-darwin-21/debug/arm_64/cxxstd-20-iso/threading-multi/visibility-hidden/compile_fail_basic_usage_constexpr.o
    ../examples/compile_fail_basic_usage_constexpr.cpp:18:22: error: constexpr variable 'z' must be initialized by a constant expression
    18 |         constexpr u8 z {x + y};
    |                      ^ ~~~~~~~
    ../../../boost/safe_numbers/detail/unsigned_integer_basis.hpp:228:21: note: subexpression not valid in a constant expression
    228 |                     throw std::overflow_error("Overflow detected in u8 addition");
    |                     ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ../../../boost/safe_numbers/detail/unsigned_integer_basis.hpp:292:13: note: in call to 'handle_overflow.operator()()'
    292 |             handle_overflow();
    |             ^~~~~~~~~~~~~~~~~
    ../../../boost/safe_numbers/detail/unsigned_integer_basis.hpp:383:12: note: in call to 'apply({255}, {2})'
    383 |     return add_helper<Policy, BasisType>::apply(lhs, rhs);
    |            ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ../../../boost/safe_numbers/detail/unsigned_integer_basis.hpp:390:12: note: in call to 'add_impl<boost::safe_numbers::overflow_policy::throw_exception, unsigned char>({255}, {2})'
    390 |     return add_impl<overflow_policy::throw_exception>(lhs, rhs);
    |            ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ../examples/compile_fail_basic_usage_constexpr.cpp:18:25: note: in call to 'operator+<unsigned char>({255}, {2})'
    18 |         constexpr u8 z {x + y};
    |                         ^~~~~
    1 error generated.

    to this:

    clang-darwin.compile.c++ ../../../bin.v2/libs/safe_numbers/test/compile_fail_basic_usage_constexpr.test/clang-darwin-21/debug/arm_64/cxxstd-20-iso/threading-multi/visibility-hidden/compile_fail_basic_usage_constexpr.o
    ../examples/compile_fail_basic_usage_constexpr.cpp:18:22: error: constexpr variable 'z' must be initialized by a constant expression
    18 |         constexpr u8 z {x + y};
    |                      ^ ~~~~~~~
    ../../../boost/safe_numbers/detail/unsigned_integer_basis.hpp:397:17: note: subexpression not valid in a constant expression
    397 |                 throw std::overflow_error("Overflow detected in u8 addition");
    |                 ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ../examples/compile_fail_basic_usage_constexpr.cpp:18:25: note: in call to 'operator+<unsigned char>({255}, {2})'
    18 |         constexpr u8 z {x + y};
    |                         ^~~~~
    1 error generated.
    */

    #if !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))

    if (std::is_constant_evaluated())
    {
        BasisType res {};
        if (impl::unsigned_no_intrin_add(static_cast<BasisType>(lhs), static_cast<BasisType>(rhs), res))
        {
            if constexpr (std::is_same_v<BasisType, std::uint8_t>)
            {
                throw std::overflow_error("Overflow detected in u8 addition");
            }
            else if constexpr (std::is_same_v<BasisType, std::uint16_t>)
            {
                throw std::overflow_error("Overflow detected in u16 addition");
            }
            else if constexpr (std::is_same_v<BasisType, std::uint32_t>)
            {
                throw std::overflow_error("Overflow detected in u32 addition");
            }
            else if constexpr (std::is_same_v<BasisType, std::uint64_t>)
            {
                throw std::overflow_error("Overflow detected in u64 addition");
            }
            else
            {
                throw std::overflow_error("Overflow detected in u128 addition");
            }
        }

        return unsigned_integer_basis<BasisType>{res};
    }

    #endif

    return add_helper<overflow_policy::throw_exception, BasisType>::apply(lhs, rhs);
}

} // namespace boost::safe_numbers::detail

#define BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP(OP_NAME, OP_SYMBOL)                                                                                \
template <boost::safe_numbers::detail::fundamental_unsigned_integral LHSBasis,                                                                                  \
          boost::safe_numbers::detail::fundamental_unsigned_integral RHSBasis>                                                                                  \
    requires (!std::is_same_v<LHSBasis, RHSBasis>)                                                                                                          \
BOOST_SAFE_NUMBERS_HOST_DEVICE                                                                                                                              \
constexpr auto OP_SYMBOL(const boost::safe_numbers::detail::unsigned_integer_basis<LHSBasis>,                                                                  \
                         const boost::safe_numbers::detail::unsigned_integer_basis<RHSBasis>)                                                                  \
{                                                                                                                                                              \
    if constexpr (std::is_same_v<LHSBasis, std::uint8_t>)                                                                                                     \
    {                                                                                                                                                          \
        if constexpr (std::is_same_v<RHSBasis, std::uint16_t>)                                                                                                \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between u8 and u16");                 \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, std::uint32_t>)                                                                                           \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between u8 and u32");                 \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, std::uint64_t>)                                                                                           \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between u8 and u64");                 \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, boost::int128::uint128_t>)                                                                                \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between u8 and u128");                \
        }                                                                                                                                                      \
        else                                                                                                                                                   \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between u8 and unknown type");        \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
    else if constexpr (std::is_same_v<LHSBasis, std::uint16_t>)                                                                                               \
    {                                                                                                                                                          \
        if constexpr (std::is_same_v<RHSBasis, std::uint8_t>)                                                                                                 \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between u16 and u8");                 \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, std::uint32_t>)                                                                                           \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between u16 and u32");                \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, std::uint64_t>)                                                                                           \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between u16 and u64");                \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, boost::int128::uint128_t>)                                                                                \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between u16 and u128");               \
        }                                                                                                                                                      \
        else                                                                                                                                                   \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between u16 and unknown type");       \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
    else if constexpr (std::is_same_v<LHSBasis, std::uint32_t>)                                                                                               \
    {                                                                                                                                                          \
        if constexpr (std::is_same_v<RHSBasis, std::uint8_t>)                                                                                                 \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between u32 and u8");                 \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, std::uint16_t>)                                                                                           \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between u32 and u16");                \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, std::uint64_t>)                                                                                           \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between u32 and u64");                \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, boost::int128::uint128_t>)                                                                                \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between u32 and u128");               \
        }                                                                                                                                                      \
        else                                                                                                                                                   \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between u32 and unknown type");       \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
    else if constexpr (std::is_same_v<LHSBasis, std::uint64_t>)                                                                                               \
    {                                                                                                                                                          \
        if constexpr (std::is_same_v<RHSBasis, std::uint8_t>)                                                                                                 \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between u64 and u8");                 \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, std::uint16_t>)                                                                                           \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between u64 and u16");                \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, std::uint32_t>)                                                                                           \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between u64 and u32");                \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, boost::int128::uint128_t>)                                                                                \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between u64 and u128");               \
        }                                                                                                                                                      \
        else                                                                                                                                                   \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between u64 and unknown type");       \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
    else if constexpr (std::is_same_v<LHSBasis, boost::int128::uint128_t>)                                                                                    \
    {                                                                                                                                                          \
        if constexpr (std::is_same_v<RHSBasis, std::uint8_t>)                                                                                                 \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between u128 and u8");                \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, std::uint16_t>)                                                                                           \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between u128 and u16");               \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, std::uint32_t>)                                                                                           \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between u128 and u32");               \
        }                                                                                                                                                      \
        else if constexpr (std::is_same_v<RHSBasis, std::uint64_t>)                                                                                           \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between u128 and u64");               \
        }                                                                                                                                                      \
        else                                                                                                                                                   \
        {                                                                                                                                                      \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between u128 and unknown type");      \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
    else                                                                                                                                                       \
    {                                                                                                                                                          \
        static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " on mixed width unsigned integer types");  \
    }                                                                                                                                                          \
                                                                                                                                                               \
    return boost::safe_numbers::detail::unsigned_integer_basis<LHSBasis>(0);                                                                                   \
}

namespace boost::safe_numbers::detail {

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("comparison", operator<=>)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("equality", operator==)

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("addition", operator+)

template <fundamental_unsigned_integral BasisType>
template <fundamental_unsigned_integral OtherBasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr auto unsigned_integer_basis<BasisType>::operator+=(const unsigned_integer_basis<OtherBasisType> rhs)
    -> unsigned_integer_basis&
{
    *this = *this + rhs;
    return *this;
}

// ------------------------------
// Subtraction
// ------------------------------

namespace impl {

#if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_sub_overflow) && !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))

template <std::unsigned_integral T>
bool unsigned_intrin_sub(T lhs, T rhs, T& result)
{
    return __builtin_sub_overflow(lhs, rhs, &result);
}

#elif defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) && !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))

template <std::unsigned_integral T>
bool unsigned_intrin_sub(T lhs, T rhs, T& result)
{
    if constexpr (std::is_same_v<T, std::uint8_t>)
    {
        return _subborrow_u8(0, lhs, rhs, &result);
    }
    else if constexpr (std::is_same_v<T, std::uint16_t>)
    {
        return _subborrow_u16(0, lhs, rhs, &result);
    }
    else if constexpr (std::is_same_v<T, std::uint32_t>)
    {
        return _subborrow_u32(0, lhs, rhs, &result);
    }
    else if constexpr (std::is_same_v<T, std::uint64_t>)
    {
        return _subborrow_u64(0, lhs, rhs, &result);
    }
}

#elif defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN) && !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))

template <std::unsigned_integral T>
bool unsigned_intrin_sub(T lhs, T rhs, T& result)
{
    if constexpr (std::is_same_v<T, std::uint8_t>)
    {
        return _subborrow_u8(0, lhs, rhs, &result);
    }
    else if constexpr (std::is_same_v<T, std::uint16_t>)
    {
        return _subborrow_u16(0, lhs, rhs, &result);
    }
    else if constexpr (std::is_same_v<T, std::uint32_t>)
    {
        return _subborrow_u32(0, lhs, rhs, &result);
    }
    else
    {
        // x86 windows does not provide _subborrow_u64 so fall back to normal impl
        result = lhs - rhs;
        return result > lhs;
    }
}

#endif

template <std::unsigned_integral T>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr bool unsigned_no_intrin_sub(const T lhs, const T rhs, T& result) noexcept
{
    if constexpr (std::is_same_v<T, std::uint8_t> || std::is_same_v<T, std::uint16_t>)
    {
        result = static_cast<T>(static_cast<std::uint32_t>(lhs - rhs));
    }
    else
    {
        result = static_cast<T>(lhs - rhs);
    }

    return result > lhs;
}

BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr bool unsigned_no_intrin_sub(const int128::uint128_t& lhs, const int128::uint128_t& rhs, int128::uint128_t& result) noexcept
{
    result = lhs - rhs;
    return result > lhs;
}

} // namespace impl

// Primary template for non-tuple policies
template <overflow_policy Policy, fundamental_unsigned_integral BasisType>
struct sub_helper
{
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType> lhs,
                                              const unsigned_integer_basis<BasisType> rhs)
        noexcept(Policy != overflow_policy::throw_exception)
        -> unsigned_integer_basis<BasisType>
    {
        using result_type = unsigned_integer_basis<BasisType>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType res {};

        auto handle_underflow = [&res]
        {
            #if !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))

            if (std::is_constant_evaluated())
            {
                if constexpr (std::is_same_v<BasisType, std::uint8_t>)
                {
                    throw std::underflow_error("Underflow detected in u8 subtraction");
                }
                else if constexpr (std::is_same_v<BasisType, std::uint16_t>)
                {
                    throw std::underflow_error("Underflow detected in u16 subtraction");
                }
                else if constexpr (std::is_same_v<BasisType, std::uint32_t>)
                {
                    throw std::underflow_error("Underflow detected in u32 subtraction");
                }
                else if constexpr (std::is_same_v<BasisType, std::uint64_t>)
                {
                    throw std::underflow_error("Underflow detected in u64 subtraction");
                }
                else
                {
                    throw std::underflow_error("Underflow detected in u128 subtraction");
                }
            }
            else
            #endif
            {
                if constexpr (Policy == overflow_policy::throw_exception)
                {
                    static_cast<void>(res);
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, underflow_sub_msg<BasisType>());
                }
                else if constexpr (Policy == overflow_policy::saturate)
                {
                    res = std::numeric_limits<BasisType>::min();
                }
                else if constexpr (Policy == overflow_policy::strict)
                {
                    static_cast<void>(res);
                    std::exit(EXIT_FAILURE);
                }
                else
                {
                    static_cast<void>(res);
                    BOOST_SAFE_NUMBERS_UNREACHABLE;
                }
            }
        };

        if constexpr (!std::is_same_v<BasisType, int128::uint128_t>)
        {
            #if (BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_sub_overflow) || BOOST_SAFE_NUMBERS_HAS_BUILTIN(_subborrow_u64) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)) && !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))

            if (!std::is_constant_evaluated())
            {
                if (impl::unsigned_intrin_sub(lhs_basis, rhs_basis, res))
                {
                    handle_underflow();
                }

                return result_type{res};
            }

            #endif
        }

        if (impl::unsigned_no_intrin_sub(lhs_basis, rhs_basis, res))
        {
            handle_underflow();
        }

        return result_type{res};
    }
};

// Partial specialization for overflow_tuple policy
template <fundamental_unsigned_integral BasisType>
struct sub_helper<overflow_policy::overflow_tuple, BasisType>
{
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType> lhs,
                                              const unsigned_integer_basis<BasisType> rhs) noexcept
        -> std::pair<unsigned_integer_basis<BasisType>, bool>
    {
        using result_type = unsigned_integer_basis<BasisType>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType res {};

        if constexpr (!std::is_same_v<BasisType, int128::uint128_t>)
        {
            #if (BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_sub_overflow) || BOOST_SAFE_NUMBERS_HAS_BUILTIN(_subborrow_u64) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)) && !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))

            if (!std::is_constant_evaluated())
            {
                const auto underflowed {impl::unsigned_intrin_sub(lhs_basis, rhs_basis, res)};
                return std::make_pair(result_type{res}, underflowed);
            }

            #endif
        }

        const auto underflowed {impl::unsigned_no_intrin_sub(lhs_basis, rhs_basis, res)};
        return std::make_pair(result_type{res}, underflowed);
    }
};

// Partial specialization for checked policy
template <fundamental_unsigned_integral BasisType>
struct sub_helper<overflow_policy::checked, BasisType>
{
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType> lhs,
                                              const unsigned_integer_basis<BasisType> rhs) noexcept
        -> std::optional<unsigned_integer_basis<BasisType>>
    {
        using result_type = unsigned_integer_basis<BasisType>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType res {};

        if constexpr (!std::is_same_v<BasisType, int128::uint128_t>)
        {
            #if (BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_sub_overflow) || BOOST_SAFE_NUMBERS_HAS_BUILTIN(_subborrow_u64) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)) && !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))

            if (!std::is_constant_evaluated())
            {
                const auto underflowed {impl::unsigned_intrin_sub(lhs_basis, rhs_basis, res)};
                return underflowed ? std::nullopt : std::make_optional(result_type{res});
            }

            #endif
        }

        const auto underflowed {impl::unsigned_no_intrin_sub(lhs_basis, rhs_basis, res)};
        return underflowed ? std::nullopt : std::make_optional(result_type{res});
    }
};

template <overflow_policy Policy, fundamental_unsigned_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto sub_impl(const unsigned_integer_basis<BasisType> lhs,
                                      const unsigned_integer_basis<BasisType> rhs)
    noexcept(Policy == overflow_policy::saturate || Policy == overflow_policy::overflow_tuple || Policy == overflow_policy::checked || Policy == overflow_policy::strict)
{
    return sub_helper<Policy, BasisType>::apply(lhs, rhs);
}

template <fundamental_unsigned_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto operator-(const unsigned_integer_basis<BasisType> lhs,
                                       const unsigned_integer_basis<BasisType> rhs) -> unsigned_integer_basis<BasisType>
{
    #if !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))

    if (std::is_constant_evaluated())
    {
        BasisType res {};
        if (impl::unsigned_no_intrin_sub(static_cast<BasisType>(lhs), static_cast<BasisType>(rhs), res))
        {
            if constexpr (std::is_same_v<BasisType, std::uint8_t>)
            {
                throw std::underflow_error("Underflow detected in u8 subtraction");
            }
            else if constexpr (std::is_same_v<BasisType, std::uint16_t>)
            {
                throw std::underflow_error("Underflow detected in u16 subtraction");
            }
            else if constexpr (std::is_same_v<BasisType, std::uint32_t>)
            {
                throw std::underflow_error("Underflow detected in u32 subtraction");
            }
            else if constexpr (std::is_same_v<BasisType, std::uint64_t>)
            {
                throw std::underflow_error("Underflow detected in u64 subtraction");
            }
            else
            {
                throw std::underflow_error("Underflow detected in u128 subtraction");
            }
        }

        return unsigned_integer_basis<BasisType>{res};
    }

    #endif

    return sub_helper<overflow_policy::throw_exception, BasisType>::apply(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("subtraction", operator-)

template <fundamental_unsigned_integral BasisType>
template <fundamental_unsigned_integral OtherBasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr auto unsigned_integer_basis<BasisType>::operator-=(const unsigned_integer_basis<OtherBasisType> rhs)
    -> unsigned_integer_basis&
{
    *this = *this - rhs;
    return *this;
}

// ------------------------------
// Multiplication
// ------------------------------

namespace impl {

#if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_mul_overflow) && !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))

template <std::unsigned_integral T>
bool unsigned_intrin_mul(const T lhs, const T rhs, T& result)
{
    return __builtin_mul_overflow(lhs, rhs, &result);
}

#elif defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) && !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))

template <std::unsigned_integral T>
bool unsigned_intrin_mul(const T lhs, const T rhs, T& result)
{
    if constexpr (std::is_same_v<T, std::uint64_t>)
    {
        std::uint64_t high;
        result = _umul128(lhs, rhs, &high);
        return high != 0U;
    }
    else
    {
        static_assert(sizeof(T) < sizeof(std::uint64_t));

        using promoted_type = std::conditional_t<std::is_same_v<T, std::uint8_t>, std::uint_fast16_t,
                                 std::conditional_t<std::is_same_v<T, std::uint16_t>, std::uint_fast32_t, std::uint_fast64_t>>;

        const auto temp {static_cast<promoted_type>(lhs) * rhs};
        result = static_cast<T>(temp);
        return temp > static_cast<promoted_type>(std::numeric_limits<T>::max());
    }
}

#elif defined(BOOST_SAFENUMBERS_HAS_WINDOWS_ARM64_INTRIN) && !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))

template <std::unsigned_integral T>
bool unsigned_intrin_mul(const T lhs, const T rhs, T& result)
{
    if constexpr (std::is_same_v<T, std::uint64_t>)
    {
        result = lhs * rhs;
        return __umulh(lhs, rhs) != 0U;
    }
    else
    {
        static_assert(sizeof(T) < sizeof(std::uint64_t));

        using promoted_type = std::conditional_t<std::is_same_v<T, std::uint8_t>, std::uint_fast16_t,
                                 std::conditional_t<std::is_same_v<T, std::uint16_t>, std::uint_fast32_t, std::uint_fast64_t>>;

        const auto temp {static_cast<promoted_type>(lhs) * rhs};
        result = static_cast<T>(temp);
        return temp > static_cast<promoted_type>(std::numeric_limits<T>::max());
    }
}

#endif

template <std::unsigned_integral T>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr bool no_intrin_mul(const T lhs, const T rhs, T& result)
{
    using promoted_type = std::conditional_t<std::is_same_v<T, std::uint8_t>, std::uint_fast16_t,
                             std::conditional_t<std::is_same_v<T, std::uint16_t>, std::uint_fast32_t,
                                std::conditional_t<std::is_same_v<T, std::uint32_t>, std::uint_fast64_t, int128::uint128_t>>>;

    const auto temp {static_cast<promoted_type>(lhs) * rhs};
    result = static_cast<T>(temp);
    return temp > static_cast<promoted_type>(std::numeric_limits<T>::max());
}

BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr bool no_intrin_mul(const int128::uint128_t& lhs, const int128::uint128_t& rhs, int128::uint128_t& result) noexcept
{
    result = lhs * rhs;
    return rhs != 0U && lhs > (std::numeric_limits<int128::uint128_t>::max() / rhs);
}

} // namespace impl

// Primary template for non-tuple policies
template <overflow_policy Policy, fundamental_unsigned_integral BasisType>
struct mul_helper
{
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType> lhs,
                                              const unsigned_integer_basis<BasisType> rhs)
        noexcept(Policy == overflow_policy::saturate || Policy == overflow_policy::strict)
        -> unsigned_integer_basis<BasisType>
    {
        using result_type = unsigned_integer_basis<BasisType>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType res {};

        auto handle_overflow = [&res]
        {
            #if !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))
            if (std::is_constant_evaluated())
            {
                if constexpr (std::is_same_v<BasisType, std::uint8_t>)
                {
                    throw std::overflow_error("Overflow detected in u8 multiplication");
                }
                else if constexpr (std::is_same_v<BasisType, std::uint16_t>)
                {
                    throw std::overflow_error("Overflow detected in u16 multiplication");
                }
                else if constexpr (std::is_same_v<BasisType, std::uint32_t>)
                {
                    throw std::overflow_error("Overflow detected in u32 multiplication");
                }
                else if constexpr (std::is_same_v<BasisType, std::uint64_t>)
                {
                    throw std::overflow_error("Overflow detected in u64 multiplication");
                }
                else
                {
                    throw std::overflow_error("Overflow detected in u128 multiplication");
                }
            }
            else
            #endif
            {
                if constexpr (Policy == overflow_policy::throw_exception)
                {
                    static_cast<void>(res);
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, overflow_mul_msg<BasisType>());
                }
                else if constexpr (Policy == overflow_policy::saturate)
                {
                    res = std::numeric_limits<BasisType>::max();
                }
                else if constexpr (Policy == overflow_policy::strict)
                {
                    static_cast<void>(res);
                    std::exit(EXIT_FAILURE);
                }
                else
                {
                    static_cast<void>(res);
                    BOOST_SAFE_NUMBERS_UNREACHABLE;
                }
            }
        };

        if constexpr (!std::is_same_v<BasisType, int128::uint128_t>)
        {
            #if (BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_mul_overflow) || BOOST_SAFE_NUMBERS_HAS_BUILTIN(_umul128)) && !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))

            if (!std::is_constant_evaluated())
            {
                if (impl::unsigned_intrin_mul(lhs_basis, rhs_basis, res))
                {
                    handle_overflow();
                }

                return result_type{res};
            }

            #endif
        }

        if (impl::no_intrin_mul(lhs_basis, rhs_basis, res))
        {
            handle_overflow();
        }

        return result_type{res};
    }
};

// Partial specialization for overflow_tuple policy
template <fundamental_unsigned_integral BasisType>
struct mul_helper<overflow_policy::overflow_tuple, BasisType>
{
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType> lhs,
                                              const unsigned_integer_basis<BasisType> rhs) noexcept
        -> std::pair<unsigned_integer_basis<BasisType>, bool>
    {
        using result_type = unsigned_integer_basis<BasisType>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType res {};

        if constexpr (!std::is_same_v<BasisType, int128::uint128_t>)
        {
            #if (BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_mul_overflow) || BOOST_SAFE_NUMBERS_HAS_BUILTIN(_umul128)) && !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))

            if (!std::is_constant_evaluated())
            {
                const auto overflowed {impl::unsigned_intrin_mul(lhs_basis, rhs_basis, res)};
                return std::make_pair(result_type{res}, overflowed);
            }

            #endif
        }

        const auto overflowed {impl::no_intrin_mul(lhs_basis, rhs_basis, res)};
        return std::make_pair(result_type{res}, overflowed);
    }
};

// Partial specialization for checked policy
template <fundamental_unsigned_integral BasisType>
struct mul_helper<overflow_policy::checked, BasisType>
{
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType> lhs,
                                              const unsigned_integer_basis<BasisType> rhs) noexcept
        -> std::optional<unsigned_integer_basis<BasisType>>
    {
        using result_type = unsigned_integer_basis<BasisType>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType res {};

        if constexpr (!std::is_same_v<BasisType, int128::uint128_t>)
        {
            #if (BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_mul_overflow) || BOOST_SAFE_NUMBERS_HAS_BUILTIN(_umul128)) && !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))

            if (!std::is_constant_evaluated())
            {
                const auto overflowed {impl::unsigned_intrin_mul(lhs_basis, rhs_basis, res)};
                return overflowed ? std::nullopt : std::make_optional(result_type{res});
            }

            #endif
        }

        const auto overflowed {impl::no_intrin_mul(lhs_basis, rhs_basis, res)};
        return overflowed ? std::nullopt : std::make_optional(result_type{res});
    }
};

// Partial specialization for widening policy
template <fundamental_unsigned_integral BasisType>
struct mul_helper<overflow_policy::widen, BasisType>
{
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType> lhs,
                                              const unsigned_integer_basis<BasisType> rhs) noexcept
    {
        using promoted_type = promoted_type<BasisType>;
        static_assert(!std::is_same_v<promoted_type, bool>, "Widening policy with uint128_t is not supported");

        using result_type = unsigned_integer_basis<promoted_type>;
        return result_type{static_cast<promoted_type>(static_cast<promoted_type>(lhs) * rhs)};
    }
};

template <overflow_policy Policy, fundamental_unsigned_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto mul_impl(const unsigned_integer_basis<BasisType> lhs,
                                      const unsigned_integer_basis<BasisType> rhs)
    noexcept(Policy == overflow_policy::saturate || Policy == overflow_policy::overflow_tuple || Policy == overflow_policy::checked || Policy == overflow_policy::strict || Policy == overflow_policy::widen)
{
    return mul_helper<Policy, BasisType>::apply(lhs, rhs);
}

template <fundamental_unsigned_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto operator*(const unsigned_integer_basis<BasisType> lhs,
                                       const unsigned_integer_basis<BasisType> rhs) -> unsigned_integer_basis<BasisType>
{
    #if !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))

    if (std::is_constant_evaluated())
    {
        BasisType res {};
        if (impl::no_intrin_mul(static_cast<BasisType>(lhs), static_cast<BasisType>(rhs), res))
        {
            if constexpr (std::is_same_v<BasisType, std::uint8_t>)
            {
                throw std::overflow_error("Overflow detected in u8 multiplication");
            }
            else if constexpr (std::is_same_v<BasisType, std::uint16_t>)
            {
                throw std::overflow_error("Overflow detected in u16 multiplication");
            }
            else if constexpr (std::is_same_v<BasisType, std::uint32_t>)
            {
                throw std::overflow_error("Overflow detected in u32 multiplication");
            }
            else if constexpr (std::is_same_v<BasisType, std::uint64_t>)
            {
                throw std::overflow_error("Overflow detected in u64 multiplication");
            }
            else
            {
                throw std::overflow_error("Overflow detected in u128 multiplication");
            }
        }

        return unsigned_integer_basis<BasisType>{res};
    }

    #endif

    return mul_helper<overflow_policy::throw_exception, BasisType>::apply(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("multiplication", operator*)

template <fundamental_unsigned_integral BasisType>
template <fundamental_unsigned_integral OtherBasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr auto unsigned_integer_basis<BasisType>::operator*=(const unsigned_integer_basis<OtherBasisType> rhs)
    -> unsigned_integer_basis&
{
    *this = *this * rhs;
    return *this;
}

// ------------------------------
// Division
// ------------------------------

// Primary template for non-tuple policies
template <overflow_policy Policy, fundamental_unsigned_integral BasisType>
struct div_helper
{
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType> lhs,
                                              const unsigned_integer_basis<BasisType> rhs)
        noexcept(Policy == overflow_policy::strict)
        -> unsigned_integer_basis<BasisType>
    {
        using result_type = unsigned_integer_basis<BasisType>;

        if (static_cast<BasisType>(rhs) == 0U) [[unlikely]]
        {
            if constexpr (Policy == overflow_policy::throw_exception)
            {
                BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, div_by_zero_msg<BasisType>());
            }
            else if constexpr (Policy == overflow_policy::saturate)
            {
                BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, div_by_zero_msg<BasisType>());
            }
            else if constexpr (Policy == overflow_policy::strict)
            {
                std::exit(EXIT_FAILURE);
            }
            else
            {
                BOOST_SAFE_NUMBERS_UNREACHABLE;
            }
        }

        if constexpr (std::is_same_v<BasisType, std::uint8_t> || std::is_same_v<BasisType, std::uint16_t>)
        {
            return result_type{static_cast<BasisType>(static_cast<BasisType>(lhs) / static_cast<BasisType>(rhs))};
        }
        else
        {
            return result_type{static_cast<BasisType>(lhs) / static_cast<BasisType>(rhs)};
        }
    }
};

// Partial specialization for overflow_tuple policy
template <fundamental_unsigned_integral BasisType>
struct div_helper<overflow_policy::overflow_tuple, BasisType>
{
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType> lhs,
                                              const unsigned_integer_basis<BasisType> rhs)
        -> std::pair<unsigned_integer_basis<BasisType>, bool>
    {
        using result_type = unsigned_integer_basis<BasisType>;

        const auto divisor {static_cast<BasisType>(rhs)};
        if (divisor == 0U) [[unlikely]]
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, div_by_zero_msg<BasisType>());
        }

        if constexpr (std::is_same_v<BasisType, std::uint8_t> || std::is_same_v<BasisType, std::uint16_t>)
        {
            return std::make_pair(result_type{static_cast<BasisType>(static_cast<BasisType>(lhs) / divisor)}, false);
        }
        else
        {
            return std::make_pair(result_type{static_cast<BasisType>(lhs) / divisor}, false);
        }
    }
};

// Partial specialization for checked policy
template <fundamental_unsigned_integral BasisType>
struct div_helper<overflow_policy::checked, BasisType>
{
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType> lhs,
                                              const unsigned_integer_basis<BasisType> rhs) noexcept
        -> std::optional<unsigned_integer_basis<BasisType>>
    {
        using result_type = unsigned_integer_basis<BasisType>;

        const auto divisor {static_cast<BasisType>(rhs)};
        if (divisor == 0U) [[unlikely]]
        {
            return std::nullopt;
        }

        if constexpr (std::is_same_v<BasisType, std::uint8_t> || std::is_same_v<BasisType, std::uint16_t>)
        {
            return std::make_optional(result_type{static_cast<BasisType>(static_cast<BasisType>(lhs) / divisor)});
        }
        else
        {
            return std::make_optional(result_type{static_cast<BasisType>(lhs) / divisor});
        }
    }
};

template <overflow_policy Policy, fundamental_unsigned_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto div_impl(const unsigned_integer_basis<BasisType> lhs,
                                      const unsigned_integer_basis<BasisType> rhs)
    noexcept(Policy == overflow_policy::checked || Policy == overflow_policy::strict)
{
    return div_helper<Policy, BasisType>::apply(lhs, rhs);
}

template <fundamental_unsigned_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto operator/(const unsigned_integer_basis<BasisType> lhs,
                                       const unsigned_integer_basis<BasisType> rhs) -> unsigned_integer_basis<BasisType>
{
    #if !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))

    if (std::is_constant_evaluated())
    {
        const auto divisor {static_cast<BasisType>(rhs)};
        if (divisor == 0U) [[unlikely]]
        {
            throw std::domain_error("Unsigned division by zero");
        }

        if constexpr (std::is_same_v<BasisType, std::uint8_t> || std::is_same_v<BasisType, std::uint16_t>)
        {
            return unsigned_integer_basis<BasisType>{static_cast<BasisType>(static_cast<BasisType>(lhs) / divisor)};
        }
        else
        {
            return unsigned_integer_basis<BasisType>{static_cast<BasisType>(lhs) / divisor};
        }
    }

    #endif

    return div_helper<overflow_policy::throw_exception, BasisType>::apply(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("division", operator/)

template <fundamental_unsigned_integral BasisType>
template <fundamental_unsigned_integral OtherBasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr auto unsigned_integer_basis<BasisType>::operator/=(const unsigned_integer_basis<OtherBasisType> rhs)
    -> unsigned_integer_basis&
{
    *this = *this / rhs;
    return *this;
}

// ------------------------------
// Modulo
// ------------------------------

// Primary template for non-tuple policies
template <overflow_policy Policy, fundamental_unsigned_integral BasisType>
struct mod_helper
{
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType> lhs,
                                              const unsigned_integer_basis<BasisType> rhs)
        noexcept(Policy == overflow_policy::strict)
        -> unsigned_integer_basis<BasisType>
    {
        using result_type = unsigned_integer_basis<BasisType>;

        if (static_cast<BasisType>(rhs) == 0U) [[unlikely]]
        {
            if constexpr (Policy == overflow_policy::throw_exception)
            {
                BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, mod_by_zero_msg<BasisType>());
            }
            else if constexpr (Policy == overflow_policy::saturate)
            {
                BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, mod_by_zero_msg<BasisType>());
            }
            else if constexpr (Policy == overflow_policy::strict)
            {
                std::exit(EXIT_FAILURE);
            }
            else
            {
                BOOST_SAFE_NUMBERS_UNREACHABLE;
            }
        }

        if constexpr (std::is_same_v<BasisType, std::uint8_t> || std::is_same_v<BasisType, std::uint16_t>)
        {
            return result_type{static_cast<BasisType>(static_cast<BasisType>(lhs) % static_cast<BasisType>(rhs))};
        }
        else
        {
            return result_type{static_cast<BasisType>(lhs) % static_cast<BasisType>(rhs)};
        }
    }
};

// Partial specialization for overflow_tuple policy
template <fundamental_unsigned_integral BasisType>
struct mod_helper<overflow_policy::overflow_tuple, BasisType>
{
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType> lhs,
                                              const unsigned_integer_basis<BasisType> rhs)
        -> std::pair<unsigned_integer_basis<BasisType>, bool>
    {
        using result_type = unsigned_integer_basis<BasisType>;

        const auto divisor {static_cast<BasisType>(rhs)};
        if (divisor == 0U) [[unlikely]]
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, div_by_zero_msg<BasisType>());
        }

        if constexpr (std::is_same_v<BasisType, std::uint8_t> || std::is_same_v<BasisType, std::uint16_t>)
        {
            return std::make_pair(result_type{static_cast<BasisType>(static_cast<BasisType>(lhs) % divisor)}, false);
        }
        else
        {
            return std::make_pair(result_type{static_cast<BasisType>(lhs) % divisor}, false);
        }
    }
};

// Partial specialization for checked policy
template <fundamental_unsigned_integral BasisType>
struct mod_helper<overflow_policy::checked, BasisType>
{
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType> lhs,
                                              const unsigned_integer_basis<BasisType> rhs) noexcept
        -> std::optional<unsigned_integer_basis<BasisType>>
    {
        using result_type = unsigned_integer_basis<BasisType>;

        const auto divisor {static_cast<BasisType>(rhs)};
        if (divisor == 0U) [[unlikely]]
        {
            return std::nullopt;
        }

        if constexpr (std::is_same_v<BasisType, std::uint8_t> || std::is_same_v<BasisType, std::uint16_t>)
        {
            return std::make_optional(result_type{static_cast<BasisType>(static_cast<BasisType>(lhs) % divisor)});
        }
        else
        {
            return std::make_optional(result_type{static_cast<BasisType>(lhs) % divisor});
        }
    }
};

template <overflow_policy Policy, fundamental_unsigned_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto mod_impl(const unsigned_integer_basis<BasisType> lhs,
                                      const unsigned_integer_basis<BasisType> rhs)
    noexcept(Policy == overflow_policy::checked || Policy == overflow_policy::strict)
{
    return mod_helper<Policy, BasisType>::apply(lhs, rhs);
}

template <fundamental_unsigned_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto operator%(const unsigned_integer_basis<BasisType> lhs,
                                       const unsigned_integer_basis<BasisType> rhs) -> unsigned_integer_basis<BasisType>
{
    #if !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))

    if (std::is_constant_evaluated())
    {
        const auto divisor {static_cast<BasisType>(rhs)};
        if (divisor == 0U) [[unlikely]]
        {
            throw std::domain_error("Unsigned modulo by zero");
        }

        if constexpr (std::is_same_v<BasisType, std::uint8_t> || std::is_same_v<BasisType, std::uint16_t>)
        {
            return unsigned_integer_basis<BasisType>{static_cast<BasisType>(static_cast<BasisType>(lhs) % divisor)};
        }
        else
        {
            return unsigned_integer_basis<BasisType>{static_cast<BasisType>(lhs) % divisor};
        }
    }

    #endif

    return mod_helper<overflow_policy::throw_exception, BasisType>::apply(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("modulo", operator%)

template <fundamental_unsigned_integral BasisType>
template <fundamental_unsigned_integral OtherBasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr auto unsigned_integer_basis<BasisType>::operator%=(const unsigned_integer_basis<OtherBasisType> rhs)
    -> unsigned_integer_basis&
{
    *this = *this % rhs;
    return *this;
}

// ------------------------------
// Pre and post increment
// ------------------------------

template <fundamental_unsigned_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr auto unsigned_integer_basis<BasisType>::operator++()
    -> unsigned_integer_basis&
{
    if (this->basis_ == std::numeric_limits<BasisType>::max()) [[unlikely]]
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, overflow_inc_msg<BasisType>());
    }

    ++this->basis_;
    return *this;
}

template <fundamental_unsigned_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr auto unsigned_integer_basis<BasisType>::operator++(int)
    -> unsigned_integer_basis
{
    if (this->basis_ == std::numeric_limits<BasisType>::max()) [[unlikely]]
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, overflow_inc_msg<BasisType>());
    }

    const auto temp {*this};
    ++this->basis_;
    return temp;
}

// ------------------------------
// Pre and post decrement
// ------------------------------

template <fundamental_unsigned_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr auto unsigned_integer_basis<BasisType>::operator--()
    -> unsigned_integer_basis&
{
    if (this->basis_ == 0U) [[unlikely]]
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, underflow_dec_msg<BasisType>());
    }

    --this->basis_;
    return *this;
}

template <fundamental_unsigned_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr auto unsigned_integer_basis<BasisType>::operator--(int)
    -> unsigned_integer_basis
{
    if (this->basis_ == 0U) [[unlikely]]
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, underflow_dec_msg<BasisType>());
    }

    const auto temp {*this};
    --this->basis_;
    return temp;
}

// ------------------------------
// Left Shift
// ------------------------------

// Primary template for non-tuple policies
template <overflow_policy Policy, fundamental_unsigned_integral BasisType>
struct shl_helper
{
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType> lhs,
                                              const unsigned_integer_basis<BasisType> rhs)
        noexcept(Policy != overflow_policy::throw_exception)
        -> unsigned_integer_basis<BasisType>
    {
        using result_type = unsigned_integer_basis<BasisType>;
        using core::bit_width;

        const auto raw_lhs {static_cast<BasisType>(lhs)};
        const auto raw_rhs {static_cast<BasisType>(rhs)};
        const auto lhs_width {static_cast<BasisType>(bit_width(raw_lhs))};
        const auto overflowed {lhs_width + raw_rhs >= static_cast<BasisType>(std::numeric_limits<BasisType>::digits)};

        if (overflowed)
        {
            if constexpr (Policy == overflow_policy::throw_exception)
            {
                BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, left_shift_overflow_msg<BasisType>());
            }
            else if constexpr (Policy == overflow_policy::saturate)
            {
                return result_type{std::numeric_limits<BasisType>::max()};
            }
            else if constexpr (Policy == overflow_policy::strict)
            {
                std::exit(EXIT_FAILURE);
            }
            else
            {
                BOOST_SAFE_NUMBERS_UNREACHABLE;
            }
        }

        return result_type{static_cast<BasisType>(raw_lhs << raw_rhs)};
    }
};

// Partial specialization for overflow_tuple policy
template <fundamental_unsigned_integral BasisType>
struct shl_helper<overflow_policy::overflow_tuple, BasisType>
{
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType> lhs,
                                              const unsigned_integer_basis<BasisType> rhs) noexcept
        -> std::pair<unsigned_integer_basis<BasisType>, bool>
    {
        using result_type = unsigned_integer_basis<BasisType>;
        using core::bit_width;

        const auto raw_lhs {static_cast<BasisType>(lhs)};
        const auto raw_rhs {static_cast<BasisType>(rhs)};
        const auto lhs_width {static_cast<BasisType>(bit_width(raw_lhs))};
        const auto overflowed {lhs_width + raw_rhs >= static_cast<BasisType>(std::numeric_limits<BasisType>::digits)};

        // Guard against UB: shifting by >= digits is undefined behavior
        if (raw_rhs >= static_cast<BasisType>(std::numeric_limits<BasisType>::digits))
        {
            return std::make_pair(result_type{0U}, true);
        }

        return std::make_pair(result_type{static_cast<BasisType>(raw_lhs << raw_rhs)}, overflowed);
    }
};

// Partial specialization for checked policy
template <fundamental_unsigned_integral BasisType>
struct shl_helper<overflow_policy::checked, BasisType>
{
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType> lhs,
                                              const unsigned_integer_basis<BasisType> rhs) noexcept
        -> std::optional<unsigned_integer_basis<BasisType>>
    {
        using result_type = unsigned_integer_basis<BasisType>;
        using core::bit_width;

        const auto raw_lhs {static_cast<BasisType>(lhs)};
        const auto raw_rhs {static_cast<BasisType>(rhs)};
        const auto lhs_width {static_cast<BasisType>(bit_width(raw_lhs))};
        const auto overflowed {lhs_width + raw_rhs >= static_cast<BasisType>(std::numeric_limits<BasisType>::digits)};

        return overflowed ? std::nullopt : std::make_optional(result_type{static_cast<BasisType>(raw_lhs << raw_rhs)});
    }
};

template <overflow_policy Policy, fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto shl_impl(const unsigned_integer_basis<BasisType> lhs,
                                      const unsigned_integer_basis<BasisType> rhs)
    noexcept(Policy == overflow_policy::saturate || Policy == overflow_policy::overflow_tuple || Policy == overflow_policy::checked || Policy == overflow_policy::strict)
{
    return shl_helper<Policy, BasisType>::apply(lhs, rhs);
}

// ------------------------------
// Right Shift
// ------------------------------

// Primary template for non-tuple policies
template <overflow_policy Policy, fundamental_unsigned_integral BasisType>
struct shr_helper
{
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType> lhs,
                                              const unsigned_integer_basis<BasisType> rhs)
        noexcept(Policy != overflow_policy::throw_exception)
        -> unsigned_integer_basis<BasisType>
    {
        using result_type = unsigned_integer_basis<BasisType>;

        const auto raw_lhs {static_cast<BasisType>(lhs)};
        const auto raw_rhs {static_cast<BasisType>(rhs)};
        const auto overflowed {raw_rhs >= static_cast<BasisType>(std::numeric_limits<BasisType>::digits)};

        if (overflowed)
        {
            if constexpr (Policy == overflow_policy::throw_exception)
            {
                BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, right_shift_overflow_msg<BasisType>());
            }
            else if constexpr (Policy == overflow_policy::saturate)
            {
                return result_type{0U};
            }
            else if constexpr (Policy == overflow_policy::strict)
            {
                std::exit(EXIT_FAILURE);
            }
            else
            {
                BOOST_SAFE_NUMBERS_UNREACHABLE;
            }
        }

        return result_type{static_cast<BasisType>(raw_lhs >> raw_rhs)};
    }
};

// Partial specialization for overflow_tuple policy
template <fundamental_unsigned_integral BasisType>
struct shr_helper<overflow_policy::overflow_tuple, BasisType>
{
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType> lhs,
                                              const unsigned_integer_basis<BasisType> rhs) noexcept
        -> std::pair<unsigned_integer_basis<BasisType>, bool>
    {
        using result_type = unsigned_integer_basis<BasisType>;

        const auto raw_lhs {static_cast<BasisType>(lhs)};
        const auto raw_rhs {static_cast<BasisType>(rhs)};
        const auto overflowed {raw_rhs >= static_cast<BasisType>(std::numeric_limits<BasisType>::digits)};

        if (overflowed)
        {
            return std::make_pair(result_type{0U}, true);
        }

        return std::make_pair(result_type{static_cast<BasisType>(raw_lhs >> raw_rhs)}, false);
    }
};

// Partial specialization for checked policy
template <fundamental_unsigned_integral BasisType>
struct shr_helper<overflow_policy::checked, BasisType>
{
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType> lhs,
                                              const unsigned_integer_basis<BasisType> rhs) noexcept
        -> std::optional<unsigned_integer_basis<BasisType>>
    {
        using result_type = unsigned_integer_basis<BasisType>;

        const auto raw_lhs {static_cast<BasisType>(lhs)};
        const auto raw_rhs {static_cast<BasisType>(rhs)};
        const auto overflowed {raw_rhs >= static_cast<BasisType>(std::numeric_limits<BasisType>::digits)};

        return overflowed ? std::nullopt : std::make_optional(result_type{static_cast<BasisType>(raw_lhs >> raw_rhs)});
    }
};

template <overflow_policy Policy, fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto shr_impl(const unsigned_integer_basis<BasisType> lhs,
                                      const unsigned_integer_basis<BasisType> rhs)
    noexcept(Policy == overflow_policy::saturate || Policy == overflow_policy::overflow_tuple || Policy == overflow_policy::checked || Policy == overflow_policy::strict)
{
    return shr_helper<Policy, BasisType>::apply(lhs, rhs);
}

} // namespace boost::safe_numbers::detail

// ------------------------------
// Saturating Math
// ------------------------------

namespace boost::safe_numbers {

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto saturating_add(const detail::unsigned_integer_basis<BasisType> lhs,
                                            const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType>
{
    return detail::add_impl<overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("saturating addition", saturating_add)

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto saturating_sub(const detail::unsigned_integer_basis<BasisType> lhs,
                                            const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType>
{
    return detail::sub_impl<overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("saturating subtraction", saturating_sub)

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto saturating_mul(const detail::unsigned_integer_basis<BasisType> lhs,
                                            const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType>
{
    return detail::mul_impl<overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("saturating multiplication", saturating_mul)

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto saturating_div(const detail::unsigned_integer_basis<BasisType> lhs,
                                            const detail::unsigned_integer_basis<BasisType> rhs)
    -> detail::unsigned_integer_basis<BasisType>
{
    return detail::div_impl<overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("saturating division", saturating_div)

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto saturating_mod(const detail::unsigned_integer_basis<BasisType> lhs,
                                            const detail::unsigned_integer_basis<BasisType> rhs)
    -> detail::unsigned_integer_basis<BasisType>
{
    return detail::mod_impl<overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("saturating modulo", saturating_mod)

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto overflowing_add(const detail::unsigned_integer_basis<BasisType> lhs,
                                             const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> std::pair<detail::unsigned_integer_basis<BasisType>, bool>
{
    return detail::add_impl<overflow_policy::overflow_tuple>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("overflowing addition", overflowing_add)

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto overflowing_sub(const detail::unsigned_integer_basis<BasisType> lhs,
                                             const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> std::pair<detail::unsigned_integer_basis<BasisType>, bool>
{
    return detail::sub_impl<overflow_policy::overflow_tuple>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("overflowing subtraction", overflowing_sub)

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto overflowing_mul(const detail::unsigned_integer_basis<BasisType> lhs,
                                             const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> std::pair<detail::unsigned_integer_basis<BasisType>, bool>
{
    return detail::mul_impl<overflow_policy::overflow_tuple>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("overflowing multiplication", overflowing_mul)

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto overflowing_div(const detail::unsigned_integer_basis<BasisType> lhs,
                                             const detail::unsigned_integer_basis<BasisType> rhs)
    -> std::pair<detail::unsigned_integer_basis<BasisType>, bool>
{
    return detail::div_impl<overflow_policy::overflow_tuple>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("overflowing division", overflowing_div)

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto overflowing_mod(const detail::unsigned_integer_basis<BasisType> lhs,
                                             const detail::unsigned_integer_basis<BasisType> rhs)
    -> std::pair<detail::unsigned_integer_basis<BasisType>, bool>
{
    return detail::mod_impl<overflow_policy::overflow_tuple>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("overflowing modulo", overflowing_mod)

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto checked_add(const detail::unsigned_integer_basis<BasisType> lhs,
                                         const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> std::optional<detail::unsigned_integer_basis<BasisType>>
{
    return detail::add_impl<overflow_policy::checked>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("checked addition", checked_add)

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto checked_sub(const detail::unsigned_integer_basis<BasisType> lhs,
                                         const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> std::optional<detail::unsigned_integer_basis<BasisType>>
{
    return detail::sub_impl<overflow_policy::checked>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("checked subtraction", checked_sub)

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto checked_mul(const detail::unsigned_integer_basis<BasisType> lhs,
                                         const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> std::optional<detail::unsigned_integer_basis<BasisType>>
{
    return detail::mul_impl<overflow_policy::checked>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("checked multiplication", checked_mul)

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto checked_div(const detail::unsigned_integer_basis<BasisType> lhs,
                                         const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> std::optional<detail::unsigned_integer_basis<BasisType>>
{
    return detail::div_impl<overflow_policy::checked>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("checked division", checked_div)

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto checked_mod(const detail::unsigned_integer_basis<BasisType> lhs,
                                         const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> std::optional<detail::unsigned_integer_basis<BasisType>>
{
    return detail::mod_impl<overflow_policy::checked>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("checked modulo", checked_mod)

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto strict_add(const detail::unsigned_integer_basis<BasisType> lhs,
                                        const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType>
{
    return detail::add_impl<overflow_policy::strict>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("strict addition", strict_add)

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto strict_sub(const detail::unsigned_integer_basis<BasisType> lhs,
                                        const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType>
{
    return detail::sub_impl<overflow_policy::strict>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("strict subtraction", strict_sub)

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto strict_mul(const detail::unsigned_integer_basis<BasisType> lhs,
                                        const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType>
{
    return detail::mul_impl<overflow_policy::strict>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("strict multiplication", strict_mul)

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto strict_div(const detail::unsigned_integer_basis<BasisType> lhs,
                                        const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType>
{
    return detail::div_impl<overflow_policy::strict>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("strict division", strict_div)

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto strict_mod(const detail::unsigned_integer_basis<BasisType> lhs,
                                        const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType>
{
    return detail::mod_impl<overflow_policy::strict>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("strict modulo", strict_mod)

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto widening_add(const detail::unsigned_integer_basis<BasisType> lhs,
                                          const detail::unsigned_integer_basis<BasisType> rhs) noexcept
{
    return detail::add_impl<overflow_policy::widen>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("widening add", widening_add)

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto widening_mul(const detail::unsigned_integer_basis<BasisType> lhs,
                                          const detail::unsigned_integer_basis<BasisType> rhs) noexcept
{
    return detail::mul_impl<overflow_policy::widen>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("widening mul", widening_mul)

// ------------------------------
// Saturating Shift
// ------------------------------

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto saturating_shl(const detail::unsigned_integer_basis<BasisType> lhs,
                                            const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType>
{
    return detail::shl_impl<overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("saturating left shift", saturating_shl)

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto saturating_shr(const detail::unsigned_integer_basis<BasisType> lhs,
                                            const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType>
{
    return detail::shr_impl<overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("saturating right shift", saturating_shr)

// ------------------------------
// Overflowing Shift
// ------------------------------

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto overflowing_shl(const detail::unsigned_integer_basis<BasisType> lhs,
                                             const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> std::pair<detail::unsigned_integer_basis<BasisType>, bool>
{
    return detail::shl_impl<overflow_policy::overflow_tuple>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("overflowing left shift", overflowing_shl)

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto overflowing_shr(const detail::unsigned_integer_basis<BasisType> lhs,
                                             const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> std::pair<detail::unsigned_integer_basis<BasisType>, bool>
{
    return detail::shr_impl<overflow_policy::overflow_tuple>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("overflowing right shift", overflowing_shr)

// ------------------------------
// Checked Shift
// ------------------------------

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto checked_shl(const detail::unsigned_integer_basis<BasisType> lhs,
                                         const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> std::optional<detail::unsigned_integer_basis<BasisType>>
{
    return detail::shl_impl<overflow_policy::checked>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("checked left shift", checked_shl)

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto checked_shr(const detail::unsigned_integer_basis<BasisType> lhs,
                                         const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> std::optional<detail::unsigned_integer_basis<BasisType>>
{
    return detail::shr_impl<overflow_policy::checked>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("checked right shift", checked_shr)

// ------------------------------
// Strict Shift
// ------------------------------

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto strict_shl(const detail::unsigned_integer_basis<BasisType> lhs,
                                        const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType>
{
    return detail::shl_impl<overflow_policy::strict>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("strict left shift", strict_shl)

template <detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto strict_shr(const detail::unsigned_integer_basis<BasisType> lhs,
                                        const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType>
{
    return detail::shr_impl<overflow_policy::strict>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("strict right shift", strict_shr)

// ------------------------------
// Generic policy-parameterized functions
// ------------------------------

template <overflow_policy Policy, detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto add(const detail::unsigned_integer_basis<BasisType> lhs,
                                 const detail::unsigned_integer_basis<BasisType> rhs)
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

template <overflow_policy Policy, detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto sub(const detail::unsigned_integer_basis<BasisType> lhs,
                                 const detail::unsigned_integer_basis<BasisType> rhs)
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

template <overflow_policy Policy, detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto mul(const detail::unsigned_integer_basis<BasisType> lhs,
                                 const detail::unsigned_integer_basis<BasisType> rhs)
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

template <overflow_policy Policy, detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto div(const detail::unsigned_integer_basis<BasisType> lhs,
                                 const detail::unsigned_integer_basis<BasisType> rhs)
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

template <overflow_policy Policy, detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto mod(const detail::unsigned_integer_basis<BasisType> lhs,
                                 const detail::unsigned_integer_basis<BasisType> rhs)
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

template <overflow_policy Policy, detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto shl(const detail::unsigned_integer_basis<BasisType> lhs,
                                 const detail::unsigned_integer_basis<BasisType> rhs)
    noexcept(Policy != overflow_policy::throw_exception)
{
    return detail::shl_impl<Policy>(lhs, rhs);
}

template <overflow_policy Policy, detail::fundamental_unsigned_integral BasisType>
[[nodiscard]] constexpr auto shr(const detail::unsigned_integer_basis<BasisType> lhs,
                                 const detail::unsigned_integer_basis<BasisType> rhs)
    noexcept(Policy != overflow_policy::throw_exception)
{
    return detail::shr_impl<Policy>(lhs, rhs);
}

template <detail::fundamental_unsigned_integral BasisType>
constexpr auto operator~(const detail::unsigned_integer_basis<BasisType> lhs) noexcept
{
    using return_type = detail::unsigned_integer_basis<BasisType>;
    return return_type{static_cast<BasisType>(~detail::raw_value(lhs))};
}

template <detail::fundamental_unsigned_integral BasisType>
constexpr auto operator&(const detail::unsigned_integer_basis<BasisType> lhs,
                         const detail::unsigned_integer_basis<BasisType> rhs) noexcept
{
    using return_type = detail::unsigned_integer_basis<BasisType>;
    return return_type{static_cast<BasisType>(detail::raw_value(lhs) & detail::raw_value(rhs))};
}

template <detail::fundamental_unsigned_integral BasisType>
constexpr auto operator|(const detail::unsigned_integer_basis<BasisType> lhs,
                         const detail::unsigned_integer_basis<BasisType> rhs) noexcept
{
    using return_type = detail::unsigned_integer_basis<BasisType>;
    return return_type{static_cast<BasisType>(detail::raw_value(lhs) | detail::raw_value(rhs))};
}

template <detail::fundamental_unsigned_integral BasisType>
constexpr auto operator^(const detail::unsigned_integer_basis<BasisType> lhs,
                         const detail::unsigned_integer_basis<BasisType> rhs) noexcept
{
    using return_type = detail::unsigned_integer_basis<BasisType>;
    return return_type{static_cast<BasisType>(detail::raw_value(lhs) ^ detail::raw_value(rhs))};
}

template <detail::fundamental_unsigned_integral BasisType>
constexpr auto operator<<(const detail::unsigned_integer_basis<BasisType> lhs,
                          const detail::unsigned_integer_basis<BasisType> rhs)
{
    return detail::shl_impl<overflow_policy::throw_exception>(lhs, rhs);
}

template <detail::fundamental_unsigned_integral BasisType>
constexpr auto operator>>(const detail::unsigned_integer_basis<BasisType> lhs,
                          const detail::unsigned_integer_basis<BasisType> rhs)
{
    return detail::shr_impl<overflow_policy::throw_exception>(lhs, rhs);
}

// ------------------------------
// Compound bitwise operators
// ------------------------------

template <detail::fundamental_unsigned_integral BasisType>
constexpr auto detail::unsigned_integer_basis<BasisType>::operator&=(const unsigned_integer_basis rhs) noexcept
    -> unsigned_integer_basis&
{
    *this = boost::safe_numbers::operator&(*this, rhs);
    return *this;
}

template <detail::fundamental_unsigned_integral BasisType>
constexpr auto detail::unsigned_integer_basis<BasisType>::operator|=(const unsigned_integer_basis rhs) noexcept
    -> unsigned_integer_basis&
{
    *this = boost::safe_numbers::operator|(*this, rhs);
    return *this;
}

template <detail::fundamental_unsigned_integral BasisType>
constexpr auto detail::unsigned_integer_basis<BasisType>::operator^=(const unsigned_integer_basis rhs) noexcept
    -> unsigned_integer_basis&
{
    *this = boost::safe_numbers::operator^(*this, rhs);
    return *this;
}

template <detail::fundamental_unsigned_integral BasisType>
constexpr auto detail::unsigned_integer_basis<BasisType>::operator<<=(const unsigned_integer_basis rhs)
    -> unsigned_integer_basis&
{
    *this = boost::safe_numbers::operator<<(*this, rhs);
    return *this;
}

template <detail::fundamental_unsigned_integral BasisType>
constexpr auto detail::unsigned_integer_basis<BasisType>::operator>>=(const unsigned_integer_basis rhs)
    -> unsigned_integer_basis&
{
    *this = boost::safe_numbers::operator>>(*this, rhs);
    return *this;
}

} // namespace boost::safe_numbers

#undef BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP

#endif // BOOST_SAFE_NUMBERS_DETAIL_UNSIGNED_INTEGER_BASIS_HPP

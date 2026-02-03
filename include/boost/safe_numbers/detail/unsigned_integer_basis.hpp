// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_UNSIGNED_INTEGER_BASIS_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_UNSIGNED_INTEGER_BASIS_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/concepts.hpp>
#include <boost/safe_numbers/detail/overflow_policy.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/throw_exception.hpp>
#include <concepts>
#include <compare>
#include <limits>
#include <stdexcept>
#include <cstdint>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers::detail {

template <unsigned_integral BasisType>
class unsigned_integer_basis
{
public:

    // This is exposed to the user so that they can convert back to built-in
    using basis_type = BasisType;

private:

    BasisType basis_ {0U};

public:

    constexpr unsigned_integer_basis() noexcept = default;

    explicit constexpr unsigned_integer_basis(const BasisType val) noexcept : basis_{val} {}

    template <typename T>
        requires std::is_same_v<T, bool>
    explicit constexpr unsigned_integer_basis(T) noexcept
    {
        static_assert(false, "Construction from bool is not allowed");
    }

    template <unsigned_integral OtherBasis>
    [[nodiscard]] explicit constexpr operator OtherBasis() const noexcept;

    [[nodiscard]] friend constexpr auto operator<=>(unsigned_integer_basis lhs, unsigned_integer_basis rhs) noexcept
        -> std::strong_ordering = default;

    template <unsigned_integral OtherBasis>
    constexpr auto operator+=(unsigned_integer_basis<OtherBasis> rhs) -> unsigned_integer_basis&;

    template <unsigned_integral OtherBasis>
    constexpr auto operator-=(unsigned_integer_basis<OtherBasis> rhs) -> unsigned_integer_basis&;

    template <unsigned_integral OtherBasis>
    constexpr auto operator*=(unsigned_integer_basis<OtherBasis> rhs) -> unsigned_integer_basis&;

    template <unsigned_integral OtherBasis>
    constexpr auto operator/=(unsigned_integer_basis<OtherBasis> rhs) -> unsigned_integer_basis&;

    template <unsigned_integral OtherBasis>
    constexpr auto operator%=(unsigned_integer_basis<OtherBasis> rhs) -> unsigned_integer_basis&;

    constexpr auto operator++() -> unsigned_integer_basis&;

    constexpr auto operator++(int) -> unsigned_integer_basis;

    constexpr auto operator--() -> unsigned_integer_basis&;

    constexpr auto operator--(int) -> unsigned_integer_basis;
};

template <unsigned_integral BasisType>
template <unsigned_integral OtherBasis>
constexpr unsigned_integer_basis<BasisType>::operator OtherBasis() const noexcept
{
    if constexpr (sizeof(OtherBasis) < sizeof(BasisType))
    {
        static_assert(false, "Narrowing conversions are not allowed");
    }

    return static_cast<OtherBasis>(basis_);
}

// ------------------------------
// Addition
// ------------------------------

namespace impl {

#if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_add_overflow)

template <std::unsigned_integral T>
bool unsigned_intrin_add(const T lhs, const T rhs, T& result)
{
    return __builtin_add_overflow(lhs, rhs, &result);
}

#elif defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN)

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

#elif defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

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
constexpr bool unsigned_no_intrin_add(const T lhs, const T rhs, T& result) noexcept
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

constexpr bool unsigned_no_intrin_add(const int128::uint128_t& lhs, const int128::uint128_t& rhs, int128::uint128_t& result) noexcept
{
    result = lhs + rhs;
    return result < lhs;
}

} // namespace impl

template <overflow_policy Policy, unsigned_integral BasisType>
[[nodiscard]] constexpr auto add_impl(const unsigned_integer_basis<BasisType> lhs,
                                      const unsigned_integer_basis<BasisType> rhs)
    noexcept(Policy != overflow_policy::throw_exception) -> unsigned_integer_basis<BasisType>
{
    using result_type = unsigned_integer_basis<BasisType>;

    const auto lhs_basis {static_cast<BasisType>(lhs)};
    const auto rhs_basis {static_cast<BasisType>(rhs)};
    BasisType res {};

    auto handle_overflow = [&res]
    {
        if constexpr (Policy == overflow_policy::throw_exception)
        {
            static_cast<void>(res);
            BOOST_THROW_EXCEPTION(std::overflow_error("Overflow detected in unsigned addition"));
        }
        else if constexpr (Policy == overflow_policy::saturate)
        {
            res = std::numeric_limits<BasisType>::max();
        }
        else
        {
            static_cast<void>(res);
            BOOST_SAFE_NUMBERS_UNREACHABLE;
        }
    };

    if constexpr (!std::is_same_v<BasisType, int128::uint128_t>)
    {
        #if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_add_overflow) || BOOST_SAFE_NUMBERS_HAS_BUILTIN(_addcarry_u64) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

        if (!std::is_constant_evaluated())
        {
            if (impl::unsigned_intrin_add(lhs_basis, rhs_basis, res))
            {
                handle_overflow();
            }

            return result_type{res};
        }

        #endif // __has_builtin(__builtin_add_overflow)
    }

    if (impl::unsigned_no_intrin_add(lhs_basis, rhs_basis, res))
    {
        handle_overflow();
    }

    return result_type{res};
}

template <unsigned_integral BasisType>
[[nodiscard]] constexpr auto operator+(const unsigned_integer_basis<BasisType> lhs,
                                       const unsigned_integer_basis<BasisType> rhs) -> unsigned_integer_basis<BasisType>
{
    return add_impl<overflow_policy::throw_exception>(lhs, rhs);
}

} // namespace boost::safe_numbers::detail

#define BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP(OP_NAME, OP_SYMBOL)                    \
template <boost::safe_numbers::detail::unsigned_integral LHSBasis,                                 \
          boost::safe_numbers::detail::unsigned_integral RHSBasis>                                 \
    requires (!std::is_same_v<LHSBasis, RHSBasis>)                                                 \
constexpr auto OP_SYMBOL(const boost::safe_numbers::detail::unsigned_integer_basis<LHSBasis>,      \
                         const boost::safe_numbers::detail::unsigned_integer_basis<RHSBasis>)      \
{                                                                                                  \
    if constexpr (std::is_same_v<LHSBasis, std::uint8_t>)                                          \
    {                                                                                              \
        if constexpr (std::is_same_v<RHSBasis, std::uint16_t>)                                     \
        {                                                                                          \
            static_assert(false, "Can not perform " OP_NAME " between u8 and u16");                \
        }                                                                                          \
        else if constexpr (std::is_same_v<RHSBasis, std::uint32_t>)                                \
        {                                                                                          \
            static_assert(false, "Can not perform " OP_NAME " between u8 and u32");                \
        }                                                                                          \
        else if constexpr (std::is_same_v<RHSBasis, std::uint64_t>)                                \
        {                                                                                          \
            static_assert(false, "Can not perform " OP_NAME " between u8 and u64");                \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            static_assert(false, "Can not perform " OP_NAME " between u8 and unknown type");       \
        }                                                                                          \
    }                                                                                              \
    else if constexpr (std::is_same_v<LHSBasis, std::uint16_t>)                                    \
    {                                                                                              \
        if constexpr (std::is_same_v<RHSBasis, std::uint8_t>)                                      \
        {                                                                                          \
            static_assert(false, "Can not perform " OP_NAME " between u16 and u8");                \
        }                                                                                          \
        else if constexpr (std::is_same_v<RHSBasis, std::uint32_t>)                                \
        {                                                                                          \
            static_assert(false, "Can not perform " OP_NAME " between u16 and u32");               \
        }                                                                                          \
        else if constexpr (std::is_same_v<RHSBasis, std::uint64_t>)                                \
        {                                                                                          \
            static_assert(false, "Can not perform " OP_NAME " between u16 and u64");               \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            static_assert(false, "Can not perform " OP_NAME " between u16 and unknown type");      \
        }                                                                                          \
    }                                                                                              \
    else if constexpr (std::is_same_v<LHSBasis, std::uint32_t>)                                    \
    {                                                                                              \
        if constexpr (std::is_same_v<RHSBasis, std::uint8_t>)                                      \
        {                                                                                          \
            static_assert(false, "Can not perform " OP_NAME " between u32 and u8");                \
        }                                                                                          \
        else if constexpr (std::is_same_v<RHSBasis, std::uint16_t>)                                \
        {                                                                                          \
            static_assert(false, "Can not perform " OP_NAME " between u32 and u16");               \
        }                                                                                          \
        else if constexpr (std::is_same_v<RHSBasis, std::uint64_t>)                                \
        {                                                                                          \
            static_assert(false, "Can not perform " OP_NAME " between u32 and u64");               \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            static_assert(false, "Can not perform " OP_NAME " between u32 and unknown type");      \
        }                                                                                          \
    }                                                                                              \
    else if constexpr (std::is_same_v<LHSBasis, std::uint64_t>)                                    \
    {                                                                                              \
        if constexpr (std::is_same_v<RHSBasis, std::uint8_t>)                                      \
        {                                                                                          \
            static_assert(false, "Can not perform " OP_NAME " between u64 and u8");                \
        }                                                                                          \
        else if constexpr (std::is_same_v<RHSBasis, std::uint16_t>)                                \
        {                                                                                          \
            static_assert(false, "Can not perform " OP_NAME " between u64 and u16");               \
        }                                                                                          \
        else if constexpr (std::is_same_v<RHSBasis, std::uint32_t>)                                \
        {                                                                                          \
            static_assert(false, "Can not perform " OP_NAME " between u64 and u32");               \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            static_assert(false, "Can not perform " OP_NAME " between u64 and unknown type");      \
        }                                                                                          \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
        static_assert(false, "Can not perform " OP_NAME " on mixed width unsigned integer types"); \
    }                                                                                              \
                                                                                                   \
    return boost::safe_numbers::detail::unsigned_integer_basis<LHSBasis>(0);                       \
}

namespace boost::safe_numbers::detail {

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("addition", operator+)

template <unsigned_integral BasisType>
template <unsigned_integral OtherBasisType>
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

#if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_sub_overflow)

template <std::unsigned_integral T>
bool unsigned_intrin_sub(T lhs, T rhs, T& result)
{
    return __builtin_sub_overflow(lhs, rhs, &result);
}

#elif defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN)

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

#elif defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

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

constexpr bool unsigned_no_intrin_sub(const int128::uint128_t& lhs, const int128::uint128_t& rhs, int128::uint128_t& result) noexcept
{
    result = lhs - rhs;
    return result > lhs;
}

} // namespace impl

template <overflow_policy Policy, unsigned_integral BasisType>
[[nodiscard]] constexpr auto sub_impl(const unsigned_integer_basis<BasisType> lhs,
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
        if constexpr (Policy == overflow_policy::throw_exception)
        {
            static_cast<void>(res);
            BOOST_THROW_EXCEPTION(std::underflow_error("Underflow detected in unsigned subtraction"));
        }
        else if constexpr (Policy == overflow_policy::saturate)
        {
            res = std::numeric_limits<BasisType>::min();
        }
        else
        {
            static_cast<void>(res);
            BOOST_SAFE_NUMBERS_UNREACHABLE;
        }
    };

    if constexpr (!std::is_same_v<BasisType, int128::uint128_t>)
    {
        #if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_sub_overflow) || BOOST_SAFE_NUMBERS_HAS_BUILTIN(_subborrow_u64) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

        if (!std::is_constant_evaluated())
        {
            if (impl::unsigned_intrin_sub(lhs_basis, rhs_basis, res))
            {
                handle_underflow();
            }

            return result_type{res};
        }

        #endif // Use builtins
    }

    if (impl::unsigned_no_intrin_sub(lhs_basis, rhs_basis, res))
    {
        handle_underflow();
    }

    return result_type{res};
}

template <unsigned_integral BasisType>
[[nodiscard]] constexpr auto operator-(const unsigned_integer_basis<BasisType> lhs,
                                       const unsigned_integer_basis<BasisType> rhs) -> unsigned_integer_basis<BasisType>
{
    return sub_impl<overflow_policy::throw_exception>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("subtraction", operator-)

template <unsigned_integral BasisType>
template <unsigned_integral OtherBasisType>
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

#if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_mul_overflow)

template <std::unsigned_integral T>
bool unsigned_intrin_mul(const T lhs, const T rhs, T& result)
{
    return __builtin_mul_overflow(lhs, rhs, &result);
}

#elif defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN)

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

#elif defined(BOOST_SAFENUMBERS_HAS_WINDOWS_ARM64_INTRIN)

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
constexpr bool no_intrin_mul(const T lhs, const T rhs, T& result)
{
    using promoted_type = std::conditional_t<std::is_same_v<T, std::uint8_t>, std::uint_fast16_t,
                             std::conditional_t<std::is_same_v<T, std::uint16_t>, std::uint_fast32_t,
                                std::conditional_t<std::is_same_v<T, std::uint32_t>, std::uint_fast64_t, int128::uint128_t>>>;

    const auto temp {static_cast<promoted_type>(lhs) * rhs};
    result = static_cast<T>(temp);
    return temp > static_cast<promoted_type>(std::numeric_limits<T>::max());
}

constexpr bool no_intrin_mul(const int128::uint128_t& lhs, const int128::uint128_t& rhs, int128::uint128_t& result) noexcept
{
    // Fall back to division check
    if (rhs != 0U && lhs > (std::numeric_limits<int128::uint128_t>::max() / rhs))
    {
        result = std::numeric_limits<int128::uint128_t>::max();
        return true;
    }
    else
    {
        result = lhs * rhs;
        return false;
    }
}

} // namespace impl

template <overflow_policy Policy, unsigned_integral BasisType>
[[nodiscard]] constexpr auto mul_impl(const unsigned_integer_basis<BasisType> lhs,
                                      const unsigned_integer_basis<BasisType> rhs)
    noexcept(Policy == overflow_policy::saturate)
    -> unsigned_integer_basis<BasisType>
{
    using result_type = unsigned_integer_basis<BasisType>;

    const auto lhs_basis {static_cast<BasisType>(lhs)};
    const auto rhs_basis {static_cast<BasisType>(rhs)};
    BasisType res {};

    auto handle_overflow = [&res]()
    {
        if constexpr (Policy == overflow_policy::throw_exception)
        {
            static_cast<void>(res);
            BOOST_THROW_EXCEPTION(std::overflow_error("Overflow detected in unsigned multiplication"));
        }
        else if constexpr (Policy == overflow_policy::saturate)
        {
            res = std::numeric_limits<BasisType>::max();
        }
        else
        {
            static_cast<void>(res);
            BOOST_SAFE_NUMBERS_UNREACHABLE;
        }
    };

    if constexpr (!std::is_same_v<BasisType, int128::uint128_t>)
    {
        #if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_mul_overflow) || BOOST_SAFE_NUMBERS_HAS_BUILTIN(_umul128)

        if (!std::is_constant_evaluated())
        {
            if (impl::unsigned_intrin_mul(lhs_basis, rhs_basis, res))
            {
                handle_overflow();
            }

            return result_type{res};
        }

        #endif // Use builtins
    }

    if (impl::no_intrin_mul(lhs_basis, rhs_basis, res))
    {
        handle_overflow();
    }

    return result_type{res};
}

template <unsigned_integral BasisType>
[[nodiscard]] constexpr auto operator*(const unsigned_integer_basis<BasisType> lhs,
                                       const unsigned_integer_basis<BasisType> rhs) -> unsigned_integer_basis<BasisType>
{
    return mul_impl<overflow_policy::throw_exception>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("multiplication", operator*)

template <unsigned_integral BasisType>
template <unsigned_integral OtherBasisType>
constexpr auto unsigned_integer_basis<BasisType>::operator*=(const unsigned_integer_basis<OtherBasisType> rhs)
    -> unsigned_integer_basis&
{
    *this = *this * rhs;
    return *this;
}

// ------------------------------
// Division
// ------------------------------

template <overflow_policy Policy, unsigned_integral BasisType>
[[nodiscard]] constexpr auto div_impl(const unsigned_integer_basis<BasisType> lhs,
                                      const unsigned_integer_basis<BasisType> rhs)
    noexcept(Policy == overflow_policy::saturate)
    -> unsigned_integer_basis<BasisType>
{
    using result_type = unsigned_integer_basis<BasisType>;

    // Normally this should trap, but throwing an exception is more elegant
    if (static_cast<BasisType>(rhs) == 0U) [[unlikely]]
    {
        if constexpr (Policy == overflow_policy::throw_exception)
        {
            BOOST_THROW_EXCEPTION(std::domain_error("Unsigned division by zero"));
        }
        else if constexpr (Policy == overflow_policy::saturate)
        {
            return result_type{std::numeric_limits<BasisType>::max()};
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

template <unsigned_integral BasisType>
[[nodiscard]] constexpr auto operator/(const unsigned_integer_basis<BasisType> lhs,
                                       const unsigned_integer_basis<BasisType> rhs) -> unsigned_integer_basis<BasisType>
{
    return div_impl<overflow_policy::throw_exception>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("division", operator/)

template <unsigned_integral BasisType>
template <unsigned_integral OtherBasisType>
constexpr auto unsigned_integer_basis<BasisType>::operator/=(const unsigned_integer_basis<OtherBasisType> rhs)
    -> unsigned_integer_basis&
{
    *this = *this / rhs;
    return *this;
}

// ------------------------------
// Modulo
// ------------------------------

template <overflow_policy Policy, unsigned_integral BasisType>
[[nodiscard]] constexpr auto mod_impl(const unsigned_integer_basis<BasisType> lhs,
                                      const unsigned_integer_basis<BasisType> rhs)
    noexcept(Policy == overflow_policy::saturate)
    -> unsigned_integer_basis<BasisType>
{
    using result_type = unsigned_integer_basis<BasisType>;

    // Normally this should trap, but throwing an exception is more elegant
    if (static_cast<BasisType>(rhs) == 0U) [[unlikely]]
    {
        if constexpr (Policy == overflow_policy::throw_exception)
        {
            BOOST_THROW_EXCEPTION(std::domain_error("Unsigned modulo by zero"));
        }
        else if constexpr (Policy == overflow_policy::saturate)
        {
            return result_type{0U};
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

template <unsigned_integral BasisType>
[[nodiscard]] constexpr auto operator%(const unsigned_integer_basis<BasisType> lhs,
                                       const unsigned_integer_basis<BasisType> rhs) -> unsigned_integer_basis<BasisType>
{
    return mod_impl<overflow_policy::throw_exception>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("modulo", operator%)

template <unsigned_integral BasisType>
template <unsigned_integral OtherBasisType>
constexpr auto unsigned_integer_basis<BasisType>::operator%=(const unsigned_integer_basis<OtherBasisType> rhs)
    -> unsigned_integer_basis&
{
    *this = *this % rhs;
    return *this;
}

// ------------------------------
// Pre and post increment
// ------------------------------

template <unsigned_integral BasisType>
constexpr auto unsigned_integer_basis<BasisType>::operator++()
    -> unsigned_integer_basis&
{
    if (this->basis_ == std::numeric_limits<BasisType>::max()) [[unlikely]]
    {
        BOOST_THROW_EXCEPTION(std::overflow_error("Overflow detected in unsigned increment"));
    }

    ++this->basis_;
    return *this;
}

template <unsigned_integral BasisType>
constexpr auto unsigned_integer_basis<BasisType>::operator++(int)
    -> unsigned_integer_basis
{
    if (this->basis_ == std::numeric_limits<BasisType>::max()) [[unlikely]]
    {
        BOOST_THROW_EXCEPTION(std::overflow_error("Overflow detected in unsigned increment"));
    }

    const auto temp {*this};
    ++this->basis_;
    return temp;
}

// ------------------------------
// Pre and post decrement
// ------------------------------

template <unsigned_integral BasisType>
constexpr auto unsigned_integer_basis<BasisType>::operator--()
    -> unsigned_integer_basis&
{
    if (this->basis_ == 0U) [[unlikely]]
    {
        BOOST_THROW_EXCEPTION(std::underflow_error("Underflow detected in unsigned decrement"));
    }

    --this->basis_;
    return *this;
}

template <unsigned_integral BasisType>
constexpr auto unsigned_integer_basis<BasisType>::operator--(int)
    -> unsigned_integer_basis
{
    if (this->basis_ == 0U) [[unlikely]]
    {
        BOOST_THROW_EXCEPTION(std::underflow_error("Underflow detected in unsigned decrement"));
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

template <detail::unsigned_integral BasisType>
[[nodiscard]] constexpr auto add_sat(const detail::unsigned_integer_basis<BasisType> lhs,
                                     const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType>
{
    return detail::add_impl<detail::overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("saturating addition", add_sat)

template <detail::unsigned_integral BasisType>
[[nodiscard]] constexpr auto sub_sat(const detail::unsigned_integer_basis<BasisType> lhs,
                                     const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType>
{
    return detail::sub_impl<detail::overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("saturating subtraction", sub_sat)

template <detail::unsigned_integral BasisType>
[[nodiscard]] constexpr auto mul_sat(const detail::unsigned_integer_basis<BasisType> lhs,
                                     const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType>
{
    return detail::mul_impl<detail::overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("saturating multiplication", mul_sat)

template <detail::unsigned_integral BasisType>
[[nodiscard]] constexpr auto div_sat(const detail::unsigned_integer_basis<BasisType> lhs,
                                     const detail::unsigned_integer_basis<BasisType> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType>
{
    return detail::div_impl<detail::overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("saturating division", div_sat)

} // namespace boost::safe_numbers

#undef BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP

#endif // BOOST_SAFE_NUMBERS_DETAIL_UNSIGNED_INTEGER_BASIS_HPP

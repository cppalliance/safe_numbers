// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_UNSIGNED_INTEGER_BASIS_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_UNSIGNED_INTEGER_BASIS_HPP

#include <boost/throw_exception.hpp>
#include <boost/safe_numbers/detail/config.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <concepts>
#include <compare>
#include <limits>
#include <stdexcept>
#include <cstdint>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers::detail {

template <std::unsigned_integral BasisType>
class unsigned_integer_basis
{
private:

    BasisType basis_;

    static constexpr auto name_ {
        std::is_same_v<BasisType, std::uint8_t> ? "u8" :
        std::is_same_v<BasisType, std::uint16_t> ? "u16" :
        std::is_same_v<BasisType, std::uint32_t> ? "u32" :
        std::is_same_v<BasisType, std::uint64_t> ? "u64" : "u128"
     };

public:

    constexpr unsigned_integer_basis() noexcept = default;

    explicit constexpr unsigned_integer_basis(const BasisType val) noexcept : basis_{val} {}

    [[nodiscard]] explicit constexpr operator BasisType() const { return basis_; }

    [[nodiscard]] friend constexpr auto operator<=>(unsigned_integer_basis lhs, unsigned_integer_basis rhs) noexcept
        -> std::strong_ordering = default;

    template <std::unsigned_integral OtherBasis>
    constexpr auto operator+=(unsigned_integer_basis<OtherBasis> rhs) noexcept -> unsigned_integer_basis&;
};

namespace impl {

#if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_add_overflow)

template <std::unsigned_integral T>
bool intrin_add(T lhs, T rhs, T& result)
{
    if constexpr (std::is_same_v<T, unsigned long long int>)
    {
        return __builtin_uaddll_overflow(lhs, rhs, &result);
    }
    else if constexpr (std::is_same_v<T, unsigned long int>)
    {
        return __builtin_uaddl_overflow(lhs, rhs, &result);
    }
    else if constexpr (std::is_same_v<T, unsigned int>)
    {
        return __builtin_uadd_overflow(lhs, rhs, &result);
    }
    else
    {
        return __builtin_add_overflow(lhs, rhs, &result);
    }
}

#elif BOOST_SAFE_NUMBERS_HAS_BUILTIN(_addcarry_u64)

template <std::unsigned_integral T>
bool intrin_add(T lhs, T rhs, T& result)
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
bool intrin_add(T lhs, T rhs, T& result)
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

} // namespace impl

template <std::unsigned_integral BasisType>
[[nodiscard]] constexpr auto operator+(const unsigned_integer_basis<BasisType> lhs,
                                       const unsigned_integer_basis<BasisType> rhs) -> unsigned_integer_basis<BasisType>
{
    using result_type = unsigned_integer_basis<BasisType>;

    #if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_add_overflow)

    if (!std::is_constant_evaluated())
    {
        BasisType res;
        if (impl::intrin_add(static_cast<BasisType>(lhs), static_cast<BasisType>(rhs), res))
        {
            BOOST_THROW_EXCEPTION(std::overflow_error("Overflow detected in unsigned addition"));
        }

        return result_type{res};
    }

    #endif // __has_builtin(__builtin_add_overflow)

    const auto lhs_basis {static_cast<BasisType>(lhs)};
    const auto rhs_basis {static_cast<BasisType>(rhs)};
    const auto res {static_cast<BasisType>(lhs_basis + rhs_basis)};

    if (res < lhs_basis)
    {
        BOOST_THROW_EXCEPTION(std::overflow_error("Overflow detected in unsigned addition"));
    }

    return result_type{res};
}

template <std::unsigned_integral LHSBasis, std::unsigned_integral RHSBasis>
constexpr auto operator+(const unsigned_integer_basis<LHSBasis>,
                         const unsigned_integer_basis<RHSBasis>)
{
    if constexpr (std::is_same_v<LHSBasis, std::uint8_t>)
    {
        if constexpr (std::is_same_v<RHSBasis, std::uint16_t>)
        {
            static_assert(false, "Can not perform addition between u8 and u16");
        }
        else if constexpr (std::is_same_v<RHSBasis, std::uint32_t>)
        {
            static_assert(false, "Can not perform addition between u8 and u32");
        }
        else if constexpr (std::is_same_v<RHSBasis, std::uint64_t>)
        {
            static_assert(false, "Can not perform addition between u8 and u64");
        }
        else
        {
            static_assert(false, "Can not perform addition between u8 and unknown type");
        }
    }
    else if constexpr (std::is_same_v<LHSBasis, std::uint16_t>)
    {
        if constexpr (std::is_same_v<RHSBasis, std::uint8_t>)
        {
            static_assert(false, "Can not perform addition between u16 and u8");
        }
        else if constexpr (std::is_same_v<RHSBasis, std::uint32_t>)
        {
            static_assert(false, "Can not perform addition between u16 and u32");
        }
        else if constexpr (std::is_same_v<RHSBasis, std::uint64_t>)
        {
            static_assert(false, "Can not perform addition between u16 and u64");
        }
        else
        {
            static_assert(false, "Can not perform addition between u16 and unknown type");
        }
    }
    else if constexpr (std::is_same_v<LHSBasis, std::uint32_t>)
    {
        if constexpr (std::is_same_v<RHSBasis, std::uint8_t>)
        {
            static_assert(false, "Can not perform addition between u32 and u8");
        }
        else if constexpr (std::is_same_v<RHSBasis, std::uint16_t>)
        {
            static_assert(false, "Can not perform addition between u32 and u16");
        }
        else if constexpr (std::is_same_v<RHSBasis, std::uint64_t>)
        {
            static_assert(false, "Can not perform addition between u32 and u64");
        }
        else
        {
            static_assert(false, "Can not perform addition between u32 and unknown type");
        }
    }
    else if constexpr (std::is_same_v<LHSBasis, std::uint64_t>)
    {
        if constexpr (std::is_same_v<RHSBasis, std::uint8_t>)
        {
            static_assert(false, "Can not perform addition between u64 and u8");
        }
        else if constexpr (std::is_same_v<RHSBasis, std::uint16_t>)
        {
            static_assert(false, "Can not perform addition between u64 and u16");
        }
        else if constexpr (std::is_same_v<RHSBasis, std::uint32_t>)
        {
            static_assert(false, "Can not perform addition between u64 and u32");
        }
        else
        {
            static_assert(false, "Can not perform addition between u32 and unknown type");
        }
    }
    else
    {
        static_assert(false, "Can not perform addition on mixed width unsigned integer types");
    }

    return unsigned_integer_basis<LHSBasis>(0);
}

template <std::unsigned_integral BasisType>
template <std::unsigned_integral OtherBasisType>
constexpr auto unsigned_integer_basis<BasisType>::operator+=(const unsigned_integer_basis<OtherBasisType> rhs) noexcept
    -> unsigned_integer_basis&
{
    *this = *this + rhs;
    return *this;
}

} // namespace boost::safe_numbers::detail

#endif // BOOST_SAFE_NUMBERS_DETAIL_UNSIGNED_INTEGER_BASIS_HPP

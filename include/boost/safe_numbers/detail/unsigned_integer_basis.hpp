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

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers::detail {

template <typename BasisType>
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

    explicit constexpr unsigned_integer_basis(const BasisType val) : basis_{val} {}

    explicit constexpr operator BasisType() const { return basis_; }

    friend constexpr auto operator<=>(unsigned_integer_basis lhs, unsigned_integer_basis rhs) noexcept;

    friend constexpr auto operator+(unsigned_integer_basis lhs, unsigned_integer_basis rhs);

    template <typename LHSBasis, typename RHSBasis>
    friend constexpr auto operator+(unsigned_integer_basis<LHSBasis> lhs, unsigned_integer_basis<RHSBasis> rhs);
};

template <typename BasisType>
[[nodiscard]] constexpr auto operator<=>(const unsigned_integer_basis<BasisType> lhs,
                                         const unsigned_integer_basis<BasisType> rhs) noexcept
{
    return lhs.basis_ <=> rhs.basis_;
}

namespace impl {

#if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_add_overflow)

template <typename T>
bool intrin_add(T lhs, T rhs, T& result)
{
    return __builtin_add_overflow(lhs, rhs, &result);
}

#elif BOOST_SAFE_NUMBERS_HAS_BUILTIN(_addcarry_u64)

template <typename T>
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

#endif

} // namespace impl

template <typename BasisType>
[[nodiscard]] constexpr auto operator+(const unsigned_integer_basis<BasisType> lhs,
                                       const unsigned_integer_basis<BasisType> rhs) -> unsigned_integer_basis<BasisType>
{
    #if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_add_overflow)

    if (!std::is_constant_evaluated())
    {
        BasisType res;
        const bool overflow_detected {impl::intrin_add(lhs.basis_, rhs.basis_, res)};

        return overflow_detected ? BOOST_THROW_EXCEPTION(std::overflow_error("Overflow detected in unsigned addition")) :
                                   unsigned_integer_basis<BasisType>(res);
    }

    #endif // __has_builtin(__builtin_add_overflow)

    const auto res {lhs.basis_ + rhs.basis_};
    if (res < lhs.basis_)
    {
        BOOST_THROW_EXCEPTION(std::overflow_error("Overflow detected in unsigned addition"));
    }

    return unsigned_integer_basis<BasisType>(res);
}

template <typename LHSBasis, typename RHSBasis>
constexpr auto operator+(const unsigned_integer_basis<LHSBasis>,
                         const unsigned_integer_basis<RHSBasis>)
{
    // TODO(mborland): Expand this completely
    if constexpr (std::is_same_v<LHSBasis, std::uint32_t>)
    {
        if constexpr (std::is_same_v<RHSBasis, std::uint64_t>)
        {
            static_assert(false, "Can not perform addition between u32 and u64");
            return unsigned_integer_basis<RHSBasis>(0);
        }
        else
        {
            static_assert(false, "Can not perform addition between u32 and unknown type");
            return unsigned_integer_basis<RHSBasis>(0);
        }
    }
    else
    {
        static_assert(false, "Can not perform addition on mixed width unsigned integer types");
        return unsigned_integer_basis<RHSBasis>(0);
    }
}

} // namespace boost::safe_numbers::detail

#endif // BOOST_SAFE_NUMBERS_DETAIL_UNSIGNED_INTEGER_BASIS_HPP

// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_UNSIGNED_INTEGER_BASIS_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_UNSIGNED_INTEGER_BASIS_HPP

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/throw_exception.hpp>
#include <boost/safe_numbers/detail/config.hpp>
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

    friend constexpr auto operator+(unsigned_integer_basis lhs, unsigned_integer_basis rhs);

    template <typename LHSBasis, typename RHSBasis>
    friend constexpr void operator+(unsigned_integer_basis<LHSBasis> lhs, unsigned_integer_basis<RHSBasis> rhs);
};

template <typename BasisType>
[[nodiscard]] constexpr auto operator+(const unsigned_integer_basis<BasisType> lhs,
                                       const unsigned_integer_basis<BasisType> rhs) -> unsigned_integer_basis<BasisType>
{
    #if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_add_overflow)

    if (!std::is_constant_evaluated())
    {
        BasisType res;
        const bool overflow_detected {__builtin_add_overflow(lhs.basis_, rhs.basis_, &res)};

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
constexpr void operator+(const unsigned_integer_basis<LHSBasis> lhs,
                         const unsigned_integer_basis<RHSBasis> rhs)
{
    constexpr auto error_msg = "Can not add types " + lhs.name_ + " and " + rhs.name_;
    BOOST_THROW_EXCEPTION(std::logic_error(error_msg));
}

} // namespace boost::safe_numbers::detail

#endif // BOOST_SAFE_NUMBERS_DETAIL_UNSIGNED_INTEGER_BASIS_HPP

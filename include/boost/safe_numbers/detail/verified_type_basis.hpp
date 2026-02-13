// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_VERIFIED_INTEGER_BASIS_HPP
#define BOOST_SAFE_NUMBERS_VERIFIED_INTEGER_BASIS_HPP

#include <boost/safe_numbers/detail/config.hpp>
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

namespace boost::safe_numbers::detail {

template <library_type BasisType>
class verified_type_basis
{
private:

    using underlying_type = underlying_type_t<BasisType>;

    BasisType basis_ {};

public:

    explicit consteval verified_type_basis(const BasisType basis) : basis_{basis} {}

    explicit consteval verified_type_basis(const underlying_type val) : verified_type_basis{BasisType{val}} {}

    [[nodiscard]] explicit constexpr operator BasisType() const noexcept { return basis_; }

    [[nodiscard]] explicit constexpr operator underlying_type() const noexcept { return static_cast<underlying_type>(basis_); }

    [[nodiscard]] friend constexpr auto operator<=>(verified_type_basis lhs, verified_type_basis rhs) noexcept
        -> std::strong_ordering = default;

    consteval auto operator+=(verified_type_basis rhs) -> verified_type_basis&;

    consteval auto operator-=(verified_type_basis rhs) -> verified_type_basis&;

    consteval auto operator*=(verified_type_basis rhs) -> verified_type_basis&;

    consteval auto operator/=(verified_type_basis rhs) -> verified_type_basis&;

    consteval auto operator%=(verified_type_basis rhs) -> verified_type_basis&;

    consteval auto operator++() -> verified_type_basis&;

    consteval auto operator++(int) -> verified_type_basis;

    consteval auto operator--() -> verified_type_basis&;

    consteval auto operator--(int) -> verified_type_basis;
};

template <library_type BasisType>
[[nodiscard]] consteval auto operator+(const verified_type_basis<BasisType> lhs,
                                       const verified_type_basis<BasisType> rhs) -> verified_type_basis<BasisType>
{
    return verified_type_basis<BasisType>{static_cast<BasisType>(lhs) + static_cast<BasisType>(rhs)};
}

template <library_type BasisType>
[[nodiscard]] consteval auto operator-(const verified_type_basis<BasisType> lhs,
                                       const verified_type_basis<BasisType> rhs) -> verified_type_basis<BasisType>
{
    return verified_type_basis<BasisType>{static_cast<BasisType>(lhs) - static_cast<BasisType>(rhs)};
}

template <library_type BasisType>
[[nodiscard]] consteval auto operator*(const verified_type_basis<BasisType> lhs,
                                       const verified_type_basis<BasisType> rhs) -> verified_type_basis<BasisType>
{
    return verified_type_basis<BasisType>{static_cast<BasisType>(lhs) * static_cast<BasisType>(rhs)};
}

template <library_type BasisType>
[[nodiscard]] consteval auto operator/(const verified_type_basis<BasisType> lhs,
                                       const verified_type_basis<BasisType> rhs) -> verified_type_basis<BasisType>
{
    return verified_type_basis<BasisType>{static_cast<BasisType>(lhs) / static_cast<BasisType>(rhs)};
}

template <library_type BasisType>
[[nodiscard]] consteval auto operator%(const verified_type_basis<BasisType> lhs,
                                       const verified_type_basis<BasisType> rhs) -> verified_type_basis<BasisType>
{
    return verified_type_basis<BasisType>{static_cast<BasisType>(lhs) % static_cast<BasisType>(rhs)};
}

template <library_type BasisType>
consteval auto verified_type_basis<BasisType>::operator+=(verified_type_basis rhs) -> verified_type_basis&
{
    *this = *this + rhs;
    return *this;
}

template <library_type BasisType>
consteval auto verified_type_basis<BasisType>::operator-=(verified_type_basis rhs) -> verified_type_basis&
{
    *this = *this - rhs;
    return *this;
}

template <library_type BasisType>
consteval auto verified_type_basis<BasisType>::operator*=(verified_type_basis rhs) -> verified_type_basis&
{
    *this = *this * rhs;
    return *this;
}

template <library_type BasisType>
consteval auto verified_type_basis<BasisType>::operator/=(verified_type_basis rhs) -> verified_type_basis&
{
    *this = *this / rhs;
    return *this;
}

template <library_type BasisType>
consteval auto verified_type_basis<BasisType>::operator%=(verified_type_basis rhs) -> verified_type_basis&
{
    *this = *this % rhs;
    return *this;
}

template <library_type BasisType>
consteval auto verified_type_basis<BasisType>::operator++() -> verified_type_basis&
{
    ++basis_;
    return *this;
}

template <library_type BasisType>
consteval auto verified_type_basis<BasisType>::operator++(int) -> verified_type_basis
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

template <library_type BasisType>
consteval auto verified_type_basis<BasisType>::operator--() -> verified_type_basis&
{
    --basis_;
    return *this;
}

template <library_type BasisType>
consteval auto verified_type_basis<BasisType>::operator--(int) -> verified_type_basis
{
    auto tmp = *this;
    --(*this);
    return tmp;
}

// Mixed-type operation guards: produce clear static_assert messages
// instead of cryptic "invalid operands to binary expression" errors.

#define BOOST_SAFE_NUMBERS_DEFINE_MIXED_VERIFIED_OP(OP_NAME, OP_SYMBOL)                                         \
template <library_type LHSBasis, library_type RHSBasis>                                                         \
    requires (!std::is_same_v<LHSBasis, RHSBasis>)                                                              \
consteval auto OP_SYMBOL(const verified_type_basis<LHSBasis>,                                                   \
                         const verified_type_basis<RHSBasis>)                                                   \
{                                                                                                               \
    static_assert(dependent_false<LHSBasis, RHSBasis>,                                                          \
                  "Can not perform " OP_NAME " between verified types with different basis types");              \
    return verified_type_basis<LHSBasis>(LHSBasis{});                                                           \
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_VERIFIED_OP("addition", operator+)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_VERIFIED_OP("subtraction", operator-)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_VERIFIED_OP("multiplication", operator*)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_VERIFIED_OP("division", operator/)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_VERIFIED_OP("modulo", operator%)

#undef BOOST_SAFE_NUMBERS_DEFINE_MIXED_VERIFIED_OP

// Mixed-type comparison guard
template <library_type LHSBasis, library_type RHSBasis>
    requires (!std::is_same_v<LHSBasis, RHSBasis>)
constexpr auto operator<=>(const verified_type_basis<LHSBasis>,
                           const verified_type_basis<RHSBasis>)
    -> std::strong_ordering
{
    static_assert(dependent_false<LHSBasis, RHSBasis>,
                  "Can not compare verified types with different basis types");
    return std::strong_ordering::equal;
}

template <library_type LHSBasis, library_type RHSBasis>
    requires (!std::is_same_v<LHSBasis, RHSBasis>)
constexpr auto operator==(const verified_type_basis<LHSBasis>,
                          const verified_type_basis<RHSBasis>)
    -> bool
{
    static_assert(dependent_false<LHSBasis, RHSBasis>,
                  "Can not compare verified types with different basis types");
    return false;
}

// At runtime, we want to enable mixed operations between verified and unverified values of the same width
// e.g., u32 + verified_u32 = u32 or u32 <=> verified_u32
// These operations will always result in the runtime value

#define BOOST_SAFE_NUMBERS_MIXED_VERIFIED_OPERATOR(op)                                              \
template <library_type VerifiedBasisType>                                                           \
constexpr auto operator op(const verified_type_basis<VerifiedBasisType> lhs,                        \
                           const VerifiedBasisType rhs) -> VerifiedBasisType                        \
{                                                                                                   \
    return static_cast<VerifiedBasisType>(lhs) op rhs;                                              \
}                                                                                                   \
                                                                                                    \
template <library_type VerifiedBasisType>                                                           \
constexpr auto operator op(const VerifiedBasisType lhs,                                             \
                           const verified_type_basis<VerifiedBasisType> rhs) -> VerifiedBasisType   \
{                                                                                                   \
    return lhs op static_cast<VerifiedBasisType>(rhs);                                              \
}                                                                                                   \
                                                                                                    \
template <library_type VerifiedBasisType, library_type OtherBasis>                                  \
constexpr auto operator op(const verified_type_basis<VerifiedBasisType> lhs,                        \
                           const OtherBasis rhs) -> VerifiedBasisType                               \
{                                                                                                   \
    return static_cast<VerifiedBasisType>(lhs) op rhs;                                              \
}                                                                                                   \
                                                                                                    \
template <library_type OtherBasis, library_type VerifiedBasisType>                                  \
constexpr auto operator op(const OtherBasis lhs,                                                    \
                           const verified_type_basis<VerifiedBasisType> rhs) -> VerifiedBasisType   \
{                                                                                                   \
    return static_cast<VerifiedBasisType>(lhs) op rhs;                                              \
}

BOOST_SAFE_NUMBERS_MIXED_VERIFIED_OPERATOR(+)
BOOST_SAFE_NUMBERS_MIXED_VERIFIED_OPERATOR(-)
BOOST_SAFE_NUMBERS_MIXED_VERIFIED_OPERATOR(*)
BOOST_SAFE_NUMBERS_MIXED_VERIFIED_OPERATOR(/)
BOOST_SAFE_NUMBERS_MIXED_VERIFIED_OPERATOR(%)

#undef BOOST_SAFE_NUMBERS_MIXED_VERIFIED_OPERATOR

// Bitwise mixed operators (&, |, ^) need separate treatment because the native
// unsigned_integer_basis bitwise operators live in boost::safe_numbers (not detail),
// so ADL cannot find them from within detail. We operate on raw values directly.

#define BOOST_SAFE_NUMBERS_MIXED_VERIFIED_BITWISE_OPERATOR(op)                                      \
template <non_bounded_unsigned_library_type VerifiedBasisType>                                      \
constexpr auto operator op(const verified_type_basis<VerifiedBasisType> lhs,                        \
                           const VerifiedBasisType rhs) -> VerifiedBasisType                        \
{                                                                                                   \
    using raw = underlying_type_t<VerifiedBasisType>;                                               \
    return VerifiedBasisType{static_cast<raw>(                                                      \
        raw_value(static_cast<VerifiedBasisType>(lhs)) op raw_value(rhs))};                         \
}                                                                                                   \
                                                                                                    \
template <non_bounded_unsigned_library_type VerifiedBasisType>                                      \
constexpr auto operator op(const VerifiedBasisType lhs,                                             \
                           const verified_type_basis<VerifiedBasisType> rhs) -> VerifiedBasisType   \
{                                                                                                   \
    using raw = underlying_type_t<VerifiedBasisType>;                                               \
    return VerifiedBasisType{static_cast<raw>(                                                      \
        raw_value(lhs) op raw_value(static_cast<VerifiedBasisType>(rhs)))};                         \
}

BOOST_SAFE_NUMBERS_MIXED_VERIFIED_BITWISE_OPERATOR(&)
BOOST_SAFE_NUMBERS_MIXED_VERIFIED_BITWISE_OPERATOR(|)
BOOST_SAFE_NUMBERS_MIXED_VERIFIED_BITWISE_OPERATOR(^)

#undef BOOST_SAFE_NUMBERS_MIXED_VERIFIED_BITWISE_OPERATOR

// Shift mixed operators (<<, >>) call shl_impl/shr_impl directly (in detail namespace)

template <non_bounded_unsigned_library_type VerifiedBasisType>
constexpr auto operator<<(const verified_type_basis<VerifiedBasisType> lhs,
                          const VerifiedBasisType rhs) -> VerifiedBasisType
{
    return shl_impl<overflow_policy::throw_exception>(static_cast<VerifiedBasisType>(lhs), rhs);
}

template <non_bounded_unsigned_library_type VerifiedBasisType>
constexpr auto operator<<(const VerifiedBasisType lhs,
                          const verified_type_basis<VerifiedBasisType> rhs) -> VerifiedBasisType
{
    return shl_impl<overflow_policy::throw_exception>(lhs, static_cast<VerifiedBasisType>(rhs));
}

template <non_bounded_unsigned_library_type VerifiedBasisType>
constexpr auto operator>>(const verified_type_basis<VerifiedBasisType> lhs,
                          const VerifiedBasisType rhs) -> VerifiedBasisType
{
    return shr_impl<overflow_policy::throw_exception>(static_cast<VerifiedBasisType>(lhs), rhs);
}

template <non_bounded_unsigned_library_type VerifiedBasisType>
constexpr auto operator>>(const VerifiedBasisType lhs,
                          const verified_type_basis<VerifiedBasisType> rhs) -> VerifiedBasisType
{
    return shr_impl<overflow_policy::throw_exception>(lhs, static_cast<VerifiedBasisType>(rhs));
}

// Separate implementations for the comparisons since we can't shoehorn them into the macros above

template <library_type VerifiedBasisType>
constexpr auto operator<=>(const verified_type_basis<VerifiedBasisType> lhs,
                           const VerifiedBasisType rhs) -> std::strong_ordering
{
    return static_cast<VerifiedBasisType>(lhs) <=> rhs;
}

template <library_type VerifiedBasisType>
constexpr auto operator==(const verified_type_basis<VerifiedBasisType> lhs,
                          const VerifiedBasisType rhs) -> bool
{
    return static_cast<VerifiedBasisType>(lhs) == rhs;
}

template <library_type VerifiedBasisType>
constexpr auto operator<=>(const VerifiedBasisType lhs,
                           const verified_type_basis<VerifiedBasisType> rhs) -> std::strong_ordering
{
    return lhs <=> static_cast<VerifiedBasisType>(rhs);
}

template <library_type VerifiedBasisType>
constexpr auto operator==(const VerifiedBasisType lhs,
                          const verified_type_basis<VerifiedBasisType> rhs) -> bool
{
    return lhs == static_cast<VerifiedBasisType>(rhs);
}

template <library_type VerifiedBasisType, library_type OtherBasis>
constexpr auto operator<=>(const verified_type_basis<VerifiedBasisType> lhs,
                           const OtherBasis rhs) -> std::strong_ordering
{
    return static_cast<VerifiedBasisType>(lhs) <=> rhs;
}

template <library_type VerifiedBasisType, library_type OtherBasis>
constexpr auto operator==(const verified_type_basis<VerifiedBasisType> lhs,
                          const OtherBasis rhs) -> bool
{
    return static_cast<VerifiedBasisType>(lhs) == rhs;
}

template <library_type OtherBasis, library_type VerifiedBasisType>
constexpr auto operator<=>(const OtherBasis lhs,
                           const verified_type_basis<VerifiedBasisType> rhs) -> std::strong_ordering
{
    return lhs <=> static_cast<VerifiedBasisType>(rhs);
}

template <library_type OtherBasis, library_type VerifiedBasisType>
constexpr auto operator==(const OtherBasis lhs,
                          const verified_type_basis<VerifiedBasisType> rhs) -> bool
{
    return lhs == static_cast<VerifiedBasisType>(rhs);
}

} // namespace boost::safe_numbers::detail

#endif // BOOST_SAFE_NUMBERS_VERIFIED_INTEGER_BASIS_HPP

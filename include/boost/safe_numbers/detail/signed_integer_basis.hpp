// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_SIGNED_INTEGER_BASIS_HPP
#define BOOST_SAFE_NUMBERS_SIGNED_INTEGER_BASIS_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>
#include <boost/safe_numbers/overflow_policy.hpp>
#include <boost/safe_numbers/detail/int128/bit.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/throw_exception.hpp>
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

template <fundamental_signed_integral BasisType>
template <fundamental_signed_integral OtherBasis>
constexpr signed_integer_basis<BasisType>::operator OtherBasis() const
{
    if constexpr (sizeof(OtherBasis) < sizeof(BasisType))
    {
        if (basis_ > static_cast<BasisType>(std::numeric_limits<OtherBasis>::max()))
        {
            BOOST_THROW_EXCEPTION(std::domain_error(std::string("Overflow in ") + signed_type_name<BasisType>() + " to " + signed_type_name<OtherBasis>() + " conversion"));
        }
        else if (basis_ < static_cast<BasisType>(std::numeric_limits<OtherBasis>::min()))
        {
            BOOST_THROW_EXCEPTION(std::domain_error(std::string("Underflow in ") + signed_type_name<BasisType>() + " to " + signed_type_name<OtherBasis>() + " conversion"));
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
        BOOST_THROW_EXCEPTION(std::domain_error(std::string("Overflow in ") + signed_type_name<BasisType>() + " unary minus operator"));
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
using make_unsigned_helper_t = make_unsigned_helper<T>::type;

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
    const bool has_overflow {((~(ulhs ^ urhs)) & (ulhs ^ temp)) >> std::numeric_limits<T>::digits};

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
                    const auto message {std::string(status == signed_overflow_status::overflow ? "Overflow" : "Underflow")
                                        + " detected in " + signed_type_name<BasisType>() + " addition"};

                    if (status == signed_overflow_status::overflow)
                    {
                        BOOST_THROW_EXCEPTION(std::overflow_error(message));
                    }
                    else
                    {
                        BOOST_THROW_EXCEPTION(std::underflow_error(message));
                    }
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

template <overflow_policy Policy, fundamental_signed_integral BasisType>
[[nodiscard]] constexpr auto add_impl(const signed_integer_basis<BasisType> lhs,
                                      const signed_integer_basis<BasisType> rhs)
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

} // namespace boost::safe_numbers::detail

#undef BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP

#endif //BOOST_SAFE_NUMBERS_SIGNED_INTEGER_BASIS_HPP

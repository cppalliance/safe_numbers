// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BASIS_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BASIS_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>
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

template <compatible_float_type BasisType>
class float_basis
{
public:

    // This is exposed to the user so that they can convert back to built-in
    using basis_type = BasisType;

private:

    BasisType basis_ {};

public:

    constexpr float_basis() noexcept = default;

    BOOST_SAFE_NUMBERS_HOST_DEVICE explicit constexpr float_basis(const BasisType val) noexcept : basis_{val} {}

    // Reject construction from any non-basis type
    // This eliminates any potential narrowing
    template <typename T>
        requires (!std::is_same_v<T, BasisType>)
    BOOST_SAFE_NUMBERS_HOST_DEVICE explicit constexpr float_basis(T) noexcept = delete;

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] explicit constexpr operator BasisType() const noexcept { return basis_; }

    // Defined manually so -Wfloat-equal stays contained here rather than
    // leaking into user code through a defaulted operator==.
    // We want the behavior to generally match the built-ins,
    // outside of the exceptional cases
    #ifdef __clang__
    #  pragma clang diagnostic push
    #  pragma clang diagnostic ignored "-Wfloat-equal"
    #elif defined(__GNUC__)
    #  pragma GCC diagnostic push
    #  pragma GCC diagnostic ignored "-Wfloat-equal"
    #endif

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] friend constexpr auto operator==(float_basis lhs, float_basis rhs) noexcept -> bool
    {
        return lhs.basis_ == rhs.basis_;
    }

    #ifdef __clang__
    #  pragma clang diagnostic pop
    #elif defined(__GNUC__)
    #  pragma GCC diagnostic pop
    #endif

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] friend constexpr auto operator<=>(float_basis lhs, float_basis rhs) noexcept -> std::partial_ordering = default;
};

// Helper to map BasisType to a short name for diagnostic messages.
template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto float_type_name() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, float>)
    {
        return "f32";
    }
    else
    {
        return "f64";
    }
}

// Device-friendly error message helpers returning const char* string literals
// These avoid std::string concatenation which is not available on CUDA device

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto overflow_add_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, float>)
    {
        return "Overflow detected in f32 addition";
    }
    else
    {
        return "Overflow detected in f64 addition";
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto underflow_add_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, float>)
    {
        return "Underflow detected in f32 addition";
    }
    else
    {
        return "Underflow detected in f64 addition";
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto nan_add_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, float>)
    {
        return "Operation with NAN detected in f32 addition";
    }
    else
    {
        return "Operation with NAN detected in f64 addition";
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto invalid_add_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, float>)
    {
        return "Invalid operation (IEEE 754-2008 section 7.2) detected in f32 addition";
    }
    else
    {
        return "Invalid operation (IEEE 754-2008 section 7.2) detected in f64 addition";
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto overflow_sub_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, float>)
    {
        return "Overflow detected in f32 subtraction";
    }
    else
    {
        return "Overflow detected in f64 subtraction";
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto underflow_sub_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, float>)
    {
        return "Underflow detected in f32 subtraction";
    }
    else
    {
        return "Underflow detected in f64 subtraction";
    }
}

// ------------------------------
// Helper <cmath> functions
// ------------------------------

namespace impl {

template <compatible_float_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto constexpr_abs(const T val) noexcept -> T
{
    return val < 0 ? -val : val;
}

template <compatible_float_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto constexpr_isinf(const T val) noexcept -> bool
{
    return val > std::numeric_limits<T>::max();
}

template <compatible_float_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto constexpr_isnan(const T val) noexcept -> bool
{
    return val != val;
}

template <compatible_float_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto constexpr_issignaling([[maybe_unused]] const T val) noexcept -> bool
{
    if constexpr (std::numeric_limits<T>::has_signaling_NaN)
    {
        using bit_type = std::conditional_t<std::is_same_v<T, float>, std::uint32_t, std::uint64_t>;

        constexpr auto signal_bits {std::bit_cast<bit_type>(std::numeric_limits<T>::signaling_NaN())};
        constexpr auto quiet_bits {std::bit_cast<bit_type>(std::numeric_limits<T>::quiet_NaN())};

        if constexpr (signal_bits == quiet_bits)
        {
            return false;
        }
        else
        {
            const auto val_bits {std::bit_cast<bit_type>(val)};

            // TODO(mborland) : This doesn't handle payloads
            return signal_bits == val_bits;
        }
    }
    else
    {
        return false;
    }
}

template <compatible_float_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto constexpr_isnormal(const T val) noexcept -> bool
{
    return !(val == T{} || constexpr_isinf(val) || constexpr_isnan(val) || constexpr_abs(val) < std::numeric_limits<T>::min());
}

template <compatible_float_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto constexpr_fpclassify(const T val) noexcept -> int
{
    if (constexpr_isnormal(val))
    {
        return FP_NAN;
    }
    else if (constexpr_isinf(val))
    {
        return FP_INFINITE;
    }
    else if (val == T{})
    {
        return FP_ZERO;
    }
    else if (constexpr_abs(val) < std::numeric_limits<T>::min())
    {
        return FP_SUBNORMAL;
    }
    else
    {
        return FP_NORMAL;
    }
}

enum class error_category
{
    no_error,
    overflow,
    underflow,
    divide_by_zero,
    nan_op,
    invalid_op
};

} // namespace impl

// ------------------------------
// Addition
// ------------------------------

namespace impl {

// Follows the conventions from IEEE 754 section 6 and 7 on what should happen with mixed non-finite operation:
//   1) Saturation to positive infinity -> Overflow
//   2) Saturation to negative infinity -> Underflow
//   3) Any operation with a QNAN       -> Nan Op
//   4) Add infs of differing sign      -> Invalid Op
//   5) Any operations with an SNAN     -> Invalid Op
template <compatible_float_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] auto checked_float_addition(const T lhs, const T rhs, T& res) -> error_category
{
    res = lhs + rhs;

    // The hot path is that our addition has nothing funny happening
    if (!constexpr_isinf(res) && !constexpr_isnan(res)) [[likely]]
    {
        return error_category::no_error;
    }

    // If the result is not normal, now we have to figure out why
    // Start with section 7.2 invalid ops
    // 7.2.a: any general computation on a signaling NAN
    if (constexpr_issignaling(lhs) || constexpr_issignaling(rhs))
    {
        return error_category::invalid_op;
    }
    // 7.2.d: addition or subtraction or FMA: magnitude subtraction of infinities
    if (constexpr_isinf(lhs) && constexpr_isinf(rhs) && (lhs < 0 != rhs < 0))
    {
        return error_category::invalid_op;
    }

    // Now the regular cases from chapter 6.
    // Section 6.2: Operations with NAN yield NAN
    if (constexpr_isnan(lhs) || constexpr_isnan(rhs))
    {
        return error_category::nan_op;
    }
    // Section 6.1: Infinity Arithmetic
    else if (constexpr_isinf(res))
    {
        return res > 0 ? error_category::overflow : error_category::underflow;
    }

    BOOST_SAFE_NUMBERS_UNREACHABLE; // LCOV_EXCL_LINE
}

}

template <overflow_policy Policy, compatible_float_type BasisType>
struct float_add_helper
{
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const float_basis<BasisType> lhs,
                                              const float_basis<BasisType> rhs)
        noexcept(Policy != overflow_policy::throw_exception) -> float_basis<BasisType>
    {
        using result_type = float_basis<BasisType>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        [[maybe_unused]] BasisType res {};

        const auto handle_overflow = []
        {
            #if !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))
            if (std::is_constant_evaluated())
            {
                if constexpr (std::is_same_v<BasisType, float>)
                {
                    throw std::overflow_error("Overflow detected in f32 addition");
                }
                else
                {
                    throw std::overflow_error("Overflow detected in f64 addition");
                }
            }
            else
            #endif
            {
                if constexpr (Policy == overflow_policy::throw_exception)
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, overflow_add_msg<BasisType>());
                }
                else
                {
                    BOOST_SAFE_NUMBERS_UNREACHABLE;
                }
            }
        };

        const auto handle_underflow = []
        {
            #if !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))
            if (std::is_constant_evaluated())
            {
                if constexpr (std::is_same_v<BasisType, float>)
                {
                    throw std::underflow_error("Overflow detected in f32 addition");
                }
                else
                {
                    throw std::underflow_error("Overflow detected in f64 addition");
                }
            }
            else
            #endif
            {
                if constexpr (Policy == overflow_policy::throw_exception)
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, underflow_add_msg<BasisType>());
                }
                else
                {
                    BOOST_SAFE_NUMBERS_UNREACHABLE;
                }
            }
        };

        const auto handle_nan = []
        {
            #if !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))
            if (std::is_constant_evaluated())
            {
                if constexpr (std::is_same_v<BasisType, float>)
                {
                    throw std::domain_error("Operation with NAN detected in f32 addition");
                }
                else
                {
                    throw std::domain_error("Operation with NAN detected in f64 addition");
                }
            }
            else
            #endif
            {
                if constexpr (Policy == overflow_policy::throw_exception)
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, nan_add_msg<BasisType>());
                }
                else
                {
                    BOOST_SAFE_NUMBERS_UNREACHABLE;
                }
            }
        };

        const auto handle_invalid = []
        {
            #if !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA))
            if (std::is_constant_evaluated())
            {
                if constexpr (std::is_same_v<BasisType, float>)
                {
                    throw std::domain_error("Invalid operation (IEEE 754-2008 section 7.2) detected in f32 addition");
                }
                else
                {
                    throw std::domain_error("Invalid operation (IEEE 754-2008 section 7.2) detected in f65 addition");
                }
            }
            else
                #endif
            {
                if constexpr (Policy == overflow_policy::throw_exception)
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, invalid_add_msg<BasisType>());
                }
                else
                {
                    BOOST_SAFE_NUMBERS_UNREACHABLE;
                }
            }
        };

        switch (impl::checked_float_addition(lhs_basis, rhs_basis, res))
        {
            case impl::error_category::no_error:
                break;
            case impl::error_category::overflow:
                handle_overflow();
                break;
            case impl::error_category::underflow:
                handle_underflow();
                break;
            case impl::error_category::nan_op:
                handle_nan();
                break;
            case impl::error_category::overflow:
                handle_invalid();
                break;
            case impl::error_category::divide_by_zero:
                BOOST_SAFE_NUMBERS_UNREACHABLE; // LCOV_EXCL_LINE
                break;                          // LCOV_EXCL_LINE
            default:
                BOOST_SAFE_NUMBERS_UNREACHABLE; // LCOV_EXCL_LINE
                break;                          // LCOV_EXCL_LINE
        }

        return result_type{res};
    }
};

} // namespace boost::safe_numbers::detail

#endif // BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BASIS_HPP

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
#include <bit>
#include <concepts>
#include <compare>
#include <limits>
#include <stdexcept>
#include <string>
#include <cstdint>
#include <cstdlib>
#include <utility>
#include <optional>
#include <cmath>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers::detail {

template <compatible_float_type BasisType, auto ErrorPolicy>
class float_basis
{
public:

    // This is exposed to the user so that they can convert back to built-in
    using basis_type = BasisType;

    static_assert(is_overflow_policy_v<ErrorPolicy> || error_handler_for<decltype(ErrorPolicy), BasisType>,
                  "ErrorPolicy must be a boost::safe_numbers::overflow_policy enumerator or a stateless "
                  "handler type providing on_error(error_kind, BasisType, const char*) returning BasisType");

    static_assert(!is_value_returning_policy<ErrorPolicy>(),
                  "overflow_tuple, checked, and widen change the result type of every operation, "
                  "so they can not be type-level policies: use the overflowing_* free functions instead");

    static_assert(!is_overflow_policy_v<ErrorPolicy> || is_valid_type_policy<ErrorPolicy>(basis_kind::floating_point),
                  "float_basis supports overflow_policy::throw_exception and overflow_policy::saturate; "
                  "strict is integer only");

    // Exposed so that generic code and tests can query the type-level policy
    static constexpr auto error_policy {ErrorPolicy};

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

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto nan_sub_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, float>)
    {
        return "Operation with NAN detected in f32 subtraction";
    }
    else
    {
        return "Operation with NAN detected in f64 subtraction";
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto invalid_sub_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, float>)
    {
        return "Invalid operation (IEEE 754-2008 section 7.2) detected in f32 subtraction";
    }
    else
    {
        return "Invalid operation (IEEE 754-2008 section 7.2) detected in f64 subtraction";
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto overflow_mul_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, float>)
    {
        return "Overflow detected in f32 multiplication";
    }
    else
    {
        return "Overflow detected in f64 multiplication";
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto underflow_mul_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, float>)
    {
        return "Underflow detected in f32 multiplication";
    }
    else
    {
        return "Underflow detected in f64 multiplication";
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto nan_mul_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, float>)
    {
        return "Operation with NAN detected in f32 multiplication";
    }
    else
    {
        return "Operation with NAN detected in f64 multiplication";
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto invalid_mul_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, float>)
    {
        return "Invalid operation (IEEE 754-2008 section 7.2) detected in f32 multiplication";
    }
    else
    {
        return "Invalid operation (IEEE 754-2008 section 7.2) detected in f64 multiplication";
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto overflow_div_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, float>)
    {
        return "Overflow detected in f32 division";
    }
    else
    {
        return "Overflow detected in f64 division";
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto underflow_div_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, float>)
    {
        return "Underflow detected in f32 division";
    }
    else
    {
        return "Underflow detected in f64 division";
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto nan_div_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, float>)
    {
        return "Operation with NAN detected in f32 division";
    }
    else
    {
        return "Operation with NAN detected in f64 division";
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto invalid_div_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, float>)
    {
        return "Invalid operation (IEEE 754-2008 section 7.2) detected in f32 division";
    }
    else
    {
        return "Invalid operation (IEEE 754-2008 section 7.2) detected in f64 division";
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto divbyzero_div_msg() noexcept -> const char*
{
    if constexpr (std::is_same_v<BasisType, float>)
    {
        return "Division by zero detected in f32 division";
    }
    else
    {
        return "Division by zero detected in f64 division";
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

// Two-sided compare rather than constexpr_abs(val) > max()
template <compatible_float_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto constexpr_isinf(const T val) noexcept -> bool
{
    return (val > std::numeric_limits<T>::max()) || (val < std::numeric_limits<T>::lowest());
}

// val != val is the canonical NAN test
#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wfloat-equal"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

template <compatible_float_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto constexpr_isnan(const T val) noexcept -> bool
{
    return val != val;
}

#ifdef __clang__
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#endif

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
            // SNAN bit patterns sit in the range (inf, QNAN) once the sign bit is stripped.
            // Stripping the sign covers SNANs of either sign,
            // and the range covers any payload value.
            constexpr bit_type sign_mask {bit_type{1} << (std::numeric_limits<bit_type>::digits - 1)};
            constexpr auto inf_abs {std::bit_cast<bit_type>(std::numeric_limits<T>::infinity()) & ~sign_mask};
            constexpr auto quiet_abs {static_cast<bit_type>(quiet_bits & ~sign_mask)};

            const auto val_bits {std::bit_cast<bit_type>(val)};
            const auto val_abs {static_cast<bit_type>(val_bits & ~sign_mask)};

            return val_abs > inf_abs && val_abs < quiet_abs;
        }
    }
    else
    {
        return false;
    }
}

// Bit-pattern test for true zero, used in IEEE 754 error classification where
// a subnormal must not be confused with zero, such as with optimized builds on the Intel Compiler.
// Defined before its users so they can avoid an unguarded -Wfloat-equal comparison.
#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wfloat-equal"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

template <compatible_float_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto is_true_zero(const T val) noexcept -> bool
{
#if defined(__INTEL_COMPILER) || defined(__INTEL_LLVM_COMPILER)
    using bit_type = std::conditional_t<std::is_same_v<T, float>, std::uint32_t, std::uint64_t>;
    constexpr bit_type sign_mask {bit_type{1} << (std::numeric_limits<bit_type>::digits - 1)};
    const auto bits {std::bit_cast<bit_type>(val)};
    return static_cast<bit_type>(bits & ~sign_mask) == bit_type{0};
#else
    return val == T{};
#endif
}

#ifdef __clang__
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#endif

template <compatible_float_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto constexpr_isnormal(const T val) noexcept -> bool
{
    return !(is_true_zero(val) || constexpr_isinf(val) || constexpr_isnan(val) || constexpr_abs(val) < std::numeric_limits<T>::min());
}

template <compatible_float_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto constexpr_fpclassify(const T val) noexcept -> int
{
    if (constexpr_isnan(val))
    {
        return FP_NAN;
    }
    else if (constexpr_isinf(val))
    {
        return FP_INFINITE;
    }
    else if (is_true_zero(val))
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

// ------------------------------
// Shared <cmath> result validation
// ------------------------------
// These helpers are reused by the free <cmath> wrappers (cmath.hpp) so the
// wrappers preserve the same class invariant as the arithmetic operators:
// an operation never silently yields a non-finite value.

// Sign-bit test that is correct for signed zero and NAN, where val < 0 is not.
template <compatible_float_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto constexpr_signbit(const T val) noexcept -> bool
{
    using bit_type = std::conditional_t<std::is_same_v<T, float>, std::uint32_t, std::uint64_t>;
    constexpr bit_type sign_mask {bit_type{1} << (std::numeric_limits<bit_type>::digits - 1)};
    return static_cast<bit_type>(std::bit_cast<bit_type>(val) & sign_mask) != bit_type{0};
}

// Classify a finished <cmath> result using the same IEEE 754 conventions as the
// arithmetic operators: a finite result is fine, a NAN is a domain violation, and
// an infinity is an overflow (+inf) or an underflow (-inf).
template <compatible_float_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto classify_cmath_result(const T res) noexcept -> error_category
{
    if (!constexpr_isinf(res) && !constexpr_isnan(res)) [[likely]]
    {
        return error_category::no_error;
    }

    if (constexpr_isnan(res))
    {
        return error_category::nan_op;
    }

    return res > 0 ? error_category::overflow : error_category::underflow;
}

#if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)

// Host-only descriptive runtime message, e.g. "Overflow detected in f64 sqrt".
template <compatible_float_type BasisType>
[[nodiscard]] inline auto cmath_error_msg(const char* const category, const char* const op) -> std::string
{
    return std::string{category} + " detected in " + float_type_name<BasisType>() + ' ' + op;
}

#endif

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto throw_cmath_overflow(const char* const op) -> void
{
    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
    if (std::is_constant_evaluated())
    {
        if constexpr (std::is_same_v<BasisType, float>)
        {
            throw std::overflow_error("Overflow detected in f32 <cmath> function"); // LCOV_EXCL_LINE
        }
        else
        {
            throw std::overflow_error("Overflow detected in f64 <cmath> function"); // LCOV_EXCL_LINE
        }
    }
    else
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, cmath_error_msg<BasisType>("Overflow", op));
    }
    #else
    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, op);
    #endif
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto throw_cmath_underflow(const char* const op) -> void
{
    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
    if (std::is_constant_evaluated())
    {
        if constexpr (std::is_same_v<BasisType, float>)
        {
            throw std::underflow_error("Underflow detected in f32 <cmath> function"); // LCOV_EXCL_LINE
        }
        else
        {
            throw std::underflow_error("Underflow detected in f64 <cmath> function"); // LCOV_EXCL_LINE
        }
    }
    else
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, cmath_error_msg<BasisType>("Underflow", op));
    }
    #else
    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, op);
    #endif
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto throw_cmath_domain(const char* const op) -> void
{
    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
    if (std::is_constant_evaluated())
    {
        if constexpr (std::is_same_v<BasisType, float>)
        {
            throw std::domain_error("Domain error detected in f32 <cmath> function"); // LCOV_EXCL_LINE
        }
        else
        {
            throw std::domain_error("Domain error detected in f64 <cmath> function"); // LCOV_EXCL_LINE
        }
    }
    else
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, cmath_error_msg<BasisType>("Domain error", op));
    }
    #else
    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, op);
    #endif
}

// Validate a <cmath> result against the float_basis invariant: returns it when finite,
// otherwise throws (+inf -> overflow_error, -inf -> underflow_error, NAN -> domain_error).
// op names the calling function for diagnostics.
template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto check_cmath_result(const BasisType res, const char* const op) -> BasisType
{
    switch (classify_cmath_result(res))
    {
        case error_category::no_error:
            break;
        case error_category::overflow:
            throw_cmath_overflow<BasisType>(op);
            break;
        case error_category::underflow:
            throw_cmath_underflow<BasisType>(op);
            break;
        case error_category::nan_op:
        case error_category::invalid_op:
        case error_category::divide_by_zero:
            throw_cmath_domain<BasisType>(op);
            break;
        default:
            BOOST_SAFE_NUMBERS_UNREACHABLE; // LCOV_EXCL_LINE
            break;                          // LCOV_EXCL_LINE
    }

    return res;
}

} // namespace impl

// ------------------------------
// Addition
// ------------------------------

namespace impl {

// It's ok that we overflow since we will check post-op
#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable:4756)
#endif

// Follows the conventions from IEEE 754 section 6 and 7 on what should happen with mixed non-finite operation:
//   1) Saturation to positive infinity -> Overflow
//   2) Saturation to negative infinity -> Underflow
//   3) Any operation with a QNAN       -> Nan Op
//   4) Add infs of differing sign      -> Invalid Op
//   5) Any operations with an SNAN     -> Invalid Op
template <compatible_float_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto checked_float_addition(const T lhs, const T rhs, T& res) -> error_category
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
    if (constexpr_isinf(lhs) && constexpr_isinf(rhs) && ((lhs < 0) != (rhs < 0)))
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

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto throw_overflow_add() -> void
{
    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
    if (std::is_constant_evaluated())
    {
        if constexpr (std::is_same_v<BasisType, float>)
        {
            throw std::overflow_error("Overflow detected in f32 addition"); // LCOV_EXCL_LINE
        }
        else
        {
            throw std::overflow_error("Overflow detected in f64 addition"); // LCOV_EXCL_LINE
        }
    }
    else
    #endif
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, overflow_add_msg<BasisType>());
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto throw_underflow_add() -> void
{
    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
    if (std::is_constant_evaluated())
    {
        if constexpr (std::is_same_v<BasisType, float>)
        {
            throw std::underflow_error("Underflow detected in f32 addition"); // LCOV_EXCL_LINE
        }
        else
        {
            throw std::underflow_error("Underflow detected in f64 addition"); // LCOV_EXCL_LINE
        }
    }
    else
    #endif
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, underflow_add_msg<BasisType>());
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto throw_nan_add() -> void
{
    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
    if (std::is_constant_evaluated())
    {
        if constexpr (std::is_same_v<BasisType, float>)
        {
            throw std::domain_error("Operation with NAN detected in f32 addition"); // LCOV_EXCL_LINE
        }
        else
        {
            throw std::domain_error("Operation with NAN detected in f64 addition"); // LCOV_EXCL_LINE
        }
    }
    else
    #endif
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, nan_add_msg<BasisType>());
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto throw_invalid_add() -> void
{
    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
    if (std::is_constant_evaluated())
    {
        if constexpr (std::is_same_v<BasisType, float>)
        {
            throw std::domain_error("Invalid operation (IEEE 754-2008 section 7.2) detected in f32 addition"); // LCOV_EXCL_LINE
        }
        else
        {
            throw std::domain_error("Invalid operation (IEEE 754-2008 section 7.2) detected in f64 addition"); // LCOV_EXCL_LINE
        }
    }
    else
    #endif
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, invalid_add_msg<BasisType>());
    }
}

} // namespace impl

template <compatible_float_type BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto operator+(const float_basis<BasisType, ErrorPolicy> lhs,
                                       const float_basis<BasisType, ErrorPolicy> rhs)
    noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> float_basis<BasisType, ErrorPolicy>
{
    const auto lhs_basis {static_cast<BasisType>(lhs)};
    const auto rhs_basis {static_cast<BasisType>(rhs)};

    if constexpr (policy_equals<ErrorPolicy>(overflow_policy::saturate))
    {
        // Raw IEEE 754 semantics: overflow saturates to infinity and NaN propagates.
        // Same expression as the value component of overflowing_add.
        return float_basis<BasisType, ErrorPolicy>{static_cast<BasisType>(lhs_basis + rhs_basis)};
    }
    else if constexpr (is_user_handler_v<ErrorPolicy>)
    {
        // The classifier stores the raw IEEE 754 result, which is the defined
        // fallback value handed to the handler
        BasisType res {};
        switch (impl::checked_float_addition(lhs_basis, rhs_basis, res))
        {
            case impl::error_category::overflow:
                res = ErrorPolicy.on_error(error_kind::overflow, res, overflow_add_msg<BasisType>());
                break;
            case impl::error_category::underflow:
                res = ErrorPolicy.on_error(error_kind::underflow, res, underflow_add_msg<BasisType>());
                break;
            case impl::error_category::nan_op:
                res = ErrorPolicy.on_error(error_kind::nan_operation, res, nan_add_msg<BasisType>());
                break;
            case impl::error_category::invalid_op:
                res = ErrorPolicy.on_error(error_kind::invalid_operation, res, invalid_add_msg<BasisType>());
                break;
            default:
                break;
        }
        return float_basis<BasisType, ErrorPolicy>{res};
    }
    else
    {
        [[maybe_unused]] BasisType res {};

        // The throw branches are inlined here (rather than calling impl::throw_*_add)
        // so BOOST_THROW_EXCEPTION captures operator+ as the source location of the throw.
        switch (impl::checked_float_addition(lhs_basis, rhs_basis, res))
        {
            case impl::error_category::no_error:
                break;
            case impl::error_category::overflow:
                #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
                if (std::is_constant_evaluated())
                {
                    if constexpr (std::is_same_v<BasisType, float>)
                    {
                        throw std::overflow_error("Overflow detected in f32 addition"); // LCOV_EXCL_LINE
                    }
                    else
                    {
                        throw std::overflow_error("Overflow detected in f64 addition"); // LCOV_EXCL_LINE
                    }
                }
                else
                #endif
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, overflow_add_msg<BasisType>());
                }
                break;
            case impl::error_category::underflow:
                #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
                if (std::is_constant_evaluated())
                {
                    if constexpr (std::is_same_v<BasisType, float>)
                    {
                        throw std::underflow_error("Underflow detected in f32 addition"); // LCOV_EXCL_LINE
                    }
                    else
                    {
                        throw std::underflow_error("Underflow detected in f64 addition"); // LCOV_EXCL_LINE
                    }
                }
                else
                #endif
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, underflow_add_msg<BasisType>());
                }
                break;
            case impl::error_category::nan_op:
                #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
                if (std::is_constant_evaluated())
                {
                    if constexpr (std::is_same_v<BasisType, float>)
                    {
                        throw std::domain_error("Operation with NAN detected in f32 addition"); // LCOV_EXCL_LINE
                    }
                    else
                    {
                        throw std::domain_error("Operation with NAN detected in f64 addition"); // LCOV_EXCL_LINE
                    }
                }
                else
                #endif
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, nan_add_msg<BasisType>());
                }
                break;
            case impl::error_category::invalid_op:
                #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
                if (std::is_constant_evaluated())
                {
                    if constexpr (std::is_same_v<BasisType, float>)
                    {
                        throw std::domain_error("Invalid operation (IEEE 754-2008 section 7.2) detected in f32 addition"); // LCOV_EXCL_LINE
                    }
                    else
                    {
                        throw std::domain_error("Invalid operation (IEEE 754-2008 section 7.2) detected in f64 addition"); // LCOV_EXCL_LINE
                    }
                }
                else
                #endif
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, invalid_add_msg<BasisType>());
                }
                break;
            case impl::error_category::divide_by_zero:
                BOOST_SAFE_NUMBERS_UNREACHABLE; // LCOV_EXCL_LINE
                break;                          // LCOV_EXCL_LINE
            default:
                BOOST_SAFE_NUMBERS_UNREACHABLE; // LCOV_EXCL_LINE
                break;                          // LCOV_EXCL_LINE
        }

        return float_basis<BasisType, ErrorPolicy>{res};
    }
}

// ------------------------------
// Subtraction
// ------------------------------

namespace impl {

// It's ok that we overflow since we will check post-op
#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable:4756)
#endif

// See comment above on checked_float_add
template <compatible_float_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto checked_float_subtraction(const T lhs, const T rhs, T& res) -> error_category
{
    res = lhs - rhs;

    // The hot path is that our subtraction has nothing funny happening
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
    // 7.2.d: subtraction of same-sign infinities cancels to NaN
    if (constexpr_isinf(lhs) && constexpr_isinf(rhs) && (lhs < 0) == (rhs < 0))
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

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto throw_overflow_sub() -> void
{
    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
    if (std::is_constant_evaluated())
    {
        if constexpr (std::is_same_v<BasisType, float>)
        {
            throw std::overflow_error("Overflow detected in f32 subtraction"); // LCOV_EXCL_LINE
        }
        else
        {
            throw std::overflow_error("Overflow detected in f64 subtraction"); // LCOV_EXCL_LINE
        }
    }
    else
    #endif
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, overflow_sub_msg<BasisType>());
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto throw_underflow_sub() -> void
{
    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
    if (std::is_constant_evaluated())
    {
        if constexpr (std::is_same_v<BasisType, float>)
        {
            throw std::underflow_error("Underflow detected in f32 subtraction"); // LCOV_EXCL_LINE
        }
        else
        {
            throw std::underflow_error("Underflow detected in f64 subtraction"); // LCOV_EXCL_LINE
        }
    }
    else
    #endif
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, underflow_sub_msg<BasisType>());
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto throw_nan_sub() -> void
{
    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
    if (std::is_constant_evaluated())
    {
        if constexpr (std::is_same_v<BasisType, float>)
        {
            throw std::domain_error("Operation with NAN detected in f32 subtraction"); // LCOV_EXCL_LINE
        }
        else
        {
            throw std::domain_error("Operation with NAN detected in f64 subtraction"); // LCOV_EXCL_LINE
        }
    }
    else
    #endif
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, nan_sub_msg<BasisType>());
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto throw_invalid_sub() -> void
{
    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
    if (std::is_constant_evaluated())
    {
        if constexpr (std::is_same_v<BasisType, float>)
        {
            throw std::domain_error("Invalid operation (IEEE 754-2008 section 7.2) detected in f32 subtraction"); // LCOV_EXCL_LINE
        }
        else
        {
            throw std::domain_error("Invalid operation (IEEE 754-2008 section 7.2) detected in f64 subtraction"); // LCOV_EXCL_LINE
        }
    }
    else
    #endif
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, invalid_sub_msg<BasisType>());
    }
}

} // namespace impl

template <compatible_float_type BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto operator-(const float_basis<BasisType, ErrorPolicy> lhs,
                                       const float_basis<BasisType, ErrorPolicy> rhs)
    noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> float_basis<BasisType, ErrorPolicy>
{
    const auto lhs_basis {static_cast<BasisType>(lhs)};
    const auto rhs_basis {static_cast<BasisType>(rhs)};

    if constexpr (policy_equals<ErrorPolicy>(overflow_policy::saturate))
    {
        // Raw IEEE 754 semantics: overflow saturates to infinity and NaN propagates.
        // Same expression as the value component of overflowing_sub.
        return float_basis<BasisType, ErrorPolicy>{static_cast<BasisType>(lhs_basis - rhs_basis)};
    }
    else if constexpr (is_user_handler_v<ErrorPolicy>)
    {
        // The classifier stores the raw IEEE 754 result, which is the defined
        // fallback value handed to the handler
        BasisType res {};
        switch (impl::checked_float_subtraction(lhs_basis, rhs_basis, res))
        {
            case impl::error_category::overflow:
                res = ErrorPolicy.on_error(error_kind::overflow, res, overflow_sub_msg<BasisType>());
                break;
            case impl::error_category::underflow:
                res = ErrorPolicy.on_error(error_kind::underflow, res, underflow_sub_msg<BasisType>());
                break;
            case impl::error_category::nan_op:
                res = ErrorPolicy.on_error(error_kind::nan_operation, res, nan_sub_msg<BasisType>());
                break;
            case impl::error_category::invalid_op:
                res = ErrorPolicy.on_error(error_kind::invalid_operation, res, invalid_sub_msg<BasisType>());
                break;
            default:
                break;
        }
        return float_basis<BasisType, ErrorPolicy>{res};
    }
    else
    {
        [[maybe_unused]] BasisType res {};

        // The throw branches are inlined here (rather than calling impl::throw_*_sub)
        // so BOOST_THROW_EXCEPTION captures operator- as the source location of the throw.
        switch (impl::checked_float_subtraction(lhs_basis, rhs_basis, res))
        {
            case impl::error_category::no_error:
                break;
            case impl::error_category::overflow:
                #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
                if (std::is_constant_evaluated())
                {
                    if constexpr (std::is_same_v<BasisType, float>)
                    {
                        throw std::overflow_error("Overflow detected in f32 subtraction"); // LCOV_EXCL_LINE
                    }
                    else
                    {
                        throw std::overflow_error("Overflow detected in f64 subtraction"); // LCOV_EXCL_LINE
                    }
                }
                else
                #endif
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, overflow_sub_msg<BasisType>());
                }
                break;
            case impl::error_category::underflow:
                #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
                if (std::is_constant_evaluated())
                {
                    if constexpr (std::is_same_v<BasisType, float>)
                    {
                        throw std::underflow_error("Underflow detected in f32 subtraction"); // LCOV_EXCL_LINE
                    }
                    else
                    {
                        throw std::underflow_error("Underflow detected in f64 subtraction"); // LCOV_EXCL_LINE
                    }
                }
                else
                #endif
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, underflow_sub_msg<BasisType>());
                }
                break;
            case impl::error_category::nan_op:
                #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
                if (std::is_constant_evaluated())
                {
                    if constexpr (std::is_same_v<BasisType, float>)
                    {
                        throw std::domain_error("Operation with NAN detected in f32 subtraction"); // LCOV_EXCL_LINE
                    }
                    else
                    {
                        throw std::domain_error("Operation with NAN detected in f64 subtraction"); // LCOV_EXCL_LINE
                    }
                }
                else
                #endif
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, nan_sub_msg<BasisType>());
                }
                break;
            case impl::error_category::invalid_op:
                #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
                if (std::is_constant_evaluated())
                {
                    if constexpr (std::is_same_v<BasisType, float>)
                    {
                        throw std::domain_error("Invalid operation (IEEE 754-2008 section 7.2) detected in f32 subtraction"); // LCOV_EXCL_LINE
                    }
                    else
                    {
                        throw std::domain_error("Invalid operation (IEEE 754-2008 section 7.2) detected in f64 subtraction"); // LCOV_EXCL_LINE
                    }
                }
                else
                #endif
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, invalid_sub_msg<BasisType>());
                }
                break;
            case impl::error_category::divide_by_zero:
                BOOST_SAFE_NUMBERS_UNREACHABLE; // LCOV_EXCL_LINE
                break;                          // LCOV_EXCL_LINE
            default:
                BOOST_SAFE_NUMBERS_UNREACHABLE; // LCOV_EXCL_LINE
                break;                          // LCOV_EXCL_LINE
        }

        return float_basis<BasisType, ErrorPolicy>{res};
    }
}

// ------------------------------
// Multiplication
// ------------------------------

namespace impl {

// Our comparison to zero is fine
#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wfloat-equal"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wfloat-equal"
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable:4756)
#endif

// See comment above on checked_float_addition
template <compatible_float_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto checked_float_multiplication(const T lhs, const T rhs, T& res) -> error_category
{
    res = lhs * rhs;

    // The hot path is that our multiplication has nothing funny happening
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
    // 7.2.c: multiplication of zero by an infinity, in either order
    if ((is_true_zero(lhs) && constexpr_isinf(rhs)) || (constexpr_isinf(lhs) && is_true_zero(rhs)))
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

#ifdef __clang__
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto throw_overflow_mul() -> void
{
    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
    if (std::is_constant_evaluated())
    {
        if constexpr (std::is_same_v<BasisType, float>)
        {
            throw std::overflow_error("Overflow detected in f32 multiplication"); // LCOV_EXCL_LINE
        }
        else
        {
            throw std::overflow_error("Overflow detected in f64 multiplication"); // LCOV_EXCL_LINE
        }
    }
    else
    #endif
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, overflow_mul_msg<BasisType>());
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto throw_underflow_mul() -> void
{
    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
    if (std::is_constant_evaluated())
    {
        if constexpr (std::is_same_v<BasisType, float>)
        {
            throw std::underflow_error("Underflow detected in f32 multiplication"); // LCOV_EXCL_LINE
        }
        else
        {
            throw std::underflow_error("Underflow detected in f64 multiplication"); // LCOV_EXCL_LINE
        }
    }
    else
    #endif
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, underflow_mul_msg<BasisType>());
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto throw_nan_mul() -> void
{
    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
    if (std::is_constant_evaluated())
    {
        if constexpr (std::is_same_v<BasisType, float>)
        {
            throw std::domain_error("Operation with NAN detected in f32 multiplication"); // LCOV_EXCL_LINE
        }
        else
        {
            throw std::domain_error("Operation with NAN detected in f64 multiplication"); // LCOV_EXCL_LINE
        }
    }
    else
    #endif
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, nan_mul_msg<BasisType>());
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto throw_invalid_mul() -> void
{
    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
    if (std::is_constant_evaluated())
    {
        if constexpr (std::is_same_v<BasisType, float>)
        {
            throw std::domain_error("Invalid operation (IEEE 754-2008 section 7.2) detected in f32 multiplication"); // LCOV_EXCL_LINE
        }
        else
        {
            throw std::domain_error("Invalid operation (IEEE 754-2008 section 7.2) detected in f64 multiplication"); // LCOV_EXCL_LINE
        }
    }
    else
    #endif
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, invalid_mul_msg<BasisType>());
    }
}

} // namespace impl

template <compatible_float_type BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto operator*(const float_basis<BasisType, ErrorPolicy> lhs,
                                       const float_basis<BasisType, ErrorPolicy> rhs)
    noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> float_basis<BasisType, ErrorPolicy>
{
    const auto lhs_basis {static_cast<BasisType>(lhs)};
    const auto rhs_basis {static_cast<BasisType>(rhs)};

    if constexpr (policy_equals<ErrorPolicy>(overflow_policy::saturate))
    {
        // Raw IEEE 754 semantics: overflow saturates to infinity and NaN propagates.
        // Same expression as the value component of overflowing_mul.
        return float_basis<BasisType, ErrorPolicy>{static_cast<BasisType>(lhs_basis * rhs_basis)};
    }
    else if constexpr (is_user_handler_v<ErrorPolicy>)
    {
        // The classifier stores the raw IEEE 754 result, which is the defined
        // fallback value handed to the handler
        BasisType res {};
        switch (impl::checked_float_multiplication(lhs_basis, rhs_basis, res))
        {
            case impl::error_category::overflow:
                res = ErrorPolicy.on_error(error_kind::overflow, res, overflow_mul_msg<BasisType>());
                break;
            case impl::error_category::underflow:
                res = ErrorPolicy.on_error(error_kind::underflow, res, underflow_mul_msg<BasisType>());
                break;
            case impl::error_category::nan_op:
                res = ErrorPolicy.on_error(error_kind::nan_operation, res, nan_mul_msg<BasisType>());
                break;
            case impl::error_category::invalid_op:
                res = ErrorPolicy.on_error(error_kind::invalid_operation, res, invalid_mul_msg<BasisType>());
                break;
            default:
                break;
        }
        return float_basis<BasisType, ErrorPolicy>{res};
    }
    else
    {
        [[maybe_unused]] BasisType res {};

        // The throw branches are inlined here (rather than calling impl::throw_*_mul)
        // so BOOST_THROW_EXCEPTION captures operator* as the source location of the throw.
        switch (impl::checked_float_multiplication(lhs_basis, rhs_basis, res))
        {
            case impl::error_category::no_error:
                break;
            case impl::error_category::overflow:
                #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
                if (std::is_constant_evaluated())
                {
                    if constexpr (std::is_same_v<BasisType, float>)
                    {
                        throw std::overflow_error("Overflow detected in f32 multiplication"); // LCOV_EXCL_LINE
                    }
                    else
                    {
                        throw std::overflow_error("Overflow detected in f64 multiplication"); // LCOV_EXCL_LINE
                    }
                }
                else
                #endif
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, overflow_mul_msg<BasisType>());
                }
                break;
            case impl::error_category::underflow:
                #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
                if (std::is_constant_evaluated())
                {
                    if constexpr (std::is_same_v<BasisType, float>)
                    {
                        throw std::underflow_error("Underflow detected in f32 multiplication"); // LCOV_EXCL_LINE
                    }
                    else
                    {
                        throw std::underflow_error("Underflow detected in f64 multiplication"); // LCOV_EXCL_LINE
                    }
                }
                else
                #endif
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, underflow_mul_msg<BasisType>());
                }
                break;
            case impl::error_category::nan_op:
                #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
                if (std::is_constant_evaluated())
                {
                    if constexpr (std::is_same_v<BasisType, float>)
                    {
                        throw std::domain_error("Operation with NAN detected in f32 multiplication"); // LCOV_EXCL_LINE
                    }
                    else
                    {
                        throw std::domain_error("Operation with NAN detected in f64 multiplication"); // LCOV_EXCL_LINE
                    }
                }
                else
                #endif
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, nan_mul_msg<BasisType>());
                }
                break;
            case impl::error_category::invalid_op:
                #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
                if (std::is_constant_evaluated())
                {
                    if constexpr (std::is_same_v<BasisType, float>)
                    {
                        throw std::domain_error("Invalid operation (IEEE 754-2008 section 7.2) detected in f32 multiplication"); // LCOV_EXCL_LINE
                    }
                    else
                    {
                        throw std::domain_error("Invalid operation (IEEE 754-2008 section 7.2) detected in f64 multiplication"); // LCOV_EXCL_LINE
                    }
                }
                else
                #endif
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, invalid_mul_msg<BasisType>());
                }
                break;
            case impl::error_category::divide_by_zero:
                BOOST_SAFE_NUMBERS_UNREACHABLE; // LCOV_EXCL_LINE
                break;                          // LCOV_EXCL_LINE
            default:
                BOOST_SAFE_NUMBERS_UNREACHABLE; // LCOV_EXCL_LINE
                break;                          // LCOV_EXCL_LINE
        }

        return float_basis<BasisType, ErrorPolicy>{res};
    }
}

// ------------------------------
// Division
// ------------------------------

namespace impl {

// Our comparison to zero is fine
#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wfloat-equal"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wfloat-equal"
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable:4756)
#endif

// See comment above on checked_float_addition
template <compatible_float_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto checked_float_division(const T lhs, const T rhs, T& res) -> error_category
{
    res = lhs / rhs;

    // The hot path is that our division has nothing funny happening
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
    // 7.2.b: division of zero by zero. Use is_true_zero so a denormal divisor
    // flushed to zero by DAZ (Intel C++ optimized builds, etc.) does not get
    // misclassified as a true zero here.
    if (is_true_zero(lhs) && is_true_zero(rhs))
    {
        return error_category::invalid_op;
    }
    // 7.2.b: division of infinity by infinity
    if (constexpr_isinf(lhs) && constexpr_isinf(rhs))
    {
        return error_category::invalid_op;
    }

    // Section 7.3: divideByZero is signaled when a finite non-zero dividend
    // is divided by zero.
    // The 0/0 case was already handled above as invalid_op,
    // and inf/0 falls through to the section 6.1 infinity classification below.
    if (is_true_zero(rhs) && !constexpr_isinf(lhs) && !constexpr_isnan(lhs))
    {
        return error_category::divide_by_zero;
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

#ifdef __clang__
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto throw_overflow_div() -> void
{
    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
    if (std::is_constant_evaluated())
    {
        if constexpr (std::is_same_v<BasisType, float>)
        {
            throw std::overflow_error("Overflow detected in f32 division"); // LCOV_EXCL_LINE
        }
        else
        {
            throw std::overflow_error("Overflow detected in f64 division"); // LCOV_EXCL_LINE
        }
    }
    else
    #endif
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, overflow_div_msg<BasisType>());
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto throw_underflow_div() -> void
{
    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
    if (std::is_constant_evaluated())
    {
        if constexpr (std::is_same_v<BasisType, float>)
        {
            throw std::underflow_error("Underflow detected in f32 division"); // LCOV_EXCL_LINE
        }
        else
        {
            throw std::underflow_error("Underflow detected in f64 division"); // LCOV_EXCL_LINE
        }
    }
    else
    #endif
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, underflow_div_msg<BasisType>());
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto throw_nan_div() -> void
{
    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
    if (std::is_constant_evaluated())
    {
        if constexpr (std::is_same_v<BasisType, float>)
        {
            throw std::domain_error("Operation with NAN detected in f32 division"); // LCOV_EXCL_LINE
        }
        else
        {
            throw std::domain_error("Operation with NAN detected in f64 division"); // LCOV_EXCL_LINE
        }
    }
    else
    #endif
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, nan_div_msg<BasisType>());
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto throw_invalid_div() -> void
{
    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
    if (std::is_constant_evaluated())
    {
        if constexpr (std::is_same_v<BasisType, float>)
        {
            throw std::domain_error("Invalid operation (IEEE 754-2008 section 7.2) detected in f32 division"); // LCOV_EXCL_LINE
        }
        else
        {
            throw std::domain_error("Invalid operation (IEEE 754-2008 section 7.2) detected in f64 division"); // LCOV_EXCL_LINE
        }
    }
    else
    #endif
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, invalid_div_msg<BasisType>());
    }
}

template <compatible_float_type BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto throw_divbyzero_div() -> void
{
    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
    if (std::is_constant_evaluated())
    {
        if constexpr (std::is_same_v<BasisType, float>)
        {
            throw std::domain_error("Division by zero detected in f32 division"); // LCOV_EXCL_LINE
        }
        else
        {
            throw std::domain_error("Division by zero detected in f64 division"); // LCOV_EXCL_LINE
        }
    }
    else
    #endif
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, divbyzero_div_msg<BasisType>());
    }
}

} // namespace impl

template <compatible_float_type BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto operator/(const float_basis<BasisType, ErrorPolicy> lhs,
                                       const float_basis<BasisType, ErrorPolicy> rhs)
    noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> float_basis<BasisType, ErrorPolicy>
{
    const auto lhs_basis {static_cast<BasisType>(lhs)};
    const auto rhs_basis {static_cast<BasisType>(rhs)};

    if constexpr (policy_equals<ErrorPolicy>(overflow_policy::saturate))
    {
        // Raw IEEE 754 semantics: overflow saturates to infinity and NaN propagates.
        // Same expression as the value component of overflowing_div.
        return float_basis<BasisType, ErrorPolicy>{static_cast<BasisType>(lhs_basis / rhs_basis)};
    }
    else if constexpr (is_user_handler_v<ErrorPolicy>)
    {
        // The classifier stores the raw IEEE 754 result, which is the defined
        // fallback value handed to the handler
        BasisType res {};
        switch (impl::checked_float_division(lhs_basis, rhs_basis, res))
        {
            case impl::error_category::overflow:
                res = ErrorPolicy.on_error(error_kind::overflow, res, overflow_div_msg<BasisType>());
                break;
            case impl::error_category::underflow:
                res = ErrorPolicy.on_error(error_kind::underflow, res, underflow_div_msg<BasisType>());
                break;
            case impl::error_category::nan_op:
                res = ErrorPolicy.on_error(error_kind::nan_operation, res, nan_div_msg<BasisType>());
                break;
            case impl::error_category::invalid_op:
                res = ErrorPolicy.on_error(error_kind::invalid_operation, res, invalid_div_msg<BasisType>());
                break;
            case impl::error_category::divide_by_zero:
                res = ErrorPolicy.on_error(error_kind::divide_by_zero, res, divbyzero_div_msg<BasisType>());
                break;
            default:
                break;
        }
        return float_basis<BasisType, ErrorPolicy>{res};
    }
    else
    {
        [[maybe_unused]] BasisType res {};

        // The throw branches are inlined here (rather than calling impl::throw_*_div)
        // so BOOST_THROW_EXCEPTION captures operator/ as the source location of the throw.
        switch (impl::checked_float_division(lhs_basis, rhs_basis, res))
        {
            case impl::error_category::no_error:
                break;
            case impl::error_category::overflow:
                #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
                if (std::is_constant_evaluated())
                {
                    if constexpr (std::is_same_v<BasisType, float>)
                    {
                        throw std::overflow_error("Overflow detected in f32 division"); // LCOV_EXCL_LINE
                    }
                    else
                    {
                        throw std::overflow_error("Overflow detected in f64 division"); // LCOV_EXCL_LINE
                    }
                }
                else
                #endif
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, overflow_div_msg<BasisType>());
                }
                break;
            case impl::error_category::underflow:
                #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
                if (std::is_constant_evaluated())
                {
                    if constexpr (std::is_same_v<BasisType, float>)
                    {
                        throw std::underflow_error("Underflow detected in f32 division"); // LCOV_EXCL_LINE
                    }
                    else
                    {
                        throw std::underflow_error("Underflow detected in f64 division"); // LCOV_EXCL_LINE
                    }
                }
                else
                #endif
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, underflow_div_msg<BasisType>());
                }
                break;
            case impl::error_category::nan_op:
                #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
                if (std::is_constant_evaluated())
                {
                    if constexpr (std::is_same_v<BasisType, float>)
                    {
                        throw std::domain_error("Operation with NAN detected in f32 division"); // LCOV_EXCL_LINE
                    }
                    else
                    {
                        throw std::domain_error("Operation with NAN detected in f64 division"); // LCOV_EXCL_LINE
                    }
                }
                else
                #endif
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, nan_div_msg<BasisType>());
                }
                break;
            case impl::error_category::invalid_op:
                #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
                if (std::is_constant_evaluated())
                {
                    if constexpr (std::is_same_v<BasisType, float>)
                    {
                        throw std::domain_error("Invalid operation (IEEE 754-2008 section 7.2) detected in f32 division"); // LCOV_EXCL_LINE
                    }
                    else
                    {
                        throw std::domain_error("Invalid operation (IEEE 754-2008 section 7.2) detected in f64 division"); // LCOV_EXCL_LINE
                    }
                }
                else
                #endif
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, invalid_div_msg<BasisType>());
                }
                break;
            case impl::error_category::divide_by_zero:
                #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
                if (std::is_constant_evaluated())
                {
                    if constexpr (std::is_same_v<BasisType, float>)
                    {
                        throw std::domain_error("Division by zero detected in f32 division"); // LCOV_EXCL_LINE
                    }
                    else
                    {
                        throw std::domain_error("Division by zero detected in f64 division"); // LCOV_EXCL_LINE
                    }
                }
                else
                #endif
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, divbyzero_div_msg<BasisType>());
                }
                break;
            default:
                BOOST_SAFE_NUMBERS_UNREACHABLE; // LCOV_EXCL_LINE
                break;                          // LCOV_EXCL_LINE
        }

        return float_basis<BasisType, ErrorPolicy>{res};
    }
}

} // namespace boost::safe_numbers::detail

// Block any mixed floating point type operation (e.g. f32 and f64) with a
// clear diagnostic, mirroring the unsigned integer mixed-type guard. Defined
// outside the namespace so the generated operators are plain free functions.

#define BOOST_SAFE_NUMBERS_DEFINE_MIXED_FLOAT_OP(OP_NAME, OP_SYMBOL)                                                                                      \
template <boost::safe_numbers::detail::compatible_float_type LHSBasis, auto LHSPolicy,                                                                                    \
          boost::safe_numbers::detail::compatible_float_type RHSBasis, auto RHSPolicy>                                                                                    \
    requires (!std::is_same_v<LHSBasis, RHSBasis> || boost::safe_numbers::detail::policies_differ<LHSPolicy, RHSPolicy>())                                                                                                        \
BOOST_SAFE_NUMBERS_HOST_DEVICE                                                                                                                            \
constexpr auto OP_SYMBOL(const boost::safe_numbers::detail::float_basis<LHSBasis, LHSPolicy>,                                                                        \
                         const boost::safe_numbers::detail::float_basis<RHSBasis, RHSPolicy>)                                                                        \
{                                                                                                                                                         \
    if constexpr (std::is_same_v<LHSBasis, RHSBasis>)                                                                                                     \
    {                                                                                                                                                     \
        static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>,                                                                   \
                      "Can not perform " OP_NAME " between same width types with different overflow policies "                                            \
                      "(e.g. f32 and sat_f32): convert explicitly through basis_type first");                                                             \
    }                                                                                                                                                     \
    else if constexpr (std::is_same_v<LHSBasis, float>)                                                                                                        \
    {                                                                                                                                                     \
        if constexpr (std::is_same_v<RHSBasis, double>)                                                                                                   \
        {                                                                                                                                                 \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between f32 and f64");           \
        }                                                                                                                                                 \
        else                                                                                                                                              \
        {                                                                                                                                                 \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between f32 and unknown type");  \
        }                                                                                                                                                 \
    }                                                                                                                                                     \
    else if constexpr (std::is_same_v<LHSBasis, double>)                                                                                                  \
    {                                                                                                                                                     \
        if constexpr (std::is_same_v<RHSBasis, float>)                                                                                                    \
        {                                                                                                                                                 \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between f64 and f32");           \
        }                                                                                                                                                 \
        else                                                                                                                                              \
        {                                                                                                                                                 \
            static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " between f64 and unknown type");  \
        }                                                                                                                                                 \
    }                                                                                                                                                     \
    else                                                                                                                                                  \
    {                                                                                                                                                     \
        static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>, "Can not perform " OP_NAME " on mixed floating point types");     \
    }                                                                                                                                                     \
                                                                                                                                                          \
    return boost::safe_numbers::detail::float_basis<LHSBasis, LHSPolicy>{LHSBasis{0}};                                                                               \
}

namespace boost::safe_numbers::detail {

BOOST_SAFE_NUMBERS_DEFINE_MIXED_FLOAT_OP("comparison", operator<=>)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_FLOAT_OP("equality", operator==)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_FLOAT_OP("addition", operator+)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_FLOAT_OP("subtraction", operator-)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_FLOAT_OP("multiplication", operator*)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_FLOAT_OP("division", operator/)

} // namespace boost::safe_numbers::detail

// ------------------------------
// Deferred (overflowing) Math
// ------------------------------

namespace boost::safe_numbers {

namespace detail::impl {

// True exactly when the checked float operators would report res as an error:
// every one of them errors precisely on a non-finite result (the classifiers
// above only choose which exception to raise). The comparison is false for
// both inf and NAN, so the negation flags both without a branch.
template <compatible_float_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto nonfinite_result(const T res) noexcept -> bool
{
    return !(constexpr_abs(res) <= (std::numeric_limits<T>::max)());
}

} // namespace detail::impl

// Non-throwing counterparts of the checked float operators for hot loops. The
// value is the raw IEEE result and the bool is true exactly when the checked
// operator would have thrown. The branch-free form keeps loops vectorizable:
// accumulate the flags in an unsigned value and test once at a boundary.

template <detail::compatible_float_type BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto overflowing_add(const detail::float_basis<BasisType, ErrorPolicy> lhs,
                                                                            const detail::float_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> std::pair<detail::float_basis<BasisType, ErrorPolicy>, bool>
{
    const auto res {static_cast<BasisType>(static_cast<BasisType>(lhs) + static_cast<BasisType>(rhs))};
    return std::make_pair(detail::float_basis<BasisType, ErrorPolicy>{res}, detail::impl::nonfinite_result(res));
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_FLOAT_OP("overflowing addition", overflowing_add)

template <detail::compatible_float_type BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto overflowing_sub(const detail::float_basis<BasisType, ErrorPolicy> lhs,
                                                                            const detail::float_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> std::pair<detail::float_basis<BasisType, ErrorPolicy>, bool>
{
    const auto res {static_cast<BasisType>(static_cast<BasisType>(lhs) - static_cast<BasisType>(rhs))};
    return std::make_pair(detail::float_basis<BasisType, ErrorPolicy>{res}, detail::impl::nonfinite_result(res));
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_FLOAT_OP("overflowing subtraction", overflowing_sub)

template <detail::compatible_float_type BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto overflowing_mul(const detail::float_basis<BasisType, ErrorPolicy> lhs,
                                                                            const detail::float_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> std::pair<detail::float_basis<BasisType, ErrorPolicy>, bool>
{
    const auto res {static_cast<BasisType>(static_cast<BasisType>(lhs) * static_cast<BasisType>(rhs))};
    return std::make_pair(detail::float_basis<BasisType, ErrorPolicy>{res}, detail::impl::nonfinite_result(res));
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_FLOAT_OP("overflowing multiplication", overflowing_mul)

template <detail::compatible_float_type BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto overflowing_div(const detail::float_basis<BasisType, ErrorPolicy> lhs,
                                                                            const detail::float_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> std::pair<detail::float_basis<BasisType, ErrorPolicy>, bool>
{
    const auto res {static_cast<BasisType>(static_cast<BasisType>(lhs) / static_cast<BasisType>(rhs))};
    return std::make_pair(detail::float_basis<BasisType, ErrorPolicy>{res}, detail::impl::nonfinite_result(res));
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_FLOAT_OP("overflowing division", overflowing_div)

} // namespace boost::safe_numbers

#undef BOOST_SAFE_NUMBERS_DEFINE_MIXED_FLOAT_OP

#endif // BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BASIS_HPP

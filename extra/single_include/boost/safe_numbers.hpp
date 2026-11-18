// boost.safe_numbers: amalgamated single header build.
// Auto-generated: all internal boost/safe_numbers headers inlined into one file
// for use in Compiler Explorer and other single-file environments.
// External Boost dependencies (Config, Core, ThrowException, Charconv) are still
// required on the include path.
// Do not edit by hand; regenerate with extra/amalgamate.py.
//
// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===== BEGIN boost/safe_numbers.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFENUMBERS_HPP
#define BOOST_SAFENUMBERS_HPP

// ===== BEGIN boost/safe_numbers/overflow_policy.hpp =====
// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_OVERFLOW_POLICY_HPP
#define BOOST_SAFE_NUMBERS_OVERFLOW_POLICY_HPP

// ===== BEGIN boost/safe_numbers/detail/config.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFENUMBERS_CONFIG_HPP
#define BOOST_SAFENUMBERS_CONFIG_HPP

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE
#  define BOOST_SAFE_NUMBERS_EXPORT export
#else
#  define BOOST_SAFE_NUMBERS_EXPORT
#endif

#ifdef __has_builtin
#  define BOOST_SAFE_NUMBERS_HAS_BUILTIN(x) __has_builtin(x)
#else
#  define BOOST_SAFE_NUMBERS_HAS_BUILTIN(x) 0
#endif // __has_builtin

// __x86_64__ remains defined on the SYCL spir64 device pass, but these x86 intrinsic
// headers do not compile for that target, so exclude them when __SYCL_DEVICE_ONLY__ is set.
#if defined(__x86_64__) && !defined(__SYCL_DEVICE_ONLY__)

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE
#  include <x86intrin.h>
#  include <emmintrin.h>
#endif

#elif defined(_M_AMD64)

#define BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN
#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE
#  include <intrin.h>
#endif

#elif defined(_M_IX86)

#define BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN
#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE
#  include <intrin.h>
#endif

#elif defined(_M_ARM64)

#define BOOST_SAFENUMBERS_HAS_WINDOWS_ARM64_INTRIN
#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE
#  include <intrin.h>
#endif


#endif

#if defined(__GNUC__) || defined(__clang__)
#  define BOOST_SAFE_NUMBERS_UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER)
#  define BOOST_SAFE_NUMBERS_UNREACHABLE __assume(0)
#else
#  define BOOST_SAFE_NUMBERS_UNREACHABLE std::abort()
#endif

// bounded_float requires floating-point non-type template parameters
// Checking defined(__cpp_nontype_template_args) && __cpp_nontype_template_args >= 201911L
// ends up removing compilers that have support, such as clang 18
#if (defined(__clang__) && __clang_major__ >= 18) || (defined(__GNUC__) && __GNUC__ >= 11) || (defined(_MSC_VER) && _MSC_VER >= 1943) || (defined(__cpp_nontype_template_args) && __cpp_nontype_template_args >= 201911L)
#  define BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT 1
#else
#  define BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT 0
#endif

namespace boost::safe_numbers::detail {

// Workaround for static_assert(false, ...) in if constexpr branches.
// Before C++23 (P2593R1), static_assert(false) is ill-formed even in
// discarded branches. Making the condition depend on a template parameter
// defers evaluation until instantiation.
template <typename...>
inline constexpr auto dependent_false {false};

} // namespace boost::safe_numbers::detail

// BOOST_SAFE_NUMBERS_HOST_DEVICE annotates every device-callable function and
// expands to "__host__ __device__" (CUDA), "SYCL_EXTERNAL" (SYCL), or nothing.
#if defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA)

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_CUDA
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_CUDA
#endif

#ifndef BOOST_CHARCONV_ENABLE_CUDA
#  define BOOST_CHARCONV_ENABLE_CUDA
#endif

#define BOOST_SAFE_NUMBERS_HOST_DEVICE __host__ __device__
#define BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT

#elif defined(BOOST_SAFE_NUMBERS_ENABLE_SYCL)

// SYCL is fully opt-in. <sycl/sycl.hpp> must be included before any safe_numbers
// header so the SYCL_EXTERNAL keyword exists; icpx defines __SYCL_DEVICE_ONLY__
// on the spir64 device pass, which forces the portable code paths downstream.
#ifndef SYCL_EXTERNAL
#  error "Include <sycl/sycl.hpp> before any Boost.safe_numbers header when BOOST_SAFE_NUMBERS_ENABLE_SYCL is defined"
#endif

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_SYCL
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_SYCL
#endif

#ifndef BOOST_CHARCONV_ENABLE_SYCL
#  define BOOST_CHARCONV_ENABLE_SYCL
#endif

#define BOOST_SAFE_NUMBERS_HOST_DEVICE SYCL_EXTERNAL
#define BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT

#else

#define BOOST_SAFE_NUMBERS_HOST_DEVICE

#endif // GPU backends

#endif // BOOST_SAFENUMBERS_CONFIG_HPP
// ===== END boost/safe_numbers/detail/config.hpp =====

namespace boost::safe_numbers {

BOOST_SAFE_NUMBERS_EXPORT enum class overflow_policy
{
    throw_exception,
    saturate,
    overflow_tuple,
    checked,
    strict,
    widen,
};

// The kind of error being reported to a user defined handler.
// divide_by_zero covers modulo by zero as well; the message distinguishes them.
// invalid_operation and nan_operation are produced by the floating point types only.
BOOST_SAFE_NUMBERS_EXPORT enum class error_kind
{
    overflow,
    underflow,
    divide_by_zero,
    invalid_operation,
    nan_operation,
};

// Tag types selecting the built-in policies through the basic_* alias templates,
// e.g. basic_u8<saturating>. A user defined handler type is used the same way:
// basic_u8<my_handler>.

BOOST_SAFE_NUMBERS_EXPORT struct throwing {};

BOOST_SAFE_NUMBERS_EXPORT struct saturating {};

BOOST_SAFE_NUMBERS_EXPORT struct strict {};

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_OVERFLOW_POLICY_HPP
// ===== END boost/safe_numbers/overflow_policy.hpp =====
// ===== BEGIN boost/safe_numbers/unsigned_integers.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_UNSIGNED_INTEGERS_HPP
#define BOOST_SAFE_NUMBERS_UNSIGNED_INTEGERS_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/config.hpp
// ===== BEGIN boost/safe_numbers/detail/unsigned_integer_basis.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_UNSIGNED_INTEGER_BASIS_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_UNSIGNED_INTEGER_BASIS_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/config.hpp
// ===== BEGIN boost/safe_numbers/detail/type_traits.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_TYPE_TRAITS_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_TYPE_TRAITS_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/config.hpp
// ===== BEGIN boost/safe_numbers/detail/int128/int128.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_INT128_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_INT128_HPP

// ===== BEGIN boost/safe_numbers/detail/int128/detail/fwd.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_FWD_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_FWD_HPP

// ===== BEGIN boost/safe_numbers/detail/int128/detail/config.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_CONFIG_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_CONFIG_HPP

// The SYCL device target (spir64) has no native 128-bit integer, so force the portable
// code path on the device pass. This mirrors a user-supplied BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_BUILTIN_INT128
// and keeps host/device selection consistent even though __x86_64__ stays defined on device.
#if defined(__SYCL_DEVICE_ONLY__) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_BUILTIN_INT128)
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_BUILTIN_INT128
#endif

// Use 128-bit integers.
// The SYCL device target (spir64) has no native 128-bit integer, so on the device pass
// we fall back to the portable path (the same one used on platforms without __int128).
#if (defined(BOOST_HAS_INT128) || (defined(__SIZEOF_INT128__) && !defined(_MSC_VER)) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_BUILTIN_INT128)) && !defined(__SYCL_DEVICE_ONLY__)

#define BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

#define BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR constexpr

namespace boost {
namespace int128 {
namespace detail {

// Avoids pedantic warnings
#ifdef __GNUC__

__extension__ using builtin_i128 = __int128 ;
__extension__ using builtin_u128 = unsigned __int128 ;

#else

using builtin_i128 = __int128 ;
using builtin_u128 = unsigned __int128;

#endif

} // namespace detail
} // namespace int128
} // namespace boost

#elif __has_include(<__msvc_int128.hpp>) && _MSVC_LANG >= 202002L && !defined(__SYCL_DEVICE_ONLY__)

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILD_MODULE
#include <__msvc_int128.hpp>
#endif

#define BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

#if _MSC_VER >= 1945
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR constexpr
#else
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR inline
#endif

namespace boost {
namespace int128 {
namespace detail {

using builtin_i128 = std::_Signed128;
using builtin_u128 = std::_Unsigned128;

} // namespace detail
} // namespace int128
} // namespace boost

#endif // builtin 128-bit detection

// Determine endianness
#if defined(_WIN32)

#define BOOST_SAFE_NUMBERS_DETAIL_INT128_ENDIAN_BIG_BYTE 0
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_ENDIAN_LITTLE_BYTE 1

#elif defined(__BYTE_ORDER__)

#define BOOST_SAFE_NUMBERS_DETAIL_INT128_ENDIAN_BIG_BYTE (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_ENDIAN_LITTLE_BYTE (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)

#else

#error Could not determine endian type. Please file an issue at https://github.com/cppalliance/INT128 with your architecture

#endif // Determine endianness

// Is constant evaluated detection
#ifdef __cpp_lib_is_constant_evaluated
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_IS_CONSTANT_EVALUATED
#endif

#ifdef __has_builtin
#  if __has_builtin(__builtin_is_constant_evaluated)
#    define BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN_IS_CONSTANT_EVALUATED
#  endif
#endif

//
// MSVC also supports __builtin_is_constant_evaluated if it's recent enough:
//
#if defined(_MSC_FULL_VER) && (_MSC_FULL_VER >= 192528326)
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN_IS_CONSTANT_EVALUATED
#endif

//
// As does GCC-9:
//
#if defined(__GNUC__) && (__GNUC__ >= 9) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN_IS_CONSTANT_EVALUATED)
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN_IS_CONSTANT_EVALUATED
#endif

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_IS_CONSTANT_EVALUATED) && !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_CUDA))
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(x) std::is_constant_evaluated()
#elif defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN_IS_CONSTANT_EVALUATED) && !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_CUDA))
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(x) __builtin_is_constant_evaluated()
#else
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(x) false
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION
#endif

// https://github.com/llvm/llvm-project/issues/55638
#if defined(__clang__) && __cplusplus > 202002L && __clang_major__ < 17
#  undef BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(x) false
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION
#endif

#if defined(_MSC_VER)
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE __attribute__((always_inline)) inline
#else
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE inline
#endif

#ifdef __x86_64__

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILD_MODULE
#  include <x86intrin.h>
#  include <emmintrin.h>
#endif

#  ifdef __ADX__
#    define BOOST_SAFE_NUMBERS_DETAIL_INT128_ADD_CARRY _addcarryx_u64
#    define BOOST_SAFE_NUMBERS_DETAIL_INT128_SUB_BORROW _subborrow_u64
#  else
#    define BOOST_SAFE_NUMBERS_DETAIL_INT128_ADD_CARRY _addcarry_u64
#    define BOOST_SAFE_NUMBERS_DETAIL_INT128_SUB_BORROW _subborrow_u64
#  endif

#elif defined(_M_AMD64)

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILD_MODULE
#  include <intrin.h>
#endif

#  ifdef __ADX__
#    define BOOST_SAFE_NUMBERS_DETAIL_INT128_ADD_CARRY _addcarryx_u64
#    define BOOST_SAFE_NUMBERS_DETAIL_INT128_SUB_BORROW _subborrow_u64
#  else
#    define BOOST_SAFE_NUMBERS_DETAIL_INT128_ADD_CARRY _addcarry_u64
#    define BOOST_SAFE_NUMBERS_DETAIL_INT128_SUB_BORROW _subborrow_u64
#  endif

#elif defined(__i386__)

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILD_MODULE
#  include <emmintrin.h>
#endif

#elif defined(_M_IX86)

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILD_MODULE
#  include <intrin.h>
#endif

#endif // Platform macros

// Hardware 128-bit by 64-bit unsigned division via the x86-64 DIV instruction
// Excluded on the CUDA and SYCL device passes (the device target is not x86-64)
#if defined(__x86_64__) && (defined(__GNUC__) || defined(__clang__)) && !defined(_MSC_VER) && !defined(__CUDA_ARCH__) && !defined(__SYCL_DEVICE_ONLY__)
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_X86_64_DIVQ
#endif

// The builtin is only constexpr from clang-7 or GCC-10
#ifdef __has_builtin
#  if __has_builtin(__builtin_sub_overflow) && ((defined(__clang__) && __clang_major__ >= 7) || (defined(__GNUC__) && __GNUC__ >= 10))
#    define BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN_SUB_OVERFLOW
#  endif
#  if __has_builtin(__builtin_add_overflow) && ((defined(__clang__) && __clang_major__ >= 7) || (defined(__GNUC__) && __GNUC__ >= 10))
#    define BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN_ADD_OVERFLOW
#  endif
#endif

#if defined(__cpp_if_constexpr) && __cpp_if_constexpr >= 201606L
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_IF_CONSTEXPR
#endif // if constexpr detection

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILD_MODULE
#include <cassert>
#endif

#define BOOST_SAFE_NUMBERS_DETAIL_INT128_ASSERT(x) assert(x)
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_ASSERT_MSG(expr, msg) assert((expr)&&(msg))

#ifdef _MSC_VER
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_ASSUME(expr) __assume(expr)
#elif defined(__clang__)
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_ASSUME(expr) __builtin_assume(expr)
#elif defined(__GNUC__)
#  if __GNUC__ >= 5 && __GNUC__ < 13
#    define BOOST_SAFE_NUMBERS_DETAIL_INT128_ASSUME(expr) if (expr) {} else { __builtin_unreachable(); }
#  else
#    define BOOST_SAFE_NUMBERS_DETAIL_INT128_ASSUME(expr) __attribute__((assume(expr)))
#  endif
#elif defined(__has_cpp_attribute)
#  if __has_cpp_attribute(assume)
#    define BOOST_SAFE_NUMBERS_DETAIL_INT128_ASSUME(expr) [[assume(expr)]]
#  else
#    define BOOST_SAFE_NUMBERS_DETAIL_INT128_ASSUME(expr) BOOST_SAFE_NUMBERS_DETAIL_INT128_ASSERT(expr)
#  endif
#else
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_ASSUME(expr) BOOST_SAFE_NUMBERS_DETAIL_INT128_ASSERT(expr)
#endif

#if defined(__has_builtin)
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN(x) __has_builtin(x)
#else
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN(x) false
#endif

#if BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN(__builtin_expect)
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_LIKELY(x) __builtin_expect(x, 1)
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY(x) __builtin_expect(x, 0)
#else
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_LIKELY(x) x
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY(x) x
#endif

#if !defined(__cpp_if_constexpr) || (__cpp_if_constexpr < 201606L)
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CXX17_IF_CONSTEXPR
#endif

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CXX17_IF_CONSTEXPR
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_IF_CONSTEXPR if constexpr
#else
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_IF_CONSTEXPR if
#endif

#if defined(__GNUC__) || defined(__clang__)
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER)
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_UNREACHABLE __assume(0)
#else
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_UNREACHABLE std::abort()
#endif

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILD_MODULE
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_INLINE_CONSTEXPR inline constexpr
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT export
#else
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_INLINE_CONSTEXPR static constexpr
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT
#endif

// Detect if we can throw or not
// First check if the user said no explicitly
// Then check if it's been disabled elsewhere

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_DISABLE_EXCEPTIONS

#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_THROW_EXCEPTION(expr)

#else

#  ifdef _MSC_VER
#    ifdef _CPPUNWIND
#      define BOOST_SAFE_NUMBERS_DETAIL_INT128_THROW_EXCEPTION(expr) throw expr;
#    else
#      define BOOST_SAFE_NUMBERS_DETAIL_INT128_THROW_EXCEPTION(expr)
#      define BOOST_SAFE_NUMBERS_DETAIL_INT128_DISABLE_EXCEPTIONS
#    endif
#  else
#    ifdef __EXCEPTIONS
#      define BOOST_SAFE_NUMBERS_DETAIL_INT128_THROW_EXCEPTION(expr) throw expr;
#    else
#      define BOOST_SAFE_NUMBERS_DETAIL_INT128_THROW_EXCEPTION(expr)
#      define BOOST_SAFE_NUMBERS_DETAIL_INT128_DISABLE_EXCEPTIONS
#    endif
#endif

#endif // Exceptions

#if defined(__cpp_impl_three_way_comparison) && __cpp_impl_three_way_comparison >= 201907L && __has_include(<compare>)
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_SPACESHIP_OPERATOR
#  ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILD_MODULE
#    include <compare>
#  endif
#endif

// GPU device support. CUDA is auto-detected via __CUDACC__ (opt-in with
// BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_CUDA). SYCL is fully opt-in via BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_SYCL;
// <sycl/sycl.hpp> must be included before <boost/int128.hpp> so SYCL_EXTERNAL exists.
#if defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_CUDA)
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_CUDA_ENABLED __host__ __device__
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_GPU_SUPPORT
#elif defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_SYCL)
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_SYCL_ENABLED SYCL_EXTERNAL
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_GPU_SUPPORT
#endif

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_CUDA_ENABLED
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_CUDA_ENABLED
#endif
#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_SYCL_ENABLED
#  define BOOST_SAFE_NUMBERS_DETAIL_INT128_SYCL_ENABLED
#endif

// Exactly one sub-macro is ever non-empty; expands to "__host__ __device__" (CUDA),
// "SYCL_EXTERNAL" (SYCL), or nothing (host).
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_CUDA_ENABLED BOOST_SAFE_NUMBERS_DETAIL_INT128_SYCL_ENABLED

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_CONFIG_HPP
// ===== END boost/safe_numbers/detail/int128/detail/config.hpp =====

namespace boost {
namespace int128 {

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT struct uint128_t;
BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT struct int128_t;

} // namespace int128
} // namespace boost

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_FWD_HPP
// ===== END boost/safe_numbers/detail/int128/detail/fwd.hpp =====
// ===== BEGIN boost/safe_numbers/detail/int128/detail/int128_imp.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_INT128_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_INT128_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/fwd.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/config.hpp
// ===== BEGIN boost/safe_numbers/detail/int128/detail/traits.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_TRAITS_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_TRAITS_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/config.hpp

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILD_MODULE

#include <type_traits>
#include <cstdint>

#endif

namespace boost {
namespace int128 {
namespace detail {

template <typename T>
struct signed_integer
{
    static constexpr bool value = (std::is_signed<T>::value && std::is_integral<T>::value)
    #ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128
    || std::is_same<T, builtin_i128>::value;
    #else
    ;
    #endif
};

template <typename T>
BOOST_SAFE_NUMBERS_DETAIL_INT128_INLINE_CONSTEXPR bool is_signed_integer_v = signed_integer<T>::value;

template <typename T>
struct unsigned_integer
{
    static constexpr bool value = (std::is_unsigned<T>::value && std::is_integral<T>::value)
    #ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128
    || std::is_same<T, builtin_u128>::value;
    #else
    ;
    #endif
};

template <typename T>
BOOST_SAFE_NUMBERS_DETAIL_INT128_INLINE_CONSTEXPR bool is_unsigned_integer_v = unsigned_integer<T>::value;

template <typename T>
BOOST_SAFE_NUMBERS_DETAIL_INT128_INLINE_CONSTEXPR bool is_any_integer_v = signed_integer<T>::value || unsigned_integer<T>::value;

// Decides if we can use a u32 or u64 implementation for some operations

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

template <typename T>
using evaluation_type_t = std::conditional_t<sizeof(T) <= sizeof(std::uint32_t), std::uint32_t,
                            std::conditional_t<sizeof(T) <= sizeof(std::uint64_t), std::uint64_t, builtin_u128>>;

#else

template <typename T>
using evaluation_type_t = std::conditional_t<sizeof(T) <= sizeof(std::uint32_t), std::uint32_t, std::uint64_t>;

#endif

} // namespace detail
} // namespace int128
} // namespace boost

#define BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT typename SignedInteger, std::enable_if_t<detail::is_signed_integer_v<SignedInteger>, bool> = true
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT typename UnsignedInteger, std::enable_if_t<detail::is_unsigned_integer_v<UnsignedInteger>, bool> = true
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_INTEGER_CONCEPT typename Integer, std::enable_if_t<detail::is_any_integer_v<Integer>, bool> = true
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_FLOATING_POINT_CONCEPT typename Float, std::enable_if_t<std::is_floating_point<Float>::value, bool> = true

#define BOOST_SAFE_NUMBERS_DETAIL_INT128_SIGNED_INTEGER_CONCEPT typename SignedInteger, std::enable_if_t<detail::is_signed_integer_v<SignedInteger>, bool>
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_UNSIGNED_INTEGER_CONCEPT typename UnsignedInteger, std::enable_if_t<detail::is_unsigned_integer_v<UnsignedInteger>, bool>
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_INTEGER_CONCEPT typename Integer, std::enable_if_t<detail::is_any_integer_v<Integer>, bool>
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_FLOATING_POINT_CONCEPT typename Float, std::enable_if_t<std::is_floating_point<Float>::value, bool>

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

#define BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_128BIT_INTEGER_CONCEPT typename SignedInteger, std::enable_if_t<std::is_same<SignedInteger, detail::builtin_i128>::value, bool> = true
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_128BIT_INTEGER_CONCEPT typename UnsignedInteger, std::enable_if_t<std::is_same<UnsignedInteger, detail::builtin_u128>::value, bool> = true
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT typename Integer, std::enable_if_t<std::is_same<Integer, detail::builtin_u128>::value || std::is_same<Integer, detail::builtin_i128>::value, bool> = true

#define BOOST_SAFE_NUMBERS_DETAIL_INT128_SIGNED_128BIT_INTEGER_CONCEPT typename SignedInteger, std::enable_if_t<std::is_same<SignedInteger, detail::builtin_i128>::value, bool>
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_UNSIGNED_128BIT_INTEGER_CONCEPT typename UnsignedInteger, std::enable_if_t<std::is_same<UnsignedInteger, detail::builtin_u128>::value, bool>
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_128BIT_INTEGER_CONCEPT typename Integer, std::enable_if_t<std::is_same<Integer, detail::builtin_u128>::value || std::is_same<Integer, detail::builtin_i128>::value, bool>

#endif

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_TRAITS_HPP
// ===== END boost/safe_numbers/detail/int128/detail/traits.hpp =====
// ===== BEGIN boost/safe_numbers/detail/int128/detail/constants.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_CONSTANTS_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_CONSTANTS_HPP

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILD_MODULE

#include <cstdint>
#include <limits>

#endif

namespace boost {
namespace int128 {
namespace detail {

BOOST_SAFE_NUMBERS_DETAIL_INT128_INLINE_CONSTEXPR std::uint64_t low_word_mask {(std::numeric_limits<std::uint64_t>::max)()};

template <typename T>
BOOST_SAFE_NUMBERS_DETAIL_INT128_INLINE_CONSTEXPR T offset_value_v = static_cast<T>((std::numeric_limits<std::uint64_t>::max)());

} // namespace detail
} // namespace int128
} // namespace boost

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_CONSTANTS_HPP
// ===== END boost/safe_numbers/detail/int128/detail/constants.hpp =====
// ===== BEGIN boost/safe_numbers/detail/int128/detail/clz.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_CLZ_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_CLZ_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/config.hpp

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILD_MODULE

#include <limits>
#include <cstdint>

#endif

namespace boost {
namespace int128 {
namespace detail {

namespace impl {

#if !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_CUDA))

// See: http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogDeBruijn
BOOST_SAFE_NUMBERS_DETAIL_INT128_INLINE_CONSTEXPR int index64[64] = {
    0, 47,  1, 56, 48, 27,  2, 60,
    57, 49, 41, 37, 28, 16,  3, 61,
    54, 58, 35, 52, 50, 42, 21, 44,
    38, 32, 29, 23, 17, 11,  4, 62,
    46, 55, 26, 59, 40, 36, 15, 53,
    34, 51, 20, 43, 31, 22, 10, 45,
    25, 39, 14, 33, 19, 30,  9, 24,
    13, 18,  8, 12,  7,  6,  5, 63
};

#endif

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int bit_scan_reverse(std::uint64_t bb) noexcept
{
    #if defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_CUDA)

    constexpr int index64[64] = {
        0, 47,  1, 56, 48, 27,  2, 60,
        57, 49, 41, 37, 28, 16,  3, 61,
        54, 58, 35, 52, 50, 42, 21, 44,
        38, 32, 29, 23, 17, 11,  4, 62,
        46, 55, 26, 59, 40, 36, 15, 53,
        34, 51, 20, 43, 31, 22, 10, 45,
        25, 39, 14, 33, 19, 30,  9, 24,
        13, 18,  8, 12,  7,  6,  5, 63
    };

    #endif

    constexpr auto debruijn64 {UINT64_C(0x03f79d71b4cb0a89)};

    BOOST_SAFE_NUMBERS_DETAIL_INT128_ASSUME(bb != 0); // LCOV_EXCL_LINE

    bb |= bb >> 1;
    bb |= bb >> 2;
    bb |= bb >> 4;
    bb |= bb >> 8;
    bb |= bb >> 16;
    bb |= bb >> 32;

    return index64[(bb * debruijn64) >> 58];
}

#if !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_CUDA))

BOOST_SAFE_NUMBERS_DETAIL_INT128_INLINE_CONSTEXPR int countl_mod37[37] = {
    32, 31, 6, 30, 9, 5, 0, 29,
    16, 8, 2, 4, 21, 0, 19, 28,
    25, 15, 0, 7, 10, 1, 17, 3,
    22, 20, 26, 0, 11, 18, 23,
    27, 12, 24, 13, 14, 0
};

#endif

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int backup_countl_impl(std::uint32_t x) noexcept
{
    #if defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_CUDA)

    constexpr int countl_mod37[37] = {
        32, 31, 6, 30, 9, 5, 0, 29,
        16, 8, 2, 4, 21, 0, 19, 28,
        25, 15, 0, 7, 10, 1, 17, 3,
        22, 20, 26, 0, 11, 18, 23,
        27, 12, 24, 13, 14, 0
    };

    #endif

    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;

    return countl_mod37[x % 37];
}

#if BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN(__builtin_clz) && !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_CUDA))

constexpr int countl_impl(unsigned int x) noexcept
{
    return x ? __builtin_clz(x) : std::numeric_limits<unsigned int>::digits;
}

constexpr int countl_impl(unsigned long x) noexcept
{
    return x ? __builtin_clzl(x) : std::numeric_limits<unsigned long>::digits;
}

constexpr int countl_impl(unsigned long long x) noexcept
{
    return x ? __builtin_clzll(x) : std::numeric_limits<unsigned long long>::digits;
}

#elif (defined(_M_AMD64) || defined(_M_ARM64)) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION) && !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_CUDA))

constexpr int countl_impl(std::uint32_t x) noexcept
{
    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(x))
    {
        return backup_countl_impl(x); // LCOV_EXCL_LINE
    }
    else
    {
        unsigned long r {};

        if (_BitScanReverse(&r, x))
        {
            return 31 - static_cast<int>(r);
        }
        else
        {
            return 32;
        }
    }
}

constexpr int countl_impl(std::uint64_t x) noexcept
{
    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(x))
    {
        return x ? bit_scan_reverse(static_cast<std::uint64_t>(x)) ^ 63 : std::numeric_limits<std::uint64_t>::digits; // LCOV_EXCL_LINE
    }
    else
    {
        unsigned long r {};

        if (_BitScanReverse64(&r, x))
        {
            return 63 - static_cast<int>(r);
        }
        else
        {
            return 64;
        }
    }
}

#elif defined(_M_IX86) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION)

constexpr int countl_impl(std::uint32_t x) noexcept
{
    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(x))
    {
        return backup_countl_impl(x); // LCOV_EXCL_LINE
    }
    else
    {
        unsigned long r {};

        if (_BitScanReverse(&r, x))
        {
            return 31 - static_cast<int>(r);
        }
        else
        {
            return 32;
        }
    }
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int countl_impl(std::uint64_t x) noexcept
{
    return x ? bit_scan_reverse(static_cast<std::uint64_t>(x)) ^ 63 : std::numeric_limits<std::uint64_t>::digits;
}

#else

template <typename T>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int countl_impl(T x) noexcept
{
    return x ? bit_scan_reverse(static_cast<std::uint64_t>(x)) ^ 63 : std::numeric_limits<T>::digits;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int countl_impl(std::uint32_t x) noexcept
{
    return backup_countl_impl(x);
}


#endif

} // namespace impl

template <typename T>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int countl_zero(T x) noexcept
{
    static_assert(std::numeric_limits<T>::is_integer && !std::numeric_limits<T>::is_signed,
                  "Can only count with unsigned integers");

    return impl::countl_impl(x);
}

} // namespace detail
} // namespace int128
} // namespace boost

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_CLZ_HPP
// ===== END boost/safe_numbers/detail/int128/detail/clz.hpp =====
// ===== BEGIN boost/safe_numbers/detail/int128/detail/common_mul.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_COMMON_MUL_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_COMMON_MUL_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/config.hpp

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILD_MODULE

#include <cstdint>

#endif

namespace boost {
namespace int128 {
namespace detail {

// High 64 bits of the 64x64 -> 128 product, computed with four 32-bit partial products
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr std::uint64_t umulh_generic(const std::uint64_t a, const std::uint64_t b) noexcept
{
    const std::uint64_t a_lo {a & UINT32_MAX};
    const std::uint64_t a_hi {a >> 32U};
    const std::uint64_t b_lo {b & UINT32_MAX};
    const std::uint64_t b_hi {b >> 32U};

    const std::uint64_t lo_lo {a_lo * b_lo};
    const std::uint64_t hi_lo {a_hi * b_lo};
    const std::uint64_t lo_hi {a_lo * b_hi};
    const std::uint64_t hi_hi {a_hi * b_hi};

    const std::uint64_t cross {(lo_lo >> 32U) + (hi_lo & UINT32_MAX) + (lo_hi & UINT32_MAX)};

    return hi_hi + (hi_lo >> 32U) + (lo_hi >> 32U) + (cross >> 32U);
}

// Full 64x64 -> 128 product
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr std::uint64_t umul(const std::uint64_t a, const std::uint64_t b, std::uint64_t& hi) noexcept
{
    #ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION

    if (!BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(a))
    {
        #if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)

        const detail::builtin_u128 product {static_cast<detail::builtin_u128>(a) * static_cast<detail::builtin_u128>(b)};
        hi = static_cast<std::uint64_t>(product >> 64U);
        return static_cast<std::uint64_t>(product);

        #elif defined(_M_AMD64) && !defined(__GNUC__) && !defined(__CUDA_ARCH__) && !defined(__SYCL_DEVICE_ONLY__)

        return _umul128(a, b, &hi);

        #elif defined(_M_ARM64) && !defined(__CUDA_ARCH__) && !defined(__SYCL_DEVICE_ONLY__)

        hi = __umulh(a, b);
        return a * b;

        #endif
    }

    #endif

    hi = umulh_generic(a, b);
    return a * b;
}

// Low 128 bits of a 128x128 product
template <typename ReturnType, typename T>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr ReturnType low_word_mul(const T& lhs, const T& rhs) noexcept
{
    using high_word_type = decltype(ReturnType{}.high);

    std::uint64_t result_high {};
    const std::uint64_t result_low {umul(lhs.low, rhs.low, result_high)};

    result_high += lhs.low * static_cast<std::uint64_t>(rhs.high);
    result_high += static_cast<std::uint64_t>(lhs.high) * rhs.low;

    return ReturnType{static_cast<high_word_type>(result_high), result_low};
}

// Low 128 bits of a 128x64 product
template <typename ReturnType, typename T>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr ReturnType low_word_mul(const T& lhs, const std::uint64_t rhs) noexcept
{
    using high_word_type = decltype(ReturnType{}.high);

    std::uint64_t result_high {};
    const std::uint64_t result_low {umul(lhs.low, rhs, result_high)};

    result_high += static_cast<std::uint64_t>(lhs.high) * rhs;

    return ReturnType{static_cast<high_word_type>(result_high), result_low};
}

// Low 128 bits of a 128x32 product
template <typename ReturnType, typename T>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr ReturnType low_word_mul(const T& lhs, const std::uint32_t rhs) noexcept
{
    return low_word_mul<ReturnType>(lhs, static_cast<std::uint64_t>(rhs));
}

} // namespace detail
} // namespace int128
} // namespace boost

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_COMMON_MUL_HPP
// ===== END boost/safe_numbers/detail/int128/detail/common_mul.hpp =====
// ===== BEGIN boost/safe_numbers/detail/int128/detail/common_div.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_COMMON_DIV_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_COMMON_DIV_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/config.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/clz.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/common_mul.hpp

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILD_MODULE

#include <cstdint>
#include <cstring>

#endif

namespace boost {
namespace int128 {
namespace detail {

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wassume"
#endif

template <typename T>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr void half_word_div(const T& lhs, const std::uint32_t rhs, T& quotient, T& remainder) noexcept
{
    using high_word_type = decltype(T{}.high);

    BOOST_SAFE_NUMBERS_DETAIL_INT128_ASSUME(rhs != 0); // LCOV_EXCL_LINE

    // Use Barrett reduction-inspired approach
    const std::uint64_t divisor {rhs};

    const auto q_high {static_cast<std::uint64_t>(lhs.high) / divisor};
    auto r {static_cast<std::uint64_t>(lhs.high) % divisor};

    const auto low_high {static_cast<std::uint32_t>(lhs.low >> 32U)};
    const auto low_low {static_cast<std::uint32_t>(lhs.low)};

    r = (r << 32U) | low_high;
    const auto q_mid {r / divisor};
    r %= divisor;

    r = (r << 32U) | low_low;
    const auto q_low {r / divisor};
    r %= divisor;

    quotient.high = static_cast<high_word_type>(q_high);
    quotient.low = (q_mid << 32U) | q_low;
    remainder.low = r;
}

template <typename T>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr void half_word_div(const T& lhs, const std::uint32_t rhs, T& quotient) noexcept
{
    using high_word_type = decltype(T{}.high);

    BOOST_SAFE_NUMBERS_DETAIL_INT128_ASSUME(rhs != 0); // LCOV_EXCL_LINE

    quotient.high = static_cast<high_word_type>(static_cast<std::uint64_t>(lhs.high) / rhs);
    auto remainder {((static_cast<std::uint64_t>(lhs.high) % rhs) << 32) | (lhs.low >> 32)};
    quotient.low = (remainder / rhs) << 32;
    remainder = ((remainder % rhs) << 32) | (lhs.low & UINT32_MAX);
    quotient.low |= (remainder / rhs) & UINT32_MAX;
}

// Portable 128-bit by 64-bit unsigned division producing a 64-bit quotient and remainder.
// This is the classic Hacker's Delight divlu (two 32-bit "digit" steps over 64-bit words).
// Precondition: u1 < d so the quotient is guaranteed to fit in 64 bits. It is constexpr-safe
// and serves as the fallback for udiv_2by1 on every target without a hardware 128/64 divide.
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr std::uint64_t divlu(std::uint64_t u1, std::uint64_t u0, std::uint64_t d, std::uint64_t& r) noexcept
{
    constexpr std::uint64_t b {UINT64_C(1) << 32U}; // Number base (2^32)

    BOOST_SAFE_NUMBERS_DETAIL_INT128_ASSUME(u1 < d); // LCOV_EXCL_LINE

    // D.1: normalize so that the divisor's most significant bit is set
    const auto s {countl_zero(d)};
    d <<= s;

    const auto vn1 {d >> 32U};
    const auto vn0 {d & UINT32_MAX};

    // Shift the dividend left by s. The (64 - s) shift is undefined when s == 0, so guard it.
    const auto un32 {s == 0 ? u1 : ((u1 << s) | (u0 >> (64 - s)))};
    const auto un10 {u0 << s};

    const auto un1 {un10 >> 32U};
    const auto un0 {un10 & UINT32_MAX};

    // First quotient digit
    auto q1 {un32 / vn1};
    auto rhat {un32 - (q1 * vn1)};

    while (q1 >= b || (q1 * vn0) > ((b * rhat) + un1))
    {
        --q1;
        rhat += vn1;
        if (rhat >= b)
        {
            break;
        }
    }

    const auto un21 {(un32 * b) + un1 - (q1 * d)};

    // Second quotient digit
    auto q0 {un21 / vn1};
    rhat = un21 - (q0 * vn1);

    while (q0 >= b || (q0 * vn0) > ((b * rhat) + un0))
    {
        --q0;
        rhat += vn1;
        if (rhat >= b)
        {
            break;
        }
    }

    // The remainder is shifted back down by the normalization amount
    r = ((un21 * b) + un0 - (q0 * d)) >> s;
    return (q1 * b) + q0;
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_X86_64_DIVQ)

// Inline asm cannot appear in a constexpr function body before C++20, so the x86-64 DIV
// instruction is wrapped in a non-constexpr helper that udiv_2by1 only calls at runtime.
BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE std::uint64_t udiv_2by1_divq(const std::uint64_t u1, const std::uint64_t u0, const std::uint64_t d, std::uint64_t& r) noexcept
{
    std::uint64_t q {};
    __asm__("divq %[d]" : "=a"(q), "=d"(r) : [d] "r"(d), "a"(u0), "d"(u1) : "cc");
    return q;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_X86_64_DIVQ

// Divides the 128-bit value (u1:u0) by d, returning a 64-bit quotient and the true remainder.
// Precondition: u1 < d. Mirrors common_mul.hpp::umul: a hardware instruction at runtime where
// one exists, and the portable divlu in constexpr evaluation and everywhere else.
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr std::uint64_t udiv_2by1(const std::uint64_t u1, const std::uint64_t u0, const std::uint64_t d, std::uint64_t& r) noexcept
{
    BOOST_SAFE_NUMBERS_DETAIL_INT128_ASSUME(u1 < d); // LCOV_EXCL_LINE

    #if (defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_X86_64_DIVQ) || (defined(_M_AMD64) && !defined(__GNUC__) && !defined(__clang__) && _MSC_VER >= 1920)) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION)

    if (!BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(u1))
    {
        #if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_X86_64_DIVQ)

        return udiv_2by1_divq(u1, u0, d, r);

        #else

        return _udiv128(u1, u0, d, &r);

        #endif
    }

    #endif

    return divlu(u1, u0, d, r);
}

#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4127) // Pre c++17 the if constexpr remainder part will hit this
#endif

// Divides the 128-bit value (uh:ul) by the 128-bit divisor (vh:vl) where vh != 0. Because the
// divisor is >= 2^64 the quotient is guaranteed to fit in a single 64-bit word, which is
// returned. When need_remainder is true the 128-bit remainder is written to (rem_hi:rem_lo).
//
// This is one normalized quotient digit (Knuth Algorithm D specialized to a 2-word divisor).
// The top-limb estimate qhat (reusing udiv_2by1, a hardware divq on x86-64) is bounded by
// Knuth Theorem B to q <= qhat <= q + 2; the D3 refinement against d0 tightens it to q <= qhat
// <= q + 1, and the conditional add-back then corrects the remaining off-by-one.
template <bool need_remainder>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr std::uint64_t div3by2(const std::uint64_t uh, const std::uint64_t ul,
    const std::uint64_t vh, const std::uint64_t vl, std::uint64_t& rem_hi, std::uint64_t& rem_lo) noexcept
{
    BOOST_SAFE_NUMBERS_DETAIL_INT128_ASSUME(vh != 0); // LCOV_EXCL_LINE

    // D.1: normalize so the divisor's most significant bit is set
    const auto s {countl_zero(vh)};
    const auto cs {64 - s};

    std::uint64_t d1 {};
    std::uint64_t d0 {};
    std::uint64_t u2 {};
    std::uint64_t u1 {};
    std::uint64_t u0 {};

    if (s == 0)
    {
        d1 = vh;
        d0 = vl;
        u2 = 0;
        u1 = uh;
        u0 = ul;
    }
    else
    {
        d1 = (vh << s) | (vl >> cs);
        d0 = vl << s;
        u2 = uh >> cs;
        u1 = (uh << s) | (ul >> cs);
        u0 = ul << s;
    }

    BOOST_SAFE_NUMBERS_DETAIL_INT128_ASSUME(u2 <= d1); // LCOV_EXCL_LINE

    // D.3: estimate the single quotient digit qhat = floor((u2:u1) / d1), clamped to 2^64 - 1.
    // rhat is the remainder of that estimate.
    std::uint64_t qhat {};
    std::uint64_t rhat {};
    bool rhat_overflow {false};
    if (u2 < d1)
    {
        qhat = udiv_2by1(u2, u1, d1, rhat);
    }
    else
    {
        // u2 == d1: floor((u2:u1)/d1) clamps to 2^64 - 1, leaving rhat == u1 + d1 (may carry).
        qhat = UINT64_MAX;
        rhat = u1 + d1;
        rhat_overflow = rhat < u1;
    }

    std::uint64_t qd0_hi {};
    auto qd0_lo {umul(qhat, d0, qd0_hi)};

    // Refine qhat against d0 (Knuth D3). The top-limb estimate alone can exceed the true quotient
    // by up to 2; this brings it down to at most one too large, which the add-back below corrects.
    // At most two iterations run, and only while the running remainder rhat stays below 2^64.
    if (!rhat_overflow)
    {
        while (qd0_hi > rhat || (qd0_hi == rhat && qd0_lo > u0))
        {
            --qhat;
            rhat += d1;
            const auto rhat_carry {rhat < d1};
            qd0_lo = umul(qhat, d0, qd0_hi);
            if (rhat_carry)
            {
                break;
            }
        }
    }

    // D.4: multiply and subtract (u2:u1:u0) - qhat * (d1:d0). qd0 already holds qhat * d0.
    std::uint64_t qd1_hi {};
    const auto qd1_lo {umul(qhat, d1, qd1_hi)};

    const auto p0 {qd0_lo};
    const auto p1 {qd0_hi + qd1_lo};
    const auto p2 {qd1_hi + static_cast<std::uint64_t>(p1 < qd0_hi)};

    const auto r0 {u0 - p0};
    const auto borrow0 {static_cast<std::uint64_t>(u0 < p0)};
    const auto t1 {u1 - p1};
    auto r1 {t1 - borrow0};
    const auto borrow1 {static_cast<std::uint64_t>(u1 < p1) + static_cast<std::uint64_t>(t1 < borrow0)};

    // D.5/D.6: if the top limb borrowed, qhat was one too large. Correct it and add the divisor
    // back into the remainder. The probability of this branch is small.
    auto r0_final {r0};
    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY((u2 < p2) || ((u2 - p2) < borrow1)))
    {
        --qhat;                                                  // LCOV_EXCL_LINE
        const auto sum0 {r0 + d0};                               // LCOV_EXCL_LINE
        r0_final = sum0;                                         // LCOV_EXCL_LINE
        r1 = r1 + d1 + static_cast<std::uint64_t>(sum0 < r0);    // LCOV_EXCL_LINE
    }

    BOOST_SAFE_NUMBERS_DETAIL_INT128_IF_CONSTEXPR (need_remainder)
    {
        if (s == 0)
        {
            rem_hi = r1;
            rem_lo = r0_final;
        }
        else
        {
            rem_lo = (r0_final >> s) | (r1 << cs);
            rem_hi = r1 >> s;
        }
    }
    else
    {
        static_cast<void>(rem_hi);
        static_cast<void>(rem_lo);
    }

    return qhat;
}

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif

namespace impl {

#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4127) // Pre c++17 the if constexpr remainder part will hit this
#endif

template <std::size_t v_size>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr void unpack_v(std::uint32_t (&vn)[4], const std::uint32_t (&v)[v_size],
    const bool needs_shift, const int s, const int complement_s, const std::integral_constant<std::size_t, 2>&) noexcept
{
    vn[1] = needs_shift ? ((v[1] << s) | (v[0] >> complement_s)) : v[1];
    vn[0] = needs_shift ? (v[0] << s) : v[0];
}

template <std::size_t v_size>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr void unpack_v(std::uint32_t (&vn)[4], const std::uint32_t (&v)[v_size],
    const bool needs_shift, const int s, const int complement_s, const std::integral_constant<std::size_t, 4>&) noexcept
{
    vn[3] = needs_shift ? ((v[3] << s) | (v[2] >> complement_s)) : v[3];
    vn[2] = needs_shift ? ((v[2] << s) | (v[1] >> complement_s)) : v[2];
    vn[1] = needs_shift ? ((v[1] << s) | (v[0] >> complement_s)) : v[1];
    vn[0] = needs_shift ? (v[0] << s) : v[0];
}

// See: The Art of Computer Programming Volume 2 (Semi-numerical algorithms) section 4.3.1
// Algorithm D: Division of Non-negative integers
template <bool need_remainder, std::size_t u_size, std::size_t v_size, std::size_t q_size>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr void knuth_divide(std::uint32_t (&u)[u_size], const std::size_t m,
                            const std::uint32_t (&v)[v_size], const std::size_t n,
                            std::uint32_t (&q)[q_size]) noexcept
{
    // D.1
    const auto s {countl_zero(v[n - 1])};
    const auto complement_s {32 - s};
    const bool needs_shift {s > 0};

    // Create normalized versions of u and v
    std::uint32_t un[5] {};
    std::uint32_t vn[4] {};

    un[4] = needs_shift ? (u[3] >> complement_s) : 0;
    un[3] = needs_shift ? ((u[3] << s) | (u[2] >> complement_s)) : u[3];
    un[2] = needs_shift ? ((u[2] << s) | (u[1] >> complement_s)) : u[2];
    un[1] = needs_shift ? ((u[1] << s) | (u[0] >> complement_s)) : u[1];
    un[0] = needs_shift ? (u[0] << s) : u[0];

    static_assert(v_size == 4 || v_size == 2, "Unknown size for denominator");
    unpack_v(vn, v, needs_shift, s, complement_s, std::integral_constant<std::size_t, v_size>{});

    // D.2
    for (std::size_t j {m - n}; j != static_cast<std::size_t>(-1); --j)
    {
        // D.3
        const auto dividend {(static_cast<std::uint64_t>(un[j+n]) << 32) | un[j+n-1]};
        const auto divisor {static_cast<std::uint64_t>(vn[n-1])};
        auto q_hat {dividend / divisor};
        auto r_hat {dividend % divisor};

        while (q_hat > UINT32_MAX ||
               (q_hat * vn[n-2]) > ((r_hat << 32) | un[j+n-2]))
        {
            --q_hat;
            r_hat += vn[n-1];
            if (r_hat > UINT32_MAX)
            {
                break;
            }
        }

        // D.4
        std::int64_t borrow {};
        for (std::size_t i {}; i < n; ++i)
        {
            const auto p {q_hat * vn[i]};
            const auto p_lo {static_cast<std::uint32_t>(p & UINT32_MAX)};
            const auto p_hi {static_cast<std::uint32_t>(p >> 32)};

            borrow += static_cast<std::int64_t>(un[j+i]) - static_cast<std::int64_t>(p_lo);
            un[j+i] = static_cast<std::uint32_t>(borrow & UINT32_MAX);
            borrow >>= 32;

            borrow -= p_hi;
        }
        borrow += un[j+n];
        un[j+n] = static_cast<std::uint32_t>(borrow & UINT32_MAX);

        // D.5
        q[j] = static_cast<std::uint32_t>(q_hat & UINT32_MAX);
        if (BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY(borrow < 0))
        {
            // D.6
            // The probability of hitting this path is about 4.7e-10
            --q[j];                                                             // LCOV_EXCL_LINE
            std::uint64_t carry {};                                             // LCOV_EXCL_LINE
            for (std::size_t i = 0; i < n; ++i)                                 // LCOV_EXCL_LINE
            {                                                                   // LCOV_EXCL_LINE
                carry += static_cast<std::uint64_t>(un[j+i]) + vn[i];           // LCOV_EXCL_LINE
                un[j+i] = static_cast<std::uint32_t>(carry & UINT32_MAX);       // LCOV_EXCL_LINE
                carry >>= 32U;                                                  // LCOV_EXCL_LINE
            }                                                                   // LCOV_EXCL_LINE
            un[j+n] += static_cast<std::uint32_t>(carry & UINT32_MAX);          // LCOV_EXCL_LINE
        }
    }

    // D.8
    // If we are only calculating division we can completely skip this step
    BOOST_SAFE_NUMBERS_DETAIL_INT128_IF_CONSTEXPR (need_remainder)
    {
        if (s > 0)
        {
            for (std::size_t i {}; i < n-1; i++)
            {
                u[i] = (un[i] >> s) | (un[i+1] << (32 - s));
            }
            u[n-1] = un[n-1] >> s;
        }
        else
        {
            for (std::size_t i {}; i < n; i++)
            {
                u[i] = un[i];
            }
        }

        // Clear anything left in u
        for (std::size_t i {n}; i < m; i++)
        {
            u[i] = 0;
        }
    }
}

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif

template <typename T>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr std::size_t to_words(const T& x, std::uint32_t (&words)[4]) noexcept
{
    #if !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION) && !BOOST_SAFE_NUMBERS_DETAIL_INT128_ENDIAN_BIG_BYTE
    if (!BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(x))
    {
        std::memcpy(&words, &x, sizeof(T));
    }
    else
    #endif
    {
        words[0] = static_cast<std::uint32_t>(x.low & UINT32_MAX);                              // LCOV_EXCL_LINE
        words[1] = static_cast<std::uint32_t>(x.low >> 32);                                     // LCOV_EXCL_LINE
        words[2] = static_cast<std::uint32_t>(static_cast<std::uint64_t>(x.high) & UINT32_MAX); // LCOV_EXCL_LINE
        words[3] = static_cast<std::uint32_t>(static_cast<std::uint64_t>(x.high) >> 32);        // LCOV_EXCL_LINE
    }

    BOOST_SAFE_NUMBERS_DETAIL_INT128_ASSERT_MSG(x != static_cast<T>(0), "Division by 0");

    std::size_t word_count {4};
    while (words[word_count - 1U] == 0U)
    {
        word_count--;
    }

    return word_count;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr std::size_t to_words(const std::uint64_t x, std::uint32_t (&words)[2]) noexcept
{
    #if !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION) && !BOOST_SAFE_NUMBERS_DETAIL_INT128_ENDIAN_BIG_BYTE
    if (!BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(x))
    {
        std::memcpy(&words, &x, sizeof(std::uint64_t));
    }
    else
    #endif
    {
        words[0] = static_cast<std::uint32_t>(x & UINT32_MAX);  // LCOV_EXCL_LINE
        words[1] = static_cast<std::uint32_t>(x >> 32);         // LCOV_EXCL_LINE
    }

    return x > UINT32_MAX ? 2 : 1;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr std::size_t to_words(const std::uint32_t x, std::uint32_t (&words)[1]) noexcept
{
    words[0] = x;

    return 1;
}

template <typename T>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr T from_words(const std::uint32_t (&words)[4]) noexcept
{
    using high_word_type = decltype(T{}.high);

    const auto low {static_cast<std::uint64_t>(words[0]) | (static_cast<std::uint64_t>(words[1]) << 32)};
    const auto high {static_cast<std::uint64_t>(words[2]) | (static_cast<std::uint64_t>(words[3]) << 32)};

    return {static_cast<high_word_type>(high), low};
}

} // namespace impl

// We only need to take the time to process the remainder in the modulo case
// In the division case it is a waste of cycles
//
// 128/64 -> 128-bit quotient (and optional 64-bit remainder) by two-step long division.
// The leading 64/64 yields the high quotient word and a remainder r < rhs, which satisfies
// the udiv_2by1 precondition for the low quotient word. This covers every rhs (including
// rhs <= UINT32_MAX) through the single hardware-or-portable udiv_2by1 primitive.

template <typename T>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr void one_word_div(const T& lhs, const std::uint64_t rhs, T& quotient) noexcept
{
    using high_word_type = decltype(T{}.high);

    BOOST_SAFE_NUMBERS_DETAIL_INT128_ASSUME(rhs != 0); // LCOV_EXCL_LINE

    const auto u_high {static_cast<std::uint64_t>(lhs.high)};

    quotient.high = static_cast<high_word_type>(u_high / rhs);
    auto r {u_high % rhs};
    quotient.low = udiv_2by1(r, lhs.low, rhs, r);
}

template <typename T>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr void one_word_div(const T& lhs, const std::uint64_t rhs, T& quotient, T& remainder) noexcept
{
    using high_word_type = decltype(T{}.high);

    BOOST_SAFE_NUMBERS_DETAIL_INT128_ASSUME(rhs != 0); // LCOV_EXCL_LINE

    const auto u_high {static_cast<std::uint64_t>(lhs.high)};

    quotient.high = static_cast<high_word_type>(u_high / rhs);
    auto r {u_high % rhs};
    quotient.low = udiv_2by1(r, lhs.low, rhs, r);

    remainder.high = static_cast<high_word_type>(0);
    remainder.low = r;
}

template <typename T>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr void one_word_div(const T& lhs, const std::uint32_t rhs, T& quotient, T& remainder) noexcept
{
    one_word_div(lhs, static_cast<std::uint64_t>(rhs), quotient, remainder);
}

template <typename T>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr void one_word_div(const T& lhs, const std::uint32_t rhs, T& quotient) noexcept
{
    one_word_div(lhs, static_cast<std::uint64_t>(rhs), quotient);
}

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4127) // Conditional expression is constant is true pre-C++17
#  pragma warning(disable : 4804) // Unsafe comparison with bool
#endif

template <typename T>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr T knuth_div(const T& dividend, const T& divisor) noexcept
{
    BOOST_SAFE_NUMBERS_DETAIL_INT128_ASSUME(divisor != static_cast<T>(0));

    using high_word_type = decltype(T{}.high);

    std::uint64_t rem_hi {};
    std::uint64_t rem_lo {};

    const auto q {div3by2<false>(static_cast<std::uint64_t>(dividend.high), dividend.low,
                                 static_cast<std::uint64_t>(divisor.high), divisor.low, rem_hi, rem_lo)};

    return T{static_cast<high_word_type>(0), q};
}

template <typename T>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr T knuth_div(const T& dividend, const T& divisor, T& remainder) noexcept
{
    BOOST_SAFE_NUMBERS_DETAIL_INT128_ASSUME(divisor != static_cast<T>(0));

    using high_word_type = decltype(T{}.high);

    std::uint64_t rem_hi {};
    std::uint64_t rem_lo {};

    const auto q {div3by2<true>(static_cast<std::uint64_t>(dividend.high), dividend.low,
                                static_cast<std::uint64_t>(divisor.high), divisor.low, rem_hi, rem_lo)};

    remainder = T{static_cast<high_word_type>(rem_hi), rem_lo};

    return T{static_cast<high_word_type>(0), q};
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

#if defined(__clang__)
#  pragma clang diagnostic pop
#endif


} // namespace detail
} // namespace int128
} // namespace boost

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_COMMON_DIV_HPP
// ===== END boost/safe_numbers/detail/int128/detail/common_div.hpp =====

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILD_MODULE

#include <cstdint>
#include <cstring>

#endif

namespace boost {
namespace int128 {

struct
    #if (defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)) && !defined(_M_IX86)
    alignas(alignof(detail::builtin_i128))
    #endif
int128_t
{
    #if BOOST_SAFE_NUMBERS_DETAIL_INT128_ENDIAN_LITTLE_BYTE
    std::uint64_t low {};
    std::int64_t high {};
    #else

    #ifdef __GNUC__
    #  pragma GCC diagnostic push
    #  pragma GCC diagnostic ignored "-Wreorder"
    #endif

    std::int64_t high {};
    std::uint64_t low {};

    #ifdef __GNUC__
    #  pragma GCC diagnostic pop
    #endif

    #endif

    // Defaulted basic construction
    constexpr int128_t() noexcept = default;
    constexpr int128_t(const int128_t&) noexcept = default;
    constexpr int128_t(int128_t&&) noexcept = default;
    constexpr int128_t& operator=(const int128_t&) noexcept = default;
    constexpr int128_t& operator=(int128_t&&) noexcept = default;

    // Requires a conversion file to be implemented
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t(const uint128_t& v) noexcept;

    // Construct from integral types
    #if BOOST_SAFE_NUMBERS_DETAIL_INT128_ENDIAN_LITTLE_BYTE

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t(const std::int64_t hi, const std::uint64_t lo) noexcept : low{lo}, high{hi} {}

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t(const SignedInteger v) noexcept : low {static_cast<std::uint64_t>(v)}, high {v < 0 ? -1 : 0} {}

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t(const UnsignedInteger v) noexcept : low {static_cast<std::uint64_t>(v)}, high {} {}

    #if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR int128_t(const detail::builtin_i128 v) noexcept : low {static_cast<std::uint64_t>(v & static_cast<detail::builtin_i128>(detail::low_word_mask))}, high {static_cast<std::int64_t>(v >> static_cast<detail::builtin_i128>(64U))} {}
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR int128_t(const detail::builtin_u128 v) noexcept : low {static_cast<std::uint64_t>(v & static_cast<detail::builtin_u128>(detail::low_word_mask))}, high {static_cast<std::int64_t>(v >> static_cast<detail::builtin_u128>(64U))} {}

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

    #else // Big endian

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t(const std::int64_t hi, const std::uint64_t lo) noexcept : high{hi}, low{lo} {}

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t(const SignedInteger v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t(const UnsignedInteger v) noexcept : high {}, low {static_cast<std::uint64_t>(v)} {}

    #if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR int128_t(const detail::builtin_i128 v) noexcept : high {static_cast<std::int64_t>(v >> 64U)}, low {static_cast<std::uint64_t>(v & detail::low_word_mask)} {}
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR int128_t(const detail::builtin_u128 v) noexcept : high {static_cast<std::int64_t>(v >> 64U)}, low {static_cast<std::uint64_t>(v & detail::low_word_mask)} {}

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_ENDIAN_LITTLE_BYTE

    // Construct from floating-point types
    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_FLOATING_POINT_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t(Float f) noexcept;

    // Integer Conversion operators
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE explicit constexpr operator bool() const noexcept { return low || high; }

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr operator SignedInteger() const noexcept { return static_cast<SignedInteger>(low); }

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr operator UnsignedInteger() const noexcept { return static_cast<UnsignedInteger>(low); }

    #if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR operator detail::builtin_i128() const noexcept { return static_cast<detail::builtin_i128>(static_cast<detail::builtin_u128>(high) << static_cast<detail::builtin_u128>(64)) | static_cast<detail::builtin_i128>(low); }

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR operator detail::builtin_u128() const noexcept { return (static_cast<detail::builtin_u128>(high) << static_cast<detail::builtin_u128>(64)) | static_cast<detail::builtin_u128>(low); }

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

    // Conversion to float
    // This is basically the same as ldexp(static_cast<T>(high), 64) + static_cast<T>(low),
    // but can be constexpr at C++11 instead of C++26
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr operator float() const noexcept;
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr operator double() const noexcept;

    // Long double does not exist on the CUDA or SYCL (spir64) device
    #if !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_GPU_SUPPORT)
    constexpr operator long double() const noexcept;
    #endif

    // Compound Or
    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& operator|=(Integer rhs) noexcept;

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& operator|=(int128_t rhs) noexcept;

    #ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline int128_t& operator|=(Integer rhs) noexcept;

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    // Compound And
    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& operator&=(Integer rhs) noexcept;

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& operator&=(int128_t rhs) noexcept;

    #ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline int128_t& operator&=(Integer rhs) noexcept;

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    // Compound XOR
    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& operator^=(Integer rhs) noexcept;

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& operator^=(int128_t rhs) noexcept;

    #ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline int128_t& operator^=(Integer rhs) noexcept;

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    // Compound Left Shift
    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& operator<<=(Integer rhs) noexcept;

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& operator<<=(int128_t rhs) noexcept;

    #ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline int128_t& operator<<=(Integer rhs) noexcept;

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    // Compound Right Shift
    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& operator>>=(Integer rhs) noexcept;

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& operator>>=(int128_t rhs) noexcept;

    #ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline int128_t& operator>>=(Integer rhs) noexcept;

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    // Prefix and postfix increment
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& operator++() noexcept;
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator++(int) noexcept;

    // Prefix and postfix decrment
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& operator--() noexcept;
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator--(int) noexcept;

    // Compound Addition
    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& operator+=(Integer rhs) noexcept;

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& operator+=(int128_t rhs) noexcept;

    #ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline int128_t& operator+=(Integer rhs) noexcept;

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    // Compound Subtraction
    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& operator-=(Integer rhs) noexcept;

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& operator-=(int128_t rhs) noexcept;

    #ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline int128_t& operator-=(Integer rhs) noexcept;

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    // Compound Multiplication
    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& operator*=(Integer rhs) noexcept;

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& operator*=(int128_t rhs) noexcept;

    #ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline int128_t& operator*=(Integer rhs) noexcept;

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    // Compound Division
    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& operator/=(Integer rhs) noexcept;

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& operator/=(int128_t rhs) noexcept;

    #ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline int128_t& operator/=(Integer rhs) noexcept;

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    // Compound Modulo
    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& operator%=(Integer rhs) noexcept;

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& operator%=(int128_t rhs) noexcept;

    #ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline int128_t& operator%=(Integer rhs) noexcept;

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128
};

//=====================================
// Absolute Value function
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t abs(int128_t value) noexcept
{
    if (value.high < 0)
    {
        value.low = ~value.low + 1U;
        value.high = static_cast<std::int64_t>(~static_cast<std::uint64_t>(value.high) + static_cast<std::uint64_t>(value.low == 0 ? 1 : 0));
    }
    
    return value;
}

//=====================================
// Float Conversion Operators
//=====================================

// The most correct way to do this would be std::ldexp(static_cast<T>(high), 64) + static_cast<T>(low);
// Since std::ldexp is not constexpr until C++23 we can work around this by multiplying the high word
// by 0xFFFFFFFF in order to generally replicate what ldexp is doing in the constexpr context.
// We also avoid pulling in <quadmath.h> for the __float128 case where we would need ldexpq

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t::operator float() const noexcept
{
    return static_cast<float>(high) * detail::offset_value_v<float> + static_cast<float>(low);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t::operator double() const noexcept
{
    return static_cast<double>(high) * detail::offset_value_v<double> + static_cast<double>(low);
}

#if !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_GPU_SUPPORT)

constexpr int128_t::operator long double() const noexcept
{
    return static_cast<long double>(high) * detail::offset_value_v<long double> + static_cast<long double>(low);
}

#endif

//=====================================
// Float Construction
//=====================================

// Inverse of operator(Float).
// NaN -> 0;
// f >= 2^127 -> INT128_MAX;
// f < -2^127 -> INT128_MIN.
template <BOOST_SAFE_NUMBERS_DETAIL_INT128_FLOATING_POINT_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t::int128_t(Float f) noexcept
{
    constexpr Float two_32 {static_cast<Float>(UINT64_C(1) << 32)};
    constexpr Float two_64 {two_32 * two_32};
    constexpr Float two_127 {two_64 * static_cast<Float>(UINT64_C(1) << 63)};

    // NaN: leave default-initialized (zero). NaN compares false to everything,
    // so neither >= 0 nor <= 0 holds.
    if (!(f >= Float{0}) && !(f <= Float{0}))
    {
        return;
    }

    if (f >= two_127)
    {
        high = (std::numeric_limits<std::int64_t>::max)();
        low = UINT64_MAX;
        return;
    }

    if (f <= -two_127)
    {
        high = (std::numeric_limits<std::int64_t>::min)();
        low = UINT64_C(0);
        return;
    }

    const bool negative {f < Float{0}};
    const Float abs_f {negative ? -f : f};

    std::uint64_t h {static_cast<std::uint64_t>(abs_f / two_64)};
    const Float remainder {abs_f - static_cast<Float>(h) * two_64};
    std::uint64_t l {static_cast<std::uint64_t>(remainder)};

    if (negative)
    {
        // Two's complement negation of (h, l): new_l = -l (with wraparound),
        // new_h = ~h if a borrow occurred (l != 0), else ~h + 1.
        const bool low_was_zero {l == UINT64_C(0)};
        l = UINT64_C(0) - l;
        h = ~h + (low_was_zero ? UINT64_C(1) : UINT64_C(0));
    }

    high = static_cast<std::int64_t>(h);
    low = l;
}

//=====================================
// Unary Operators
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator+(const int128_t value) noexcept
{
    return value;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator-(const int128_t value) noexcept
{
    return (value.low == 0) ? int128_t{static_cast<std::int64_t>(0ULL - static_cast<std::uint64_t>(value.high)), 0} :
                              int128_t{~value.high, ~value.low + 1};
}

//=====================================
// Equality Operators
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator==(const int128_t lhs, const bool rhs) noexcept
{
    return lhs.high == 0 && lhs.low == static_cast<std::uint64_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator==(const bool lhs, const int128_t rhs) noexcept
{
    return rhs.high == 0 && rhs.low == static_cast<std::uint64_t>(lhs);
}

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wsign-conversion"
#  pragma clang diagnostic ignored "-Wsign-compare"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#  pragma GCC diagnostic ignored "-Wsign-compare"
#endif

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator==(const int128_t lhs, const int128_t rhs) noexcept
{
    // x64 and ARM64 like the values in opposite directions

    #if defined(__aarch64__) || defined(_M_ARM64) || defined(__x86_64__) || defined(_M_X64) || defined(_M_IX86)

    return lhs.low == rhs.low && lhs.high == rhs.high;

    #else

    return lhs.high == rhs.high && lhs.low == rhs.low;

    #endif
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator==(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return lhs.high == (rhs < 0 ? -1 : 0) && lhs.low == static_cast<std::uint64_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator==(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high == (lhs < 0 ? -1 : 0) && rhs.low == static_cast<std::uint64_t>(lhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator==(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high == 0 && lhs.low == static_cast<std::uint64_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator==(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high == 0 && rhs.low == static_cast<std::uint64_t>(lhs);
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator==(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs == static_cast<int128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator==(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) == rhs;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

//=====================================
// Inequality Operators
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator!=(const int128_t lhs, const int128_t rhs) noexcept
{
    // x64 and ARM64 like the values in opposite directions

    #if defined(__aarch64__) || defined(_M_ARM64) || defined(_M_X64) || defined(_M_IX86)

    return lhs.low != rhs.low || lhs.high != rhs.high;

    #elif defined(__x86_64__) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__) && !defined(__clang__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high != rhs.high || lhs.low != rhs.low;
    }
    else
    {
        detail::builtin_i128 builtin_lhs {};
        detail::builtin_i128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_lhs));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_rhs));

        return builtin_lhs != builtin_rhs;
    }

    #else

    return lhs.high != rhs.high || lhs.low != rhs.low;

    #endif
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator!=(const int128_t lhs, const bool rhs) noexcept
{
    return lhs.high != 0 || lhs.low != static_cast<std::uint64_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator!=(const bool lhs, const int128_t rhs) noexcept
{
    return rhs.high != 0 || rhs.low != static_cast<std::uint64_t>(lhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator!=(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return lhs.high != (rhs < 0 ? -1 : 0) || lhs.low != static_cast<std::uint64_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator!=(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high != (lhs < 0 ? -1 : 0) || rhs.low != static_cast<std::uint64_t>(lhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator!=(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high != 0 || lhs.low != static_cast<std::uint64_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator!=(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high != 0 || rhs.low != static_cast<std::uint64_t>(lhs);
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator!=(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs != static_cast<int128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator!=(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) != rhs;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

//=====================================
// Less than Operators
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator<(const int128_t lhs, const int128_t rhs) noexcept
{
    // On ARM macs only with the clang compiler is casting to __int128 uniformly better (and seemingly cost free)
    #if defined(__aarch64__) && defined(__APPLE__) && defined(__clang__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)

    return static_cast<detail::builtin_i128>(lhs) < static_cast<detail::builtin_i128>(rhs);

    #elif defined(__x86_64__) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__) && !defined(__clang__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? lhs.low < rhs.low : lhs.high < rhs.high;
    }
    else
    {
        detail::builtin_i128 builtin_lhs {};
        detail::builtin_i128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_lhs));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_rhs));

        return builtin_lhs < builtin_rhs;
    }

    #else

    return lhs.high == rhs.high ? lhs.low < rhs.low : lhs.high < rhs.high;

    #endif
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator<(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high < 0 || (lhs.high == 0 && lhs.low < static_cast<std::uint64_t>(rhs));
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator<(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high > 0 || (rhs.high == 0 && static_cast<std::uint64_t>(lhs) < rhs.low);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator<(const int128_t lhs, const SignedInteger rhs) noexcept
{
    if (lhs.high < 0)
    {
        return rhs >= 0 ? true : lhs < static_cast<int128_t>(rhs);
    }

    if (lhs.high > 0 || rhs < 0)
    {
        return false;
    }

    return lhs.low < static_cast<std::uint64_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator<(const SignedInteger lhs, const int128_t rhs) noexcept
{
    if (rhs.high < 0)
    {
        return lhs >= 0 ? false : static_cast<int128_t>(lhs) < rhs;
    }

    // rhs is positive
    if (rhs.high > 0 || lhs < 0)
    {
        return true;
    }

    return static_cast<std::uint64_t>(lhs) < rhs.low;
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator<(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs < static_cast<int128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator<(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) < rhs;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

//=====================================
// Greater than Operators
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator>(const int128_t lhs, const int128_t rhs) noexcept
{
    // On ARM macs only with the clang compiler is casting to __int128 uniformly better (and seemingly cost free)
    #if defined(__aarch64__) && defined(__APPLE__) && defined(__clang__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)

    return static_cast<detail::builtin_i128>(lhs) > static_cast<detail::builtin_i128>(rhs);

    #elif defined(__x86_64__) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__) && !defined(__clang__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? lhs.low > rhs.low : lhs.high > rhs.high;
    }
    else
    {
        detail::builtin_i128 builtin_lhs {};
        detail::builtin_i128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_lhs));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_rhs));

        return builtin_lhs > builtin_rhs;
    }

    #else

    return lhs.high == rhs.high ? lhs.low > rhs.low : lhs.high > rhs.high;

    #endif
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator>(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return !(lhs < rhs) && !(lhs == rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator>(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return !(lhs < rhs) && !(lhs == rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator>(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high > 0 || (lhs.high == 0 && lhs.low > static_cast<std::uint64_t>(rhs));
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator>(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high < 0 || (rhs.high == 0 && static_cast<std::uint64_t>(lhs) > rhs.low);
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator>(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs > static_cast<int128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator>(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) > rhs;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

//=====================================
// Less Equal Operators
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator<=(const int128_t lhs, const int128_t rhs) noexcept
{
    // On ARM macs only with the clang compiler is casting to __int128 uniformly better (and seemingly cost free)
    #if defined(__aarch64__) && defined(__APPLE__) && defined(__clang__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)

    return static_cast<detail::builtin_i128>(lhs) <= static_cast<detail::builtin_i128>(rhs);

    #elif defined(__x86_64__) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__) && !defined(__clang__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? lhs.low <= rhs.low : lhs.high <= rhs.high;
    }
    else
    {
        detail::builtin_i128 builtin_lhs {};
        detail::builtin_i128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_lhs));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_rhs));

        return builtin_lhs <= builtin_rhs;
    }

    #else

    return lhs.high == rhs.high ? lhs.low <= rhs.low : lhs.high <= rhs.high;

    #endif
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator<=(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return !(lhs > rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator<=(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return !(lhs > rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator<=(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high < 0 || (lhs.high == 0 && lhs.low <= static_cast<std::uint64_t>(rhs));
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator<=(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high > 0 || (rhs.high == 0 && static_cast<std::uint64_t>(lhs) <= rhs.low);
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator<=(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs <= static_cast<int128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator<=(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) <= rhs;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

//=====================================
// Greater Equal Operators
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator>=(const int128_t lhs, const int128_t rhs) noexcept
{
    // On ARM macs only with the clang compiler is casting to __int128 uniformly better (and seemingly cost free)
    #if defined(__aarch64__) && defined(__APPLE__) && defined(__clang__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)

    return static_cast<detail::builtin_i128>(lhs) >= static_cast<detail::builtin_i128>(rhs);

    #elif defined(__x86_64__) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__) && !defined(__clang__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? lhs.low >= rhs.low : lhs.high >= rhs.high;
    }
    else
    {
        detail::builtin_i128 builtin_lhs {};
        detail::builtin_i128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_lhs));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_rhs));

        return builtin_lhs >= builtin_rhs;
    }

    #else

    return lhs.high == rhs.high ? lhs.low >= rhs.low : lhs.high >= rhs.high;

    #endif
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator>=(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return !(lhs < rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator>=(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return !(lhs < rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator>=(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high > 0 || (lhs.high == 0 && lhs.low >= static_cast<std::uint64_t>(rhs));
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator>=(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high < 0 || (rhs.high == 0 && static_cast<std::uint64_t>(lhs) >= rhs.low);
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator>=(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs >= static_cast<int128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator>=(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) >= rhs;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

//=====================================
// Spaceship Operator
//=====================================

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_SPACESHIP_OPERATOR

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr std::strong_ordering operator<=>(const int128_t lhs, const int128_t rhs) noexcept
{
    if (lhs < rhs)
    {
        return std::strong_ordering::less;
    }
    else if (lhs == rhs)
    {
        return std::strong_ordering::equivalent;
    }
    else
    {
        return std::strong_ordering::greater;
    }
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr std::strong_ordering operator<=>(const int128_t lhs, const SignedInteger rhs) noexcept
{
    if (lhs < rhs)
    {
        return std::strong_ordering::less;
    }
    else if (lhs == rhs)
    {
        return std::strong_ordering::equivalent;
    }
    else
    {
        return std::strong_ordering::greater;
    }
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr std::strong_ordering operator<=>(const SignedInteger lhs, const int128_t rhs) noexcept
{
    if (lhs < rhs)
    {
        return std::strong_ordering::less;
    }
    else if (lhs == rhs)
    {
        return std::strong_ordering::equivalent;
    }
    else
    {
        return std::strong_ordering::greater;
    }
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr std::strong_ordering operator<=>(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    if (lhs < rhs)
    {
        return std::strong_ordering::less;
    }
    else if (lhs == rhs)
    {
        return std::strong_ordering::equivalent;
    }
    else
    {
        return std::strong_ordering::greater;
    }
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr std::strong_ordering operator<=>(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    if (lhs < rhs)
    {
        return std::strong_ordering::less;
    }
    else if (lhs == rhs)
    {
        return std::strong_ordering::equivalent;
    }
    else
    {
        return std::strong_ordering::greater;
    }
}

#endif

//=====================================
// Not Operator
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator~(const int128_t rhs) noexcept
{
    return {~rhs.high, ~rhs.low};
}

//=====================================
// Or Operator
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator|(const int128_t lhs, const int128_t rhs) noexcept
{
    return {lhs.high | rhs.high, lhs.low | rhs.low};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator|(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return {lhs.high | (rhs < 0 ? -1 : 0), lhs.low | static_cast<std::uint64_t>(rhs)};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator|(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return {rhs.high | (lhs < 0 ? -1 : 0), static_cast<std::uint64_t>(lhs) | rhs.low};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator|(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return {lhs.high, lhs.low | static_cast<std::uint64_t>(rhs)};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator|(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return {rhs.high, static_cast<std::uint64_t>(lhs) | rhs.low};
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR int128_t operator|(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs | static_cast<int128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR int128_t operator|(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) | rhs;
}


#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

//=====================================
// Compound OR Operator
//=====================================

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator|=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this | rhs);
    return *this;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator|=(const int128_t rhs) noexcept
{
    *this = *this | rhs;
    return *this;
}

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_128BIT_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline int128_t& int128_t::operator|=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this | rhs);
    return *this;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

//=====================================
// And Operator
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator&(const int128_t lhs, const int128_t rhs) noexcept
{
    return {lhs.high & rhs.high, lhs.low & rhs.low};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator&(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return {lhs.high & (rhs < 0 ? -1 : 0), lhs.low & static_cast<std::uint64_t>(rhs)};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator&(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return {rhs.high & (lhs < 0 ? -1 : 0), static_cast<std::uint64_t>(lhs) & rhs.low};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator&(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return {0, lhs.low & static_cast<std::uint64_t>(rhs)};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator&(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return {0, static_cast<std::uint64_t>(lhs) & rhs.low};
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR int128_t operator&(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs & static_cast<int128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR int128_t operator&(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) & rhs;
}


#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_128BIT_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline int128_t& int128_t::operator&=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this & rhs);
    return *this;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

//=====================================
// Compound And Operator
//=====================================

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator&=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this & rhs);
    return *this;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator&=(const int128_t rhs) noexcept
{
    *this = *this & rhs;
    return *this;
}

//=====================================
// XOR Operator
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator^(const int128_t lhs, const int128_t rhs) noexcept
{
    return {lhs.high ^ rhs.high, lhs.low ^ rhs.low};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator^(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return {lhs.high ^ (rhs < 0 ? -1 : 0), lhs.low ^ static_cast<std::uint64_t>(rhs)};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator^(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return {rhs.high ^ (lhs < 0 ? -1 : 0), static_cast<std::uint64_t>(lhs) ^ rhs.low};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator^(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return {lhs.high, lhs.low ^ static_cast<std::uint64_t>(rhs)};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator^(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return {rhs.high, static_cast<std::uint64_t>(lhs) ^ rhs.low};
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR int128_t operator^(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs ^ static_cast<int128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR int128_t operator^(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) ^ rhs;
}


#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

//=====================================
// Compound XOR Operator
//=====================================

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator^=(Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this ^ rhs);
    return *this;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator^=(int128_t rhs) noexcept
{
    *this = *this ^ rhs;
    return *this;
}

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_128BIT_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline int128_t& int128_t::operator^=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this ^ rhs);
    return *this;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

//=====================================
// Left Shift Operator
//=====================================

namespace detail {

template <typename Integer>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t default_ls_impl(const int128_t lhs, const Integer rhs) noexcept
{
    static_assert(std::is_integral<Integer>::value, "Only builtin types allowed");

    BOOST_SAFE_NUMBERS_DETAIL_INT128_IF_CONSTEXPR (std::numeric_limits<Integer>::is_signed)
    {
        if (rhs < 0 || rhs >= 128)
        {
            return {0, 0};
        }
    }
    else
    {
        if (rhs >= 128)
        {
            return {0, 0};
        }
    }

    if (rhs == 0)
    {
        return lhs;
    }

    if (rhs == 64)
    {
        return {static_cast<std::int64_t>(lhs.low), 0};
    }

    if (rhs > 64)
    {
        return {static_cast<std::int64_t>(lhs.low << (rhs - 64)), 0};
    }

    // For shifts < 64
    std::uint64_t high_part = (static_cast<std::uint64_t>(lhs.high) << rhs) |
                              (lhs.low >> (64 - rhs));

    return {
        static_cast<std::int64_t>(high_part),
        lhs.low << rhs
    };
}

template <typename Integer>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE int128_t intrinsic_ls_impl(const int128_t lhs, const Integer rhs) noexcept
{
    BOOST_SAFE_NUMBERS_DETAIL_INT128_IF_CONSTEXPR (std::numeric_limits<Integer>::is_signed)
    {
        if (BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY(rhs >= 128 || rhs < 0))
        {
            return {0, 0};
        }
    }
    else
    {
        if (BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY(rhs >= 128))
        {
            return {0, 0};
        }
    }

    #ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

    // Left-shifting a negative builtin_i128 is UB pre-C++20
    #  if defined(__aarch64__)

    #if defined(__GNUC__) && __GNUC__ >= 8
    #  pragma GCC diagnostic push
    #  pragma GCC diagnostic ignored "-Wclass-memaccess"
    #endif

    builtin_u128 value;
    std::memcpy(&value, &lhs, sizeof(builtin_u128));
    const auto res {value << rhs};

    int128_t return_value;
    std::memcpy(&return_value, &res, sizeof(int128_t));
    return return_value;

    #if defined(__GNUC__) && __GNUC__ >= 8
    #  pragma GCC diagnostic pop
    #endif

    #  else

    return int128_t{static_cast<builtin_u128>(lhs) << rhs};

    #  endif

    #elif defined(_M_AMD64)

    if (rhs >= 64)
    {
        return {static_cast<std::int64_t>(lhs.low << (rhs - 64)), 0};
    }
    else
    {
        int128_t res;
        res.high = static_cast<std::int64_t>(__shiftleft128(lhs.low, static_cast<std::uint64_t>(lhs.high), static_cast<unsigned char>(rhs)));
        res.low = lhs.low << rhs;

        return res;
    }

    #else

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY(rhs == 0))
    {
        return lhs;
    }
    if (rhs == 64)
    {
        return {static_cast<std::int64_t>(lhs.low), 0};
    }

    if (rhs > 64)
    {
        return {static_cast<std::int64_t>(lhs.low << (rhs - 64)), 0};
    }

    // For shifts < 64
    const auto high_part = (static_cast<std::uint64_t>(lhs.high) << rhs) |
                           (lhs.low >> (64 - rhs));

    return {
        static_cast<std::int64_t>(high_part),
        lhs.low << rhs
    };

    #endif
}

} // namespace detail

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator<<(const int128_t lhs, const Integer rhs) noexcept
{
    #ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return detail::default_ls_impl(lhs, rhs); // LCOV_EXCL_LINE
    }
    else
    {
        return detail::intrinsic_ls_impl(lhs, rhs);
    }

    #else

    return detail::default_ls_impl(lhs, rhs);

    #endif
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator<<(const int128_t lhs, const int128_t rhs) noexcept
{
    if (rhs.high != 0 || rhs.low >= 128)
    {
        return 0;
    }

    return lhs << rhs.low;
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR detail::builtin_u128 operator<<(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(detail::builtin_u128) * 8};

    if (rhs.high != 0 || rhs.low >= bit_width)
    {
        return 0;
    }
    
    return lhs << static_cast<detail::builtin_u128>(rhs.low);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR detail::builtin_i128 operator<<(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(detail::builtin_i128) * 8};

    if (rhs.high != 0 || rhs.low >= bit_width)
    {
        return 0;
    }

    return lhs << static_cast<detail::builtin_u128>(rhs.low);
}

#endif

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <typename SignedInteger, std::enable_if_t<detail::is_signed_integer_v<SignedInteger> && (sizeof(SignedInteger) * 8 <= 16), bool> = true>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int operator<<(const SignedInteger lhs, const int128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(SignedInteger) * 8};

    if (rhs.high != 0 || rhs.low >= bit_width)
    {
        return 0;
    }

    return static_cast<int>(lhs) << rhs.low;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <typename UnsignedInteger, std::enable_if_t<detail::is_unsigned_integer_v<UnsignedInteger> && (sizeof(UnsignedInteger) * 8 <= 16), bool> = true>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr unsigned operator<<(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(UnsignedInteger) * 8};

    if (rhs.high != 0 || rhs.low >= bit_width)
    {
        return 0;
    }

    return static_cast<unsigned>(lhs) << rhs.low;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4804) // Unsafe use of type bool in operation
#endif // _MSC_VER

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator<<=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this << rhs);
    return *this;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator<<=(const int128_t rhs) noexcept
{
    *this = *this << rhs;
    return *this;
}

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_128BIT_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline int128_t& int128_t::operator<<=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this << rhs);
    return *this;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

//=====================================
// Right Shift Operator
//=====================================

namespace detail {

template <typename Integer>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t default_rs_impl(const int128_t lhs, const Integer rhs) noexcept
{
    BOOST_SAFE_NUMBERS_DETAIL_INT128_IF_CONSTEXPR (std::numeric_limits<Integer>::is_signed)
    {
        if (rhs >= 128 || rhs < 0)
        {
            return lhs.high < 0 ? int128_t{-1, UINT64_MAX} : int128_t{0, 0};
        }
    }
    else
    {
        if (rhs >= 128)
        {
            return lhs.high < 0 ? int128_t{-1, UINT64_MAX} : int128_t{0, 0};
        }
    }

    if (rhs == 0)
    {
        return lhs;
    }

    if (rhs >= 64)
    {
        return {lhs.high < 0 ? -1 : 0, static_cast<std::uint64_t>(lhs.high >> (rhs - 64))};
    }

    // For shifts < 64
    const auto high_to_low {static_cast<std::uint64_t>(lhs.high) << (64 - rhs)};
    const auto low_shifted {lhs.low >> rhs};
    const auto low_part {high_to_low | low_shifted};

    return {
        lhs.high >> rhs,
        low_part
    };
}

template <typename Integer>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE int128_t intrinsic_rs_impl(const int128_t lhs, const Integer rhs) noexcept
{
    BOOST_SAFE_NUMBERS_DETAIL_INT128_IF_CONSTEXPR (std::numeric_limits<Integer>::is_signed)
    {
        if (rhs >= 128 || rhs < 0)
        {
            return lhs.high < 0 ? int128_t{-1, UINT64_MAX} : int128_t{0, 0};
        }
    }
    else
    {
        if (rhs >= 128)
        {
            return lhs.high < 0 ? int128_t{-1, UINT64_MAX} : int128_t{0, 0};
        }
    }

    #ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

    #  if defined(__aarch64__)

    #if defined(__GNUC__) && __GNUC__ >= 8
    #  pragma GCC diagnostic push
    #  pragma GCC diagnostic ignored "-Wclass-memaccess"
    #endif

    builtin_i128 value;
    std::memcpy(&value, &lhs, sizeof(builtin_i128));
    const auto res {value >> rhs};

    int128_t return_value;
    std::memcpy(&return_value, &res, sizeof(int128_t));
    return return_value;

    #if defined(__GNUC__) && __GNUC__ >= 8
    #  pragma GCC diagnostic pop
    #endif

    #  else

    return static_cast<builtin_i128>(lhs) >> rhs;

    #  endif

    #elif defined(_M_AMD64)

    if (rhs >= 64)
    {
        return {lhs.high < 0 ? -1 : 0, static_cast<std::uint64_t>(lhs.high >> (rhs - 64))};
    }
    else
    {
        int128_t res;
        res.low = __shiftright128(lhs.low, static_cast<std::uint64_t>(lhs.high), static_cast<unsigned char>(rhs));
        res.high = lhs.high >> rhs;

        return res;
    }

    #else

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY(rhs == 0))
    {
        return lhs;
    }

    if (rhs >= 64)
    {
        return {lhs.high < 0 ? -1 : 0, static_cast<std::uint64_t>(lhs.high >> (rhs - 64))};
    }

    // For shifts < 64
    const auto high_to_low {static_cast<std::uint64_t>(lhs.high) << (64 - rhs)};
    const auto low_shifted {lhs.low >> rhs};
    const auto low_part {high_to_low | low_shifted};

    return {
        lhs.high >> rhs,
        low_part
    };

    #endif
}

} // namespace detail

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator>>(const int128_t lhs, const Integer rhs) noexcept
{
    #ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return detail::default_rs_impl(lhs, rhs); // LCOV_EXCL_LINE
    }
    else
    {
        return detail::intrinsic_rs_impl(lhs, rhs);
    }

    #else

    return detail::default_rs_impl(lhs, rhs);

    #endif
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator>>(const int128_t lhs, const int128_t rhs) noexcept
{
    if (rhs.high != 0 || rhs.low >= 128)
    {
        return 0;
    }

    return lhs >> rhs.low;
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR detail::builtin_u128 operator>>(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(detail::builtin_u128) * 8};

    if (rhs.high != 0 || rhs.low >= bit_width)
    {
        return 0;
    }

    return lhs >> static_cast<detail::builtin_u128>(rhs.low);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR detail::builtin_i128 operator>>(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(detail::builtin_i128) * 8};

    if (rhs.high != 0 || rhs.low >= bit_width)
    {
        return 0;
    }

    return lhs >> static_cast<detail::builtin_u128>(rhs.low);
}

#endif

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <typename SignedInteger, std::enable_if_t<detail::is_signed_integer_v<SignedInteger> && (sizeof(SignedInteger) * 8 <= 16), bool> = true>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int operator>>(const SignedInteger lhs, const int128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(SignedInteger) * 8};

    if (rhs.high != 0 || rhs.low >= bit_width)
    {
        return 0;
    }

    return static_cast<int>(lhs) >> rhs.low;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <typename UnsignedInteger, std::enable_if_t<detail::is_unsigned_integer_v<UnsignedInteger> && (sizeof(UnsignedInteger) * 8 <= 16), bool> = true>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr unsigned operator>>(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(UnsignedInteger) * 8};

    if (rhs.high != 0 || rhs.low >= bit_width)
    {
        return 0;
    }

    return static_cast<unsigned>(lhs) >> rhs.low;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4804) // Unsafe use of type bool in operation
#endif // _MSC_VER

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator>>=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this >> rhs);
    return *this;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator>>=(const int128_t rhs) noexcept
{
    *this = *this >> rhs;
    return *this;
}

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_128BIT_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline int128_t& int128_t::operator>>=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this >> rhs);
    return *this;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

//=====================================
// Increment Operators
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator++() noexcept
{
    if (++low == UINT64_C(0))
    {
        ++high;
    }

    return *this;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t int128_t::operator++(int) noexcept
{
    const auto temp {*this};
    ++(*this);
    return temp;
}

//=====================================
// Decrement Operators
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator--() noexcept
{
    if (low-- == UINT64_C(0))
    {
        --high;
    }

    return *this;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t int128_t::operator--(int) noexcept
{
    const auto temp {*this};
    --(*this);
    return temp;
}

//=====================================
// Addition Operators
//=====================================

namespace detail {

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr int128_t library_add(const int128_t lhs, const int128_t rhs) noexcept
{
    const auto new_low {lhs.low + rhs.low};
    const auto new_high {static_cast<std::uint64_t>(lhs.high) +
                                        static_cast<std::uint64_t>(rhs.high) +
                                        static_cast<std::uint64_t>(new_low < lhs.low)};

    return int128_t{static_cast<std::int64_t>(new_high), new_low};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr int128_t default_add(const int128_t lhs, const int128_t rhs) noexcept
{
    #if (defined(__x86_64__) || (defined(__aarch64__) && !defined(__APPLE__))) && !defined(_WIN32) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)

    // Compute in the unsigned domain so that overflow wraps modulo 2^128
    return int128_t{static_cast<detail::builtin_u128>(lhs) + static_cast<detail::builtin_u128>(rhs)};

    #elif defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN_ADD_OVERFLOW)

    std::uint64_t result_low {};
    std::uint64_t result_high {};

    result_high = static_cast<std::uint64_t>(lhs.high) + static_cast<std::uint64_t>(rhs.high) + __builtin_add_overflow(lhs.low, rhs.low, &result_low);

    return int128_t{static_cast<std::int64_t>(result_high), result_low};

    #elif defined(_M_AMD64) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return library_add(lhs, rhs); // LCOV_EXCL_LINE
    }
    else
    {
        int128_t result {};
        const auto carry {BOOST_SAFE_NUMBERS_DETAIL_INT128_ADD_CARRY(0, lhs.low, rhs.low, &result.low)};
        BOOST_SAFE_NUMBERS_DETAIL_INT128_ADD_CARRY(carry, static_cast<std::uint64_t>(lhs.high), static_cast<std::uint64_t>(rhs.high), reinterpret_cast<std::uint64_t*>(&result.high));

        return result;
    }

    #else

    return library_add(lhs, rhs);

    #endif
}

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr int128_t default_add(const int128_t lhs, const Integer rhs) noexcept
{
    const auto new_low {lhs.low + rhs};
    const auto new_high {static_cast<std::uint64_t>(lhs.high) + static_cast<std::uint64_t>(new_low < lhs.low)};

    return int128_t{static_cast<std::int64_t>(new_high), new_low};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr int128_t library_sub(const int128_t lhs, const int128_t rhs) noexcept
{
    const auto new_low {lhs.low - rhs.low};
    const auto new_high {static_cast<std::uint64_t>(lhs.high) - static_cast<std::uint64_t>(rhs.high) - static_cast<std::uint64_t>(lhs.low < rhs.low)};

    return int128_t{static_cast<std::int64_t>(new_high), new_low};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr int128_t default_sub(const int128_t lhs, const int128_t rhs) noexcept
{
    #if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN_SUB_OVERFLOW) && (!defined(__aarch64__) || defined(__APPLE__) || !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)) && !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_CUDA))

    // __builtin_sub_overflow is marked constexpr so we don't need if consteval handling
    std::uint64_t result_low {};
    const auto result_high {static_cast<std::uint64_t>(lhs.high) - static_cast<std::uint64_t>(rhs.high) - static_cast<std::uint64_t>(__builtin_sub_overflow(lhs.low, rhs.low, &result_low))};

    return int128_t{static_cast<std::int64_t>(result_high), result_low};

    #elif defined(__aarch64__) && !defined(__APPLE__)

    // Unsigned wrap for consistent two's-complement semantics
    return int128_t{static_cast<detail::builtin_u128>(lhs) - static_cast<detail::builtin_u128>(rhs)};

    #elif defined(_M_AMD64) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return library_sub(lhs, rhs); // LCOV_EXCL_LINE
    }
    else
    {
        int128_t result {};
        const auto borrow {BOOST_SAFE_NUMBERS_DETAIL_INT128_SUB_BORROW(0, lhs.low, rhs.low, &result.low)};
        BOOST_SAFE_NUMBERS_DETAIL_INT128_SUB_BORROW(borrow, static_cast<std::uint64_t>(lhs.high), static_cast<std::uint64_t>(rhs.high), reinterpret_cast<std::uint64_t*>(&result.high));

        return result;
    }

    #else

    return library_sub(lhs, rhs);

    #endif
}

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr int128_t default_sub(const int128_t lhs, const Integer rhs) noexcept
{
    const auto new_low {lhs.low - rhs};
    const auto new_high {static_cast<std::uint64_t>(lhs.high) - static_cast<std::uint64_t>(new_low > lhs.low)};
    return int128_t{static_cast<std::int64_t>(new_high), new_low};
}

}

// On s390x with multiple different versions of GCC and language standards
// doing addition via subtraction is >10% faster in the benchmarks
#if defined(__s390__) || defined(__s390x__)

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator+(const int128_t lhs, const int128_t rhs) noexcept
{
    return detail::default_sub(lhs, -rhs);
}

#else

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator+(const int128_t lhs, const int128_t rhs) noexcept
{
    return detail::default_add(lhs, rhs);
}

#endif

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator+(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return detail::default_add(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator+(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return detail::default_add(rhs, lhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator+(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return rhs > 0 ? detail::default_add(lhs, rhs) : detail::default_sub(lhs, -rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator+(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return lhs > 0 ? detail::default_add(rhs, lhs) : detail::default_sub(rhs, -lhs);
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)


BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR int128_t operator+(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return detail::default_add(lhs, static_cast<int128_t>(rhs));
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR int128_t operator+(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return detail::default_add(rhs, static_cast<int128_t>(lhs));
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator+=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this + rhs);
    return *this;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator+=(const int128_t rhs) noexcept
{
    *this = *this + rhs;
    return *this;
}

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_128BIT_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline int128_t& int128_t::operator+=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this + rhs);
    return *this;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

//=====================================
// Subtraction Operators
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator-(const int128_t lhs, const int128_t rhs) noexcept
{
    return detail::default_sub(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator-(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return detail::default_sub(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator-(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return detail::default_add(-rhs, lhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator-(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return detail::default_sub(lhs, static_cast<int128_t>(rhs));
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator-(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return detail::default_sub(static_cast<int128_t>(lhs), rhs);
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)


BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR int128_t operator-(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs - static_cast<int128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR int128_t operator-(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) - rhs;
}

#endif

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator-=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this - rhs);
    return *this;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator-=(const int128_t rhs) noexcept
{
    *this = *this - rhs;
    return *this;
}

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_128BIT_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline int128_t& int128_t::operator-=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this - rhs);
    return *this;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

//=====================================
// Multiplication Operators
//=====================================

namespace detail {

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr int128_t default_mul(const int128_t lhs, const std::uint64_t rhs) noexcept
{
    return low_word_mul<int128_t>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr int128_t default_mul(const int128_t lhs, const std::uint32_t rhs) noexcept
{
    return low_word_mul<int128_t>(lhs, rhs);
}

#if defined(_M_AMD64) && !defined(__GNUC__)

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE int128_t msvc_amd64_mul(const int128_t lhs, const int128_t rhs) noexcept
{
    int128_t result {};
    result.low = _umul128(lhs.low, rhs.low, reinterpret_cast<std::uint64_t*>(&result.high));
    result.high += lhs.low * rhs.high;
    result.high += lhs.high * rhs.low;

    return result;
}

#endif

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr int128_t default_mul(const int128_t lhs, const int128_t rhs) noexcept
{
    #if ((defined(__aarch64__) && defined(__APPLE__)) || defined(__x86_64__) || defined(__PPC__) || defined(__powerpc__)) && defined(__GNUC__) && !defined(__clang__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)

    #  if !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return low_word_mul<int128_t>(lhs, rhs);
    }
    else
    {
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wclass-memaccess"

        detail::builtin_u128 new_lhs {};
        detail::builtin_u128 new_rhs {};

        std::memcpy(&new_lhs, &lhs, sizeof(detail::builtin_u128));
        std::memcpy(&new_rhs, &rhs, sizeof(detail::builtin_u128));

        const auto res {new_lhs * new_rhs};
        int128_t library_res {};

        std::memcpy(&library_res, &res, sizeof(detail::builtin_u128));

        return library_res;

        #pragma GCC diagnostic pop
    }

    #  elif defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)

    // Unsigned wrap for consistent two's-complement semantics
    return int128_t{static_cast<detail::builtin_u128>(lhs) * static_cast<detail::builtin_u128>(rhs)};

    #  else

    return low_word_mul<int128_t>(lhs, rhs);

    #  endif

    #elif defined(__aarch64__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)

    return int128_t{static_cast<detail::builtin_u128>(lhs) * static_cast<detail::builtin_u128>(rhs)};

    #elif defined(_M_AMD64) && !defined(__GNUC__) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(rhs))
    {
        return low_word_mul<int128_t>(lhs, rhs); // LCOV_EXCL_LINE
    }
    else
    {
        return msvc_amd64_mul(lhs, rhs);
    }

    #else

    return low_word_mul<int128_t>(lhs, rhs);

    #endif
}

} // namespace detail

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator*(const int128_t lhs, const int128_t rhs) noexcept
{
    return detail::default_mul(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator*(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    using local_eval_type = detail::evaluation_type_t<UnsignedInteger>;
    return detail::default_mul(lhs, static_cast<local_eval_type>(rhs));
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator*(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    using local_eval_type = detail::evaluation_type_t<UnsignedInteger>;
    return detail::default_mul(rhs, static_cast<local_eval_type>(lhs));
}

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4146) // Unary minus applied to unsigned
#endif

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator*(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return rhs < 0 ? -detail::default_mul(lhs, -static_cast<std::uint64_t>(rhs)) :
                      detail::default_mul(lhs, static_cast<std::uint64_t>(rhs));
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator*(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return lhs < 0 ? -detail::default_mul(rhs, -static_cast<std::uint64_t>(lhs)) :
                      detail::default_mul(rhs, static_cast<std::uint64_t>(lhs));
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR int128_t operator*(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return detail::default_mul(lhs, static_cast<int128_t>(rhs));
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR int128_t operator*(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return detail::default_mul(rhs, static_cast<int128_t>(lhs));
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator*=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this * rhs);
    return *this;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator*=(const int128_t rhs) noexcept
{
    *this = *this * rhs;
    return *this;
}

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_128BIT_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline int128_t& int128_t::operator*=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this * rhs);
    return *this;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

//=====================================
// Division Operator
//=====================================

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wassume"
#endif

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator/(const int128_t lhs, const int128_t rhs) noexcept
{
    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY(rhs == 0))
    {
        return {0, 0};
    }

    constexpr int128_t min_val {INT64_MIN, 0};
    const auto abs_lhs {abs(lhs)};
    const auto abs_rhs {abs(rhs)};

    if (lhs != min_val && abs_lhs < abs_rhs)
    {
        return {0,0};
    }

    const auto negative_res {(lhs.high < 0) != (rhs.high < 0)};

    // Narrow fast path: when the divisor magnitude fits in 64 bits, divide the magnitudes with
    // the hardware-accelerated one_word_div and reapply the sign. This reuses the abs values
    // computed above and beats native signed division (the out-of-line __divti3) for this case.
    if (abs_rhs.high == 0)
    {
        int128_t quotient {};

        if (abs_lhs.high == 0)
        {
            quotient = {0, abs_lhs.low / abs_rhs.low};
        }
        else
        {
            detail::one_word_div(abs_lhs, abs_rhs.low, quotient);
        }

        return negative_res ? -quotient : quotient;
    }

    #if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)

    return static_cast<int128_t>(static_cast<detail::builtin_i128>(lhs) / static_cast<detail::builtin_i128>(rhs));

    #else

    const auto quotient {detail::knuth_div(abs_lhs, abs_rhs)};
    return negative_res ? -quotient : quotient;

    #endif
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator/(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<UnsignedInteger>;

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY(rhs == 0))
    {
        return {0, 0};
    }

    const auto abs_lhs {abs(lhs)};

    int128_t quotient {};
    detail::one_word_div(abs_lhs, static_cast<eval_type>(rhs), quotient);
    return lhs < 0 ? -quotient : quotient;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator/(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY(rhs == 0))
    {
        return {0, 0};
    }

    if (rhs.high != 0 && rhs.high != -1)
    {
        return {0,0};
    }
    else
    {
        auto abs_rhs {abs(rhs)};
        const auto res {static_cast<std::uint64_t>(lhs) / abs_rhs.low};
        const int128_t result {0, res};
        return rhs < 0 ? -result : result;
    }
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator/(const int128_t lhs, const SignedInteger rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<SignedInteger>;

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY(rhs == 0))
    {
        return {0, 0};
    }

    int128_t quotient {};

    constexpr int128_t min_val {INT64_MIN, 0};
    const auto negative_res {static_cast<bool>((lhs.high < 0) ^ (rhs < 0))};
    const auto abs_rhs {rhs < 0 ? -rhs : rhs};
    const auto abs_lhs {abs(lhs)};

    if (lhs != min_val && abs_lhs < abs_rhs)
    {
        return {0, 0};
    }

    detail::one_word_div(abs_lhs, static_cast<eval_type>(abs_rhs), quotient);

    return negative_res ? -quotient : quotient;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator/(const SignedInteger lhs, const int128_t rhs) noexcept
{
    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY(rhs == 0))
    {
        return {0, 0};
    }

    if (rhs.high != 0 && rhs.high != -1)
    {
        return {0,0};
    }
    else
    {
        const auto negative_res {static_cast<bool>((rhs.high < 0) ^ (lhs < 0))};
        const auto abs_rhs {abs(rhs)};
        const auto abs_lhs {lhs < 0 ? -lhs : lhs};
        const int128_t res {0, static_cast<std::uint64_t>(abs_lhs) / abs_rhs.low};

        return negative_res ? -res : res;
    }
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)


BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR int128_t operator/(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return static_cast<int128_t>(static_cast<detail::builtin_i128>(lhs) / rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR int128_t operator/(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs / static_cast<detail::builtin_i128>(rhs));
}

#elif defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)


BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline int128_t operator/(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs / static_cast<int128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline int128_t operator/(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) / rhs;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator/=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this / rhs);
    return *this;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator/=(const int128_t rhs) noexcept
{
    *this = *this / rhs;
    return *this;
}

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_128BIT_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline int128_t& int128_t::operator/=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this / rhs);
    return *this;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#endif

//=====================================
// Modulo Operator
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator%(int128_t lhs, UnsignedInteger rhs) noexcept;

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator%(UnsignedInteger lhs, int128_t rhs) noexcept;

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator%(int128_t lhs, SignedInteger rhs) noexcept;

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator%(SignedInteger lhs, int128_t rhs) noexcept;

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator%(int128_t lhs, int128_t rhs) noexcept;

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator%(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<UnsignedInteger>;

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY(rhs == 0))
    {
        return {0, 0};
    }

    int128_t quotient {};
    int128_t remainder {};

    const auto abs_lhs {abs(lhs)};

    detail::one_word_div(abs_lhs, static_cast<eval_type>(rhs), quotient, remainder);

    return lhs < 0 ? -remainder : remainder;
}

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator%(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<UnsignedInteger>;

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY(rhs == 0))
    {
        return {0, 0};
    }

    const auto abs_rhs {abs(rhs)};

    if (abs_rhs > lhs)
    {
        return lhs;
    }

    const int128_t remainder {0, static_cast<eval_type>(lhs) % abs_rhs.low};

    return remainder;
}

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator%(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return lhs % static_cast<int128_t>(rhs);
}

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator%(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) % rhs;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator%(const int128_t lhs, const int128_t rhs) noexcept
{
    if (rhs == 0)
    {
        return {0, 0};
    }

    constexpr int128_t min_val {INT64_MIN, 0};
    const auto abs_lhs {abs(lhs)};
    const auto abs_rhs {abs(rhs)};

    if (lhs != min_val && rhs != min_val && abs_rhs > abs_lhs)
    {
        return lhs;
    }

    const auto is_neg {lhs < 0};

    // Narrow fast path: when the divisor magnitude fits in 64 bits, take the remainder of the
    // magnitudes with the hardware-accelerated one_word_div and reapply the dividend's sign.
    if (abs_rhs.high == 0)
    {
        int128_t remainder {};

        if (abs_lhs.high == 0)
        {
            remainder = int128_t{0, abs_lhs.low % abs_rhs.low};
        }
        else
        {
            int128_t quotient {};
            detail::one_word_div(abs_lhs, abs_rhs.low, quotient, remainder);
        }

        return is_neg ? -remainder : remainder;
    }

    #if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)

    return static_cast<int128_t>(static_cast<detail::builtin_i128>(lhs) % static_cast<detail::builtin_i128>(rhs));

    #else

    int128_t remainder {};
    detail::knuth_div(abs_lhs, abs_rhs, remainder);
    return is_neg ? -remainder : remainder;

    #endif
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR int128_t operator%(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return static_cast<detail::builtin_i128>(lhs) % rhs;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR int128_t operator%(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return lhs % static_cast<detail::builtin_i128>(rhs);
}


#elif defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline int128_t operator%(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs % static_cast<int128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline int128_t operator%(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) % rhs;
}


#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator%=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this % rhs);
    return *this;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator%=(const int128_t rhs) noexcept
{
    *this = *this % rhs;
    return *this;
}

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_128BIT_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline int128_t& int128_t::operator%=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this % rhs);
    return *this;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

namespace detail {

template <bool>
class numeric_limits_impl_i128
{
public:

        // Member constants
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;

    // C++23 deprecated the following two members
    #if defined(__GNUC__) && __cplusplus > 202002L
    #  pragma GCC diagnostic push
    #  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    #elif defined(_MSC_VER)
    #  pragma warning(push)
    #  pragma warning(disable:4996)
    #endif

    static constexpr std::float_denorm_style has_denorm = std::denorm_absent;
    static constexpr bool has_denorm_loss = false;

    #if defined(__GNUC__) && __cplusplus > 202002L
    #  pragma GCC diagnostic pop
    #elif defined(_MSC_VER)
    #  pragma warning(pop)
    #endif

    static constexpr std::float_round_style round_style = std::round_toward_zero;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = true;
    static constexpr int digits = 127;
    static constexpr int digits10 = 38;
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 2;
    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;
    static constexpr bool traps = std::numeric_limits<std::uint64_t>::traps;
    static constexpr bool tinyness_before = false;

    // Member functions
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE static constexpr auto (min)        () -> boost::int128::int128_t { return {INT64_MIN, 0}; }
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE static constexpr auto lowest       () -> boost::int128::int128_t { return {INT64_MIN, 0}; }
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE static constexpr auto (max)        () -> boost::int128::int128_t { return {INT64_MAX, UINT64_MAX}; }
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE static constexpr auto epsilon      () -> boost::int128::int128_t { return {0, 0}; }
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE static constexpr auto round_error  () -> boost::int128::int128_t { return {0, 0}; }
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE static constexpr auto infinity     () -> boost::int128::int128_t { return {0, 0}; }
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE static constexpr auto quiet_NaN    () -> boost::int128::int128_t { return {0, 0}; }
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE static constexpr auto signaling_NaN() -> boost::int128::int128_t { return {0, 0}; }
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE static constexpr auto denorm_min   () -> boost::int128::int128_t { return {0, 0}; }
};

#if !defined(__cpp_inline_variables) || __cpp_inline_variables < 201606L

template <bool b> constexpr bool numeric_limits_impl_i128<b>::is_specialized;
template <bool b> constexpr bool numeric_limits_impl_i128<b>::is_signed;
template <bool b> constexpr bool numeric_limits_impl_i128<b>::is_integer;
template <bool b> constexpr bool numeric_limits_impl_i128<b>::is_exact;
template <bool b> constexpr bool numeric_limits_impl_i128<b>::has_infinity;
template <bool b> constexpr bool numeric_limits_impl_i128<b>::has_quiet_NaN;
template <bool b> constexpr bool numeric_limits_impl_i128<b>::has_signaling_NaN;

// These members were deprecated in C++23
#if ((!defined(_MSC_VER) && (__cplusplus <= 202002L)) || (defined(_MSC_VER) && (_MSVC_LANG <= 202002L)))
template <bool b> constexpr std::float_denorm_style numeric_limits_impl_i128<b>::has_denorm;
template <bool b> constexpr bool numeric_limits_impl_i128<b>::has_denorm_loss;
#endif

template <bool b> constexpr std::float_round_style numeric_limits_impl_i128<b>::round_style;
template <bool b> constexpr bool numeric_limits_impl_i128<b>::is_iec559;
template <bool b> constexpr bool numeric_limits_impl_i128<b>::is_bounded;
template <bool b> constexpr bool numeric_limits_impl_i128<b>::is_modulo;
template <bool b> constexpr int numeric_limits_impl_i128<b>::digits;
template <bool b> constexpr int numeric_limits_impl_i128<b>::digits10;
template <bool b> constexpr int numeric_limits_impl_i128<b>::max_digits10;
template <bool b> constexpr int numeric_limits_impl_i128<b>::radix;
template <bool b> constexpr int numeric_limits_impl_i128<b>::min_exponent;
template <bool b> constexpr int numeric_limits_impl_i128<b>::min_exponent10;
template <bool b> constexpr int numeric_limits_impl_i128<b>::max_exponent;
template <bool b> constexpr int numeric_limits_impl_i128<b>::max_exponent10;
template <bool b> constexpr bool numeric_limits_impl_i128<b>::traps;
template <bool b> constexpr bool numeric_limits_impl_i128<b>::tinyness_before;

#endif // !defined(__cpp_inline_variables) || __cpp_inline_variables < 201606L

} // namespace detail

} // namespace int128
} // namespace boost

namespace std {

#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wmismatched-tags"
#endif

template <>
class numeric_limits<boost::int128::int128_t> :
    public boost::int128::detail::numeric_limits_impl_i128<true> {};

#ifdef __clang__
#  pragma clang diagnostic pop
#endif

} // namespace std

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_INT128_HPP
// ===== END boost/safe_numbers/detail/int128/detail/int128_imp.hpp =====
// ===== BEGIN boost/safe_numbers/detail/int128/detail/uint128_imp.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_UINT128_IMP_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_UINT128_IMP_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/fwd.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/config.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/traits.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/constants.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/clz.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/common_mul.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/common_div.hpp

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILD_MODULE

#include <cstdint>
#include <cstring>
#include <climits>

#endif

namespace boost {
namespace int128 {

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT struct
    #if (defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)) && !defined(_M_IX86)
    alignas(alignof(detail::builtin_u128))
    #endif
uint128_t
{
    #if BOOST_SAFE_NUMBERS_DETAIL_INT128_ENDIAN_LITTLE_BYTE
    std::uint64_t low {};
    std::uint64_t high {};
    #else

    #ifdef __GNUC__
    #  pragma GCC diagnostic push
    #  pragma GCC diagnostic ignored "-Wreorder"
    #endif

    std::uint64_t high {};
    std::uint64_t low {};

    #ifdef __GNUC__
    #  pragma GCC diagnostic pop
    #endif

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_ENDIAN_LITTLE_BYTE

    // Defaulted basic construction
    constexpr uint128_t() noexcept = default;
    constexpr uint128_t(const uint128_t&) noexcept = default;
    constexpr uint128_t(uint128_t&&) noexcept = default;
    constexpr uint128_t& operator=(const uint128_t&) noexcept = default;
    constexpr uint128_t& operator=(uint128_t&&) noexcept = default;

    // Requires a conversion file to be implemented
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t(const int128_t& v) noexcept;

    // Construct from integral types
    #if BOOST_SAFE_NUMBERS_DETAIL_INT128_ENDIAN_LITTLE_BYTE

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t(const std::uint64_t hi, const std::uint64_t lo) noexcept : low {lo}, high {hi} {}

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t(const SignedInteger v) noexcept : low {static_cast<std::uint64_t>(v)}, high {v < 0 ? UINT64_MAX : UINT64_C(0)} {}

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t(const UnsignedInteger v) noexcept : low {static_cast<std::uint64_t>(v)}, high {} {}

    #if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t(const detail::builtin_i128 v) noexcept :
        low {static_cast<std::uint64_t>(v)},
        high {static_cast<std::uint64_t>(static_cast<detail::builtin_u128>(v) >> static_cast<detail::builtin_u128>(64U))} {}

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t(const detail::builtin_u128 v) noexcept :
        low {static_cast<std::uint64_t>(v)},
        high {static_cast<std::uint64_t>(v >> static_cast<detail::builtin_i128>(64U))} {}

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

    #else // Big endian

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t(const std::uint64_t hi, const std::uint64_t lo) noexcept : high {hi}, low {lo} {}

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t(const SignedInteger v) noexcept : high {v < 0 ? UINT64_MAX : UINT64_C(0)}, low {static_cast<std::uint64_t>(v)} {}

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t(const UnsignedInteger v) noexcept : high {}, low {static_cast<std::uint64_t>(v)} {}

    #if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t(const detail::builtin_i128 v) noexcept :
        high {static_cast<std::uint64_t>(static_cast<detail::builtin_u128>(v) >> 64U)},
        low {static_cast<std::uint64_t>(v)} {}

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t(const detail::builtin_u128 v) noexcept :
        high {static_cast<std::uint64_t>(v >> 64U)},
        low {static_cast<std::uint64_t>(v)} {}

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_ENDIAN_LITTLE_BYTE

    // Construct from floating-point types
    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_FLOATING_POINT_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t(Float f) noexcept;

    // Integer conversion operators
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE explicit constexpr operator bool() const noexcept {return low || high; }

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr operator SignedInteger() const noexcept { return static_cast<SignedInteger>(low); }

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr operator UnsignedInteger() const noexcept { return static_cast<UnsignedInteger>(low); }

    #if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR operator detail::builtin_i128() const noexcept { return static_cast<detail::builtin_i128>(static_cast<detail::builtin_u128>(high) << static_cast<detail::builtin_u128>(64)) | static_cast<detail::builtin_i128>(low); }

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR operator detail::builtin_u128() const noexcept { return (static_cast<detail::builtin_u128>(high) << static_cast<detail::builtin_u128>(64)) | static_cast<detail::builtin_u128>(low); }

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

    // Conversion to float
    // This is basically the same as ldexp(static_cast<T>(high), 64) + static_cast<T>(low),
    // but can be constexpr at C++11 instead of C++26
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr operator float() const noexcept;
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr operator double() const noexcept;

    // long doubles do not exist on the CUDA or SYCL (spir64) device
    #if !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_GPU_SUPPORT)
    constexpr operator long double() const noexcept;
    #endif

    // Compound OR
    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& operator|=(Integer rhs) noexcept;

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& operator|=(uint128_t rhs) noexcept;

    #ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline uint128_t& operator|=(Integer rhs) noexcept;

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    // Compound AND
    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& operator&=(Integer rhs) noexcept;

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& operator&=(uint128_t rhs) noexcept;

    #ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline uint128_t& operator&=(Integer rhs) noexcept;

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    // Compound XOR
    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& operator^=(Integer rhs) noexcept;

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& operator^=(uint128_t rhs) noexcept;

    #ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline uint128_t& operator^=(Integer rhs) noexcept;

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    // Compound Left Shift
    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& operator<<=(Integer rhs) noexcept;

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& operator<<=(uint128_t rhs) noexcept;

    #ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline uint128_t& operator<<=(Integer rhs) noexcept;

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    // Compound Right Shift
    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& operator>>=(Integer rhs) noexcept;

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& operator>>=(uint128_t rhs) noexcept;

    #ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline uint128_t& operator>>=(Integer rhs) noexcept;

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& operator++() noexcept;
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator++(int) noexcept;
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& operator--() noexcept;
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator--(int) noexcept;

    // Compound Addition
    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& operator+=(Integer rhs) noexcept;

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& operator+=(uint128_t rhs) noexcept;

    #ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline uint128_t& operator+=(Integer rhs) noexcept;

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    // Compound Subtraction
    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& operator-=(Integer rhs) noexcept;

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& operator-=(uint128_t rhs) noexcept;

    #ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline uint128_t& operator-=(Integer rhs) noexcept;

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    // Compound Multiplication
    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& operator*=(Integer rhs) noexcept;

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& operator*=(uint128_t rhs) noexcept;

    #ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline uint128_t& operator*=(Integer rhs) noexcept;

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    // Compound Division
    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& operator/=(Integer rhs) noexcept;

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& operator/=(uint128_t rhs) noexcept;

    #ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline uint128_t& operator/=(Integer rhs) noexcept;

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    // Compound modulo
    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& operator%=(Integer rhs) noexcept;

    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& operator%=(uint128_t rhs) noexcept;

    #ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

    template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline uint128_t& operator%=(Integer rhs) noexcept;

    #endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128
};

//=====================================
// Absolute Value function
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t abs(const uint128_t value) noexcept
{
    return value;
}

//=====================================
// Float Conversion Operators
//=====================================

// The most correct way to do this would be std::ldexp(static_cast<T>(high), 64) + static_cast<T>(low);
// Since std::ldexp is not constexpr until C++23 we can work around this by multiplying the high word
// by 0xFFFFFFFF in order to generally replicate what ldexp is doing in the constexpr context.
// We also avoid pulling in <quadmath.h> for the __float128 case where we would need ldexpq

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t::operator float() const noexcept
{
    return static_cast<float>(high) * detail::offset_value_v<float> + static_cast<float>(low);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t::operator double() const noexcept
{
    return static_cast<double>(high) * detail::offset_value_v<double> + static_cast<double>(low);
}

#if !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_GPU_SUPPORT)

constexpr uint128_t::operator long double() const noexcept
{
    return static_cast<long double>(high) * detail::offset_value_v<long double> + static_cast<long double>(low);
}

#endif // __NVCC__

//=====================================
// Float Construction
//=====================================

// Inverse of operator(Float): decompose f into (high, low) by dividing by 2^64.
// NaN/negative -> 0
// overflow -> UINT128_MAX.
template <BOOST_SAFE_NUMBERS_DETAIL_INT128_FLOATING_POINT_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t::uint128_t(Float f) noexcept
{
    constexpr Float two_32 {static_cast<Float>(UINT64_C(1) << 32)};
    constexpr Float two_64 {two_32 * two_32};

    // !(f >= 0) catches both NaN and negative values without using <cmath>
    if (!(f >= Float{0}))
    {
        return;
    }

    // Overflow test: f >= 2^128 iff f / 2^64 >= 2^64. Comparing scaled values
    // avoids materializing 2^128 as a Float, which overflows to +infinity for
    // `float` and is therefore not constant-evaluable on older compilers.
    const Float scaled {f / two_64};
    if (scaled >= two_64)
    {
        high = UINT64_MAX;
        low = UINT64_MAX;
        return;
    }

    high = static_cast<std::uint64_t>(scaled);
    const Float remainder {f - static_cast<Float>(high) * two_64};
    low = static_cast<std::uint64_t>(remainder);
}

//=====================================
// Unary Operators
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator+(const uint128_t value) noexcept
{
    return value;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator-(const uint128_t value) noexcept
{
    return {~value.high + static_cast<std::uint64_t>(value.low == UINT64_C(0)), ~value.low + UINT64_C(1)};
}

//=====================================
// Equality Operators
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator==(const uint128_t lhs, const bool rhs) noexcept
{
    return lhs.high == UINT64_C(0) && lhs.low == static_cast<std::uint64_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator==(const bool lhs, const uint128_t rhs) noexcept
{
    return rhs.high == UINT64_C(0) && rhs.low == static_cast<std::uint64_t>(lhs);
}

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wsign-conversion"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#endif

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator==(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    const uint128_t rhs_u {rhs};
    return lhs.high == rhs_u.high && lhs.low == rhs_u.low;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator==(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    const uint128_t lhs_u {lhs};
    return lhs_u.high == rhs.high && lhs_u.low == rhs.low;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator==(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high == UINT64_C(0) && lhs.low == static_cast<std::uint64_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator==(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return rhs.high == UINT64_C(0) && rhs.low == static_cast<std::uint64_t>(lhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator==(const uint128_t lhs, const uint128_t rhs) noexcept
{
    #if defined(__aarch64__) || defined(_M_ARM64) || defined(_M_AMD64)

    return lhs.low == rhs.low && lhs.high == rhs.high;

    #elif defined (__x86_64__) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_BUILTIN_INT128)

    return static_cast<detail::builtin_u128>(lhs) == static_cast<detail::builtin_u128>(rhs);

    #elif (defined(__i386__) || defined(_M_IX86) || defined(_M_AMD64)) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION) && defined(__SSE2__)

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.low == rhs.low && lhs.high == rhs.high;
    }
    else
    {
        __m128i a = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&lhs));
        __m128i b = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&rhs));
        __m128i cmp = _mm_cmpeq_epi32(a, b);

        return _mm_movemask_ps(_mm_castsi128_ps(cmp)) == 0xF;
    }

    #else

    return lhs.high == rhs.high && lhs.low == rhs.low;

    #endif
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator==(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs == static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator==(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) == rhs;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator==(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs == static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator==(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) == rhs;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

//=====================================
// Inequality Operators
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator!=(const uint128_t lhs, const bool rhs) noexcept
{
    return lhs.high != UINT64_C(0) || lhs.low != static_cast<std::uint64_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator!=(const bool lhs, const uint128_t rhs) noexcept
{
    return rhs.high != UINT64_C(0) || rhs.low != static_cast<std::uint64_t>(lhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator!=(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    const uint128_t rhs_u {rhs};
    return lhs.high != rhs_u.high || lhs.low != rhs_u.low;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator!=(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    const uint128_t lhs_u {lhs};
    return lhs_u.high != rhs.high || lhs_u.low != rhs.low;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator!=(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high != UINT64_C(0) || lhs.low != static_cast<std::uint64_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator!=(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return rhs.high != UINT64_C(0) || rhs.low != static_cast<std::uint64_t>(lhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator!=(const uint128_t lhs, const uint128_t rhs) noexcept
{
    #if defined(__aarch64__) || defined(_M_ARM64) || defined(_M_AMD64)

    return lhs.low != rhs.low || lhs.high != rhs.high;

    #elif defined(__x86_64__) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_BUILTIN_INT128)

    return static_cast<detail::builtin_u128>(lhs) != static_cast<detail::builtin_u128>(rhs);

    #elif (defined(__i386__) || defined(_M_IX86)) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION) && defined(__SSE2__)

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.low != rhs.low || lhs.high != rhs.high;
    }
    else
    {
        __m128i a = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&lhs));
        __m128i b = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&rhs));
        __m128i cmp = _mm_cmpeq_epi32(a, b);

        return _mm_movemask_ps(_mm_castsi128_ps(cmp)) != 0xF;
    }

    #else

    return lhs.high != rhs.high || lhs.low != rhs.low;

    #endif
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR)


BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator!=(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs != static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator!=(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) != rhs;
}


BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator!=(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs != static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator!=(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) != rhs;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

//=====================================
// Less than Operators
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator<(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    const uint128_t rhs_u {rhs};
    return lhs.high == rhs_u.high ? lhs.low < rhs_u.low : lhs.high < rhs_u.high;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator<(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    const uint128_t lhs_u {lhs};
    return lhs_u.high == rhs.high ? lhs_u.low < rhs.low : lhs_u.high < rhs.high;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator<(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high == UINT64_C(0) && lhs.low < static_cast<std::uint64_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator<(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return rhs.high > UINT64_C(0) || static_cast<std::uint64_t>(lhs) < rhs.low;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator<(const uint128_t lhs, const uint128_t rhs) noexcept
{
    // On ARM macs only with the clang compiler is casting to unsigned __int128 uniformly better (and seemingly cost free)
    #if defined(__clang__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)

    return static_cast<detail::builtin_u128>(lhs) < static_cast<detail::builtin_u128>(rhs);

    #elif defined(__x86_64__) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__) && !defined(__clang__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? lhs.low < rhs.low : lhs.high < rhs.high; // LCOV_EXCL_LINE
    }
    else
    {
        detail::builtin_u128 builtin_lhs {};
        detail::builtin_u128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_lhs));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_rhs));

        return builtin_lhs < builtin_rhs;
    }

    #elif (defined(__i386__) || defined(_M_IX86) || defined(__arm__)) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? lhs.low < rhs.low : lhs.high < rhs.high; // LCOV_EXCL_LINE
    }
    else
    {
        std::uint32_t l[4] {};
        std::uint32_t r[4] {};
        std::memcpy(l, &lhs, sizeof(lhs));
        std::memcpy(r, &rhs, sizeof(rhs));

        if (l[3] != r[3])
        {
            return l[3] < r[3];
        }
        else if (l[2] != r[2])
        {
            return l[2] < r[2];
        }
        else if (l[1] != r[1])
        {
            return l[1] < r[1];
        }
        else
        {
            return l[0] < r[0];
        }
    }

    #else

    return lhs.high == rhs.high ? lhs.low < rhs.low : lhs.high < rhs.high;

    #endif
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)


BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator<(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs < static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator<(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) < rhs;
}


BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator<(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs < static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator<(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) < rhs;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

//=====================================
// Less Equal Operators
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator<=(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    const uint128_t rhs_u {rhs};
    return lhs.high == rhs_u.high ? lhs.low <= rhs_u.low : lhs.high < rhs_u.high;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator<=(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    const uint128_t lhs_u {lhs};
    return lhs_u.high == rhs.high ? lhs_u.low <= rhs.low : lhs_u.high < rhs.high;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator<=(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high == UINT64_C(0) && lhs.low <= static_cast<std::uint64_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator<=(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return rhs.high > UINT64_C(0) || static_cast<std::uint64_t>(lhs) <= rhs.low;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator<=(const uint128_t lhs, const uint128_t rhs) noexcept
{
    #if defined(__clang__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)

    return static_cast<detail::builtin_u128>(lhs) <= static_cast<detail::builtin_u128>(rhs);

    #elif defined(__x86_64__) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__) && !defined(__clang__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? lhs.low <= rhs.low : lhs.high <= rhs.high; // LCOV_EXCL_LINE
    }
    else
    {
        detail::builtin_u128 builtin_lhs {};
        detail::builtin_u128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_lhs));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_rhs));

        return builtin_lhs <= builtin_rhs;
    }

    #elif (defined(__i386__) || defined(_M_IX86) || defined(__arm__)) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? lhs.low <= rhs.low : lhs.high <= rhs.high; // LCOV_EXCL_LINE
    }
    else
    {
        std::uint32_t l[4] {};
        std::uint32_t r[4] {};
        std::memcpy(l, &lhs, sizeof(lhs));
        std::memcpy(r, &rhs, sizeof(rhs));

        if (l[3] != r[3])
        {
            return l[3] < r[3];
        }
        else if (l[2] != r[2])
        {
            return l[2] < r[2];
        }
        else if (l[1] != r[1])
        {
            return l[1] < r[1];
        }
        else
        {
            return l[0] <= r[0];
        }
    }

    #else

    return lhs.high == rhs.high ? lhs.low <= rhs.low : lhs.high <= rhs.high;

    #endif
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)


BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator<=(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs <= static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator<=(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) <= rhs;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator<=(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs <= static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator<=(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) <= rhs;
}


#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

//=====================================
// Greater Than Operators
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator>(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    const uint128_t rhs_u {rhs};
    return lhs.high == rhs_u.high ? lhs.low > rhs_u.low : lhs.high > rhs_u.high;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator>(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    const uint128_t lhs_u {lhs};
    return lhs_u.high == rhs.high ? lhs_u.low > rhs.low : lhs_u.high > rhs.high;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator>(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high > UINT64_C(0) || lhs.low > static_cast<std::uint64_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator>(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return rhs.high == UINT64_C(0) && static_cast<std::uint64_t>(lhs) > rhs.low;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator>(const uint128_t lhs, const uint128_t rhs) noexcept
{
    #if defined(__clang__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)

    return static_cast<detail::builtin_u128>(lhs) > static_cast<detail::builtin_u128>(rhs);

    #elif defined(__x86_64__) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__) && !defined(__clang__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? rhs.low < lhs.low : rhs.high < lhs.high; // LCOV_EXCL_LINE
    }
    else
    {
        detail::builtin_u128 builtin_lhs {};
        detail::builtin_u128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_lhs));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_rhs));

        return builtin_lhs > builtin_rhs;
    }

    #elif (defined(__i386__) || defined(_M_IX86) || defined(__arm__)) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? rhs.low < lhs.low : rhs.high < lhs.high; // LCOV_EXCL_LINE
    }
    else
    {
        std::uint32_t l[4] {};
        std::uint32_t r[4] {};
        std::memcpy(l, &lhs, sizeof(lhs));
        std::memcpy(r, &rhs, sizeof(rhs));

        if (l[3] != r[3])
        {
            return l[3] > r[3];
        }
        else if (l[2] != r[2])
        {
            return l[2] > r[2];
        }
        else if (l[1] != r[1])
        {
            return l[1] > r[1];
        }
        else
        {
            return l[0] > r[0];
        }
    }

    #else

    return lhs.high == rhs.high ? rhs.low < lhs.low : rhs.high < lhs.high;

    #endif
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)


BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator>(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs > static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator>(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) > rhs;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator>(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs > static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator>(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) > rhs;
}


#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

//=====================================
// Greater-equal Operators
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator>=(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    const uint128_t rhs_u {rhs};
    return lhs.high == rhs_u.high ? lhs.low >= rhs_u.low : lhs.high > rhs_u.high;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator>=(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    const uint128_t lhs_u {lhs};
    return lhs_u.high == rhs.high ? lhs_u.low >= rhs.low : lhs_u.high > rhs.high;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator>=(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high > UINT64_C(0) || lhs.low >= static_cast<std::uint64_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator>=(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return rhs.high == UINT64_C(0) && static_cast<std::uint64_t>(lhs) >= rhs.low;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator>=(const uint128_t lhs, const uint128_t rhs) noexcept
{
    #if defined(__clang__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)

    return static_cast<detail::builtin_u128>(lhs) >= static_cast<detail::builtin_u128>(rhs);

    #elif defined(__x86_64__) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__) && !defined(__clang__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? rhs.low <= lhs.low : rhs.high <= lhs.high; // LCOV_EXCL_LINE
    }
    else
    {
        detail::builtin_u128 builtin_lhs {};
        detail::builtin_u128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_lhs));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_rhs));

        return builtin_lhs >= builtin_rhs;
    }

    #elif (defined(__i386__) || defined(_M_IX86) || defined(__arm__)) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? rhs.low <= lhs.low : rhs.high <= lhs.high; // LCOV_EXCL_LINE
    }
    else
    {
        std::uint32_t l[4] {};
        std::uint32_t r[4] {};
        std::memcpy(l, &lhs, sizeof(lhs));
        std::memcpy(r, &rhs, sizeof(rhs));

        if (l[3] != r[3])
        {
            return l[3] > r[3];
        }
        else if (l[2] != r[2])
        {
            return l[2] > r[2];
        }
        else if (l[1] != r[1])
        {
            return l[1] > r[1];
        }
        else
        {
            return l[0] >= r[0];
        }
    }

    #else

    return lhs.high == rhs.high ? rhs.low <= lhs.low : rhs.high <= lhs.high;

    #endif
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator>=(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs >= static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator>=(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) >= rhs;
}


BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator>=(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs >= static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator>=(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) >= rhs;
}


#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

//=====================================
// Spaceship Operator
//=====================================

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_SPACESHIP_OPERATOR

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr std::strong_ordering operator<=>(const uint128_t lhs, const uint128_t rhs) noexcept
{
    if (lhs < rhs)
    {
        return std::strong_ordering::less;
    }
    else if (lhs == rhs)
    {
        return std::strong_ordering::equivalent;
    }
    else
    {
        return std::strong_ordering::greater;
    }
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr std::strong_ordering operator<=>(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    if (lhs < rhs)
    {
        return std::strong_ordering::less;
    }
    else if (lhs == rhs)
    {
        return std::strong_ordering::equivalent;
    }
    else
    {
        return std::strong_ordering::greater;
    }
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr std::strong_ordering operator<=>(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    if (lhs < rhs)
    {
        return std::strong_ordering::less;
    }
    else if (lhs == rhs)
    {
        return std::strong_ordering::equivalent;
    }
    else
    {
        return std::strong_ordering::greater;
    }
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr std::strong_ordering operator<=>(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    if (lhs < rhs)
    {
        return std::strong_ordering::less;
    }
    else if (lhs == rhs)
    {
        return std::strong_ordering::equivalent;
    }
    else
    {
        return std::strong_ordering::greater;
    }
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr std::strong_ordering operator<=>(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    if (lhs < rhs)
    {
        return std::strong_ordering::less;
    }
    else if (lhs == rhs)
    {
        return std::strong_ordering::equivalent;
    }
    else
    {
        return std::strong_ordering::greater;
    }
}

#endif

//=====================================
// Not Operator
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator~(const uint128_t rhs) noexcept
{
    return {~rhs.high, ~rhs.low};
}

//=====================================
// OR Operator
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator|(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    return {lhs.high | (rhs < 0 ? ~UINT64_C(0) : UINT64_C(0)), lhs.low | static_cast<std::uint64_t>(rhs)};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator|(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    return {rhs.high | (lhs < 0 ? ~UINT64_C(0) : UINT64_C(0)), rhs.low | static_cast<std::uint64_t>(lhs)};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator|(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return {lhs.high, lhs.low | static_cast<std::uint64_t>(rhs)};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator|(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return {rhs.high, rhs.low | static_cast<std::uint64_t>(lhs)};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator|(const uint128_t lhs, const uint128_t rhs) noexcept
{
    return {lhs.high | rhs.high, lhs.low | rhs.low};
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)


BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator|(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs | static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator|(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) | rhs;
}


BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator|(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs | static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator|(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) | rhs;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator|=(const Integer rhs) noexcept
{
    *this = *this | rhs;
    return *this;
}
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator|=(const uint128_t rhs) noexcept
{
    *this = *this | rhs;
    return *this;
}

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_128BIT_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline uint128_t& uint128_t::operator|=(const Integer rhs) noexcept
{
    *this = *this | rhs;
    return *this;
}

#endif

//=====================================
// AND Operator
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator&(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    return {lhs.high & (rhs < 0 ? ~UINT64_C(0) : UINT64_C(0)), lhs.low & static_cast<std::uint64_t>(rhs)};
}

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator&(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    return {rhs.high & (lhs < 0 ? ~UINT64_C(0) : UINT64_C(0)), rhs.low & static_cast<std::uint64_t>(lhs)};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator&(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return {UINT64_C(0), lhs.low & static_cast<std::uint64_t>(rhs)};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator&(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return {UINT64_C(0), rhs.low & static_cast<std::uint64_t>(lhs)};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator&(const uint128_t lhs, const uint128_t rhs) noexcept
{
    return {lhs.high & rhs.high, lhs.low & rhs.low};
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)


BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator&(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs & static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator&(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) & rhs;
}


BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator&(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs & static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator&(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) & rhs;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator&=(const Integer rhs) noexcept
{
    *this = *this & rhs;
    return *this;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator&=(const uint128_t rhs) noexcept
{
    *this = *this & rhs;
    return *this;
}

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_128BIT_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline uint128_t& uint128_t::operator&=(Integer rhs) noexcept
{
    *this = *this & rhs;
    return *this;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128


//=====================================
// XOR Operator
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator^(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    return {lhs.high ^ (rhs < 0 ? ~UINT64_C(0) : UINT64_C(0)), lhs.low ^ static_cast<std::uint64_t>(rhs)};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator^(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    return {rhs.high ^ (lhs < 0 ? ~UINT64_C(0) : UINT64_C(0)), rhs.low ^ static_cast<std::uint64_t>(lhs)};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator^(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return {lhs.high, lhs.low ^ static_cast<std::uint64_t>(rhs)};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator^(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return {rhs.high, rhs.low ^ static_cast<std::uint64_t>(lhs)};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator^(const uint128_t lhs, const uint128_t rhs) noexcept
{
    return {lhs.high ^ rhs.high, lhs.low ^ rhs.low};
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)


BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator^(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs ^ static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator^(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) ^ rhs;
}


BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator^(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs ^ static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator^(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) ^ rhs;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator^=(const Integer rhs) noexcept
{
    *this = *this ^ rhs;
    return *this;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator^=(const uint128_t rhs) noexcept
{
    *this = *this ^ rhs;
    return *this;
}

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_128BIT_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline uint128_t& uint128_t::operator^=(Integer rhs) noexcept
{
    *this = *this ^ rhs;
    return *this;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

//=====================================
// Left Shift Operator
//=====================================

namespace detail {

template <typename Integer>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t default_ls_impl(const uint128_t lhs, const Integer rhs) noexcept
{
    static_assert(std::is_integral<Integer>::value, "Needs to be a builtin type");

    BOOST_SAFE_NUMBERS_DETAIL_INT128_IF_CONSTEXPR (std::numeric_limits<Integer>::is_signed)
    {
        if (rhs < 0 || rhs >= 128)
        {
            return {0, 0};
        }
    }
    else
    {
        if (rhs >= 128)
        {
            return {0, 0};
        }
    }

    if (rhs == 0)
    {
        return lhs;
    }

    if (rhs == 64)
    {
        return {lhs.low, 0};
    }

    if (rhs > 64)
    {
        return {lhs.low << (rhs - 64), 0};
    }

    return {
        (lhs.high << rhs) | (lhs.low >> (64 - rhs)),
        lhs.low << rhs
    };
}

template <typename T>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE uint128_t intrinsic_ls_impl(const uint128_t lhs, const T rhs) noexcept
{
    BOOST_SAFE_NUMBERS_DETAIL_INT128_IF_CONSTEXPR (std::numeric_limits<T>::is_signed)
    {
        if (BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY(rhs >= 128 || rhs < 0))
        {
            return {0, 0};
        }
    }
    else
    {
        if (BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY(rhs >= 128))
        {
            return {0, 0};
        }
    }

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY(rhs == 0))
    {
        return lhs;
    }

    #ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

    #  if defined(__aarch64__)

        #if defined(__GNUC__) && __GNUC__ >= 8
        #  pragma GCC diagnostic push
        #  pragma GCC diagnostic ignored "-Wclass-memaccess"
        #endif

        builtin_u128 value;
        std::memcpy(&value, &lhs, sizeof(builtin_u128));
        const auto res {value << rhs};

        uint128_t return_value;
        std::memcpy(&return_value, &res, sizeof(uint128_t));
        return return_value;

        #if defined(__GNUC__) && __GNUC__ >= 8
        #  pragma GCC diagnostic pop
        #endif

    #  else

        return static_cast<builtin_u128>(lhs) << rhs;

    #  endif

    #else

    if (rhs == 64)
    {
        return {lhs.low, 0};
    }

    if (rhs > 64)
    {
        return {lhs.low << (rhs - 64), 0};
    }

    return {
        (lhs.high << rhs) | (lhs.low >> (64 - rhs)),
        lhs.low << rhs
    };

    #endif
}

} // namespace detail

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator<<(const uint128_t lhs, const Integer rhs) noexcept
{
    #ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return detail::default_ls_impl(lhs, rhs); // LCOV_EXCL_LINE
    }
    else
    {
        return detail::intrinsic_ls_impl(lhs, rhs);
    }

    #else

    return detail::default_ls_impl(lhs, rhs);

    #endif
}

// A number of different overloads to ensure that we return the same type as the builtins would

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator<<(const uint128_t lhs, const uint128_t rhs) noexcept
{
    if (rhs.high > UINT64_C(0) || rhs.low >= UINT64_C(128))
    {
        return uint128_t{0};
    }

    return lhs << rhs.low;
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR detail::builtin_u128 operator<<(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(detail::builtin_u128 ) * 8};

    if (rhs.high > UINT64_C(0) || rhs.low >= bit_width)
    {
        return 0;
    }
    return lhs << static_cast<detail::builtin_u128>(rhs.low);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR detail::builtin_i128 operator<<(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(detail::builtin_u128) * 8};

    if (rhs.high > UINT64_C(0) || rhs.low >= bit_width)
    {
        return 0;
    }

    return lhs << static_cast<detail::builtin_u128>(rhs.low);
}

#endif

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <typename SignedInteger, std::enable_if_t<detail::is_signed_integer_v<SignedInteger> && (sizeof(SignedInteger) * 8 <= 16), bool> = true>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int operator<<(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(SignedInteger) * 8};

    if (rhs.high > UINT64_C(0) || rhs.low >= bit_width)
    {
        return 0;
    }

    return static_cast<int>(lhs) << rhs.low;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <typename UnsignedInteger, std::enable_if_t<detail::is_unsigned_integer_v<UnsignedInteger> && (sizeof(UnsignedInteger) * 8 <= 16), bool> = true>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr unsigned int operator<<(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(UnsignedInteger) * 8};

    if (rhs.high > UINT64_C(0) || rhs.low >= bit_width)
    {
        return 0;
    }

    return static_cast<unsigned int>(lhs) << rhs.low;
}

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator<<=(const Integer rhs) noexcept
{
    *this = *this << rhs;
    return *this;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator<<=(const uint128_t rhs) noexcept
{
    *this = *this << rhs;
    return *this;
}

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_128BIT_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline uint128_t& uint128_t::operator<<=(Integer rhs) noexcept
{
    *this = *this << rhs;
    return *this;
}

#endif

//=====================================
// Right Shift Operator
//=====================================

namespace detail {

template <typename Integer>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t default_rs_impl(const uint128_t lhs, const Integer rhs) noexcept
{
    BOOST_SAFE_NUMBERS_DETAIL_INT128_IF_CONSTEXPR (std::numeric_limits<Integer>::is_signed)
    {
        if (rhs < 0 || rhs >= 128)
        {
            return {0, 0};
        }
    }
    else
    {
        if (rhs >= 128)
        {
            return {0, 0};
        }
    }

    if (rhs == 0)
    {
        return lhs;
    }

    if (rhs == 64)
    {
        return {0, lhs.high};
    }

    if (rhs > 64)
    {
        return {0, lhs.high >> (rhs - 64)};
    }

    return {
        lhs.high >> rhs,
        (lhs.low >> rhs) | (lhs.high << (64 - rhs))
    };
}

template <typename Integer>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE uint128_t intrinsic_rs_impl(const uint128_t lhs, const Integer rhs) noexcept
{
    BOOST_SAFE_NUMBERS_DETAIL_INT128_IF_CONSTEXPR (std::numeric_limits<Integer>::is_signed)
    {
        if (BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY(rhs >= 128 || rhs < 0))
        {
            return {0, 0};
        }
    }
    else
    {
        if (BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY(rhs >= 128))
        {
            return {0, 0};
        }
    }

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY(rhs == 0))
    {
        return lhs;
    }

    #ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

    #  ifdef __aarch64__

        #if defined(__GNUC__) && __GNUC__ >= 8
        #  pragma GCC diagnostic push
        #  pragma GCC diagnostic ignored "-Wclass-memaccess"
        #endif

        builtin_u128 value;
        std::memcpy(&value, &lhs, sizeof(builtin_u128));
        const auto res {value >> rhs};

        uint128_t return_value;
        std::memcpy(&return_value, &res, sizeof(uint128_t));
        return return_value;

        #if defined(__GNUC__) && __GNUC__ >= 8
        #  pragma GCC diagnostic pop
        #endif

    #  else
        return static_cast<builtin_u128>(lhs) >> rhs;
    #  endif

    #else

    if (rhs == 64)
    {
        return {0, lhs.high};
    }

    if (rhs < 64)
    {
        const auto result_low {(lhs.low >> rhs) | (lhs.high << (64 - rhs))};
        const auto result_high {lhs.high >> rhs};
        return {result_high, result_low};
    }

    return {0, lhs.high >> (rhs - 64)};

    #endif
}

} // namespace detail

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <typename Integer, std::enable_if_t<std::is_integral<Integer>::value, bool> = true>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator>>(const uint128_t lhs, const Integer rhs) noexcept
{
    #ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return detail::default_rs_impl(lhs, rhs); // LCOV_EXCL_LINE
    }
    else
    {
        return detail::intrinsic_rs_impl(lhs, rhs);
    }

    #else

    return detail::default_rs_impl(lhs, rhs);

    #endif
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator>>(const uint128_t lhs, const uint128_t rhs) noexcept
{
    if (rhs.high > UINT64_C(0) || rhs.low >= UINT64_C(128))
    {
        return uint128_t{0};
    }

    return lhs >> rhs.low;
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR detail::builtin_u128 operator>>(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    constexpr auto bit_width = sizeof(detail::builtin_u128) * 8;

    if (rhs.high > UINT64_C(0) || rhs.low >= bit_width)
    {
        return 0;
    }

    return lhs >> static_cast<detail::builtin_u128>(rhs.low);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR detail::builtin_i128 operator>>(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    constexpr auto bit_width = sizeof(detail::builtin_i128) * 8;

    if (rhs.high > UINT64_C(0) || rhs.low >= bit_width)
    {
        return 0;
    }

    return lhs >> static_cast<detail::builtin_u128>(rhs.low);
}

#endif

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <typename SignedInteger, std::enable_if_t<detail::is_signed_integer_v<SignedInteger> && (sizeof(SignedInteger) * 8 <= 16), bool> = true>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int operator>>(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    constexpr auto bit_width = sizeof(SignedInteger) * 8;

    if (rhs.high > UINT64_C(0) || rhs.low >= bit_width)
    {
        return 0;
    }

    return static_cast<int>(lhs) >> rhs.low;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <typename UnsignedInteger, std::enable_if_t<detail::is_unsigned_integer_v<UnsignedInteger> && (sizeof(UnsignedInteger) * 8 <= 16), bool> = true>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr unsigned operator>>(UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    constexpr auto bit_width = sizeof(UnsignedInteger) * 8;

    if (rhs.high > UINT64_C(0) || rhs.low >= bit_width)
    {
        return 0;
    }

    return static_cast<unsigned>(lhs) >> rhs.low;
}

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator>>=(const Integer rhs) noexcept
{
    *this = *this >> rhs;
    return *this;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator>>=(const uint128_t rhs) noexcept
{
    *this = *this >> rhs;
    return *this;
}

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_128BIT_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline uint128_t& uint128_t::operator>>=(Integer rhs) noexcept
{
    *this = *this >> rhs;
    return *this;
}

#endif

//=====================================
// Increment Operator
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator++() noexcept
{
    if (++low == UINT64_C(0))
    {
        ++high;
    }

    return *this;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t uint128_t::operator++(int) noexcept
{
    const auto temp {*this};
    ++(*this);
    return temp;
}

//=====================================
// Decrement Operator
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator--() noexcept
{
    if (--low == UINT64_MAX)
    {
        --high;
    }

    return *this;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t uint128_t::operator--(int) noexcept
{
    const auto temp {*this};
    --(*this);
    return temp;
}

//=====================================
// Addition Operator
//=====================================

namespace impl {

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr uint128_t default_add(const uint128_t lhs, const uint128_t rhs) noexcept
{
    #if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN_ADD_OVERFLOW) && (defined(__i386__) || (defined(__aarch64__) && !defined(__APPLE__)) || defined(__arm__) || (defined(__s390__) || defined(__s390x__)))

    uint128_t res {};
    res.high = lhs.high + rhs.high + __builtin_add_overflow(lhs.low, rhs.low, &res.low);

    return res;

    #else

    uint128_t temp {lhs.high + rhs.high, lhs.low + rhs.low};

    if (temp.low < lhs.low)
    {
        ++temp.high;
    }

    return temp;

    #endif
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr uint128_t default_add(const uint128_t lhs, const std::uint64_t rhs) noexcept
{
    #if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN_ADD_OVERFLOW) && (defined(__i386__) || (defined(__aarch64__) && !defined(__APPLE__)) || defined(__arm__) || (defined(__s390__) || defined(__s390x__)))

    uint128_t res {};
    res.high = lhs.high + __builtin_add_overflow(lhs.low, rhs, &res.low);

    return res;

    #else

    uint128_t temp {lhs.high, lhs.low + rhs};

    if (temp.low < lhs.low)
    {
        ++temp.high;
    }

    return temp;

    #endif
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr uint128_t default_sub(const uint128_t lhs, const uint128_t rhs) noexcept
{
    #if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN_SUB_OVERFLOW) && (defined(__i386__) || defined(__arm__) || (defined(__s390__) || defined(__s390x__)))

    uint128_t res {};
    res.high = lhs.high - rhs.high - __builtin_sub_overflow(lhs.low, rhs.low, &res.low);

    return res;

    #elif (defined(__x86_64__) || (defined(__aarch64__) && !defined(__APPLE__))) && !defined(_MSC_VER) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)

    return static_cast<uint128_t>(static_cast<detail::builtin_u128>(lhs) - static_cast<detail::builtin_u128>(rhs));

    #else

    uint128_t temp {lhs.high - rhs.high, lhs.low - rhs.low};

    // Check for carry
    if (lhs.low < rhs.low)
    {
        --temp.high;
    }

    return temp;

    #endif
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr uint128_t default_sub(const uint128_t lhs, const std::uint64_t rhs) noexcept
{
    #if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN_SUB_OVERFLOW) && (defined(__i386__) || (defined(__aarch64__) && !defined(__APPLE__)) || defined(__arm__) || (defined(__s390__) || defined(__s390x__)))

    uint128_t res {};
    res.high = lhs.high - __builtin_sub_overflow(lhs.low, rhs, &res.low);

    return res;

    #else

    uint128_t temp {lhs.high, lhs.low - rhs};

    // Check for carry
    if (lhs.low < rhs)
    {
        --temp.high;
    }

    return temp;

    #endif
}

} // namespace impl

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4146)
#endif

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator+(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    return rhs < 0 ? impl::default_sub(lhs, -static_cast<std::uint64_t>(rhs)) :
                     impl::default_add(lhs, static_cast<std::uint64_t>(rhs));
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator+(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    return lhs < 0 ? impl::default_sub(rhs, -static_cast<std::uint64_t>(lhs)) :
                     impl::default_add(rhs, static_cast<std::uint64_t>(lhs));
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator+(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return impl::default_add(lhs, static_cast<std::uint64_t>(rhs));
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator+(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return impl::default_add(rhs, static_cast<std::uint64_t>(lhs));
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator+(const uint128_t lhs, const uint128_t rhs) noexcept
{
    return impl::default_add(lhs, rhs);
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)


BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator+(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return impl::default_add(lhs, static_cast<uint128_t>(rhs));
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator+(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return impl::default_add(static_cast<uint128_t>(lhs), rhs);
}


BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator+(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return impl::default_add(lhs, static_cast<uint128_t>(rhs));
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator+(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return impl::default_add(static_cast<uint128_t>(lhs), rhs);
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator+=(const Integer rhs) noexcept
{
    *this = *this + rhs;
    return *this;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator+=(const uint128_t rhs) noexcept
{
    *this = *this + rhs;
    return *this;
}

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_128BIT_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline uint128_t& uint128_t::operator+=(const Integer rhs) noexcept
{
    *this = *this + rhs;
    return *this;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128


//=====================================
// Subtraction Operator
//=====================================

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4146)
#endif

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator-(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    return rhs < 0 ? impl::default_add(lhs, -static_cast<std::uint64_t>(rhs)) :
                     impl::default_sub(lhs, static_cast<std::uint64_t>(rhs));
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator-(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    return lhs < 0 ? impl::default_sub(-rhs, -static_cast<std::uint64_t>(lhs)) :
                     impl::default_add(-rhs, static_cast<std::uint64_t>(lhs));
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator-(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return impl::default_sub(lhs, static_cast<std::uint64_t>(rhs));
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator-(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return impl::default_add(-rhs, static_cast<std::uint64_t>(lhs));
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator-(const uint128_t lhs, const uint128_t rhs) noexcept
{
    return impl::default_sub(lhs, rhs);
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)


BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator-(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs - static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator-(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) - rhs;
}


BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator-(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs - static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator-(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) - rhs;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator-=(const Integer rhs) noexcept
{
    *this = *this - rhs;
    return *this;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator-=(const uint128_t rhs) noexcept
{
    *this = *this - rhs;
    return *this;
}

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_128BIT_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline uint128_t& uint128_t::operator-=(const Integer rhs) noexcept
{
    *this = *this - rhs;
    return *this;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

//=====================================
// Multiplication Operator
//=====================================

#if defined(__GNUC__) && __GNUC__ >= 8
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif

namespace detail {

#if defined(_M_AMD64) && !defined(__GNUC__)

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE uint128_t msvc_mul(const uint128_t lhs, const uint128_t rhs) noexcept
{
    uint128_t result {};
    result.low = _umul128(lhs.low, rhs.low, &result.high);
    result.high += lhs.low * rhs.high;
    result.high += lhs.high * rhs.low;

    return result;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE uint128_t msvc_mul(const uint128_t lhs, const std::uint64_t rhs) noexcept
{
    uint128_t result {};
    result.low = _umul128(lhs.low, rhs, &result.high);
    result.high += lhs.high * rhs;

    return result;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE uint128_t msvc_mul(const uint128_t lhs, const std::uint32_t rhs) noexcept
{
    uint128_t result {};
    result.low = _umul128(lhs.low, static_cast<std::uint64_t>(rhs), &result.high);
    result.high += lhs.high * static_cast<std::uint64_t>(rhs);

    return result;
}

#elif defined(_M_ARM64)

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE uint128_t msvc_mul(const uint128_t lhs, const uint128_t rhs) noexcept
{
    const auto low_low{lhs.low * rhs.low};
    const auto high_low_low{__umulh(lhs.low, rhs.low)};

    const auto low_high{lhs.low * rhs.high};
    const auto high_low{lhs.high * rhs.low};

    const auto high{high_low + low_high + high_low_low};

    return {high, low_low};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE uint128_t msvc_mul(const uint128_t lhs, const std::uint64_t rhs) noexcept
{
    const auto low{lhs.low * rhs};
    const auto high{__umulh(lhs.low, rhs) + (lhs.high * rhs)};

    return {high, low};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE uint128_t msvc_mul(const uint128_t lhs, const std::uint32_t rhs) noexcept
{
    const auto low{lhs.low * rhs};
    const auto high{__umulh(lhs.low, static_cast<std::uint64_t>(rhs)) + (lhs.high * rhs)};

    return {high, low};
}

#endif // MSVC implementations

template <typename UnsignedInteger>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr uint128_t default_mul(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    #if (defined(__aarch64__) || defined(__x86_64__) || defined(__PPC__) || defined(__powerpc__)) && defined(__GNUC__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)

    #  if !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION)

    if (!BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        detail::builtin_u128 new_lhs {};
        detail::builtin_u128 new_rhs {};

        std::memcpy(&new_lhs, &lhs, sizeof(uint128_t));
        std::memcpy(&new_rhs, &rhs, sizeof(UnsignedInteger));

        const auto res {new_lhs * new_rhs};

        uint128_t library_res {};

        std::memcpy(&library_res, &res, sizeof(uint128_t));

        return library_res;
    }

    #  elif defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)
    #    define BOOST_SAFE_NUMBERS_DETAIL_INT128_HIDE_MUL

        return static_cast<uint128_t>(static_cast<detail::builtin_u128>(lhs) * static_cast<detail::builtin_u128>(rhs));

    #  endif

    // s390x intentionally falls through to the synthetic low_word_mul below. Casting to builtin_u128
    // makes GCC reconstruct the value through a vector-unit stack round-trip that is several times
    // slower, and the memcpy path is unsafe for the narrow (scalar rhs) overloads on big-endian.
    #elif ((defined(_M_AMD64) && !defined(__GNUC__)) || defined(_M_ARM64)) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION)

    if (!BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return msvc_mul(lhs, rhs);
    }

    #endif

    // We need to hide this if we use a non-const eval method above to avoid a litany of cross-platform warnings
    #ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_HIDE_MUL

    return low_word_mul<uint128_t>(lhs, rhs);

    #else
    #undef BOOST_SAFE_NUMBERS_DETAIL_INT128_HIDE_MUL
    #endif //BOOST_SAFE_NUMBERS_DETAIL_INT128_HIDE_MUL
}

} // namespace detail

#if defined(__GNUC__) && __GNUC__ >= 8
#  pragma GCC diagnostic pop
#endif

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4146)
#endif

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator*(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<SignedInteger>;

    const auto abs_rhs {rhs < 0 ? -static_cast<eval_type>(rhs) : static_cast<eval_type>(rhs)};
    const auto res {detail::default_mul(lhs, abs_rhs)};

    return rhs < 0 ? -res : res;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator*(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<SignedInteger>;

    const auto abs_lhs {lhs < 0 ? -static_cast<eval_type>(lhs) : static_cast<eval_type>(lhs)};
    const auto res {detail::default_mul(rhs, abs_lhs)};

    return lhs < 0 ? -res : res;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator*(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return detail::default_mul(lhs, static_cast<std::uint64_t>(rhs));
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator*(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return detail::default_mul(rhs, static_cast<std::uint64_t>(lhs));
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator*(const uint128_t lhs, const uint128_t rhs) noexcept
{
    return detail::default_mul(lhs, rhs);
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)


BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator*(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    const detail::builtin_u128 rhs_bits {static_cast<detail::builtin_u128>(rhs)};
    const bool rhs_negative {static_cast<std::int64_t>(static_cast<std::uint64_t>(rhs_bits >> static_cast<detail::builtin_u128>(64U))) < 0};
    const uint128_t rhs_u {rhs_bits};
    const uint128_t abs_rhs {rhs_negative ? -rhs_u : rhs_u};
    const uint128_t res {lhs * abs_rhs};

    return rhs_negative ? -res : res;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator*(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    const detail::builtin_u128 lhs_bits {static_cast<detail::builtin_u128>(lhs)};
    const bool lhs_negative {static_cast<std::int64_t>(static_cast<std::uint64_t>(lhs_bits >> static_cast<detail::builtin_u128>(64U))) < 0};
    const uint128_t lhs_u {lhs_bits};
    const uint128_t abs_lhs {lhs_negative ? -lhs_u : lhs_u};
    const uint128_t res {abs_lhs * rhs};

    return lhs_negative ? -res : res;
}


BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator*(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs * static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator*(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) * rhs;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator*=(const Integer rhs) noexcept
{
    *this = *this * rhs;
    return *this;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator*=(const uint128_t rhs) noexcept
{
    *this = *this * rhs;
    return *this;
}

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_128BIT_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline uint128_t& uint128_t::operator*=(const Integer rhs) noexcept
{
    *this = *this * rhs;
    return *this;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

//=====================================
// Division Operator
//=====================================

// For div we need forward declarations since we mix and match the arguments
BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator/(uint128_t lhs, SignedInteger rhs) noexcept;

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator/(SignedInteger lhs, uint128_t rhs) noexcept;

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator/(uint128_t lhs, UnsignedInteger rhs) noexcept;

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator/(UnsignedInteger lhs, uint128_t rhs) noexcept;

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator/(uint128_t lhs, uint128_t rhs) noexcept;

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator/(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<SignedInteger>;
    return rhs < 0 ? lhs / static_cast<uint128_t>(rhs) : lhs / static_cast<eval_type>(rhs);
}

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator/(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<SignedInteger>;
    return lhs < 0 ? static_cast<uint128_t>(lhs) / rhs : static_cast<eval_type>(lhs) / rhs;
}

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator/(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<UnsignedInteger>;

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY(rhs == 0U))
    {
        return {0, 0};
    }

    if (lhs < rhs)
    {
        return {0, 0};
    }

    uint128_t quotient {};

    detail::one_word_div(lhs, static_cast<eval_type>(rhs), quotient);

    return quotient;
}

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator/(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<UnsignedInteger>;

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY(rhs == 0U))
    {
        return {0, 0};
    }

    if (lhs < rhs)
    {
        return {0, 0};
    }

    return {0, static_cast<eval_type>(lhs) / rhs.low};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator/(const uint128_t lhs, const uint128_t rhs) noexcept
{
    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY(rhs == 0U))
    {
        return {0, 0};
    }

    if (lhs < rhs)
    {
        return {0, 0};
    }

    // A divisor that fits in 64 bits is handled by the hardware-accelerated narrow path. This
    // beats the native 128/128 divide for this common case on every platform (it avoids the
    // out-of-line __udivti3 call on GCC/Clang and uses divq / _udiv128 directly where present).
    if (rhs.high == 0U)
    {
        if (lhs.high == 0U)
        {
            return {0, lhs.low / rhs.low};
        }

        uint128_t quotient {};
        detail::one_word_div(lhs, rhs.low, quotient);
        return quotient;
    }

    #if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) && !defined(__s390__) && !defined(__s390x__)

    return static_cast<uint128_t>(static_cast<detail::builtin_u128>(lhs) / static_cast<detail::builtin_u128>(rhs));

    #else

    return detail::knuth_div(lhs, rhs);

    #endif
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator/(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs / static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator/(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) / rhs;
}


BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator/(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs / static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator/(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) / rhs;
}


#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator/=(const Integer rhs) noexcept
{
    *this = *this / rhs;
    return *this;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator/=(const uint128_t rhs) noexcept
{
    *this = *this / rhs;
    return *this;
}

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_128BIT_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline uint128_t& uint128_t::operator/=(const Integer rhs) noexcept
{
    *this = *this / rhs;
    return *this;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

//=====================================
// Modulo Operator
//=====================================

// For div we need forward declarations since we mix and match the arguments
BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator%(uint128_t lhs, SignedInteger rhs) noexcept;

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator%(SignedInteger lhs, uint128_t rhs) noexcept;

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator%(uint128_t lhs, UnsignedInteger rhs) noexcept;

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <BOOST_SAFE_NUMBERS_DETAIL_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator%(UnsignedInteger lhs, uint128_t rhs) noexcept;

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator%(uint128_t lhs, uint128_t rhs) noexcept;

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator%(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<SignedInteger>;
    return rhs < 0 ? lhs % static_cast<uint128_t>(rhs) : lhs % static_cast<eval_type>(rhs);
}

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_SIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator%(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<SignedInteger>;
    return lhs < 0 ? static_cast<uint128_t>(lhs) % rhs : static_cast<eval_type>(lhs) % rhs;
}

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator%(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<UnsignedInteger>;

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY(rhs == 0U))
    {
        return {0, 0};
    }

    if (lhs.high != 0)
    {
        uint128_t quotient {};
        uint128_t remainder {};

        detail::one_word_div(lhs, static_cast<eval_type>(rhs), quotient, remainder);

        return remainder;
    }
    else
    {
        return {0, lhs.low % rhs};
    }
}

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_UNSIGNED_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator%(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<UnsignedInteger>;

    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY(rhs == 0U))
    {
        return {0, 0};
    }
    else if (rhs > lhs)
    {
        return lhs;
    }

    return {0, static_cast<eval_type>(lhs) % rhs.low};
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator%(const uint128_t lhs, const uint128_t rhs) noexcept
{
    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY(rhs == 0U))
    {
        return {0, 0};
    }
    if (rhs > lhs)
    {
        return lhs;
    }

    // A divisor that fits in 64 bits is handled by the hardware-accelerated narrow path, which
    // beats the native 128/128 divide for this common case on every platform.
    if (rhs.high == 0U)
    {
        if (lhs.high == 0U)
        {
            return {0, lhs.low % rhs.low};
        }

        uint128_t quotient {};
        uint128_t remainder {};
        detail::one_word_div(lhs, rhs.low, quotient, remainder);
        return remainder;
    }

    #if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) && !defined(__s390__) && !defined(__s390x__)

    return static_cast<uint128_t>(static_cast<detail::builtin_u128>(lhs) % static_cast<detail::builtin_u128>(rhs));

    #else

    uint128_t remainder {};
    detail::knuth_div(lhs, rhs, remainder);
    return remainder;

    #endif
}

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator%(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs % static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator%(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) % rhs;
}


BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator%(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs % static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator%(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) % rhs;
}


#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator%=(const Integer rhs) noexcept
{
    *this = *this % rhs;
    return *this;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator%=(const uint128_t rhs) noexcept
{
    *this = *this % rhs;
    return *this;
}

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

template <BOOST_SAFE_NUMBERS_DETAIL_INT128_128BIT_INTEGER_CONCEPT>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE inline uint128_t& uint128_t::operator%=(const Integer rhs) noexcept
{
    * this = *this % rhs;
    return *this;
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128

namespace detail {

template <bool>
class numeric_limits_impl_u128
{
public:

        // Member constants
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = false;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;

    // C++23 deprecated the following two members
    #if defined(__GNUC__) && __cplusplus > 202002L
    #  pragma GCC diagnostic push
    #  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    #elif defined(_MSC_VER)
    #  pragma warning(push)
    #  pragma warning(disable:4996)
    #endif

    static constexpr std::float_denorm_style has_denorm = std::denorm_absent;
    static constexpr bool has_denorm_loss = false;

    #if defined(__GNUC__) && __cplusplus > 202002L
    #  pragma GCC diagnostic pop
    #elif defined(_MSC_VER)
    #  pragma warning(pop)
    #endif

    static constexpr std::float_round_style round_style = std::round_toward_zero;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = true;
    static constexpr int digits = 128;
    static constexpr int digits10 = 38;
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 2;
    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;
    static constexpr bool traps = std::numeric_limits<std::uint64_t>::traps;
    static constexpr bool tinyness_before = false;

    // Member functions
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE static constexpr auto (min)        () -> boost::int128::uint128_t { return {0, 0}; }
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE static constexpr auto lowest       () -> boost::int128::uint128_t { return {0, 0}; }
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE static constexpr auto (max)        () -> boost::int128::uint128_t { return {UINT64_MAX, UINT64_MAX}; }
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE static constexpr auto epsilon      () -> boost::int128::uint128_t { return {0, 0}; }
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE static constexpr auto round_error  () -> boost::int128::uint128_t { return {0, 0}; }
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE static constexpr auto infinity     () -> boost::int128::uint128_t { return {0, 0}; }
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE static constexpr auto quiet_NaN    () -> boost::int128::uint128_t { return {0, 0}; }
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE static constexpr auto signaling_NaN() -> boost::int128::uint128_t { return {0, 0}; }
    BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE static constexpr auto denorm_min   () -> boost::int128::uint128_t { return {0, 0}; }
};

#if !defined(__cpp_inline_variables) || __cpp_inline_variables < 201606L

template <bool b> constexpr bool numeric_limits_impl_u128<b>::is_specialized;
template <bool b> constexpr bool numeric_limits_impl_u128<b>::is_signed;
template <bool b> constexpr bool numeric_limits_impl_u128<b>::is_integer;
template <bool b> constexpr bool numeric_limits_impl_u128<b>::is_exact;
template <bool b> constexpr bool numeric_limits_impl_u128<b>::has_infinity;
template <bool b> constexpr bool numeric_limits_impl_u128<b>::has_quiet_NaN;
template <bool b> constexpr bool numeric_limits_impl_u128<b>::has_signaling_NaN;

// These members were deprecated in C++23
#if ((!defined(_MSC_VER) && (__cplusplus <= 202002L)) || (defined(_MSC_VER) && (_MSVC_LANG <= 202002L)))
template <bool b> constexpr std::float_denorm_style numeric_limits_impl_u128<b>::has_denorm;
template <bool b> constexpr bool numeric_limits_impl_u128<b>::has_denorm_loss;
#endif

template <bool b> constexpr std::float_round_style numeric_limits_impl_u128<b>::round_style;
template <bool b> constexpr bool numeric_limits_impl_u128<b>::is_iec559;
template <bool b> constexpr bool numeric_limits_impl_u128<b>::is_bounded;
template <bool b> constexpr bool numeric_limits_impl_u128<b>::is_modulo;
template <bool b> constexpr int numeric_limits_impl_u128<b>::digits;
template <bool b> constexpr int numeric_limits_impl_u128<b>::digits10;
template <bool b> constexpr int numeric_limits_impl_u128<b>::max_digits10;
template <bool b> constexpr int numeric_limits_impl_u128<b>::radix;
template <bool b> constexpr int numeric_limits_impl_u128<b>::min_exponent;
template <bool b> constexpr int numeric_limits_impl_u128<b>::min_exponent10;
template <bool b> constexpr int numeric_limits_impl_u128<b>::max_exponent;
template <bool b> constexpr int numeric_limits_impl_u128<b>::max_exponent10;
template <bool b> constexpr bool numeric_limits_impl_u128<b>::traps;
template <bool b> constexpr bool numeric_limits_impl_u128<b>::tinyness_before;

#endif // !defined(__cpp_inline_variables) || __cpp_inline_variables < 201606L


} // namespace detail

} // namespace int128
} // namespace boost

namespace std {

#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wmismatched-tags"
#endif

template <>
class numeric_limits<boost::int128::uint128_t> :
    public boost::int128::detail::numeric_limits_impl_u128<true> {};

#ifdef __clang__
#  pragma clang diagnostic pop
#endif

} // namespace std

#endif //BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_UINT128_IMP_HPP
// ===== END boost/safe_numbers/detail/int128/detail/uint128_imp.hpp =====
// ===== BEGIN boost/safe_numbers/detail/int128/detail/conversions.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_CONVERSIONS_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_CONVERSIONS_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/int128_imp.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/uint128_imp.hpp

namespace boost {
namespace int128 {

namespace detail {

template <typename T>
struct valid_overload
{
    static constexpr bool value = std::is_same<T, uint128_t>::value || std::is_same<T, int128_t>::value;
};

template <typename T>
BOOST_SAFE_NUMBERS_DETAIL_INT128_INLINE_CONSTEXPR bool is_valid_overload_v = valid_overload<T>::value;

} // namespace detail

#if BOOST_SAFE_NUMBERS_DETAIL_INT128_ENDIAN_LITTLE_BYTE

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t::int128_t(const uint128_t& v) noexcept : low {v.low}, high {static_cast<std::int64_t>(v.high)} {}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t::uint128_t(const int128_t& v) noexcept : low {v.low}, high {static_cast<std::uint64_t>(v.high)} {}

#else

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t::int128_t(const uint128_t& v) noexcept : high {static_cast<std::int64_t>(v.high)}, low {v.low} {}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t::uint128_t(const int128_t& v) noexcept : high {static_cast<std::uint64_t>(v.high)}, low {v.low} {}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_ENDIAN_LITTLE_BYTE

//=====================================
// Comparison Operators
//=====================================

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4127)
#endif

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator==(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) == static_cast<uint128_t>(rhs);
}

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator!=(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) != static_cast<uint128_t>(rhs);
}

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator<(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) < static_cast<uint128_t>(rhs);
}

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator<=(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) <= static_cast<uint128_t>(rhs);
}

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator>(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) > static_cast<uint128_t>(rhs);
}

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool operator>=(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) >= static_cast<uint128_t>(rhs);
}

//=====================================
// Arithmetic Operators
//=====================================

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator+(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) + static_cast<uint128_t>(rhs);
}

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator-(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) - static_cast<uint128_t>(rhs);
}

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator*(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) * static_cast<uint128_t>(rhs);
}

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator/(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) / static_cast<uint128_t>(rhs);
}

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator%(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) % static_cast<uint128_t>(rhs);
}

//=====================================
// Cross-type Bitwise Operators
//=====================================

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator|(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) | static_cast<uint128_t>(rhs);
}

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator&(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) & static_cast<uint128_t>(rhs);
}

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator^(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) ^ static_cast<uint128_t>(rhs);
}

//=====================================
// Cross-type Shift Operators
//=====================================

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator<<(const int128_t lhs, const uint128_t rhs) noexcept
{
    return lhs << static_cast<int128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator<<(const uint128_t lhs, const int128_t rhs) noexcept
{
    return lhs << static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator>>(const int128_t lhs, const uint128_t rhs) noexcept
{
    return lhs >> static_cast<int128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator>>(const uint128_t lhs, const int128_t rhs) noexcept
{
    return lhs >> static_cast<uint128_t>(rhs);
}

//=====================================
// int128_t with builtin unsigned __int128 comparison operators
//
// These live here (not in int128_imp.hpp) 
// to avoid C++20 rewritten-candidate ambiguity on MSVC
//=====================================

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator==(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) == rhs;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator==(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs == static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator!=(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) != rhs;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator!=(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs != static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator<(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) < rhs;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator<(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs < static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator<=(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) <= rhs;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator<=(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs <= static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator>(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) > rhs;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator>(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs > static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator>=(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) >= rhs;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR bool operator>=(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs >= static_cast<uint128_t>(rhs);
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

//=====================================
// int128_t with builtin unsigned __int128 binary operators
//=====================================

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator|(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) | rhs;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator|(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs | static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator&(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) & rhs;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator&(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs & static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator^(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) ^ rhs;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator^(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs ^ static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator+(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) + rhs;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator+(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs + static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator-(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) - rhs;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator-(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs - static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator*(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) * rhs;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator*(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs * static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator/(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) / rhs;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator/(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs / static_cast<uint128_t>(rhs);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator%(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) % rhs;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILTIN_CONSTEXPR uint128_t operator%(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs % static_cast<uint128_t>(rhs);
}

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

#ifdef _MSC_VER
#pragma warning(pop)
#endif

} // namespace int128
} // namespace boost

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_CONVERSIONS_HPP
// ===== END boost/safe_numbers/detail/int128/detail/conversions.hpp =====

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_INT128_HPP
// ===== END boost/safe_numbers/detail/int128/int128.hpp =====
// [amalgamate] skipped duplicate include of boost/safe_numbers/overflow_policy.hpp

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <concepts>
#include <type_traits>

#endif // ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers::detail {

// is_fundamental_unsigned_integral

namespace impl {

template <typename T>
struct is_fundamental_unsigned_integral : std::bool_constant<std::is_unsigned_v<T> || std::is_same_v<std::remove_cv_t<T>, int128::uint128_t>> {};

template <typename T>
struct is_fundamental_signed_integral : std::bool_constant<std::is_signed_v<T> || std::is_same_v<std::remove_cv_t<T>, int128::int128_t>> {};

template <typename T>
struct is_compatible_float_type : std::bool_constant<std::is_same_v<float, std::remove_cv_t<T>> || std::is_same_v<double, std::remove_cv_t<T>>> {};

} // namespace impl

template <typename T>
inline constexpr bool is_fundamental_unsigned_integral_v = impl::is_fundamental_unsigned_integral<T>::value;

template <typename T>
concept fundamental_unsigned_integral = is_fundamental_unsigned_integral_v<T>;

template <typename T>
inline constexpr bool is_fundamental_signed_integral_v = impl::is_fundamental_signed_integral<T>::value;

template <typename T>
concept fundamental_signed_integral = is_fundamental_signed_integral_v<T>;

template <typename T>
inline constexpr bool is_compatible_float_type = impl::is_compatible_float_type<T>::value;

template <typename T>
concept compatible_float_type = is_compatible_float_type<T>;

// Which family of basis template a type-level error policy is validated against
enum class basis_kind
{
    unsigned_integer,
    signed_integer,
    floating_point
};

// True when the NTTP is an overflow_policy enumerator. Every comparison against
// an enumerator must sit behind this check because comparing a non-enum NTTP
// against overflow_policy would be ill-formed rather than false.
template <auto Policy>
inline constexpr bool is_overflow_policy_v = std::is_same_v<std::remove_cv_t<decltype(Policy)>, overflow_policy>;

// True when the NTTP is a user defined handler object rather than an
// overflow_policy enumerator. The handler interface is validated separately
// by the error_handler_for concept.
template <auto Policy>
inline constexpr bool is_user_handler_v = std::is_class_v<std::remove_cv_t<decltype(Policy)>>;

// A user defined handler is a stateless class whose on_error is callable on a
// const object with the error kind, a defined fallback value (the wrapped
// integer result, the dividend, or the raw IEEE 754 result), and the
// diagnostic message. Whatever it returns becomes the operation's result.
template <typename Handler, typename T>
concept error_handler_for = std::is_empty_v<Handler> &&
    requires(const Handler handler, const T value, const char* msg)
    {
        { handler.on_error(error_kind::overflow, value, msg) } -> std::same_as<T>;
    };

// Compares a policy NTTP against an enumerator, funneled so that user handler
// objects compare unequal instead of making the comparison ill-formed.
// consteval, so uses in runtime conditions fold to a constant.
template <auto Policy>
consteval auto policy_equals(const overflow_policy policy) noexcept -> bool
{
    if constexpr (is_overflow_policy_v<Policy>)
    {
        return Policy == policy;
    }
    else
    {
        return false;
    }
}

// noexcept specification for operations whose only error is overflow or
// underflow (add, sub, mul, shifts, increment, decrement, unary minus, and
// every float operator): throwing throws, saturate and strict do not, and a
// user handler propagates the noexcept of its on_error.
template <auto Policy, typename T>
consteval auto policy_is_nothrow_arith() noexcept -> bool
{
    if constexpr (is_overflow_policy_v<Policy>)
    {
        return Policy != overflow_policy::throw_exception;
    }
    else
    {
        return noexcept(Policy.on_error(error_kind::overflow, T{}, static_cast<const char*>(nullptr)));
    }
}

// noexcept specification for integer division and modulo, where division by
// zero throws under both throw_exception and saturate.
template <auto Policy, typename T>
consteval auto policy_is_nothrow_div() noexcept -> bool
{
    if constexpr (is_overflow_policy_v<Policy>)
    {
        return Policy == overflow_policy::strict;
    }
    else
    {
        return noexcept(Policy.on_error(error_kind::divide_by_zero, T{}, static_cast<const char*>(nullptr)));
    }
}

// True when two policy NTTPs differ, without requiring comparability between
// unrelated handler types. Handlers are stateless, so same type means equal.
template <auto LHSPolicy, auto RHSPolicy>
consteval auto policies_differ() noexcept -> bool
{
    if constexpr (!std::is_same_v<decltype(LHSPolicy), decltype(RHSPolicy)>)
    {
        return true;
    }
    else if constexpr (is_overflow_policy_v<LHSPolicy>)
    {
        return LHSPolicy != RHSPolicy;
    }
    else
    {
        return false;
    }
}

// Policies whose result is not the operand type (pair, optional, or a wider type)
// can never live in the type itself; they remain free functions.
template <auto Policy>
consteval auto is_value_returning_policy() noexcept -> bool
{
    if constexpr (is_overflow_policy_v<Policy>)
    {
        return Policy == overflow_policy::overflow_tuple ||
               Policy == overflow_policy::checked ||
               Policy == overflow_policy::widen;
    }
    else
    {
        return false;
    }
}

// The subset of overflow_policy values allowed as a type-level policy for the
// given basis kind: throw_exception and saturate everywhere, strict only for integers.
template <auto Policy>
consteval auto is_valid_type_policy(const basis_kind kind) noexcept -> bool
{
    if constexpr (is_overflow_policy_v<Policy>)
    {
        // One expression rather than an if on a constant condition, which MSVC rejects
        // under /W4 /WX (C4127)
        return Policy == overflow_policy::throw_exception ||
               Policy == overflow_policy::saturate ||
               (Policy == overflow_policy::strict && kind != basis_kind::floating_point);
    }
    else
    {
        return false;
    }
}

template <fundamental_unsigned_integral BasisType, auto ErrorPolicy = overflow_policy::throw_exception>
class unsigned_integer_basis;

template <fundamental_signed_integral BasisType, auto ErrorPolicy = overflow_policy::throw_exception>
class signed_integer_basis;

template <compatible_float_type BasisType, auto ErrorPolicy = overflow_policy::throw_exception>
class float_basis;

// Maps the type argument of the basic_* alias templates onto the basis NTTP:
// the tag types select the built-in enum policies and any other type is a user
// defined handler passed by value.
template <typename ErrorHandler>
inline constexpr auto type_policy_v = ErrorHandler{};

template <>
inline constexpr auto type_policy_v<throwing> = overflow_policy::throw_exception;

template <>
inline constexpr auto type_policy_v<saturating> = overflow_policy::saturate;

template <>
inline constexpr auto type_policy_v<strict> = overflow_policy::strict;

// is_unsigned_library_type (base + unsigned_integer_basis specialization)

namespace impl {

template <typename>
struct is_unsigned_library_type : std::false_type {};

template <typename>
struct is_signed_library_type : std::false_type {};

template <typename>
struct is_float_library_type : std::false_type {};

template <typename T, auto ErrorPolicy>
struct is_unsigned_library_type<unsigned_integer_basis<T, ErrorPolicy>> : std::true_type {};

template <typename T, auto ErrorPolicy>
struct is_signed_library_type<signed_integer_basis<T, ErrorPolicy>> : std::true_type {};

template <typename T, auto ErrorPolicy>
struct is_float_library_type<float_basis<T, ErrorPolicy>> : std::true_type {};

} // namespace impl

template <typename T>
inline constexpr bool is_unsigned_library_type_v = impl::is_unsigned_library_type<T>::value;

template <typename T>
inline constexpr bool is_signed_library_type_v = impl::is_signed_library_type<T>::value;

template <typename T>
inline constexpr bool is_float_library_type_v = impl::is_float_library_type<T>::value;

// underlying type trait (base + unsigned_integer_basis specialization)

namespace impl {

template <typename T>
struct underlying
{
    using type = std::remove_cv_t<std::remove_reference_t<T>>;
};

template <typename T, auto ErrorPolicy>
struct underlying<unsigned_integer_basis<T, ErrorPolicy>>
{
    using type = T;
};

template <typename T, auto ErrorPolicy>
struct underlying<signed_integer_basis<T, ErrorPolicy>>
{
    using type = T;
};

template <typename T, auto ErrorPolicy>
struct underlying<float_basis<T, ErrorPolicy>>
{
    using type = T;
};

} // namespace impl

template <typename T>
using underlying_type_t = typename impl::underlying<T>::type;

// valid_bound concept

template <typename T>
concept valid_bound = !std::is_same_v<T, bool> && (is_unsigned_library_type_v<T> || is_fundamental_unsigned_integral_v<T>);

// raw_value function

template <typename T>
    requires valid_bound<T>
constexpr auto raw_value(T val) noexcept
{
    if constexpr (is_unsigned_library_type_v<T>)
    {
        return static_cast<underlying_type_t<T>>(val);
    }
    else
    {
        return val;
    }
}

// valid_signed_bound concept

template <typename T>
concept valid_signed_bound = !std::is_same_v<T, bool> && (is_signed_library_type_v<T> || is_fundamental_signed_integral_v<T>);

// signed_raw_value function

template <typename T>
    requires valid_signed_bound<T>
constexpr auto signed_raw_value(T val) noexcept
{
    if constexpr (is_signed_library_type_v<T>)
    {
        return static_cast<underlying_type_t<T>>(val);
    }
    else
    {
        return val;
    }
}

// valid_float_bound concept

template <typename T>
concept valid_float_bound = is_compatible_float_type<T> || is_float_library_type_v<T>;

// float_raw_value function

template <typename T>
    requires valid_float_bound<T>
constexpr auto float_raw_value(T val) noexcept
{
    if constexpr (is_float_library_type_v<T>)
    {
        return static_cast<underlying_type_t<T>>(val);
    }
    else
    {
        return val;
    }
}

} // namespace boost::safe_numbers::detail

// Constrained forward declaration of bounded_uint
namespace boost::safe_numbers {

template <auto Min, auto Max>
    requires (detail::valid_bound<decltype(Min)> &&
              detail::valid_bound<decltype(Max)> &&
              detail::raw_value(Max) > detail::raw_value(Min))
class bounded_uint;

} // namespace boost::safe_numbers

// Constrained forward declaration of bounded_int
namespace boost::safe_numbers {

template <auto Min, auto Max>
    requires (detail::valid_signed_bound<decltype(Min)> &&
              detail::valid_signed_bound<decltype(Max)> &&
              detail::signed_raw_value(Max) > detail::signed_raw_value(Min))
class bounded_int;

} // namespace boost::safe_numbers

// Constrained forward declaration of bounded_float (requires float NTTP support)
#if BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT

namespace boost::safe_numbers {

template <auto Min, auto Max>
    requires (detail::valid_float_bound<decltype(Min)> &&
              detail::valid_float_bound<decltype(Max)> &&
              std::is_same_v<decltype(Min), decltype(Max)> &&
              detail::float_raw_value(Min) == detail::float_raw_value(Min) &&
              detail::float_raw_value(Max) == detail::float_raw_value(Max) &&
              detail::float_raw_value(Max) > detail::float_raw_value(Min))
class bounded_float;

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT

// bounded_uint specialization of is_unsigned_library_type
namespace boost::safe_numbers::detail::impl {

template <auto Min, auto Max>
struct is_unsigned_library_type<bounded_uint<Min, Max>> : std::true_type {};

} // namespace boost::safe_numbers::detail::impl

// bounded_int specialization of is_signed_library_type
namespace boost::safe_numbers::detail::impl {

template <auto Min, auto Max>
struct is_signed_library_type<bounded_int<Min, Max>> : std::true_type {};

} // namespace boost::safe_numbers::detail::impl

// bounded_float specialization of is_float_library_type
#if BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT

namespace boost::safe_numbers::detail::impl {

template <auto Min, auto Max>
struct is_float_library_type<bounded_float<Min, Max>> : std::true_type {};

} // namespace boost::safe_numbers::detail::impl

#endif // BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT

// is_bounded_type trait
namespace boost::safe_numbers::detail {

namespace impl {

template <typename>
struct is_bounded_type : std::false_type {};

template <auto Min, auto Max>
struct is_bounded_type<bounded_uint<Min, Max>> : std::true_type {};

template <auto Min, auto Max>
struct is_bounded_type<bounded_int<Min, Max>> : std::true_type {};

#if BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT
template <auto Min, auto Max>
struct is_bounded_type<bounded_float<Min, Max>> : std::true_type {};
#endif

} // namespace impl

template <typename T>
inline constexpr bool is_bounded_type_v = impl::is_bounded_type<T>::value;

// is_library_type

namespace impl {

template <typename>
struct is_library_type : std::false_type {};

template <typename T, auto ErrorPolicy>
struct is_library_type<unsigned_integer_basis<T, ErrorPolicy>> : std::true_type {};

template <typename T, auto ErrorPolicy>
struct is_library_type<signed_integer_basis<T, ErrorPolicy>> : std::true_type {};

template <typename T, auto ErrorPolicy>
struct is_library_type<float_basis<T, ErrorPolicy>> : std::true_type {};

template <auto Min, auto Max>
struct is_library_type<bounded_uint<Min, Max>> : std::true_type {};

template <auto Min, auto Max>
struct is_library_type<bounded_int<Min, Max>> : std::true_type {};

#if BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT
template <auto Min, auto Max>
struct is_library_type<bounded_float<Min, Max>> : std::true_type {};
#endif

template <typename>
struct is_integral_library_type : std::false_type {};

template <typename T, auto ErrorPolicy>
struct is_integral_library_type<unsigned_integer_basis<T, ErrorPolicy>> : std::true_type {};

template <typename T, auto ErrorPolicy>
struct is_integral_library_type<signed_integer_basis<T, ErrorPolicy>> : std::true_type {};

template <auto Min, auto Max>
struct is_integral_library_type<bounded_uint<Min, Max>> : std::true_type {};

template <auto Min, auto Max>
struct is_integral_library_type<bounded_int<Min, Max>> : std::true_type {};

} // namespace impl

template <typename T>
inline constexpr bool is_library_type_v = impl::is_library_type<T>::value;

template <typename T>
inline constexpr bool is_integral_library_type_v = impl::is_integral_library_type<T>::value;

template <typename T>
concept library_type = is_library_type_v<T>;

template <typename T>
concept integral_library_type = is_integral_library_type_v<T>;

template <typename T>
concept unsigned_library_type = is_unsigned_library_type_v<T>;

template <typename T>
concept non_bounded_unsigned_library_type = is_unsigned_library_type_v<T> && !is_bounded_type_v<T>;

template <typename T>
concept non_bounded_integral_library_type = is_integral_library_type_v<T> && !is_bounded_type_v<T>;

template <typename T>
concept float_library_type = is_float_library_type_v<T>;

template <typename T>
concept non_bounded_float_library_type = is_float_library_type_v<T> && !is_bounded_type_v<T>;

// underlying specialization for bounded_uint

namespace impl {

template <auto Min, auto Max>
struct underlying<bounded_uint<Min, Max>>
{
    using type = typename underlying<typename bounded_uint<Min, Max>::basis_type>::type;
};

template <auto Min, auto Max>
struct underlying<bounded_int<Min, Max>>
{
    using type = typename underlying<typename bounded_int<Min, Max>::basis_type>::type;
};

#if BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT
template <auto Min, auto Max>
struct underlying<bounded_float<Min, Max>>
{
    using type = typename underlying<typename bounded_float<Min, Max>::basis_type>::type;
};
#endif

} // namespace impl

// Promotes an unsigned integer to the next higher type
// uint128_t becomes bool so that we can static_assert on bool check that we can't widen uint128_t
template <fundamental_unsigned_integral T>
using promoted_type = std::conditional_t<std::is_same_v<T, std::uint8_t>, std::uint16_t,
                          std::conditional_t<std::is_same_v<T, std::uint16_t>, std::uint32_t,
                              std::conditional_t<std::is_same_v<T, std::uint32_t>, std::uint64_t,
                                  std::conditional_t<std::is_same_v<T, std::uint64_t>, int128::uint128_t, bool>>>>;

// Promotes a signed integer to the next higher type
// int128_t becomes bool so that we can static_assert on bool check that we can't widen int128_t
template <fundamental_signed_integral T>
using signed_promoted_type = std::conditional_t<std::is_same_v<T, std::int8_t>, std::int16_t,
                                 std::conditional_t<std::is_same_v<T, std::int16_t>, std::int32_t,
                                     std::conditional_t<std::is_same_v<T, std::int32_t>, std::int64_t,
                                         std::conditional_t<std::is_same_v<T, std::int64_t>, int128::int128_t, bool>>>>;

} // namespace boost::safe_numbers::detail

#endif // BOOST_SAFE_NUMBERS_DETAIL_TYPE_TRAITS_HPP
// ===== END boost/safe_numbers/detail/type_traits.hpp =====
// ===== BEGIN boost/safe_numbers/detail/throw_exception.hpp =====
// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// BOOST_THROW_EXCEPTION fits our needs for CPU tasks
// We need to define our own version of thrown exception so that we can also handle the device environment
// This is the only file in the lib that should attempt inclusion of boost/throw_exception.hpp

#ifndef BOOST_SAFE_NUMBERS_THROW_EXCEPTION_HPP
#define BOOST_SAFE_NUMBERS_THROW_EXCEPTION_HPP

// ===== BEGIN boost/safe_numbers/device_error_reporting.hpp =====
// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// Public entry point for device error reporting. Include this to obtain
// device_error_context. It pulls in the backend-neutral pieces (so
// device_exception_mode is always available) and dispatches to the CUDA or SYCL
// backend based on compiler detection and the enable macro.
//
//   * CUDA: nvcc (__CUDACC__) + BOOST_SAFE_NUMBERS_ENABLE_CUDA
//   * SYCL: BOOST_SAFE_NUMBERS_ENABLE_SYCL (with <sycl/sycl.hpp> included first)
//
// On a pure host build neither backend is included and only device_exception_mode
// / the detail enums are defined.

#ifndef BOOST_SAFE_NUMBERS_DEVICE_ERROR_REPORTING_HPP
#define BOOST_SAFE_NUMBERS_DEVICE_ERROR_REPORTING_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/config.hpp
// ===== BEGIN boost/safe_numbers/detail/device_error_common.hpp =====
// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// Backend-neutral pieces shared by the CUDA and SYCL device error reporters.
// Included by detail/cuda_error_reporting.hpp, detail/sycl_error_reporting.hpp,
// and the public device_error_reporting.hpp dispatcher.

#ifndef BOOST_SAFE_NUMBERS_DETAIL_DEVICE_ERROR_COMMON_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_DEVICE_ERROR_COMMON_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/config.hpp

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/throw_exception.hpp>
#include <string>
#include <stdexcept>
#include <type_traits>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

// Using a macro instead of a global constant because inline constexpr is not available on device
#ifdef PATH_MAX
#  define BOOST_SAFE_NUMBERS_DEVICE_ERROR_BUFFER_SIZE PATH_MAX
#else
#  define BOOST_SAFE_NUMBERS_DEVICE_ERROR_BUFFER_SIZE 512
#endif

namespace boost::safe_numbers {

// Selects how the device reacts to a captured error. On CUDA, trapped hard-fails
// the kernel via __trap() while untrapped defers to the host. SYCL has no
// recoverable device trap, so under SYCL it is always deferred (trapped degrades
// to untrapped); the enum is retained for source-level symmetry with CUDA.
enum class device_exception_mode : unsigned
{
    trapped,
    untrapped,
};

inline constexpr auto trapped = device_exception_mode::trapped;
inline constexpr auto untrapped = device_exception_mode::untrapped;

namespace detail {

enum class exception_type : unsigned
{
    domain_error,
    overflow,
    underflow,
    invalid_argument,
    unknown,
};

// POD error record copied by value between device and host.
struct device_error_record
{
    int  flag;                                                      // 0 = no error, 1 = error captured
    int  line;                                                      // __LINE__
    int  thread_id;                                                 // thread / work-item id (-1 = unknown)
    exception_type exception;                                       // exception that would have been thrown on CPU
    char file[BOOST_SAFE_NUMBERS_DEVICE_ERROR_BUFFER_SIZE];         // __FILE__ copied by value
    char expression[BOOST_SAFE_NUMBERS_DEVICE_ERROR_BUFFER_SIZE];   // message copied by value
};

// Back-compat alias for the original CUDA-specific name.
using cuda_device_error = device_error_record;

// Compile-time map from a std exception type to our enum.
template <typename T>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto to_exception_enum() noexcept -> exception_type
{
    if constexpr (std::is_same_v<T, std::domain_error>)
    {
        return exception_type::domain_error;
    }
    else if constexpr (std::is_same_v<T, std::overflow_error>)
    {
        return exception_type::overflow;
    }
    else if constexpr (std::is_same_v<T, std::underflow_error>)
    {
        return exception_type::underflow;
    }
    else if constexpr (std::is_same_v<T, std::invalid_argument>)
    {
        return exception_type::invalid_argument;
    }
    else
    {
        return exception_type::unknown;
    }
}

// Device-safe bounded string copy (device has no strncpy).
BOOST_SAFE_NUMBERS_HOST_DEVICE inline void copy_to_buf(char* dst, const char* src, const int max_len)
{
    int i = 0;
    for (; i < max_len - 1 && src[i] != '\0'; ++i)
    {
        dst[i] = src[i];
    }
    dst[i] = '\0';
}

// Host-side translation from the enum to a real std exception. Used by both
// backends' host branch and by both device_error_context::synchronize() methods.
inline void throw_from_exception_type(const exception_type exc, const std::string& msg)
{
    switch (exc)
    {
        case exception_type::domain_error:
            BOOST_THROW_EXCEPTION(std::domain_error(msg));
            break;
        case exception_type::overflow:
            BOOST_THROW_EXCEPTION(std::overflow_error(msg));
            break;
        case exception_type::underflow:
            BOOST_THROW_EXCEPTION(std::underflow_error(msg));
            break;
        case exception_type::invalid_argument:
            BOOST_THROW_EXCEPTION(std::invalid_argument(msg));
            break;
        case exception_type::unknown:
            [[fallthrough]];
        default:
            BOOST_THROW_EXCEPTION(std::runtime_error(msg));
    }
}

// Tracks whether a device_error_context instance is alive. Only one may exist at
// a time to prevent races on the device error buffer.
inline bool g_device_error_context_active = false;

} // namespace detail
} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_DETAIL_DEVICE_ERROR_COMMON_HPP
// ===== END boost/safe_numbers/detail/device_error_common.hpp =====

#if defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA)
// ===== BEGIN boost/safe_numbers/detail/cuda_error_reporting.hpp =====
// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// CUDA device error-reporting backend. Selected by the public
// device_error_reporting.hpp dispatcher when compiling with nvcc and
// BOOST_SAFE_NUMBERS_ENABLE_CUDA. Backend-neutral pieces live in
// detail/device_error_common.hpp.

#ifndef BOOST_SAFE_NUMBERS_DETAIL_CUDA_ERROR_REPORTING_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_CUDA_ERROR_REPORTING_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/config.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/device_error_common.hpp

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/throw_exception.hpp>
#include <string>
#include <stdexcept>
#include <sstream>

#ifdef __CUDACC__
#include <cuda_runtime.h>
#endif

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers {

namespace detail {

#ifdef __CUDACC__

// Managed memory error record accessible from both host and device.
// Since we never destroy the CUDA context, __managed__ is safe to use.
__managed__ device_error_record g_device_error {};

// Managed enum controlling what report_device_error does on failure.
// Default to trapped as that's the best way to ensure hard failure on error.
__managed__ device_exception_mode g_device_fail_type {device_exception_mode::trapped};

__host__ __device__ inline void report_device_error(
    exception_type exc,
    const char* file,
    int line,
    const char* expression)
{
    #ifdef __CUDA_ARCH__

    if (atomicCAS(&g_device_error.flag, 0, 1) == 0)
    {
        g_device_error.line = line;
        g_device_error.thread_id = blockIdx.x * blockDim.x + threadIdx.x;
        g_device_error.exception = exc;

        copy_to_buf(g_device_error.file, file, BOOST_SAFE_NUMBERS_DEVICE_ERROR_BUFFER_SIZE);
        copy_to_buf(g_device_error.expression, expression, BOOST_SAFE_NUMBERS_DEVICE_ERROR_BUFFER_SIZE);
        __threadfence_system();

        if (g_device_fail_type == device_exception_mode::trapped)
        {
            __trap();
        }
    }

    switch (g_device_fail_type)
    {
        case device_exception_mode::trapped:
            // In the event that __trap() is called the error is non-recoverable.
            // Other threads: spin until the trap terminates the kernel.
            while (true)
            {
                __nanosleep(1000000);
            }
            break;

        case device_exception_mode::untrapped:
            // Return instead of trapping. The kernel completes; synchronize()
            // detects the error via the flag and throws on the host.
            return;
            break;
    }

    #else

    const auto msg = std::string(file) + ":" + std::to_string(line) + ": " + expression;
    throw_from_exception_type(exc, msg);

    #endif
}

#endif // __CUDACC__

} // namespace detail

#ifdef __CUDACC__

class device_error_context
{
public:

    // Clears the error state. Only one device_error_context may exist at a time.
    device_error_context()
    {
        if (detail::g_device_error_context_active)
        {
            BOOST_THROW_EXCEPTION(std::logic_error(
                "Only one device_error_context may exist at a time"));
        }
        detail::g_device_error_context_active = true;
        reset();
    }

    // Sets the failure mode on the managed global up front.
    device_error_context(const device_exception_mode e)
    {
        if (detail::g_device_error_context_active)
        {
            BOOST_THROW_EXCEPTION(std::logic_error(
                "Only one device_error_context may exist at a time"));
        }

        detail::g_device_fail_type = e;

        detail::g_device_error_context_active = true;
        reset();
    }

    ~device_error_context()
    {
        detail::g_device_error_context_active = false;
    }

    device_error_context(const device_error_context&) = delete;
    device_error_context& operator=(const device_error_context&) = delete;

    // Clears the error fields so the context can be reused across kernel launches.
    void reset()
    {
        detail::g_device_error.flag = 0;
        detail::g_device_error.line = 0;
        detail::g_device_error.thread_id = 0;
        detail::g_device_error.exception = detail::exception_type::unknown;
        detail::g_device_error.file[0] = '\0';
        detail::g_device_error.expression[0] = '\0';
    }

    // Post-construction way of setting the failure mode for the device.
    void set_device_exception_method(const device_exception_mode e)
    {
        detail::g_device_fail_type = e;
    }

    // Synchronizes the device and checks for errors captured by device code.
    // If an error was detected, the error state is cleared (so the context is
    // immediately reusable), and the appropriate std::exception is thrown.
    void synchronize()
    {
        const auto status = cudaDeviceSynchronize();

        const auto flag = detail::g_device_error.flag;

        if (flag != 0)
        {
            const auto thread_id = detail::g_device_error.thread_id;
            const auto line = detail::g_device_error.line;
            const auto exc = detail::g_device_error.exception;

            std::ostringstream oss;
            oss << "Device error on thread " << thread_id
                << " at " << detail::g_device_error.file
                << ":" << line
                << ": " << detail::g_device_error.expression;

            const auto msg = oss.str();

            // Clear the error state so the context can be reused
            // immediately after catching the exception.
            reset();

            detail::throw_from_exception_type(exc, msg);
        }
        else
        {
            reset();
        }

        if (status != cudaSuccess)
        {
            cudaGetLastError();
            BOOST_THROW_EXCEPTION(std::runtime_error(cudaGetErrorString(status)));
        }
    }
};

#endif // __CUDACC__

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_DETAIL_CUDA_ERROR_REPORTING_HPP
// ===== END boost/safe_numbers/detail/cuda_error_reporting.hpp =====
#elif defined(BOOST_SAFE_NUMBERS_ENABLE_SYCL)
// ===== BEGIN boost/safe_numbers/detail/sycl_error_reporting.hpp =====
// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// SYCL device error-reporting backend. Selected by the public
// device_error_reporting.hpp dispatcher when BOOST_SAFE_NUMBERS_ENABLE_SYCL is
// set. Requires <sycl/sycl.hpp> to have been included first. Backend-neutral
// pieces live in detail/device_error_common.hpp.

#ifndef BOOST_SAFE_NUMBERS_DETAIL_SYCL_ERROR_REPORTING_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_SYCL_ERROR_REPORTING_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/config.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/device_error_common.hpp

#if defined(BOOST_SAFE_NUMBERS_ENABLE_SYCL)

#ifndef SYCL_EXTERNAL
#  error "Include <sycl/sycl.hpp> before <boost/safe_numbers/...> when BOOST_SAFE_NUMBERS_ENABLE_SYCL is defined"
#endif

// The device error buffer (g_device_error below) is a device_global, which core
// SYCL 2020 does not provide - section 5.4 forbids mutable variables with static
// storage duration in device code. device_global is the sycl_ext_oneapi_device_global
// extension that lifts that restriction, and it is the only way a deeply-nested
// operation can reach the shared error record without threading a pointer through
// every call. Require the extension explicitly (its feature-test macro) rather than
// letting device_global fail to resolve with a cryptic diagnostic on implementations
// that do not support it.
#ifndef SYCL_EXT_ONEAPI_DEVICE_GLOBAL
#  error "Boost.safe_numbers SYCL device error reporting requires the device_global extension (feature-test macro SYCL_EXT_ONEAPI_DEVICE_GLOBAL), which this SYCL implementation does not advertise"
#endif

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/throw_exception.hpp>
#include <string>
#include <stdexcept>
#include <sstream>
#include <utility>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers {

namespace detail {

// SYCL analog of the CUDA __managed__ error buffer. A single inline instance
// (the SYCL 2020 device_global extension explicitly permits inline) that any
// device-side function can write by name, giving the same zero-pointer-plumbing
// property as __managed__. No device_image_scope property is set, so host
// queue.copy access is unrestricted (the variable is backed by a shared buffer).
inline sycl::ext::oneapi::experimental::device_global<device_error_record> g_device_error;

// Reports an error from device or host. Compiled for both passes of the
// single-source TU: the device pass records into the device_global (first writer
// wins, then returns, since SYCL has no recoverable device trap); the host pass
// throws the matching std exception. Mirrors how the CUDA reporter branches on
// __CUDA_ARCH__.
BOOST_SAFE_NUMBERS_HOST_DEVICE inline void report_device_error(
    exception_type exc,
    const char* file,
    int line,
    const char* expression)
{
    #ifdef __SYCL_DEVICE_ONLY__

    auto& rec = g_device_error.get();
    sycl::atomic_ref<int,
        sycl::memory_order::relaxed,
        sycl::memory_scope::device,
        sycl::access::address_space::global_space> flag_ref(rec.flag);

    // First failing work-item wins and records the details; losers never touch
    // the payload, so the host can never observe a torn record.
    int expected {0};
    if (flag_ref.compare_exchange_strong(expected, 1))
    {
        rec.line = line;
        rec.exception = exc;
        #if defined(SYCL_EXT_ONEAPI_FREE_FUNCTION_QUERIES) && !defined(BOOST_SAFE_NUMBERS_SYCL_DISABLE_WORKITEM_ID)
        // The failing work-item's dimension-0 global linear id, the analog of
        // the CUDA backend's blockIdx.x * blockDim.x + threadIdx.x. Core SYCL
        // 2020 only passes the id into the kernel lambda, so reaching it from
        // here needs the free-function queries extension; on DPC++ it returns
        // the correct id for plain range kernels, nd_range kernels, and 0 for
        // single_task. Kernels launched over more than one dimension report
        // only dimension 0 (same limitation as the CUDA backend).
        rec.thread_id = static_cast<int>(
            sycl::ext::oneapi::this_work_item::get_nd_item<1>().get_global_linear_id());
        #else
        rec.thread_id = -1;
        #endif
        copy_to_buf(rec.file, file, BOOST_SAFE_NUMBERS_DEVICE_ERROR_BUFFER_SIZE);
        copy_to_buf(rec.expression, expression, BOOST_SAFE_NUMBERS_DEVICE_ERROR_BUFFER_SIZE);
    }

    // No __trap()/recoverable device abort in SYCL: record and return.
    // synchronize() surfaces the error on the host after the kernel completes.
    return;

    #else

    const auto msg = std::string(file) + ":" + std::to_string(line) + ": " + expression;
    throw_from_exception_type(exc, msg);

    #endif
}

} // namespace detail

// Host-facing RAII context placed around SYCL kernel launches. Mirrors the CUDA
// device_error_context but binds to the user's queue (SYCL has no implicit
// device). Only one may exist at a time. Defined on both compiler passes (its
// methods are host-only and never called from device code, so the device pass
// parses but does not codegen them) so that host code referencing it parses on
// the device pass too.
class device_error_context
{
    sycl::queue queue_;
    [[maybe_unused]] device_exception_mode mode_ {device_exception_mode::untrapped};

    static void guard()
    {
        if (detail::g_device_error_context_active)
        {
            BOOST_THROW_EXCEPTION(std::logic_error(
                "Only one device_error_context may exist at a time"));
        }
    }

public:

    explicit device_error_context(sycl::queue q)
        : queue_ {std::move(q)}
    {
        guard();
        detail::g_device_error_context_active = true;
        reset();
    }

    // The mode is advisory under SYCL (always deferred); kept for symmetry.
    device_error_context(sycl::queue q, const device_exception_mode e)
        : queue_ {std::move(q)}, mode_ {e}
    {
        guard();
        detail::g_device_error_context_active = true;
        reset();
    }

    ~device_error_context()
    {
        detail::g_device_error_context_active = false;
    }

    device_error_context(const device_error_context&) = delete;
    device_error_context& operator=(const device_error_context&) = delete;

    // Zero the device buffer (host -> device) so the context is reusable.
    void reset()
    {
        const detail::device_error_record zero {};
        queue_.copy(&zero, detail::g_device_error).wait();
    }

    // SYCL is always deferred; retained for source-level symmetry with CUDA.
    void set_device_exception_method(const device_exception_mode e)
    {
        mode_ = e;
    }

    // Wait for all enqueued kernels, read the buffer back (device -> host), and
    // if an error was captured clear it and throw the matching std::exception.
    void synchronize()
    {
        queue_.wait();

        detail::device_error_record rec {};
        queue_.copy(detail::g_device_error, &rec).wait();

        if (rec.flag != 0)
        {
            std::ostringstream oss;
            oss << "Device error on thread " << rec.thread_id
                << " at " << rec.file
                << ":" << rec.line
                << ": " << rec.expression;

            const auto msg = oss.str();

            // Clear before throwing so the context is reusable.
            reset();

            detail::throw_from_exception_type(rec.exception, msg);
        }
        else
        {
            reset();
        }
    }
};

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_ENABLE_SYCL

#endif // BOOST_SAFE_NUMBERS_DETAIL_SYCL_ERROR_REPORTING_HPP
// ===== END boost/safe_numbers/detail/sycl_error_reporting.hpp =====
#endif

#if (defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA)) || defined(BOOST_SAFE_NUMBERS_ENABLE_SYCL)

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE
#include <utility>
#endif

namespace boost::safe_numbers {

// Deferred device error reporting for kernels built on the overflowing_*
// families. The per-operation flags accumulate branch-free, so the kernel
// body stays vectorizable on CPU style devices where the per-operation report
// branch of the throwing operators inhibits SPMD vectorization. report() is
// the one branch per work item: on the device it records into the same error
// state that device_error_context::synchronize() rethrows from, and on the
// host it throws immediately. The individual error categories are not
// preserved; a deferred error always surfaces as overflow.
class deferred_errors
{
public:
    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr void accumulate(const bool had_error) noexcept
    {
        flags_ |= static_cast<unsigned>(had_error);
    }

    // Accumulates the flag of an overflowing_* result and returns its value
    template <typename T>
    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto unwrap(const std::pair<T, bool> result) noexcept -> T
    {
        accumulate(result.second);
        return result.first;
    }

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto any() const noexcept -> bool
    {
        return flags_ != 0U;
    }

    // Call once at the end of the kernel body
    BOOST_SAFE_NUMBERS_HOST_DEVICE void report(const char* file = "deferred_errors",
                                               const int line = 0,
                                               const char* expression = "deferred checked arithmetic") const
    {
        if (flags_ != 0U)
        {
            detail::report_device_error(detail::exception_type::overflow, file, line, expression);
        }
    }

private:
    unsigned flags_ {0U};
};

} // namespace boost::safe_numbers

#endif

#endif // BOOST_SAFE_NUMBERS_DEVICE_ERROR_REPORTING_HPP
// ===== END boost/safe_numbers/device_error_reporting.hpp =====

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/throw_exception.hpp>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

// Two-argument form: (exception_type, message)
// On host: constructs and throws the exception with the message.
// On a GPU device backend (CUDA or SYCL): forwards to the device error reporter,
// which itself branches host/device internally.
#ifdef BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT

#define BOOST_SAFE_NUMBERS_THROW_EXCEPTION(exc_type, msg) boost::safe_numbers::detail::report_device_error(boost::safe_numbers::detail::to_exception_enum<exc_type>(), __FILE__, __LINE__, msg)

#else

#define BOOST_SAFE_NUMBERS_THROW_EXCEPTION(exc_type, msg) BOOST_THROW_EXCEPTION(exc_type(msg))

#endif // BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT

#endif // BOOST_SAFE_NUMBERS_THROW_EXCEPTION_HPP
// ===== END boost/safe_numbers/detail/throw_exception.hpp =====
// ===== BEGIN boost/safe_numbers/detail/int128/bit.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_BIT_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_BIT_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/int128.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/config.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/clz.hpp
// ===== BEGIN boost/safe_numbers/detail/int128/detail/ctz.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_CTZ_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_CTZ_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/config.hpp

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILD_MODULE

#include <limits>
#include <cstdint>

#endif

namespace boost {
namespace int128 {
namespace detail {

namespace impl {

#if BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN(__builtin_ctz) && !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_CUDA))

constexpr int countr_impl(unsigned int x) noexcept
{
    return x ? __builtin_ctz(x) : std::numeric_limits<unsigned int>::digits;
}

constexpr int countr_impl(unsigned long x) noexcept
{
    return x ? __builtin_ctzl(x) : std::numeric_limits<unsigned long>::digits;
}

constexpr int countr_impl(unsigned long long x) noexcept
{
    return x ? __builtin_ctzll(x) : std::numeric_limits<unsigned long long>::digits;
}

#endif

#if !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_CUDA))

BOOST_SAFE_NUMBERS_DETAIL_INT128_INLINE_CONSTEXPR int countr_mod37[37] = {
    32, 0, 1, 26, 2, 23, 27, 0,
    3, 16, 24, 30, 28, 11, 0, 13,
    4, 7, 17, 0, 25, 22, 31, 15,
    29, 10, 12, 6, 0, 21, 14, 9,
    5, 20, 8, 19, 18
};

#endif

#if defined(_MSC_VER) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION) && !BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN(__builtin_ctz)

#pragma warning(push)
#pragma warning(disable : 4146) // unary minus operator applied to unsigned type, result still unsigned

constexpr int countr_impl(std::uint32_t x) noexcept
{
    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(x))
    {
        return countr_mod37[(-x & x) % 37]; // LCOV_EXCL_LINE
    }
    else
    {
        unsigned long r {};

        if(_BitScanForward(&r, x))
        {
            return static_cast<int>(r);
        }
        else
        {
            return 32;
        }
    }
}

#pragma warning(pop)

#elif !BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN(__builtin_ctz) || (defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_CUDA))

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4146) // unary minus operator applied to unsigned type, result still unsigned
#endif

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int countr_impl(std::uint32_t x) noexcept
{
    #if defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_CUDA)

    constexpr int countr_mod37[37] = {
        32, 0, 1, 26, 2, 23, 27, 0,
        3, 16, 24, 30, 28, 11, 0, 13,
        4, 7, 17, 0, 25, 22, 31, 15,
        29, 10, 12, 6, 0, 21, 14, 9,
        5, 20, 8, 19, 18
    };

    #endif

    return countr_mod37[(-x & x) % 37];
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif

#if (defined(_M_AMD64) || defined(_M_ARM64)) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION) && !BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN(__builtin_ctz) && !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_CUDA))

constexpr int countr_impl(std::uint64_t x) noexcept
{
    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(x))
    {
        return static_cast<std::uint32_t>(x) != 0 ? countr_impl(static_cast<std::uint32_t>(x)) : countr_impl(static_cast<std::uint32_t>(x >> 32)) + 32; // LCOV_EXCL_LINE
    }
    else
    {
        unsigned long r {};

        if(_BitScanForward64(&r, x))
        {
            return static_cast<int>(r);
        }
        else
        {
            return 64;
        }
    }
}

#elif !BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN(__builtin_ctz) || (defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_CUDA))

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int countr_impl(std::uint64_t x) noexcept
{
    return static_cast<std::uint32_t>(x) != 0 ? countr_impl(static_cast<std::uint32_t>(x)) :
                                                countr_impl(static_cast<std::uint32_t>(x >> 32)) + 32;
}

#endif

} // namespace impl

template <typename T>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int countr_zero(T x) noexcept
{
    static_assert(std::numeric_limits<T>::is_integer && !std::numeric_limits<T>::is_signed,
                  "Can only count with unsigned integers");

    return impl::countr_impl(x);
}

} // namespace detail
} // namespace int128
} // namespace boost

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_CTZ_HPP
// ===== END boost/safe_numbers/detail/int128/detail/ctz.hpp =====

namespace boost {
namespace int128 {

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr bool has_single_bit(const uint128_t x) noexcept
{
    return x && !(x & (x - 1U));
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int countl_zero(const uint128_t x) noexcept
{
    return x.high == 0 ? 64 + detail::countl_zero(x.low) : detail::countl_zero(x.high);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int countl_one(const uint128_t x) noexcept
{
    return countl_zero(~x);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int bit_width(const uint128_t x) noexcept
{
    return x ? 128 - countl_zero(x) : 0;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t bit_ceil(const uint128_t x) noexcept
{
    return x <= 1U ? static_cast<uint128_t>(1) : static_cast<uint128_t>(1) << bit_width(x - 1U);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t bit_floor(const uint128_t x) noexcept
{
    return x > 0U ? static_cast<uint128_t>(1) << (bit_width(x) - 1U) : static_cast<uint128_t>(0);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int countr_zero(const uint128_t x) noexcept
{
    return x.low == 0 ? 64 + detail::countr_zero(x.high) : detail::countr_zero(x.low);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int countr_one(const uint128_t x) noexcept
{
    return countr_zero(~x);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t rotl(const uint128_t x, const int s) noexcept
{
    constexpr auto mask {127U};
    return x << (static_cast<unsigned>(s) & mask) | x >> (static_cast<unsigned>(-s) & mask);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t rotr(const uint128_t x, const int s) noexcept
{
    constexpr auto mask {127U};
    return x >> (static_cast<unsigned>(s) & mask) | x << (static_cast<unsigned>(-s) & mask);
}

#if BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN(__builtin_popcountll) && !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_CUDA))

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int popcount(const uint128_t x) noexcept
{
    return __builtin_popcountll(x.high) + __builtin_popcountll(x.low);
}

#endif

namespace impl {

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int popcount_impl(std::uint64_t x) noexcept
{
    x = x - ((x >> 1U) & UINT64_C(0x5555555555555555));
    x = (x & UINT64_C(0x3333333333333333)) + ((x >> 2U) & UINT64_C(0x3333333333333333));
    x = (x + (x >> 4U)) & UINT64_C(0x0F0F0F0F0F0F0F0F);

    return static_cast<int>((x * UINT64_C(0x0101010101010101)) >> 56U);
}

} // namespace impl

#if defined(_M_AMD64) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION) && !BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN(__builtin_popcountll)

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int popcount(const uint128_t x) noexcept
{
    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(x))
    {
        return impl::popcount_impl(x.high) + impl::popcount_impl(x.low); // LCOV_EXCL_LINE
    }
    else
    {
        #ifdef __AVX__

        return static_cast<int>(_mm_popcnt_u64(x.high) +  _mm_popcnt_u64(x.low));

        #else

        return static_cast<int>(__popcnt64(x.high) + __popcnt64(x.low));

        #endif
    }
}

#elif defined(_M_IX86) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION) && !BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN(__builtin_popcountll)

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int popcount(const uint128_t x) noexcept
{
    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(x))
    {
        return impl::popcount_impl(x.high) + impl::popcount_impl(x.low); // LCOV_EXCL_LINE
    }
    else
    {
        #ifdef __AVX__

        return static_cast<int>(
               _mm_popcnt_u32(static_cast<unsigned>(x.high)) +
               _mm_popcnt_u32(static_cast<unsigned>(x.high >> 32U)) +
               _mm_popcnt_u32(static_cast<unsigned>(x.low)) +
               _mm_popcnt_u32(static_cast<unsigned>(x.low >> 32U)));

        #else

        return static_cast<int>(
               __popcnt(static_cast<unsigned>(x.high)) +
               __popcnt(static_cast<unsigned>(x.high >> 32U)) +
               __popcnt(static_cast<unsigned>(x.low)) +
               __popcnt(static_cast<unsigned>(x.low >> 32U)));

        #endif
    }
}

#elif !BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN(__builtin_popcountll) || (defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_CUDA))

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int popcount(const uint128_t x) noexcept
{
    return impl::popcount_impl(x.high) + impl::popcount_impl(x.low);
}

#endif

#if BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN(__builtin_bswap64) && !(defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_CUDA))

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t byteswap(const uint128_t x) noexcept
{
    return {__builtin_bswap64(x.low), __builtin_bswap64(x.high)};
}

#endif

namespace impl {

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr std::uint64_t byteswap_impl(const std::uint64_t x) noexcept
{
    const auto step32 {x << 32U | x >> 32U};
    const auto step16 {(step32 & UINT64_C(0x0000FFFF0000FFFF)) << 16U | (step32 & UINT64_C(0xFFFF0000FFFF0000)) >> 16U};
    return (step16 & UINT64_C(0x00FF00FF00FF00FF)) << 8U | (step16 & UINT64_C(0xFF00FF00FF00FF00)) >> 8U;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t byteswap_impl(const uint128_t x) noexcept
{
    return {byteswap_impl(x.low), byteswap_impl(x.high)};
}

} // namespace impl

#if defined(_MSC_VER) && !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_NO_CONSTEVAL_DETECTION) && !BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN(__builtin_bswap64)

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t byteswap(const uint128_t x) noexcept
{
    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_IS_CONSTANT_EVALUATED(x))
    {
        return impl::byteswap_impl(x); // LCOV_EXCL_LINE
    }
    else
    {
        return {_byteswap_uint64(x.low), _byteswap_uint64(x.high)};
    }
}

#elif !BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_BUILTIN(__builtin_bswap64) || (defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_CUDA))

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t byteswap(const uint128_t x) noexcept
{
    return impl::byteswap_impl(x);
}

#endif

} // namespace int128
} // namespace boost

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_BIT_HPP
// ===== END boost/safe_numbers/detail/int128/bit.hpp =====
// ===== BEGIN boost/safe_numbers/detail/compile_assert.hpp =====
// Copyright 2023 - 2026 Jonathan Grant <jg@jguk.org>
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_COMPILE_ASSERT_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_COMPILE_ASSERT_HPP

/**
 * @file compile_assert.h
 * @brief Header file providing a macro for compile-time assertions builds.
 */

// Utilize GCC attribute error as part of an optimized build to stop when conditions
// are not met. This is at build time, by the compiler, when it does redundant code
// removal, aka dead code removal.
//
// Implemented in C, it can be used in C++ projects as well.
//
// compile_assert() is kept as a macro so GCC shows the line it's invoked as in
// any asserts that fire. (If changed to inline, GCC shows the inline function code instead)
//
// Note this relies upon an optimized compiler build, in -O0 builds, it "compiles out"


/**
 * @brief Utilize GCC attribute error to stop when conditions
 * are not met. This is at build time, by the compiler, when it does redundant code
 * removal, aka dead code removal.
 *
 * Implemented in C, it can be used in C++ projects as well.
 *
 * @note compile_assert() is kept as a macro so GCC shows the line it's invoked as in
 * any asserts that fire. (If changed to inline, GCC shows the inline function code instead)
 *
 * @note This only works in an Optimized build; in -O0 builds, it "compiles out".
 *
 * @note Future work: The description isn't logged yet - never will be, just note
 * to the programmer to refer to when they look up the file and line number of the compiler
 * output.
 */

// BOOST_SAFE_NUMBERS_COMPILE_ASSERT() takes an optional message: (expr) or (expr, "why").
#define BOOST_SAFE_NUMBERS_CA_FIRST(first, ...) first
#define BOOST_SAFE_NUMBERS_CA_MSG(...) BOOST_SAFE_NUMBERS_CA_MSG_(__VA_ARGS__, "")
#define BOOST_SAFE_NUMBERS_CA_MSG_(expression, message, ...) message

#ifdef __GNUC__

// The compile-time-error mechanism relies on the GCC/Clang function 'error' attribute.
// GCC has always supported it; Clang only supports it from version 14. If the feature is
// requested on a compiler that lacks it (for example Clang 13), fail with a clear message
// instead of the confusing -Wunknown-attributes diagnostic that -Werror turns into an error.
#if defined(BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT) && !(defined(__has_attribute) && __has_attribute(error))
#error "BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT requires the 'error' function attribute, available on GCC and on Clang 14 or later."
#endif

#if defined(__OPTIMIZE__) && defined(BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT)
#define BOOST_SAFE_NUMBERS_GCC_COMPILE_ASSERT
#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT_ACTIVE
#endif // defined(__OPTIMIZE__) && defined(BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT)
#endif // __GNUC__

#ifdef BOOST_SAFE_NUMBERS_GCC_COMPILE_ASSERT

/**
 * @brief Function to stop compilation with an error message if a compile_assert condition is not satisfied.
 * There is no implementation as it is only used to stop the compiler.
 * @see compile_assert
 */

// The failure function carries the error message via the error attribute. Its name is
// made unique per expansion with __COUNTER__ so that two assertions with different
// messages in one translation unit do not redeclare the same symbol with conflicting
// attributes (clang -Wignored-attributes).
#define BOOST_SAFE_NUMBERS_CA_CAT2(a, b) a##b
#define BOOST_SAFE_NUMBERS_CA_CAT(a, b) BOOST_SAFE_NUMBERS_CA_CAT2(a, b)

/**
 * @def compile_assert
 * @brief Macro for compile-time assertions.
 * @param expression The compile-time condition to be checked.
 * @param message Optional description of the assertion; defaults to "" when omitted.
 */
#define BOOST_SAFE_NUMBERS_CA_ASSERT_IMPL(expression, message, fn) \
    do { \
        void fn() __attribute__ ((error(message))); \
        if (!(expression)) { \
            fn(); \
        } \
    } while (0)
#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT(...) \
    BOOST_SAFE_NUMBERS_CA_ASSERT_IMPL(BOOST_SAFE_NUMBERS_CA_FIRST(__VA_ARGS__), BOOST_SAFE_NUMBERS_CA_MSG(__VA_ARGS__), BOOST_SAFE_NUMBERS_CA_CAT(_compile_assert_fail_, __COUNTER__))


// CONST_P turns a provably-constant precondition violation into a build error. The optional
// trailing arguments name the operands that must be compile-time constants for the check to
// fire; with none given, the condition itself is the guarded operand:
//
//   CONST_P(condition, message)             guard on the condition
//   CONST_P(condition, message, a)          guard on operand a
//   CONST_P(condition, message, a, b, ...)  guard on every listed operand (ANDed)
//
// GCC before 13 does not fold __builtin_constant_p when it is applied to a value derived from
// an overflow builtin, or even to a comparison in a deep constexpr call chain, so a guard on
// the condition is missed and the assertion never fires. Naming the raw operands (plain
// integers, which fold on every supported GCC) makes the check fire on GCC 11 and 12 as well;
// the condition is still evaluated by dead-code removal exactly as before.

// __builtin_constant_p of every listed operand, ANDed together. The levels chain through
// distinct names so each re-expands cleanly; up to eight operands are supported.
#define BOOST_SAFE_NUMBERS_CA_G1(a, ...) __builtin_constant_p(a) __VA_OPT__(&& BOOST_SAFE_NUMBERS_CA_G2(__VA_ARGS__))
#define BOOST_SAFE_NUMBERS_CA_G2(a, ...) __builtin_constant_p(a) __VA_OPT__(&& BOOST_SAFE_NUMBERS_CA_G3(__VA_ARGS__))
#define BOOST_SAFE_NUMBERS_CA_G3(a, ...) __builtin_constant_p(a) __VA_OPT__(&& BOOST_SAFE_NUMBERS_CA_G4(__VA_ARGS__))
#define BOOST_SAFE_NUMBERS_CA_G4(a, ...) __builtin_constant_p(a) __VA_OPT__(&& BOOST_SAFE_NUMBERS_CA_G5(__VA_ARGS__))
#define BOOST_SAFE_NUMBERS_CA_G5(a, ...) __builtin_constant_p(a) __VA_OPT__(&& BOOST_SAFE_NUMBERS_CA_G6(__VA_ARGS__))
#define BOOST_SAFE_NUMBERS_CA_G6(a, ...) __builtin_constant_p(a) __VA_OPT__(&& BOOST_SAFE_NUMBERS_CA_G7(__VA_ARGS__))
#define BOOST_SAFE_NUMBERS_CA_G7(a, ...) __builtin_constant_p(a) __VA_OPT__(&& BOOST_SAFE_NUMBERS_CA_G8(__VA_ARGS__))
#define BOOST_SAFE_NUMBERS_CA_G8(a) __builtin_constant_p(a)
#define BOOST_SAFE_NUMBERS_CA_GUARD(...) BOOST_SAFE_NUMBERS_CA_G1(__VA_ARGS__)

// Pick the guard expression: the condition when no operands were given, else the ANDed operands.
#define BOOST_SAFE_NUMBERS_CA_CONST_P_GUARD_(condition, ...)  __builtin_constant_p(condition)
#define BOOST_SAFE_NUMBERS_CA_CONST_P_GUARD_1(condition, ...) BOOST_SAFE_NUMBERS_CA_GUARD(__VA_ARGS__)
#define BOOST_SAFE_NUMBERS_CA_CONST_P_GUARD(condition, ...) \
    BOOST_SAFE_NUMBERS_CA_CAT(BOOST_SAFE_NUMBERS_CA_CONST_P_GUARD_, __VA_OPT__(1))(condition, __VA_ARGS__)

#define BOOST_SAFE_NUMBERS_CA_CONST_P_IMPL(condition, message, guard, fn) \
    do { \
        if (guard) { \
            if (!(condition)) { \
                void fn() __attribute__ ((error(message))); \
                fn(); \
            } \
        } \
    } while (0)
#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(condition, message, ...) \
    BOOST_SAFE_NUMBERS_CA_CONST_P_IMPL( \
        condition, message, \
        BOOST_SAFE_NUMBERS_CA_CONST_P_GUARD(condition, __VA_ARGS__), \
        BOOST_SAFE_NUMBERS_CA_CAT(_compile_assert_fail_, __COUNTER__))

#else
#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT(...)
#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(condition, message, ...)
#endif


#ifdef BOOST_SAFE_NUMBERS_GCC_COMPILE_ASSERT
namespace boost::safe_numbers::detail {
void * stop_compile2() __attribute__ ((error("'compile_assert pointer error detected'")));
} // namespace boost::safe_numbers::detail
/**
 * @def compile_assert_never_null
 * @brief Macro to ensure a pointer is never NULL.
 * @param ptr The pointer to be checked for NULL.
 * @return The pointer ptr if not NULL.
 */
#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT_NEVER_NULL(ptr) ((ptr) ? (ptr) : boost::safe_numbers::detail::stop_compile2())

#else
#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT_NEVER_NULL(ptr) ptr
#endif


#ifdef BOOST_SAFE_NUMBERS_GCC_COMPILE_ASSERT
/**
 * @def compile_assert_ptr
 * @brief Macro to check a condition and show the pointer, or stop the
 * compiler by calling the error function in optimized builds.
 * @param condition
 * @param ptr The pointer.
 * @return The pointer.
 */
#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT_PTR(condition, ptr) ((condition) ? (ptr) : boost::safe_numbers::detail::stop_compile2())

#else
#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT_PTR(condition, ptr) ptr
#endif


#ifdef BOOST_SAFE_NUMBERS_GCC_COMPILE_ASSERT
namespace boost::safe_numbers::detail {
int stop_compile3() __attribute__ ((error("'compile_assert_scalar error detected'")));
} // namespace boost::safe_numbers::detail
/**
 * @def compile_assert_scalar
 * @brief Macro to check a condition and substitute with the scalar in an optimized build.
 * @param condition
 * @param scalar The value.
 * @return The scalar value.
 */
#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT_SCALAR(condition, scalar) ((condition) ? (scalar) : boost::safe_numbers::detail::stop_compile3())

#else
#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT_SCALAR(condition, scalar) scalar
#endif

// make lack of COMPILE_FILE a hard error
//#if defined(_MSC_VER)
//#ifndef COMPILE_FILE
//#error MSVC compile_assert requires COMPILE_FILE to be passed from makefile
//#endif // COMPILE_FILE
//#endif // _MSC_VER

#if defined(_MSC_VER)
#ifndef BOOST_SAFE_NUMBERS_COMPILE_FILE
#define BOOST_SAFE_NUMBERS_COMPILE_FILE filename_not_set
#endif // COMPILE_FILE
#endif // _MSC_VER


#if defined(_MSC_VER)
#if defined(BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT)
#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT_ACTIVE
#define BOOST_SAFE_NUMBERS_MSVC_COMPILE_ASSERT
/* Requires makefile to pass the filename as a macro: cl /DCOMPILE_FILE=__FILE_msvc18_cpp_
 * The preprocessor merges the three macros as a missing function call.
 * the failure shows as eg _compile_assert__FILE_msvc18_cpp_23(void)
 */
#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT_ACTIVE
#define BOOST_SAFE_NUMBERS_MERGE2(a,b) a##b
#define BOOST_SAFE_NUMBERS_MERGE1(a,b) BOOST_SAFE_NUMBERS_MERGE2(a,b)
#define BOOST_SAFE_NUMBERS_MERGE3(a,b,c) BOOST_SAFE_NUMBERS_MERGE1(a, BOOST_SAFE_NUMBERS_MERGE1(b,c))

// The non-active fallback above already defined this as empty
#undef BOOST_SAFE_NUMBERS_COMPILE_ASSERT
#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT(...) \
do { \
    if (!(BOOST_SAFE_NUMBERS_CA_FIRST(__VA_ARGS__))) { \
      extern void BOOST_SAFE_NUMBERS_MERGE3(_compile_assert, BOOST_SAFE_NUMBERS_COMPILE_FILE, __LINE__)(); \
      BOOST_SAFE_NUMBERS_MERGE3(_compile_assert, BOOST_SAFE_NUMBERS_COMPILE_FILE, __LINE__)(); \
    } \
} while (0)
#else

#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT(...)
#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT_ACTIVE

#endif // defined(BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT)
#endif // defined(_MSC_VER)

// PTR / NEVER_NULL / SCALAR are already defined for every non-GCC-active configuration
// (including MSVC) by the #else branches above, so they are not redefined here. Redefining
// them for MSVC caused a C4005 macro-redefinition error under /WX (the PTR fallback differed).

// Generic compiler support, via a missing symbol
#if defined(BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT)
#if !defined(BOOST_SAFE_NUMBERS_COMPILE_ASSERT)
#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT_ACTIVE

#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT(...) \
    do { \
        void _compile_assert_fail(); \
        if (!(BOOST_SAFE_NUMBERS_CA_FIRST(__VA_ARGS__))) { \
            _compile_assert_fail(); \
        } \
    } while (0)
#endif // !defined(compile_assert)
#endif // defined(BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT)


#ifndef BOOST_SAFE_NUMBERS_COMPILE_ASSERT
#error "BOOST_SAFE_NUMBERS_COMPILE_ASSERT not defined"
#endif

#ifndef BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P
#error "BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P not defined"
#endif

#endif // BOOST_SAFE_NUMBERS_DETAIL_COMPILE_ASSERT_HPP
// ===== END boost/safe_numbers/detail/compile_assert.hpp =====
// [amalgamate] skipped duplicate include of boost/safe_numbers/overflow_policy.hpp

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

template <fundamental_unsigned_integral BasisType, auto ErrorPolicy>
class unsigned_integer_basis
{
public:

    // This is exposed to the user so that they can convert back to built-in
    using basis_type = BasisType;

    static_assert(is_overflow_policy_v<ErrorPolicy> || error_handler_for<decltype(ErrorPolicy), BasisType>,
                  "ErrorPolicy must be a boost::safe_numbers::overflow_policy enumerator or a stateless "
                  "handler type providing on_error(error_kind, BasisType, const char*) returning BasisType");

    static_assert(!is_value_returning_policy<ErrorPolicy>(),
                  "overflow_tuple, checked, and widen change the result type of every operation, "
                  "so they can not be type-level policies: use the overflowing_*, checked_*, "
                  "and widening_* free functions instead");

    static_assert(!is_overflow_policy_v<ErrorPolicy> || is_valid_type_policy<ErrorPolicy>(basis_kind::unsigned_integer),
                  "unsigned_integer_basis supports overflow_policy::throw_exception, "
                  "overflow_policy::saturate, and overflow_policy::strict");

    // Exposed so that generic code and tests can query the type-level policy
    static constexpr auto error_policy {ErrorPolicy};

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

    template <fundamental_unsigned_integral OtherBasis, auto OtherPolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator+=(unsigned_integer_basis<OtherBasis, OtherPolicy> rhs)
        noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> unsigned_integer_basis&;

    template <fundamental_unsigned_integral OtherBasis, auto OtherPolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator-=(unsigned_integer_basis<OtherBasis, OtherPolicy> rhs)
        noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> unsigned_integer_basis&;

    template <fundamental_unsigned_integral OtherBasis, auto OtherPolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator*=(unsigned_integer_basis<OtherBasis, OtherPolicy> rhs)
        noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> unsigned_integer_basis&;

    template <fundamental_unsigned_integral OtherBasis, auto OtherPolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator/=(unsigned_integer_basis<OtherBasis, OtherPolicy> rhs)
        noexcept(policy_is_nothrow_div<ErrorPolicy, BasisType>()) -> unsigned_integer_basis&;

    template <fundamental_unsigned_integral OtherBasis, auto OtherPolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator%=(unsigned_integer_basis<OtherBasis, OtherPolicy> rhs)
        noexcept(policy_is_nothrow_div<ErrorPolicy, BasisType>()) -> unsigned_integer_basis&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator&=(unsigned_integer_basis rhs) noexcept -> unsigned_integer_basis&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator|=(unsigned_integer_basis rhs) noexcept -> unsigned_integer_basis&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator^=(unsigned_integer_basis rhs) noexcept -> unsigned_integer_basis&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator<<=(unsigned_integer_basis rhs)
        noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> unsigned_integer_basis&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator>>=(unsigned_integer_basis rhs)
        noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> unsigned_integer_basis&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator++()
        noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> unsigned_integer_basis&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator++(int)
        noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> unsigned_integer_basis;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator--()
        noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> unsigned_integer_basis&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator--(int)
        noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> unsigned_integer_basis;

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

template <fundamental_unsigned_integral BasisType, auto ErrorPolicy>
template <fundamental_unsigned_integral OtherBasis>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr unsigned_integer_basis<BasisType, ErrorPolicy>::operator OtherBasis() const
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

#if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_add_overflow) && !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)

template <std::unsigned_integral T>
bool unsigned_intrin_add(const T lhs, const T rhs, T& result)
{
    return __builtin_add_overflow(lhs, rhs, &result);
}

#elif defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) && !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)

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

#elif defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN) && !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)

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
template <auto Policy, fundamental_unsigned_integral BasisType>
struct add_helper
{
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                              const unsigned_integer_basis<BasisType, TypePolicy> rhs)
        noexcept(policy_is_nothrow_arith<Policy, BasisType>())
        -> unsigned_integer_basis<BasisType, TypePolicy>
    {
        using result_type = unsigned_integer_basis<BasisType, TypePolicy>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType res {};

        auto handle_overflow = [&res]
        {
            #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
            // Saturation produces a defined value, so it must keep evaluating at
            // constant evaluation time instead of failing the build with a throw
            if (std::is_constant_evaluated() && !policy_equals<Policy>(overflow_policy::saturate) && !is_user_handler_v<Policy>)
            {
                if constexpr (std::is_same_v<BasisType, std::uint8_t>)
                {
                    throw std::overflow_error("Overflow detected in u8 addition"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::uint16_t>)
                {
                    throw std::overflow_error("Overflow detected in u16 addition"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::uint32_t>)
                {
                    throw std::overflow_error("Overflow detected in u32 addition"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::uint64_t>)
                {
                    throw std::overflow_error("Overflow detected in u64 addition"); // LCOV_EXCL_LINE
                }
                else
                {
                    throw std::overflow_error("Overflow detected in u128 addition"); // LCOV_EXCL_LINE
                }
            }
            else
            #endif
            {
                if constexpr (policy_equals<Policy>(overflow_policy::throw_exception))
                {
                    static_cast<void>(res);
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, overflow_add_msg<BasisType>());
                }
                else if constexpr (policy_equals<Policy>(overflow_policy::saturate))
                {
                    res = std::numeric_limits<BasisType>::max();
                }
                else if constexpr (policy_equals<Policy>(overflow_policy::strict))
                {
                    static_cast<void>(res);
                    std::exit(EXIT_FAILURE);
                }
                else if constexpr (is_user_handler_v<Policy>)
                {
                    res = Policy.on_error(error_kind::overflow, res, overflow_add_msg<BasisType>());
                }
                else
                {
                    static_cast<void>(res);
                    BOOST_SAFE_NUMBERS_UNREACHABLE;
                }
            }
        };

        // A constant operation that overflows is a build error under the error policies
        // (throw_exception, strict); saturate/checked/overflow_tuple return a defined
        // value and are excluded. The assert reuses the same overflow result the runtime
        // path already computes, so it costs nothing when it does not fire.
        #if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_add_overflow) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

        if constexpr (!std::is_same_v<BasisType, int128::uint128_t>)
        {
            #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)

            if (!std::is_constant_evaluated())
            {
                const bool overflowed {impl::unsigned_intrin_add(lhs_basis, rhs_basis, res)};

                #ifdef BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT
                if constexpr (policy_equals<Policy>(overflow_policy::throw_exception) || policy_equals<Policy>(overflow_policy::strict))
                {
                    BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(!overflowed, "unsigned addition overflow", lhs_basis, rhs_basis);
                }
                #endif

                if (overflowed)
                {
                    handle_overflow();
                }

                return result_type{res};
            }

            #endif
        }

        #endif // BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_add_overflow) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

        const bool overflowed {impl::unsigned_no_intrin_add(lhs_basis, rhs_basis, res)};

        #ifdef BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT
        if constexpr (policy_equals<Policy>(overflow_policy::throw_exception) || policy_equals<Policy>(overflow_policy::strict))
        {
            BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(!overflowed, "unsigned addition overflow", lhs_basis, rhs_basis);
        }
        #endif

        if (overflowed)
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
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                              const unsigned_integer_basis<BasisType, TypePolicy> rhs) noexcept
        -> std::pair<unsigned_integer_basis<BasisType, TypePolicy>, bool>
    {
        using result_type = unsigned_integer_basis<BasisType, TypePolicy>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType res {};

        // No overflow intrinsic here on purpose: the *.with.overflow intrinsics
        // block loop vectorization when the flag is consumed as a value, while
        // this compare form vectorizes and emits the same scalar code.
        const auto overflowed {impl::unsigned_no_intrin_add(lhs_basis, rhs_basis, res)};
        return std::make_pair(result_type{res}, overflowed);
    }
};

// Partial specialization for overflow_tuple policy
template <fundamental_unsigned_integral BasisType>
struct add_helper<overflow_policy::checked, BasisType>
{
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                              const unsigned_integer_basis<BasisType, TypePolicy> rhs) noexcept
        -> std::optional<unsigned_integer_basis<BasisType, TypePolicy>>
    {
        using result_type = unsigned_integer_basis<BasisType, TypePolicy>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType res {};

        // No overflow intrinsic here on purpose: the *.with.overflow intrinsics
        // block loop vectorization when the flag is consumed as a value, while
        // this compare form vectorizes and emits the same scalar code.
        const auto overflowed {impl::unsigned_no_intrin_add(lhs_basis, rhs_basis, res)};
        return overflowed ? std::nullopt : std::make_optional(result_type{res});
    }
};

// Partial specialization for widening policy
template <fundamental_unsigned_integral BasisType>
struct add_helper<overflow_policy::widen, BasisType>
{
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                              const unsigned_integer_basis<BasisType, TypePolicy> rhs) noexcept
    {
        using promoted_type = promoted_type<BasisType>;
        static_assert(!std::is_same_v<promoted_type, bool>, "Widening policy with uint128_t is not supported");

        using result_type = unsigned_integer_basis<promoted_type, TypePolicy>;
        return result_type{static_cast<promoted_type>(static_cast<promoted_type>(static_cast<BasisType>(lhs)) + static_cast<BasisType>(rhs))};
    }
};

template <auto Policy, fundamental_unsigned_integral BasisType, auto TypePolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto add_impl(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                      const unsigned_integer_basis<BasisType, TypePolicy> rhs)
    noexcept(policy_is_nothrow_arith<Policy, BasisType>())
{
    return add_helper<Policy, BasisType>::apply(lhs, rhs);
}

template <fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto operator+(const unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                       const unsigned_integer_basis<BasisType, ErrorPolicy> rhs)
    noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> unsigned_integer_basis<BasisType, ErrorPolicy>
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

    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)

    // Only the throwing policy needs the duplicated diagnostics; the test is if constexpr
    // so the throw is discarded where the operator is noexcept (GCC -Wterminate)
    if constexpr (policy_equals<ErrorPolicy>(overflow_policy::throw_exception))
    {
        if (std::is_constant_evaluated())
        {
            BasisType res {};
            if (impl::unsigned_no_intrin_add(static_cast<BasisType>(lhs), static_cast<BasisType>(rhs), res))
            {
                if constexpr (std::is_same_v<BasisType, std::uint8_t>)
                {
                    throw std::overflow_error("Overflow detected in u8 addition"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::uint16_t>)
                {
                    throw std::overflow_error("Overflow detected in u16 addition"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::uint32_t>)
                {
                    throw std::overflow_error("Overflow detected in u32 addition"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::uint64_t>)
                {
                    throw std::overflow_error("Overflow detected in u64 addition"); // LCOV_EXCL_LINE
                }
                else
                {
                    throw std::overflow_error("Overflow detected in u128 addition"); // LCOV_EXCL_LINE
                }
            }

            return unsigned_integer_basis<BasisType, ErrorPolicy>{res};
        }
    }

    #endif

    return add_helper<ErrorPolicy, BasisType>::apply(lhs, rhs);
}

} // namespace boost::safe_numbers::detail

#define BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP(OP_NAME, OP_SYMBOL)                                                                                \
template <boost::safe_numbers::detail::fundamental_unsigned_integral LHSBasis, auto LHSPolicy,                                                                  \
          boost::safe_numbers::detail::fundamental_unsigned_integral RHSBasis, auto RHSPolicy>                                                                  \
    requires (!std::is_same_v<LHSBasis, RHSBasis> || boost::safe_numbers::detail::policies_differ<LHSPolicy, RHSPolicy>())                                                                                \
BOOST_SAFE_NUMBERS_HOST_DEVICE                                                                                                                              \
constexpr auto OP_SYMBOL(const boost::safe_numbers::detail::unsigned_integer_basis<LHSBasis, LHSPolicy>,                                                       \
                         const boost::safe_numbers::detail::unsigned_integer_basis<RHSBasis, RHSPolicy>)                                                       \
{                                                                                                                                                              \
    if constexpr (std::is_same_v<LHSBasis, RHSBasis>)                                                                                                         \
    {                                                                                                                                                          \
        static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>,                                                                        \
                      "Can not perform " OP_NAME " between same width types with different overflow policies "                                                 \
                      "(e.g. u8 and sat_u8): convert explicitly through basis_type first");                                                                    \
    }                                                                                                                                                          \
    else if constexpr (std::is_same_v<LHSBasis, std::uint8_t>)                                                                                                 \
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
    return boost::safe_numbers::detail::unsigned_integer_basis<LHSBasis, LHSPolicy>(0);                                                                        \
}

namespace boost::safe_numbers::detail {

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("comparison", operator<=>)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("equality", operator==)

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("addition", operator+)

template <fundamental_unsigned_integral BasisType, auto ErrorPolicy>
template <fundamental_unsigned_integral OtherBasisType, auto OtherPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr auto unsigned_integer_basis<BasisType, ErrorPolicy>::operator+=(const unsigned_integer_basis<OtherBasisType, OtherPolicy> rhs)
    noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> unsigned_integer_basis&
{
    *this = *this + rhs;
    return *this;
}

// ------------------------------
// Subtraction
// ------------------------------

namespace impl {

#if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_sub_overflow) && !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)

template <std::unsigned_integral T>
bool unsigned_intrin_sub(T lhs, T rhs, T& result)
{
    return __builtin_sub_overflow(lhs, rhs, &result);
}

#elif defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) && !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)

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

#elif defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN) && !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)

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
template <auto Policy, fundamental_unsigned_integral BasisType>
struct sub_helper
{
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                              const unsigned_integer_basis<BasisType, TypePolicy> rhs)
        noexcept(policy_is_nothrow_arith<Policy, BasisType>())
        -> unsigned_integer_basis<BasisType, TypePolicy>
    {
        using result_type = unsigned_integer_basis<BasisType, TypePolicy>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType res {};

        auto handle_underflow = [&res]
        {
            #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)

            // Saturation produces a defined value, so it must keep evaluating at
            // constant evaluation time instead of failing the build with a throw
            if (std::is_constant_evaluated() && !policy_equals<Policy>(overflow_policy::saturate) && !is_user_handler_v<Policy>)
            {
                if constexpr (std::is_same_v<BasisType, std::uint8_t>)
                {
                    throw std::underflow_error("Underflow detected in u8 subtraction"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::uint16_t>)
                {
                    throw std::underflow_error("Underflow detected in u16 subtraction"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::uint32_t>)
                {
                    throw std::underflow_error("Underflow detected in u32 subtraction"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::uint64_t>)
                {
                    throw std::underflow_error("Underflow detected in u64 subtraction"); // LCOV_EXCL_LINE
                }
                else
                {
                    throw std::underflow_error("Underflow detected in u128 subtraction"); // LCOV_EXCL_LINE
                }
            }
            else
            #endif
            {
                if constexpr (policy_equals<Policy>(overflow_policy::throw_exception))
                {
                    static_cast<void>(res);
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, underflow_sub_msg<BasisType>());
                }
                else if constexpr (policy_equals<Policy>(overflow_policy::saturate))
                {
                    res = std::numeric_limits<BasisType>::min();
                }
                else if constexpr (policy_equals<Policy>(overflow_policy::strict))
                {
                    static_cast<void>(res);
                    std::exit(EXIT_FAILURE);
                }
                else if constexpr (is_user_handler_v<Policy>)
                {
                    res = Policy.on_error(error_kind::underflow, res, underflow_sub_msg<BasisType>());
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
            #if (BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_sub_overflow) || BOOST_SAFE_NUMBERS_HAS_BUILTIN(_subborrow_u64) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)) && !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)

            if (!std::is_constant_evaluated())
            {
                const bool underflowed {impl::unsigned_intrin_sub(lhs_basis, rhs_basis, res)};

                // A constant subtraction that underflows is a build error under the error
                // policies (throw_exception, strict); value policies are excluded.
                #ifdef BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT
                if constexpr (policy_equals<Policy>(overflow_policy::throw_exception) || policy_equals<Policy>(overflow_policy::strict))
                {
                    BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(!underflowed, "unsigned subtraction underflow");
                }
                #endif

                if (underflowed)
                {
                    handle_underflow();
                }

                return result_type{res};
            }

            #endif
        }

        const bool underflowed {impl::unsigned_no_intrin_sub(lhs_basis, rhs_basis, res)};

        #ifdef BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT
        if constexpr (policy_equals<Policy>(overflow_policy::throw_exception) || policy_equals<Policy>(overflow_policy::strict))
        {
            BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(!underflowed, "unsigned subtraction underflow");
        }
        #endif

        if (underflowed)
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
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                              const unsigned_integer_basis<BasisType, TypePolicy> rhs) noexcept
        -> std::pair<unsigned_integer_basis<BasisType, TypePolicy>, bool>
    {
        using result_type = unsigned_integer_basis<BasisType, TypePolicy>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType res {};

        // No overflow intrinsic here on purpose: the *.with.overflow intrinsics
        // block loop vectorization when the flag is consumed as a value, while
        // this compare form vectorizes and emits the same scalar code.
        const auto underflowed {impl::unsigned_no_intrin_sub(lhs_basis, rhs_basis, res)};
        return std::make_pair(result_type{res}, underflowed);
    }
};

// Partial specialization for checked policy
template <fundamental_unsigned_integral BasisType>
struct sub_helper<overflow_policy::checked, BasisType>
{
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                              const unsigned_integer_basis<BasisType, TypePolicy> rhs) noexcept
        -> std::optional<unsigned_integer_basis<BasisType, TypePolicy>>
    {
        using result_type = unsigned_integer_basis<BasisType, TypePolicy>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType res {};

        // No overflow intrinsic here on purpose: the *.with.overflow intrinsics
        // block loop vectorization when the flag is consumed as a value, while
        // this compare form vectorizes and emits the same scalar code.
        const auto underflowed {impl::unsigned_no_intrin_sub(lhs_basis, rhs_basis, res)};
        return underflowed ? std::nullopt : std::make_optional(result_type{res});
    }
};

template <auto Policy, fundamental_unsigned_integral BasisType, auto TypePolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto sub_impl(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                      const unsigned_integer_basis<BasisType, TypePolicy> rhs)
    noexcept(policy_is_nothrow_arith<Policy, BasisType>())
{
    return sub_helper<Policy, BasisType>::apply(lhs, rhs);
}

template <fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto operator-(const unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                       const unsigned_integer_basis<BasisType, ErrorPolicy> rhs)
    noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> unsigned_integer_basis<BasisType, ErrorPolicy>
{
    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)

    if constexpr (policy_equals<ErrorPolicy>(overflow_policy::throw_exception))
    {
        if (std::is_constant_evaluated())
        {
            BasisType res {};
            if (impl::unsigned_no_intrin_sub(static_cast<BasisType>(lhs), static_cast<BasisType>(rhs), res))
            {
                if constexpr (std::is_same_v<BasisType, std::uint8_t>)
                {
                    throw std::underflow_error("Underflow detected in u8 subtraction"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::uint16_t>)
                {
                    throw std::underflow_error("Underflow detected in u16 subtraction"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::uint32_t>)
                {
                    throw std::underflow_error("Underflow detected in u32 subtraction"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::uint64_t>)
                {
                    throw std::underflow_error("Underflow detected in u64 subtraction"); // LCOV_EXCL_LINE
                }
                else
                {
                    throw std::underflow_error("Underflow detected in u128 subtraction"); // LCOV_EXCL_LINE
                }
            }

            return unsigned_integer_basis<BasisType, ErrorPolicy>{res};
        }
    }

    #endif

    return sub_helper<ErrorPolicy, BasisType>::apply(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("subtraction", operator-)

template <fundamental_unsigned_integral BasisType, auto ErrorPolicy>
template <fundamental_unsigned_integral OtherBasisType, auto OtherPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr auto unsigned_integer_basis<BasisType, ErrorPolicy>::operator-=(const unsigned_integer_basis<OtherBasisType, OtherPolicy> rhs)
    noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> unsigned_integer_basis&
{
    *this = *this - rhs;
    return *this;
}

// ------------------------------
// Multiplication
// ------------------------------

namespace impl {

#if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_mul_overflow) && !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)

template <std::unsigned_integral T>
bool unsigned_intrin_mul(const T lhs, const T rhs, T& result)
{
    return __builtin_mul_overflow(lhs, rhs, &result);
}

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128

inline bool unsigned_intrin_mul(const int128::uint128_t lhs, const int128::uint128_t rhs, int128::uint128_t& result)
{
    __uint128_t builtin_result;
    const auto overflow {__builtin_mul_overflow(static_cast<__uint128_t>(lhs), static_cast<__uint128_t>(rhs), &builtin_result)};
    result = builtin_result;
    return overflow;
}

#endif

#elif defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) && !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)

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

#elif defined(BOOST_SAFENUMBERS_HAS_WINDOWS_ARM64_INTRIN) && !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)

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
template <auto Policy, fundamental_unsigned_integral BasisType>
struct mul_helper
{
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                              const unsigned_integer_basis<BasisType, TypePolicy> rhs)
        noexcept(policy_is_nothrow_arith<Policy, BasisType>())
        -> unsigned_integer_basis<BasisType, TypePolicy>
    {
        using result_type = unsigned_integer_basis<BasisType, TypePolicy>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType res {};

        auto handle_overflow = [&res]
        {
            #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
            // Saturation produces a defined value, so it must keep evaluating at
            // constant evaluation time instead of failing the build with a throw
            if (std::is_constant_evaluated() && !policy_equals<Policy>(overflow_policy::saturate) && !is_user_handler_v<Policy>)
            {
                if constexpr (std::is_same_v<BasisType, std::uint8_t>)
                {
                    throw std::overflow_error("Overflow detected in u8 multiplication"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::uint16_t>)
                {
                    throw std::overflow_error("Overflow detected in u16 multiplication"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::uint32_t>)
                {
                    throw std::overflow_error("Overflow detected in u32 multiplication"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::uint64_t>)
                {
                    throw std::overflow_error("Overflow detected in u64 multiplication"); // LCOV_EXCL_LINE
                }
                else
                {
                    throw std::overflow_error("Overflow detected in u128 multiplication"); // LCOV_EXCL_LINE
                }
            }
            else
            #endif
            {
                if constexpr (policy_equals<Policy>(overflow_policy::throw_exception))
                {
                    static_cast<void>(res);
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, overflow_mul_msg<BasisType>());
                }
                else if constexpr (policy_equals<Policy>(overflow_policy::saturate))
                {
                    res = std::numeric_limits<BasisType>::max();
                }
                else if constexpr (policy_equals<Policy>(overflow_policy::strict))
                {
                    static_cast<void>(res);
                    std::exit(EXIT_FAILURE);
                }
                else if constexpr (is_user_handler_v<Policy>)
                {
                    res = Policy.on_error(error_kind::overflow, res, overflow_mul_msg<BasisType>());
                }
                else
                {
                    static_cast<void>(res);
                    BOOST_SAFE_NUMBERS_UNREACHABLE;
                }
            }
        };

        // We have an intrin path, but only with __builtin_mul_overflow
        #if BOOST_SAFE_NUMBERS_HAS_BUILTIN(_umul128) || !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128)
        if constexpr (!std::is_same_v<BasisType, int128::uint128_t>)
        #endif
        {
            #if (BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_mul_overflow) || BOOST_SAFE_NUMBERS_HAS_BUILTIN(_umul128)) && !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)

            if (!std::is_constant_evaluated())
            {
                const bool overflowed {impl::unsigned_intrin_mul(lhs_basis, rhs_basis, res)};

                // A constant multiplication that overflows is a build error under the error
                // policies (throw_exception, strict); value policies are excluded.
                #ifdef BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT
                if constexpr (policy_equals<Policy>(overflow_policy::throw_exception) || policy_equals<Policy>(overflow_policy::strict))
                {
                    BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(!overflowed, "unsigned multiplication overflow");
                }
                #endif

                if (overflowed)
                {
                    handle_overflow();
                }

                return result_type{res};
            }

            #endif
        }

        const bool overflowed {impl::no_intrin_mul(lhs_basis, rhs_basis, res)};

        #ifdef BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT
        if constexpr (policy_equals<Policy>(overflow_policy::throw_exception) || policy_equals<Policy>(overflow_policy::strict))
        {
            BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(!overflowed, "unsigned multiplication overflow");
        }
        #endif

        if (overflowed)
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
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                              const unsigned_integer_basis<BasisType, TypePolicy> rhs) noexcept
        -> std::pair<unsigned_integer_basis<BasisType, TypePolicy>, bool>
    {
        using result_type = unsigned_integer_basis<BasisType, TypePolicy>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType res {};

        // No overflow intrinsic here on purpose: the *.with.overflow intrinsics
        // block loop vectorization when the flag is consumed as a value, while
        // this compare form vectorizes and emits the same scalar code.
        const auto overflowed {impl::no_intrin_mul(lhs_basis, rhs_basis, res)};
        return std::make_pair(result_type{res}, overflowed);
    }
};

// Partial specialization for checked policy
template <fundamental_unsigned_integral BasisType>
struct mul_helper<overflow_policy::checked, BasisType>
{
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                              const unsigned_integer_basis<BasisType, TypePolicy> rhs) noexcept
        -> std::optional<unsigned_integer_basis<BasisType, TypePolicy>>
    {
        using result_type = unsigned_integer_basis<BasisType, TypePolicy>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType res {};

        // No overflow intrinsic here on purpose: the *.with.overflow intrinsics
        // block loop vectorization when the flag is consumed as a value, while
        // this compare form vectorizes and emits the same scalar code.
        const auto overflowed {impl::no_intrin_mul(lhs_basis, rhs_basis, res)};
        return overflowed ? std::nullopt : std::make_optional(result_type{res});
    }
};

// Partial specialization for widening policy
template <fundamental_unsigned_integral BasisType>
struct mul_helper<overflow_policy::widen, BasisType>
{
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                              const unsigned_integer_basis<BasisType, TypePolicy> rhs) noexcept
    {
        using promoted_type = promoted_type<BasisType>;
        static_assert(!std::is_same_v<promoted_type, bool>, "Widening policy with uint128_t is not supported");

        using result_type = unsigned_integer_basis<promoted_type, TypePolicy>;
        return result_type{static_cast<promoted_type>(static_cast<promoted_type>(static_cast<BasisType>(lhs)) * static_cast<BasisType>(rhs))};
    }
};

template <auto Policy, fundamental_unsigned_integral BasisType, auto TypePolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto mul_impl(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                      const unsigned_integer_basis<BasisType, TypePolicy> rhs)
    noexcept(policy_is_nothrow_arith<Policy, BasisType>())
{
    return mul_helper<Policy, BasisType>::apply(lhs, rhs);
}

template <fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto operator*(const unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                       const unsigned_integer_basis<BasisType, ErrorPolicy> rhs)
    noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> unsigned_integer_basis<BasisType, ErrorPolicy>
{
    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)

    if constexpr (policy_equals<ErrorPolicy>(overflow_policy::throw_exception))
    {
        if (std::is_constant_evaluated())
        {
            BasisType res {};
            if (impl::no_intrin_mul(static_cast<BasisType>(lhs), static_cast<BasisType>(rhs), res))
            {
                if constexpr (std::is_same_v<BasisType, std::uint8_t>)
                {
                    throw std::overflow_error("Overflow detected in u8 multiplication"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::uint16_t>)
                {
                    throw std::overflow_error("Overflow detected in u16 multiplication"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::uint32_t>)
                {
                    throw std::overflow_error("Overflow detected in u32 multiplication"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::uint64_t>)
                {
                    throw std::overflow_error("Overflow detected in u64 multiplication"); // LCOV_EXCL_LINE
                }
                else
                {
                    throw std::overflow_error("Overflow detected in u128 multiplication"); // LCOV_EXCL_LINE
                }
            }

            return unsigned_integer_basis<BasisType, ErrorPolicy>{res};
        }
    }

    #endif

    return mul_helper<ErrorPolicy, BasisType>::apply(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("multiplication", operator*)

template <fundamental_unsigned_integral BasisType, auto ErrorPolicy>
template <fundamental_unsigned_integral OtherBasisType, auto OtherPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr auto unsigned_integer_basis<BasisType, ErrorPolicy>::operator*=(const unsigned_integer_basis<OtherBasisType, OtherPolicy> rhs)
    noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> unsigned_integer_basis&
{
    *this = *this * rhs;
    return *this;
}

// ------------------------------
// Division
// ------------------------------

// Primary template for non-tuple policies
template <auto Policy, fundamental_unsigned_integral BasisType>
struct div_helper
{
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                              const unsigned_integer_basis<BasisType, TypePolicy> rhs)
        noexcept(policy_is_nothrow_div<Policy, BasisType>())
        -> unsigned_integer_basis<BasisType, TypePolicy>
    {
        using result_type = unsigned_integer_basis<BasisType, TypePolicy>;

        const auto divisor {static_cast<BasisType>(rhs)};

        // Divide-by-zero throws or exits under every policy that reaches this template
        // (throw_exception, saturate, strict), so a constant zero divisor is always a bug.
        #ifdef BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT
        if constexpr (!is_user_handler_v<Policy>)
        {
            BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(divisor != 0U, "unsigned division by zero");
        }
        #endif

        if (divisor == 0U) [[unlikely]]
        {
            if constexpr (policy_equals<Policy>(overflow_policy::throw_exception))
            {
                BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, div_by_zero_msg<BasisType>());
            }
            else if constexpr (policy_equals<Policy>(overflow_policy::saturate))
            {
                BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, div_by_zero_msg<BasisType>());
            }
            else if constexpr (policy_equals<Policy>(overflow_policy::strict))
            {
                std::exit(EXIT_FAILURE);
            }
            else if constexpr (is_user_handler_v<Policy>)
            {
                return result_type{Policy.on_error(error_kind::divide_by_zero, static_cast<BasisType>(lhs), div_by_zero_msg<BasisType>())};
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
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                              const unsigned_integer_basis<BasisType, TypePolicy> rhs)
        -> std::pair<unsigned_integer_basis<BasisType, TypePolicy>, bool>
    {
        using result_type = unsigned_integer_basis<BasisType, TypePolicy>;

        const auto divisor {static_cast<BasisType>(rhs)};

        // overflow_tuple divide-by-zero throws, so a constant zero divisor is a build error
        #ifdef BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT
        BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(divisor != 0U, "unsigned division by zero");
        #endif

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
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                              const unsigned_integer_basis<BasisType, TypePolicy> rhs) noexcept
        -> std::optional<unsigned_integer_basis<BasisType, TypePolicy>>
    {
        using result_type = unsigned_integer_basis<BasisType, TypePolicy>;

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

template <auto Policy, fundamental_unsigned_integral BasisType, auto TypePolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto div_impl(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                      const unsigned_integer_basis<BasisType, TypePolicy> rhs)
    noexcept(policy_equals<Policy>(overflow_policy::checked) || policy_is_nothrow_div<Policy, BasisType>())
{
    return div_helper<Policy, BasisType>::apply(lhs, rhs);
}

template <fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto operator/(const unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                       const unsigned_integer_basis<BasisType, ErrorPolicy> rhs)
    noexcept(policy_is_nothrow_div<ErrorPolicy, BasisType>()) -> unsigned_integer_basis<BasisType, ErrorPolicy>
{
    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)

    // Divide-by-zero throws under both throw_exception and saturate,
    // so both keep the clean constant evaluation diagnostic
    if constexpr (policy_equals<ErrorPolicy>(overflow_policy::throw_exception) || policy_equals<ErrorPolicy>(overflow_policy::saturate))
    {
        if (std::is_constant_evaluated())
        {
            const auto divisor {static_cast<BasisType>(rhs)};
            if (divisor == 0U) [[unlikely]]
            {
                throw std::domain_error("Unsigned division by zero"); // LCOV_EXCL_LINE
            }

            if constexpr (std::is_same_v<BasisType, std::uint8_t> || std::is_same_v<BasisType, std::uint16_t>)
            {
                return unsigned_integer_basis<BasisType, ErrorPolicy>{static_cast<BasisType>(static_cast<BasisType>(lhs) / divisor)};
            }
            else
            {
                return unsigned_integer_basis<BasisType, ErrorPolicy>{static_cast<BasisType>(lhs) / divisor};
            }
        }
    }

    #endif

    return div_helper<ErrorPolicy, BasisType>::apply(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("division", operator/)

template <fundamental_unsigned_integral BasisType, auto ErrorPolicy>
template <fundamental_unsigned_integral OtherBasisType, auto OtherPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr auto unsigned_integer_basis<BasisType, ErrorPolicy>::operator/=(const unsigned_integer_basis<OtherBasisType, OtherPolicy> rhs)
    noexcept(policy_is_nothrow_div<ErrorPolicy, BasisType>()) -> unsigned_integer_basis&
{
    *this = *this / rhs;
    return *this;
}

// ------------------------------
// Modulo
// ------------------------------

// Primary template for non-tuple policies
template <auto Policy, fundamental_unsigned_integral BasisType>
struct mod_helper
{
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                              const unsigned_integer_basis<BasisType, TypePolicy> rhs)
        noexcept(policy_is_nothrow_div<Policy, BasisType>())
        -> unsigned_integer_basis<BasisType, TypePolicy>
    {
        using result_type = unsigned_integer_basis<BasisType, TypePolicy>;

        const auto divisor {static_cast<BasisType>(rhs)};

        // Modulo-by-zero throws or exits under every policy that reaches this template
        // (throw_exception, saturate, strict), so a constant zero divisor is always a bug.
        #ifdef BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT
        if constexpr (!is_user_handler_v<Policy>)
        {
            BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(divisor != 0U, "unsigned modulo by zero");
        }
        #endif

        if (divisor == 0U) [[unlikely]]
        {
            if constexpr (policy_equals<Policy>(overflow_policy::throw_exception))
            {
                BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, mod_by_zero_msg<BasisType>());
            }
            else if constexpr (policy_equals<Policy>(overflow_policy::saturate))
            {
                BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, mod_by_zero_msg<BasisType>());
            }
            else if constexpr (policy_equals<Policy>(overflow_policy::strict))
            {
                std::exit(EXIT_FAILURE);
            }
            else if constexpr (is_user_handler_v<Policy>)
            {
                return result_type{Policy.on_error(error_kind::divide_by_zero, static_cast<BasisType>(lhs), mod_by_zero_msg<BasisType>())};
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
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                              const unsigned_integer_basis<BasisType, TypePolicy> rhs)
        -> std::pair<unsigned_integer_basis<BasisType, TypePolicy>, bool>
    {
        using result_type = unsigned_integer_basis<BasisType, TypePolicy>;

        const auto divisor {static_cast<BasisType>(rhs)};

        // overflow_tuple modulo-by-zero throws, so a constant zero divisor is a build error
        #ifdef BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT
        BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(divisor != 0U, "unsigned modulo by zero");
        #endif

        if (divisor == 0U) [[unlikely]]
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, mod_by_zero_msg<BasisType>());
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
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                              const unsigned_integer_basis<BasisType, TypePolicy> rhs) noexcept
        -> std::optional<unsigned_integer_basis<BasisType, TypePolicy>>
    {
        using result_type = unsigned_integer_basis<BasisType, TypePolicy>;

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

template <auto Policy, fundamental_unsigned_integral BasisType, auto TypePolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto mod_impl(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                      const unsigned_integer_basis<BasisType, TypePolicy> rhs)
    noexcept(policy_equals<Policy>(overflow_policy::checked) || policy_is_nothrow_div<Policy, BasisType>())
{
    return mod_helper<Policy, BasisType>::apply(lhs, rhs);
}

template <fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto operator%(const unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                       const unsigned_integer_basis<BasisType, ErrorPolicy> rhs)
    noexcept(policy_is_nothrow_div<ErrorPolicy, BasisType>()) -> unsigned_integer_basis<BasisType, ErrorPolicy>
{
    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)

    // Modulo-by-zero throws under both throw_exception and saturate,
    // so both keep the clean constant evaluation diagnostic
    if constexpr (policy_equals<ErrorPolicy>(overflow_policy::throw_exception) || policy_equals<ErrorPolicy>(overflow_policy::saturate))
    {
        if (std::is_constant_evaluated())
        {
            const auto divisor {static_cast<BasisType>(rhs)};
            if (divisor == 0U) [[unlikely]]
            {
                throw std::domain_error("Unsigned modulo by zero"); // LCOV_EXCL_LINE
            }

            if constexpr (std::is_same_v<BasisType, std::uint8_t> || std::is_same_v<BasisType, std::uint16_t>)
            {
                return unsigned_integer_basis<BasisType, ErrorPolicy>{static_cast<BasisType>(static_cast<BasisType>(lhs) % divisor)};
            }
            else
            {
                return unsigned_integer_basis<BasisType, ErrorPolicy>{static_cast<BasisType>(lhs) % divisor};
            }
        }
    }

    #endif

    return mod_helper<ErrorPolicy, BasisType>::apply(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("modulo", operator%)

template <fundamental_unsigned_integral BasisType, auto ErrorPolicy>
template <fundamental_unsigned_integral OtherBasisType, auto OtherPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr auto unsigned_integer_basis<BasisType, ErrorPolicy>::operator%=(const unsigned_integer_basis<OtherBasisType, OtherPolicy> rhs)
    noexcept(policy_is_nothrow_div<ErrorPolicy, BasisType>()) -> unsigned_integer_basis&
{
    *this = *this % rhs;
    return *this;
}

// ------------------------------
// Pre and post increment
// ------------------------------

template <fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr auto unsigned_integer_basis<BasisType, ErrorPolicy>::operator++()
    noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> unsigned_integer_basis&
{
    if (this->basis_ == std::numeric_limits<BasisType>::max()) [[unlikely]]
    {
        if constexpr (policy_equals<ErrorPolicy>(overflow_policy::throw_exception))
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, overflow_inc_msg<BasisType>());
        }
        else if constexpr (policy_equals<ErrorPolicy>(overflow_policy::saturate))
        {
            return *this;
        }
        else if constexpr (policy_equals<ErrorPolicy>(overflow_policy::strict))
        {
            std::exit(EXIT_FAILURE);
        }
        else
        {
            this->basis_ = ErrorPolicy.on_error(error_kind::overflow, std::numeric_limits<BasisType>::min(), overflow_inc_msg<BasisType>());
            return *this;
        }
    }

    ++this->basis_;
    return *this;
}

template <fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr auto unsigned_integer_basis<BasisType, ErrorPolicy>::operator++(int)
    noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> unsigned_integer_basis
{
    if (this->basis_ == std::numeric_limits<BasisType>::max()) [[unlikely]]
    {
        if constexpr (policy_equals<ErrorPolicy>(overflow_policy::throw_exception))
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, overflow_inc_msg<BasisType>());
        }
        else if constexpr (policy_equals<ErrorPolicy>(overflow_policy::saturate))
        {
            return *this;
        }
        else if constexpr (policy_equals<ErrorPolicy>(overflow_policy::strict))
        {
            std::exit(EXIT_FAILURE);
        }
        else
        {
            const auto temp {*this};
            this->basis_ = ErrorPolicy.on_error(error_kind::overflow, std::numeric_limits<BasisType>::min(), overflow_inc_msg<BasisType>());
            return temp;
        }
    }

    const auto temp {*this};
    ++this->basis_;
    return temp;
}

// ------------------------------
// Pre and post decrement
// ------------------------------

template <fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr auto unsigned_integer_basis<BasisType, ErrorPolicy>::operator--()
    noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> unsigned_integer_basis&
{
    if (this->basis_ == 0U) [[unlikely]]
    {
        if constexpr (policy_equals<ErrorPolicy>(overflow_policy::throw_exception))
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, underflow_dec_msg<BasisType>());
        }
        else if constexpr (policy_equals<ErrorPolicy>(overflow_policy::saturate))
        {
            return *this;
        }
        else if constexpr (policy_equals<ErrorPolicy>(overflow_policy::strict))
        {
            std::exit(EXIT_FAILURE);
        }
        else
        {
            this->basis_ = ErrorPolicy.on_error(error_kind::underflow, std::numeric_limits<BasisType>::max(), underflow_dec_msg<BasisType>());
            return *this;
        }
    }

    --this->basis_;
    return *this;
}

template <fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr auto unsigned_integer_basis<BasisType, ErrorPolicy>::operator--(int)
    noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> unsigned_integer_basis
{
    if (this->basis_ == 0U) [[unlikely]]
    {
        if constexpr (policy_equals<ErrorPolicy>(overflow_policy::throw_exception))
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, underflow_dec_msg<BasisType>());
        }
        else if constexpr (policy_equals<ErrorPolicy>(overflow_policy::saturate))
        {
            return *this;
        }
        else if constexpr (policy_equals<ErrorPolicy>(overflow_policy::strict))
        {
            std::exit(EXIT_FAILURE);
        }
        else
        {
            const auto temp {*this};
            this->basis_ = ErrorPolicy.on_error(error_kind::underflow, std::numeric_limits<BasisType>::max(), underflow_dec_msg<BasisType>());
            return temp;
        }
    }

    const auto temp {*this};
    --this->basis_;
    return temp;
}

// ------------------------------
// Left Shift
// ------------------------------

// Primary template for non-tuple policies
template <auto Policy, fundamental_unsigned_integral BasisType>
struct shl_helper
{
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                              const unsigned_integer_basis<BasisType, TypePolicy> rhs)
        noexcept(policy_is_nothrow_arith<Policy, BasisType>())
        -> unsigned_integer_basis<BasisType, TypePolicy>
    {
        using result_type = unsigned_integer_basis<BasisType, TypePolicy>;
        using core::bit_width;

        const auto raw_lhs {static_cast<BasisType>(lhs)};
        const auto raw_rhs {static_cast<BasisType>(rhs)};
        const auto lhs_width {static_cast<BasisType>(bit_width(raw_lhs))};
        const auto overflowed {lhs_width + raw_rhs >= static_cast<BasisType>(std::numeric_limits<BasisType>::digits)};

        // Left shift past the type width is an error under throw_exception and strict;
        // saturate/checked/overflow_tuple return a defined value, so they are excluded.
        #ifdef BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT
        if constexpr (policy_equals<Policy>(overflow_policy::throw_exception) || policy_equals<Policy>(overflow_policy::strict))
        {
            BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(!overflowed, "unsigned left shift past type width");
        }
        #endif

        if (overflowed)
        {
            if constexpr (policy_equals<Policy>(overflow_policy::throw_exception))
            {
                BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, left_shift_overflow_msg<BasisType>());
            }
            else if constexpr (policy_equals<Policy>(overflow_policy::saturate))
            {
                return result_type{std::numeric_limits<BasisType>::max()};
            }
            else if constexpr (policy_equals<Policy>(overflow_policy::strict))
            {
                std::exit(EXIT_FAILURE);
            }
            else if constexpr (is_user_handler_v<Policy>)
            {
                const auto wrapped {static_cast<BasisType>(raw_lhs << (raw_rhs % static_cast<BasisType>(std::numeric_limits<BasisType>::digits)))};
                return result_type{Policy.on_error(error_kind::overflow, wrapped, left_shift_overflow_msg<BasisType>())};
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
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                              const unsigned_integer_basis<BasisType, TypePolicy> rhs) noexcept
        -> std::pair<unsigned_integer_basis<BasisType, TypePolicy>, bool>
    {
        using result_type = unsigned_integer_basis<BasisType, TypePolicy>;
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
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                              const unsigned_integer_basis<BasisType, TypePolicy> rhs) noexcept
        -> std::optional<unsigned_integer_basis<BasisType, TypePolicy>>
    {
        using result_type = unsigned_integer_basis<BasisType, TypePolicy>;
        using core::bit_width;

        const auto raw_lhs {static_cast<BasisType>(lhs)};
        const auto raw_rhs {static_cast<BasisType>(rhs)};
        const auto lhs_width {static_cast<BasisType>(bit_width(raw_lhs))};
        const auto overflowed {lhs_width + raw_rhs >= static_cast<BasisType>(std::numeric_limits<BasisType>::digits)};

        return overflowed ? std::nullopt : std::make_optional(result_type{static_cast<BasisType>(raw_lhs << raw_rhs)});
    }
};

template <auto Policy, fundamental_unsigned_integral BasisType, auto TypePolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto shl_impl(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                      const unsigned_integer_basis<BasisType, TypePolicy> rhs)
    noexcept(policy_is_nothrow_arith<Policy, BasisType>())
{
    return shl_helper<Policy, BasisType>::apply(lhs, rhs);
}

// ------------------------------
// Right Shift
// ------------------------------

// Primary template for non-tuple policies
template <auto Policy, fundamental_unsigned_integral BasisType>
struct shr_helper
{
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                              const unsigned_integer_basis<BasisType, TypePolicy> rhs)
        noexcept(policy_is_nothrow_arith<Policy, BasisType>())
        -> unsigned_integer_basis<BasisType, TypePolicy>
    {
        using result_type = unsigned_integer_basis<BasisType, TypePolicy>;

        const auto raw_lhs {static_cast<BasisType>(lhs)};
        const auto raw_rhs {static_cast<BasisType>(rhs)};
        const auto overflowed {raw_rhs >= static_cast<BasisType>(std::numeric_limits<BasisType>::digits)};

        // Right shift past the type width is an error under throw_exception and strict;
        // saturate/checked/overflow_tuple return a defined value, so they are excluded.
        #ifdef BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT
        if constexpr (policy_equals<Policy>(overflow_policy::throw_exception) || policy_equals<Policy>(overflow_policy::strict))
        {
            BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(!overflowed, "unsigned right shift past type width");
        }
        #endif

        if (overflowed)
        {
            if constexpr (policy_equals<Policy>(overflow_policy::throw_exception))
            {
                BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, right_shift_overflow_msg<BasisType>());
            }
            else if constexpr (policy_equals<Policy>(overflow_policy::saturate))
            {
                return result_type{0U};
            }
            else if constexpr (policy_equals<Policy>(overflow_policy::strict))
            {
                std::exit(EXIT_FAILURE);
            }
            else if constexpr (is_user_handler_v<Policy>)
            {
                const auto wrapped {static_cast<BasisType>(raw_lhs >> (raw_rhs % static_cast<BasisType>(std::numeric_limits<BasisType>::digits)))};
                return result_type{Policy.on_error(error_kind::overflow, wrapped, right_shift_overflow_msg<BasisType>())};
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
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                              const unsigned_integer_basis<BasisType, TypePolicy> rhs) noexcept
        -> std::pair<unsigned_integer_basis<BasisType, TypePolicy>, bool>
    {
        using result_type = unsigned_integer_basis<BasisType, TypePolicy>;

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
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                              const unsigned_integer_basis<BasisType, TypePolicy> rhs) noexcept
        -> std::optional<unsigned_integer_basis<BasisType, TypePolicy>>
    {
        using result_type = unsigned_integer_basis<BasisType, TypePolicy>;

        const auto raw_lhs {static_cast<BasisType>(lhs)};
        const auto raw_rhs {static_cast<BasisType>(rhs)};
        const auto overflowed {raw_rhs >= static_cast<BasisType>(std::numeric_limits<BasisType>::digits)};

        return overflowed ? std::nullopt : std::make_optional(result_type{static_cast<BasisType>(raw_lhs >> raw_rhs)});
    }
};

template <auto Policy, fundamental_unsigned_integral BasisType, auto TypePolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto shr_impl(const unsigned_integer_basis<BasisType, TypePolicy> lhs,
                                      const unsigned_integer_basis<BasisType, TypePolicy> rhs)
    noexcept(policy_is_nothrow_arith<Policy, BasisType>())
{
    return shr_helper<Policy, BasisType>::apply(lhs, rhs);
}

} // namespace boost::safe_numbers::detail

// ------------------------------
// Saturating Math
// ------------------------------

namespace boost::safe_numbers {

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto saturating_add(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                            const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType, ErrorPolicy>
{
    return detail::add_impl<overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("saturating addition", saturating_add)

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto saturating_sub(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                            const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType, ErrorPolicy>
{
    return detail::sub_impl<overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("saturating subtraction", saturating_sub)

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto saturating_mul(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                            const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType, ErrorPolicy>
{
    return detail::mul_impl<overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("saturating multiplication", saturating_mul)

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto saturating_div(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                            const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs)
    -> detail::unsigned_integer_basis<BasisType, ErrorPolicy>
{
    return detail::div_impl<overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("saturating division", saturating_div)

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto saturating_mod(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                            const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs)
    -> detail::unsigned_integer_basis<BasisType, ErrorPolicy>
{
    return detail::mod_impl<overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("saturating modulo", saturating_mod)

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto overflowing_add(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                             const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> std::pair<detail::unsigned_integer_basis<BasisType, ErrorPolicy>, bool>
{
    return detail::add_impl<overflow_policy::overflow_tuple>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("overflowing addition", overflowing_add)

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto overflowing_sub(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                             const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> std::pair<detail::unsigned_integer_basis<BasisType, ErrorPolicy>, bool>
{
    return detail::sub_impl<overflow_policy::overflow_tuple>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("overflowing subtraction", overflowing_sub)

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto overflowing_mul(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                             const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> std::pair<detail::unsigned_integer_basis<BasisType, ErrorPolicy>, bool>
{
    return detail::mul_impl<overflow_policy::overflow_tuple>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("overflowing multiplication", overflowing_mul)

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto overflowing_div(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                             const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs)
    -> std::pair<detail::unsigned_integer_basis<BasisType, ErrorPolicy>, bool>
{
    return detail::div_impl<overflow_policy::overflow_tuple>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("overflowing division", overflowing_div)

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto overflowing_mod(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                             const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs)
    -> std::pair<detail::unsigned_integer_basis<BasisType, ErrorPolicy>, bool>
{
    return detail::mod_impl<overflow_policy::overflow_tuple>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("overflowing modulo", overflowing_mod)

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto checked_add(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                         const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> std::optional<detail::unsigned_integer_basis<BasisType, ErrorPolicy>>
{
    return detail::add_impl<overflow_policy::checked>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("checked addition", checked_add)

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto checked_sub(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                         const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> std::optional<detail::unsigned_integer_basis<BasisType, ErrorPolicy>>
{
    return detail::sub_impl<overflow_policy::checked>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("checked subtraction", checked_sub)

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto checked_mul(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                         const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> std::optional<detail::unsigned_integer_basis<BasisType, ErrorPolicy>>
{
    return detail::mul_impl<overflow_policy::checked>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("checked multiplication", checked_mul)

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto checked_div(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                         const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> std::optional<detail::unsigned_integer_basis<BasisType, ErrorPolicy>>
{
    return detail::div_impl<overflow_policy::checked>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("checked division", checked_div)

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto checked_mod(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                         const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> std::optional<detail::unsigned_integer_basis<BasisType, ErrorPolicy>>
{
    return detail::mod_impl<overflow_policy::checked>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("checked modulo", checked_mod)

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto strict_add(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                        const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType, ErrorPolicy>
{
    return detail::add_impl<overflow_policy::strict>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("strict addition", strict_add)

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto strict_sub(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                        const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType, ErrorPolicy>
{
    return detail::sub_impl<overflow_policy::strict>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("strict subtraction", strict_sub)

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto strict_mul(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                        const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType, ErrorPolicy>
{
    return detail::mul_impl<overflow_policy::strict>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("strict multiplication", strict_mul)

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto strict_div(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                        const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType, ErrorPolicy>
{
    return detail::div_impl<overflow_policy::strict>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("strict division", strict_div)

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto strict_mod(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                        const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType, ErrorPolicy>
{
    return detail::mod_impl<overflow_policy::strict>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("strict modulo", strict_mod)

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto widening_add(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                          const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
{
    return detail::add_impl<overflow_policy::widen>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("widening add", widening_add)

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto widening_mul(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                          const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
{
    return detail::mul_impl<overflow_policy::widen>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("widening mul", widening_mul)

// ------------------------------
// Saturating Shift
// ------------------------------

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto saturating_shl(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                            const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType, ErrorPolicy>
{
    return detail::shl_impl<overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("saturating left shift", saturating_shl)

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto saturating_shr(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                            const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType, ErrorPolicy>
{
    return detail::shr_impl<overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("saturating right shift", saturating_shr)

// ------------------------------
// Overflowing Shift
// ------------------------------

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto overflowing_shl(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                             const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> std::pair<detail::unsigned_integer_basis<BasisType, ErrorPolicy>, bool>
{
    return detail::shl_impl<overflow_policy::overflow_tuple>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("overflowing left shift", overflowing_shl)

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto overflowing_shr(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                             const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> std::pair<detail::unsigned_integer_basis<BasisType, ErrorPolicy>, bool>
{
    return detail::shr_impl<overflow_policy::overflow_tuple>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("overflowing right shift", overflowing_shr)

// ------------------------------
// Checked Shift
// ------------------------------

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto checked_shl(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                         const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> std::optional<detail::unsigned_integer_basis<BasisType, ErrorPolicy>>
{
    return detail::shl_impl<overflow_policy::checked>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("checked left shift", checked_shl)

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto checked_shr(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                         const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> std::optional<detail::unsigned_integer_basis<BasisType, ErrorPolicy>>
{
    return detail::shr_impl<overflow_policy::checked>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("checked right shift", checked_shr)

// ------------------------------
// Strict Shift
// ------------------------------

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto strict_shl(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                        const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType, ErrorPolicy>
{
    return detail::shl_impl<overflow_policy::strict>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("strict left shift", strict_shl)

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto strict_shr(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                        const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> detail::unsigned_integer_basis<BasisType, ErrorPolicy>
{
    return detail::shr_impl<overflow_policy::strict>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP("strict right shift", strict_shr)

// ------------------------------
// Generic policy-parameterized functions
// ------------------------------

template <overflow_policy Policy, detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto add(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                 const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs)
    noexcept(Policy != overflow_policy::throw_exception)
{
    if constexpr (Policy == overflow_policy::throw_exception)
    {
        // Operand types with a non-throwing type-level policy must not fall back to
        // their own operator, so route them straight to the throwing implementation
        if constexpr (detail::policy_equals<ErrorPolicy>(overflow_policy::throw_exception))
        {
            return lhs + rhs;
        }
        else
        {
            return detail::add_impl<overflow_policy::throw_exception>(lhs, rhs);
        }
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

template <overflow_policy Policy, detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto sub(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                 const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs)
    noexcept(Policy != overflow_policy::throw_exception)
{
    if constexpr (Policy == overflow_policy::throw_exception)
    {
        if constexpr (detail::policy_equals<ErrorPolicy>(overflow_policy::throw_exception))
        {
            return lhs - rhs;
        }
        else
        {
            return detail::sub_impl<overflow_policy::throw_exception>(lhs, rhs);
        }
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

template <overflow_policy Policy, detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto mul(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                 const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs)
    noexcept(Policy != overflow_policy::throw_exception)
{
    if constexpr (Policy == overflow_policy::throw_exception)
    {
        if constexpr (detail::policy_equals<ErrorPolicy>(overflow_policy::throw_exception))
        {
            return lhs * rhs;
        }
        else
        {
            return detail::mul_impl<overflow_policy::throw_exception>(lhs, rhs);
        }
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

template <overflow_policy Policy, detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto div(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                 const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs)
    noexcept(Policy == overflow_policy::checked || Policy == overflow_policy::strict)
{
    if constexpr (Policy == overflow_policy::throw_exception)
    {
        if constexpr (detail::policy_equals<ErrorPolicy>(overflow_policy::throw_exception))
        {
            return lhs / rhs;
        }
        else
        {
            return detail::div_impl<overflow_policy::throw_exception>(lhs, rhs);
        }
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

template <overflow_policy Policy, detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto mod(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                 const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs)
    noexcept(Policy == overflow_policy::checked || Policy == overflow_policy::strict)
{
    if constexpr (Policy == overflow_policy::throw_exception)
    {
        if constexpr (detail::policy_equals<ErrorPolicy>(overflow_policy::throw_exception))
        {
            return lhs % rhs;
        }
        else
        {
            return detail::mod_impl<overflow_policy::throw_exception>(lhs, rhs);
        }
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

template <overflow_policy Policy, detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto shl(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                 const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs)
    noexcept(Policy != overflow_policy::throw_exception)
{
    return detail::shl_impl<Policy>(lhs, rhs);
}

template <overflow_policy Policy, detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto shr(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                                 const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs)
    noexcept(Policy != overflow_policy::throw_exception)
{
    return detail::shr_impl<Policy>(lhs, rhs);
}

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
constexpr auto operator~(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs) noexcept
{
    using return_type = detail::unsigned_integer_basis<BasisType, ErrorPolicy>;
    return return_type{static_cast<BasisType>(~detail::raw_value(lhs))};
}

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
constexpr auto operator&(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                         const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
{
    using return_type = detail::unsigned_integer_basis<BasisType, ErrorPolicy>;
    return return_type{static_cast<BasisType>(detail::raw_value(lhs) & detail::raw_value(rhs))};
}

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
constexpr auto operator|(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                         const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
{
    using return_type = detail::unsigned_integer_basis<BasisType, ErrorPolicy>;
    return return_type{static_cast<BasisType>(detail::raw_value(lhs) | detail::raw_value(rhs))};
}

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
constexpr auto operator^(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                         const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
{
    using return_type = detail::unsigned_integer_basis<BasisType, ErrorPolicy>;
    return return_type{static_cast<BasisType>(detail::raw_value(lhs) ^ detail::raw_value(rhs))};
}

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
constexpr auto operator<<(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                          const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs)
    noexcept(detail::policy_is_nothrow_arith<ErrorPolicy, BasisType>())
{
    return detail::shl_impl<ErrorPolicy>(lhs, rhs);
}

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
constexpr auto operator>>(const detail::unsigned_integer_basis<BasisType, ErrorPolicy> lhs,
                          const detail::unsigned_integer_basis<BasisType, ErrorPolicy> rhs)
    noexcept(detail::policy_is_nothrow_arith<ErrorPolicy, BasisType>())
{
    return detail::shr_impl<ErrorPolicy>(lhs, rhs);
}

// ------------------------------
// Compound bitwise operators
// ------------------------------

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
constexpr auto detail::unsigned_integer_basis<BasisType, ErrorPolicy>::operator&=(const unsigned_integer_basis rhs) noexcept
    -> unsigned_integer_basis&
{
    *this = boost::safe_numbers::operator&(*this, rhs);
    return *this;
}

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
constexpr auto detail::unsigned_integer_basis<BasisType, ErrorPolicy>::operator|=(const unsigned_integer_basis rhs) noexcept
    -> unsigned_integer_basis&
{
    *this = boost::safe_numbers::operator|(*this, rhs);
    return *this;
}

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
constexpr auto detail::unsigned_integer_basis<BasisType, ErrorPolicy>::operator^=(const unsigned_integer_basis rhs) noexcept
    -> unsigned_integer_basis&
{
    *this = boost::safe_numbers::operator^(*this, rhs);
    return *this;
}

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
constexpr auto detail::unsigned_integer_basis<BasisType, ErrorPolicy>::operator<<=(const unsigned_integer_basis rhs)
    noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> unsigned_integer_basis&
{
    *this = boost::safe_numbers::operator<<(*this, rhs);
    return *this;
}

template <detail::fundamental_unsigned_integral BasisType, auto ErrorPolicy>
constexpr auto detail::unsigned_integer_basis<BasisType, ErrorPolicy>::operator>>=(const unsigned_integer_basis rhs)
    noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> unsigned_integer_basis&
{
    *this = boost::safe_numbers::operator>>(*this, rhs);
    return *this;
}

} // namespace boost::safe_numbers

#undef BOOST_SAFE_NUMBERS_DEFINE_MIXED_UNSIGNED_INTEGER_OP

#endif // BOOST_SAFE_NUMBERS_DETAIL_UNSIGNED_INTEGER_BASIS_HPP
// ===== END boost/safe_numbers/detail/unsigned_integer_basis.hpp =====
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/int128.hpp

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <cstdint>

#endif

namespace boost::safe_numbers {

BOOST_SAFE_NUMBERS_EXPORT using u8 = detail::unsigned_integer_basis<std::uint8_t>;

BOOST_SAFE_NUMBERS_EXPORT using u16 = detail::unsigned_integer_basis<std::uint16_t>;

BOOST_SAFE_NUMBERS_EXPORT using u32 = detail::unsigned_integer_basis<std::uint32_t>;

BOOST_SAFE_NUMBERS_EXPORT using u64 = detail::unsigned_integer_basis<std::uint64_t>;

BOOST_SAFE_NUMBERS_EXPORT using u128 = detail::unsigned_integer_basis<int128::uint128_t>;

// Saturating counterparts: overflow clamps to the numeric limits instead of throwing.
// Division and modulo by zero still throw std::domain_error.

BOOST_SAFE_NUMBERS_EXPORT using sat_u8 = detail::unsigned_integer_basis<std::uint8_t, overflow_policy::saturate>;

BOOST_SAFE_NUMBERS_EXPORT using sat_u16 = detail::unsigned_integer_basis<std::uint16_t, overflow_policy::saturate>;

BOOST_SAFE_NUMBERS_EXPORT using sat_u32 = detail::unsigned_integer_basis<std::uint32_t, overflow_policy::saturate>;

BOOST_SAFE_NUMBERS_EXPORT using sat_u64 = detail::unsigned_integer_basis<std::uint64_t, overflow_policy::saturate>;

BOOST_SAFE_NUMBERS_EXPORT using sat_u128 = detail::unsigned_integer_basis<int128::uint128_t, overflow_policy::saturate>;

// Strict counterparts: any error terminates via std::exit(EXIT_FAILURE). Host only.

BOOST_SAFE_NUMBERS_EXPORT using strict_u8 = detail::unsigned_integer_basis<std::uint8_t, overflow_policy::strict>;

BOOST_SAFE_NUMBERS_EXPORT using strict_u16 = detail::unsigned_integer_basis<std::uint16_t, overflow_policy::strict>;

BOOST_SAFE_NUMBERS_EXPORT using strict_u32 = detail::unsigned_integer_basis<std::uint32_t, overflow_policy::strict>;

BOOST_SAFE_NUMBERS_EXPORT using strict_u64 = detail::unsigned_integer_basis<std::uint64_t, overflow_policy::strict>;

BOOST_SAFE_NUMBERS_EXPORT using strict_u128 = detail::unsigned_integer_basis<int128::uint128_t, overflow_policy::strict>;

// Width-named alias templates selecting the policy by type: basic_u8<> is u8,
// basic_u8<saturating> is sat_u8, and basic_u8<my_handler> carries a user
// defined handler (see the error_handler_for concept).

BOOST_SAFE_NUMBERS_EXPORT template <typename ErrorHandler = throwing>
using basic_u8 = detail::unsigned_integer_basis<std::uint8_t, detail::type_policy_v<ErrorHandler>>;

BOOST_SAFE_NUMBERS_EXPORT template <typename ErrorHandler = throwing>
using basic_u16 = detail::unsigned_integer_basis<std::uint16_t, detail::type_policy_v<ErrorHandler>>;

BOOST_SAFE_NUMBERS_EXPORT template <typename ErrorHandler = throwing>
using basic_u32 = detail::unsigned_integer_basis<std::uint32_t, detail::type_policy_v<ErrorHandler>>;

BOOST_SAFE_NUMBERS_EXPORT template <typename ErrorHandler = throwing>
using basic_u64 = detail::unsigned_integer_basis<std::uint64_t, detail::type_policy_v<ErrorHandler>>;

BOOST_SAFE_NUMBERS_EXPORT template <typename ErrorHandler = throwing>
using basic_u128 = detail::unsigned_integer_basis<int128::uint128_t, detail::type_policy_v<ErrorHandler>>;

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_UNSIGNED_INTEGERS_HPP
// ===== END boost/safe_numbers/unsigned_integers.hpp =====
// ===== BEGIN boost/safe_numbers/signed_integers.hpp =====
// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_SIGNED_INTEGERS_HPP
#define BOOST_SAFE_NUMBERS_SIGNED_INTEGERS_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/config.hpp
// ===== BEGIN boost/safe_numbers/detail/signed_integer_basis.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_SIGNED_INTEGER_BASIS_HPP
#define BOOST_SAFE_NUMBERS_SIGNED_INTEGER_BASIS_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/config.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/type_traits.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/compile_assert.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/bit.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/throw_exception.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/overflow_policy.hpp

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

template <fundamental_signed_integral BasisType, auto ErrorPolicy>
class signed_integer_basis
{
public:

    // This is exposed to the user so that they can convert back to built-in
    using basis_type = BasisType;

    static_assert(is_overflow_policy_v<ErrorPolicy> || error_handler_for<decltype(ErrorPolicy), BasisType>,
                  "ErrorPolicy must be a boost::safe_numbers::overflow_policy enumerator or a stateless "
                  "handler type providing on_error(error_kind, BasisType, const char*) returning BasisType");

    static_assert(!is_value_returning_policy<ErrorPolicy>(),
                  "overflow_tuple, checked, and widen change the result type of every operation, "
                  "so they can not be type-level policies: use the overflowing_*, checked_*, "
                  "and widening_* free functions instead");

    static_assert(!is_overflow_policy_v<ErrorPolicy> || is_valid_type_policy<ErrorPolicy>(basis_kind::signed_integer),
                  "signed_integer_basis supports overflow_policy::throw_exception, "
                  "overflow_policy::saturate, and overflow_policy::strict");

    // Exposed so that generic code and tests can query the type-level policy
    static constexpr auto error_policy {ErrorPolicy};

private:

    BasisType basis_ {0U};

public:

    constexpr signed_integer_basis() noexcept = default;

    BOOST_SAFE_NUMBERS_HOST_DEVICE explicit constexpr signed_integer_basis(const BasisType val) noexcept : basis_{val} {}

    template <typename T>
        requires std::is_same_v<T, bool>
    BOOST_SAFE_NUMBERS_HOST_DEVICE explicit constexpr signed_integer_basis(T) noexcept
    {
        static_assert(dependent_false<T>, "Construction from bool is not allowed");
    }

    template <fundamental_signed_integral OtherBasis>
    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] explicit constexpr operator OtherBasis() const;

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] explicit constexpr operator BasisType() const noexcept { return basis_; }

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] friend constexpr auto operator<=>(signed_integer_basis lhs, signed_integer_basis rhs) noexcept
        -> std::strong_ordering = default;

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto operator+() const noexcept -> signed_integer_basis;

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto operator-() const
        noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> signed_integer_basis;

    template <fundamental_signed_integral OtherBasis, auto OtherPolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator+=(signed_integer_basis<OtherBasis, OtherPolicy> rhs)
        noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> signed_integer_basis&;

    template <fundamental_signed_integral OtherBasis, auto OtherPolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator-=(signed_integer_basis<OtherBasis, OtherPolicy> rhs)
        noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> signed_integer_basis&;

    template <fundamental_signed_integral OtherBasis, auto OtherPolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator*=(signed_integer_basis<OtherBasis, OtherPolicy> rhs)
        noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> signed_integer_basis&;

    template <fundamental_signed_integral OtherBasis, auto OtherPolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator/=(signed_integer_basis<OtherBasis, OtherPolicy> rhs)
        noexcept(policy_is_nothrow_div<ErrorPolicy, BasisType>()) -> signed_integer_basis&;

    template <fundamental_signed_integral OtherBasis, auto OtherPolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator%=(signed_integer_basis<OtherBasis, OtherPolicy> rhs)
        noexcept(policy_is_nothrow_div<ErrorPolicy, BasisType>()) -> signed_integer_basis&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator&=(signed_integer_basis) -> signed_integer_basis&
    {
        static_assert(dependent_false<BasisType>, "Bitwise AND is deliberately disabled for signed safe integers (see Ada)");
        return *this; // LCOV_EXCL_LINE : deliberately unreachable
    }

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator|=(signed_integer_basis) -> signed_integer_basis&
    {
        static_assert(dependent_false<BasisType>, "Bitwise OR is deliberately disabled for signed safe integers (see Ada)");
        return *this; // LCOV_EXCL_LINE : deliberately unreachable
    }

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator^=(signed_integer_basis) -> signed_integer_basis&
    {
        static_assert(dependent_false<BasisType>, "Bitwise XOR is deliberately disabled for signed safe integers (see Ada)");
        return *this; // LCOV_EXCL_LINE : deliberately unreachable
    }

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator<<=(signed_integer_basis) -> signed_integer_basis&
    {
        static_assert(dependent_false<BasisType>, "Left shift is deliberately disabled for signed safe integers (see Ada)");
        return *this; // LCOV_EXCL_LINE : deliberately unreachable
    }

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator>>=(signed_integer_basis) -> signed_integer_basis&
    {
        static_assert(dependent_false<BasisType>, "Right shift is deliberately disabled for signed safe integers (see Ada)");
        return *this; // LCOV_EXCL_LINE : deliberately unreachable
    }

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator++()
        noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> signed_integer_basis&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator++(int)
        noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> signed_integer_basis;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator--()
        noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> signed_integer_basis&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator--(int)
        noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> signed_integer_basis;
};

// Helper for diagnostic messages
template <typename BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto signed_type_name() noexcept -> const char*
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
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto signed_overflow_add_msg() noexcept -> const char*
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
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto signed_underflow_add_msg() noexcept -> const char*
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
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto signed_unary_minus_overflow_msg() noexcept -> const char*
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
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto signed_overflow_conversion_msg() noexcept -> const char*
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
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto signed_underflow_conversion_msg() noexcept -> const char*
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

template <fundamental_signed_integral BasisType, auto ErrorPolicy>
template <fundamental_signed_integral OtherBasis>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr signed_integer_basis<BasisType, ErrorPolicy>::operator OtherBasis() const
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

template <fundamental_signed_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto signed_integer_basis<BasisType, ErrorPolicy>::operator+() const noexcept -> signed_integer_basis
{
    return signed_integer_basis{basis_};
}

template <fundamental_signed_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto signed_integer_basis<BasisType, ErrorPolicy>::operator-() const
    noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> signed_integer_basis
{
    if (basis_ == std::numeric_limits<BasisType>::min()) [[unlikely]]
    {
        if constexpr (policy_equals<ErrorPolicy>(overflow_policy::throw_exception))
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, signed_unary_minus_overflow_msg<BasisType>());
        }
        else if constexpr (policy_equals<ErrorPolicy>(overflow_policy::saturate))
        {
            return signed_integer_basis{std::numeric_limits<BasisType>::max()};
        }
        else if constexpr (policy_equals<ErrorPolicy>(overflow_policy::strict))
        {
            std::exit(EXIT_FAILURE);
        }
        else
        {
            return signed_integer_basis{ErrorPolicy.on_error(error_kind::overflow, std::numeric_limits<BasisType>::min(), signed_unary_minus_overflow_msg<BasisType>())};
        }
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
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto classify_signed_overflow(const T lhs) noexcept -> signed_overflow_status
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
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto signed_no_intrin_add(const T lhs, const T rhs, T& result) noexcept -> signed_overflow_status
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

template <auto Policy, fundamental_signed_integral BasisType>
struct signed_add_helper
{
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType, TypePolicy> lhs,
                                              const signed_integer_basis<BasisType, TypePolicy> rhs)
        noexcept(policy_is_nothrow_arith<Policy, BasisType>())
        -> signed_integer_basis<BasisType, TypePolicy>
    {
        using result_type = signed_integer_basis<BasisType, TypePolicy>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType result {};

        auto handle_error = [&result](signed_overflow_status status)
        {
            #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
            // Saturation produces a defined value, so it must keep evaluating at
            // constant evaluation time instead of failing the build with a throw
            if (std::is_constant_evaluated() && !policy_equals<Policy>(overflow_policy::saturate) && !is_user_handler_v<Policy>)
            {
                if (status == signed_overflow_status::overflow)
                {
                    if constexpr (std::is_same_v<BasisType, std::int8_t>)
                    {
                        throw std::overflow_error("Overflow detected in i8 addition"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
                    {
                        throw std::overflow_error("Overflow detected in i16 addition"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
                    {
                        throw std::overflow_error("Overflow detected in i32 addition"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
                    {
                        throw std::overflow_error("Overflow detected in i64 addition"); // LCOV_EXCL_LINE
                    }
                    else
                    {
                        throw std::overflow_error("Overflow detected in i128 addition"); // LCOV_EXCL_LINE
                    }
                }
                else
                {
                    if constexpr (std::is_same_v<BasisType, std::int8_t>)
                    {
                        throw std::underflow_error("Underflow detected in i8 addition"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
                    {
                        throw std::underflow_error("Underflow detected in i16 addition"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
                    {
                        throw std::underflow_error("Underflow detected in i32 addition"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
                    {
                        throw std::underflow_error("Underflow detected in i64 addition"); // LCOV_EXCL_LINE
                    }
                    else
                    {
                        throw std::underflow_error("Underflow detected in i128 addition"); // LCOV_EXCL_LINE
                    }
                }
            }
            else
            #endif
            {
                if constexpr (policy_equals<Policy>(overflow_policy::throw_exception))
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
                else if constexpr (policy_equals<Policy>(overflow_policy::saturate))
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
                else if constexpr (policy_equals<Policy>(overflow_policy::strict))
                {
                    static_cast<void>(result);
                    std::exit(EXIT_FAILURE);
                }
                else if constexpr (is_user_handler_v<Policy>)
                {
                    if (status == signed_overflow_status::overflow)
                    {
                        result = Policy.on_error(error_kind::overflow, result, signed_overflow_add_msg<BasisType>());
                    }
                    else
                    {
                        result = Policy.on_error(error_kind::underflow, result, signed_underflow_add_msg<BasisType>());
                    }
                }
                else
                {
                    static_cast<void>(result);
                    BOOST_SAFE_NUMBERS_UNREACHABLE;
                }
            }
        };

        // A constant operation that overflows is a build error under the error policies
        // (throw_exception, strict); saturate/checked/overflow_tuple return a defined value
        // and are excluded. The assert reuses the status the runtime path already computes.
        #if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_add_overflow) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

        if constexpr (!std::is_same_v<BasisType, int128::int128_t>)
        {
            #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)

            if (!std::is_constant_evaluated())
            {
                const auto status {impl::signed_intrin_add(lhs_basis, rhs_basis, result)};

                #ifdef BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT
                if constexpr (policy_equals<Policy>(overflow_policy::throw_exception) || policy_equals<Policy>(overflow_policy::strict))
                {
                    BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(status == signed_overflow_status::no_error, "signed addition overflow");
                }
                #endif

                if (status != signed_overflow_status::no_error)
                {
                    handle_error(status);
                }

                return result_type{result};
            }

            #endif
        }

        #endif // BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_add_overflow) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

        const auto status {impl::signed_no_intrin_add(lhs_basis, rhs_basis, result)};

        #ifdef BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT
        if constexpr (policy_equals<Policy>(overflow_policy::throw_exception) || policy_equals<Policy>(overflow_policy::strict))
        {
            BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(status == signed_overflow_status::no_error, "signed addition overflow");
        }
        #endif

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
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType, TypePolicy> lhs,
                                              const signed_integer_basis<BasisType, TypePolicy> rhs) noexcept
        -> std::pair<signed_integer_basis<BasisType, TypePolicy>, bool>
    {
        using result_type = signed_integer_basis<BasisType, TypePolicy>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType result {};

        // No overflow intrinsic here on purpose: the *.with.overflow intrinsics
        // block loop vectorization when the flag is consumed as a value, while
        // this compare form vectorizes and emits the same scalar code.
        const auto status {impl::signed_no_intrin_add(lhs_basis, rhs_basis, result)};
        return std::make_pair(result_type{result}, status != signed_overflow_status::no_error);
    }
};

// Partial specialization for checked policy
template <fundamental_signed_integral BasisType>
struct signed_add_helper<overflow_policy::checked, BasisType>
{
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType, TypePolicy> lhs,
                                              const signed_integer_basis<BasisType, TypePolicy> rhs) noexcept
        -> std::optional<signed_integer_basis<BasisType, TypePolicy>>
    {
        using result_type = signed_integer_basis<BasisType, TypePolicy>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType result {};

        // No overflow intrinsic here on purpose: the *.with.overflow intrinsics
        // block loop vectorization when the flag is consumed as a value, while
        // this compare form vectorizes and emits the same scalar code.
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
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType, TypePolicy> lhs,
                                              const signed_integer_basis<BasisType, TypePolicy> rhs) noexcept
    {
        using promoted = signed_promoted_type<BasisType>;
        static_assert(!std::is_same_v<promoted, bool>, "Widening policy with int128_t is not supported");

        using result_type = signed_integer_basis<promoted, TypePolicy>;
        return result_type{static_cast<promoted>(static_cast<promoted>(static_cast<BasisType>(lhs)) + static_cast<BasisType>(rhs))};
    }
};

template <auto Policy, fundamental_signed_integral BasisType, auto TypePolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto add_impl(const signed_integer_basis<BasisType, TypePolicy> lhs,
                                      const signed_integer_basis<BasisType, TypePolicy> rhs)
    noexcept(policy_is_nothrow_arith<Policy, BasisType>())
{
    return signed_add_helper<Policy, BasisType>::apply(lhs,rhs);
}

} // namespace impl

template <fundamental_signed_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto operator+(const signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                       const signed_integer_basis<BasisType, ErrorPolicy> rhs)
    noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> signed_integer_basis<BasisType, ErrorPolicy>
{
    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)

    // Only the throwing policy needs the duplicated diagnostics; the test is if constexpr
    // so the throw is discarded where the operator is noexcept (GCC -Wterminate)
    if constexpr (policy_equals<ErrorPolicy>(overflow_policy::throw_exception))
    {
        if (std::is_constant_evaluated())
        {
            BasisType res {};
            const auto status {impl::signed_no_intrin_add(static_cast<BasisType>(lhs), static_cast<BasisType>(rhs), res)};
            if (status == impl::signed_overflow_status::overflow)
            {
                if constexpr (std::is_same_v<BasisType, std::int8_t>)
                {
                    throw std::overflow_error("Overflow detected in i8 addition"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::int16_t>)
                {
                    throw std::overflow_error("Overflow detected in i16 addition"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::int32_t>)
                {
                    throw std::overflow_error("Overflow detected in i32 addition"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::int64_t>)
                {
                    throw std::overflow_error("Overflow detected in i64 addition"); // LCOV_EXCL_LINE
                }
                else
                {
                    throw std::overflow_error("Overflow detected in i128 addition"); // LCOV_EXCL_LINE
                }
            }
            else if (status == impl::signed_overflow_status::underflow)
            {
                if constexpr (std::is_same_v<BasisType, std::int8_t>)
                {
                    throw std::underflow_error("Underflow detected in i8 addition"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::int16_t>)
                {
                    throw std::underflow_error("Underflow detected in i16 addition"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::int32_t>)
                {
                    throw std::underflow_error("Underflow detected in i32 addition"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::int64_t>)
                {
                    throw std::underflow_error("Underflow detected in i64 addition"); // LCOV_EXCL_LINE
                }
                else
                {
                    throw std::underflow_error("Underflow detected in i128 addition"); // LCOV_EXCL_LINE
                }
            }

            return signed_integer_basis<BasisType, ErrorPolicy>{res};
        }
    }

    #endif

    return impl::signed_add_helper<ErrorPolicy, BasisType>::apply(lhs, rhs);
}

} // namespace boost::safe_numbers::detail

#define BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP(OP_NAME, OP_SYMBOL)                                                                                \
template <boost::safe_numbers::detail::fundamental_signed_integral LHSBasis, auto LHSPolicy,                                                                                    \
          boost::safe_numbers::detail::fundamental_signed_integral RHSBasis, auto RHSPolicy>                                                                                    \
    requires (!std::is_same_v<LHSBasis, RHSBasis> || boost::safe_numbers::detail::policies_differ<LHSPolicy, RHSPolicy>())                                                                                                             \
BOOST_SAFE_NUMBERS_HOST_DEVICE                                                                                                                                 \
constexpr auto OP_SYMBOL(const boost::safe_numbers::detail::signed_integer_basis<LHSBasis, LHSPolicy>,                                                                    \
                         const boost::safe_numbers::detail::signed_integer_basis<RHSBasis, RHSPolicy>)                                                                    \
{                                                                                                                                                              \
    if constexpr (std::is_same_v<LHSBasis, RHSBasis>)                                                                                                         \
    {                                                                                                                                                         \
        static_assert(boost::safe_numbers::detail::dependent_false<LHSBasis, RHSBasis>,                                                                       \
                      "Can not perform " OP_NAME " between same width types with different overflow policies "                                                \
                      "(e.g. i8 and sat_i8): convert explicitly through basis_type first");                                                                   \
    }                                                                                                                                                         \
    else if constexpr (std::is_same_v<LHSBasis, std::int8_t>)                                                                                                      \
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
    return boost::safe_numbers::detail::signed_integer_basis<LHSBasis, LHSPolicy>(0);                                                                                    \
}


namespace boost::safe_numbers::detail {

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("comparison", operator<=>)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("equality", operator==)

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("addition", operator+)

template <fundamental_signed_integral BasisType, auto ErrorPolicy>
template <fundamental_signed_integral OtherBasisType, auto OtherPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr auto signed_integer_basis<BasisType, ErrorPolicy>::operator+=(const signed_integer_basis<OtherBasisType, OtherPolicy> rhs)
    noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> signed_integer_basis&
{
    *this = *this + rhs;
    return *this;
}

// ------------------------------
// Subtraction
// ------------------------------

namespace impl {

template <fundamental_signed_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto signed_overflow_sub_msg() noexcept -> const char*
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
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto signed_underflow_sub_msg() noexcept -> const char*
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
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto signed_no_intrin_sub(const T lhs, const T rhs, T& result) noexcept -> signed_overflow_status
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

template <auto Policy, fundamental_signed_integral BasisType>
struct signed_sub_helper
{
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType, TypePolicy> lhs,
                                              const signed_integer_basis<BasisType, TypePolicy> rhs)
        noexcept(policy_is_nothrow_arith<Policy, BasisType>())
        -> signed_integer_basis<BasisType, TypePolicy>
    {
        using result_type = signed_integer_basis<BasisType, TypePolicy>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType result {};

        auto handle_error = [&result](signed_overflow_status status)
        {
            #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
            // Saturation produces a defined value, so it must keep evaluating at
            // constant evaluation time instead of failing the build with a throw
            if (std::is_constant_evaluated() && !policy_equals<Policy>(overflow_policy::saturate) && !is_user_handler_v<Policy>)
            {
                if (status == signed_overflow_status::overflow)
                {
                    if constexpr (std::is_same_v<BasisType, std::int8_t>)
                    {
                        throw std::overflow_error("Overflow detected in i8 subtraction"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
                    {
                        throw std::overflow_error("Overflow detected in i16 subtraction"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
                    {
                        throw std::overflow_error("Overflow detected in i32 subtraction"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
                    {
                        throw std::overflow_error("Overflow detected in i64 subtraction"); // LCOV_EXCL_LINE
                    }
                    else
                    {
                        throw std::overflow_error("Overflow detected in i128 subtraction"); // LCOV_EXCL_LINE
                    }
                }
                else
                {
                    if constexpr (std::is_same_v<BasisType, std::int8_t>)
                    {
                        throw std::underflow_error("Underflow detected in i8 subtraction"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
                    {
                        throw std::underflow_error("Underflow detected in i16 subtraction"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
                    {
                        throw std::underflow_error("Underflow detected in i32 subtraction"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
                    {
                        throw std::underflow_error("Underflow detected in i64 subtraction"); // LCOV_EXCL_LINE
                    }
                    else
                    {
                        throw std::underflow_error("Underflow detected in i128 subtraction"); // LCOV_EXCL_LINE
                    }
                }
            }
            else
            #endif
            {
                if constexpr (policy_equals<Policy>(overflow_policy::throw_exception))
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
                else if constexpr (policy_equals<Policy>(overflow_policy::saturate))
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
                else if constexpr (policy_equals<Policy>(overflow_policy::strict))
                {
                    static_cast<void>(result);
                    std::exit(EXIT_FAILURE);
                }
                else if constexpr (is_user_handler_v<Policy>)
                {
                    if (status == signed_overflow_status::overflow)
                    {
                        result = Policy.on_error(error_kind::overflow, result, signed_overflow_sub_msg<BasisType>());
                    }
                    else
                    {
                        result = Policy.on_error(error_kind::underflow, result, signed_underflow_sub_msg<BasisType>());
                    }
                }
                else
                {
                    static_cast<void>(result);
                    BOOST_SAFE_NUMBERS_UNREACHABLE;
                }
            }
        };

        // A constant operation that overflows is a build error under the error policies
        // (throw_exception, strict); saturate/checked/overflow_tuple return a defined value
        // and are excluded. The assert reuses the status the runtime path already computes.
        #if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_sub_overflow) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

        if constexpr (!std::is_same_v<BasisType, int128::int128_t>)
        {
            #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)

            if (!std::is_constant_evaluated())
            {
                const auto status {impl::signed_intrin_sub(lhs_basis, rhs_basis, result)};

                #ifdef BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT
                if constexpr (policy_equals<Policy>(overflow_policy::throw_exception) || policy_equals<Policy>(overflow_policy::strict))
                {
                    BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(status == signed_overflow_status::no_error, "signed subtraction overflow");
                }
                #endif

                if (status != signed_overflow_status::no_error)
                {
                    handle_error(status);
                }

                return result_type{result};
            }

            #endif
        }

        #endif // BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_sub_overflow) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

        const auto status {impl::signed_no_intrin_sub(lhs_basis, rhs_basis, result)};

        #ifdef BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT
        if constexpr (policy_equals<Policy>(overflow_policy::throw_exception) || policy_equals<Policy>(overflow_policy::strict))
        {
            BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(status == signed_overflow_status::no_error, "signed subtraction overflow");
        }
        #endif

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
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType, TypePolicy> lhs,
                                              const signed_integer_basis<BasisType, TypePolicy> rhs) noexcept
        -> std::pair<signed_integer_basis<BasisType, TypePolicy>, bool>
    {
        using result_type = signed_integer_basis<BasisType, TypePolicy>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType result {};

        // No overflow intrinsic here on purpose: the *.with.overflow intrinsics
        // block loop vectorization when the flag is consumed as a value, while
        // this compare form vectorizes and emits the same scalar code.
        const auto status {impl::signed_no_intrin_sub(lhs_basis, rhs_basis, result)};
        return std::make_pair(result_type{result}, status != signed_overflow_status::no_error);
    }
};

// Partial specialization for checked policy
template <fundamental_signed_integral BasisType>
struct signed_sub_helper<overflow_policy::checked, BasisType>
{
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType, TypePolicy> lhs,
                                              const signed_integer_basis<BasisType, TypePolicy> rhs) noexcept
        -> std::optional<signed_integer_basis<BasisType, TypePolicy>>
    {
        using result_type = signed_integer_basis<BasisType, TypePolicy>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType result {};

        // No overflow intrinsic here on purpose: the *.with.overflow intrinsics
        // block loop vectorization when the flag is consumed as a value, while
        // this compare form vectorizes and emits the same scalar code.
        const auto status {impl::signed_no_intrin_sub(lhs_basis, rhs_basis, result)};
        return status != signed_overflow_status::no_error
            ? std::nullopt
            : std::make_optional(result_type{result});
    }
};

template <auto Policy, fundamental_signed_integral BasisType, auto TypePolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto sub_impl(const signed_integer_basis<BasisType, TypePolicy> lhs,
                                      const signed_integer_basis<BasisType, TypePolicy> rhs)
    noexcept(policy_is_nothrow_arith<Policy, BasisType>())
{
    return signed_sub_helper<Policy, BasisType>::apply(lhs, rhs);
}

} // namespace impl

template <fundamental_signed_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto operator-(const signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                       const signed_integer_basis<BasisType, ErrorPolicy> rhs)
    noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> signed_integer_basis<BasisType, ErrorPolicy>
{
    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)

    if constexpr (policy_equals<ErrorPolicy>(overflow_policy::throw_exception))
    {
        if (std::is_constant_evaluated())
        {
            BasisType res {};
            const auto status {impl::signed_no_intrin_sub(static_cast<BasisType>(lhs), static_cast<BasisType>(rhs), res)};
            if (status == impl::signed_overflow_status::overflow)
            {
                if constexpr (std::is_same_v<BasisType, std::int8_t>)
                {
                    throw std::overflow_error("Overflow detected in i8 subtraction"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::int16_t>)
                {
                    throw std::overflow_error("Overflow detected in i16 subtraction"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::int32_t>)
                {
                    throw std::overflow_error("Overflow detected in i32 subtraction"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::int64_t>)
                {
                    throw std::overflow_error("Overflow detected in i64 subtraction"); // LCOV_EXCL_LINE
                }
                else
                {
                    throw std::overflow_error("Overflow detected in i128 subtraction"); // LCOV_EXCL_LINE
                }
            }
            else if (status == impl::signed_overflow_status::underflow)
            {
                if constexpr (std::is_same_v<BasisType, std::int8_t>)
                {
                    throw std::underflow_error("Underflow detected in i8 subtraction"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::int16_t>)
                {
                    throw std::underflow_error("Underflow detected in i16 subtraction"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::int32_t>)
                {
                    throw std::underflow_error("Underflow detected in i32 subtraction"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::int64_t>)
                {
                    throw std::underflow_error("Underflow detected in i64 subtraction"); // LCOV_EXCL_LINE
                }
                else
                {
                    throw std::underflow_error("Underflow detected in i128 subtraction"); // LCOV_EXCL_LINE
                }
            }

            return signed_integer_basis<BasisType, ErrorPolicy>{res};
        }
    }

    #endif

    return impl::signed_sub_helper<ErrorPolicy, BasisType>::apply(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("subtraction", operator-)

template <fundamental_signed_integral BasisType, auto ErrorPolicy>
template <fundamental_signed_integral OtherBasisType, auto OtherPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr auto signed_integer_basis<BasisType, ErrorPolicy>::operator-=(const signed_integer_basis<OtherBasisType, OtherPolicy> rhs)
    noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> signed_integer_basis&
{
    *this = *this - rhs;
    return *this;
}

// ------------------------------
// Multiplication
// ------------------------------

namespace impl {

template <fundamental_signed_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto signed_overflow_mul_msg() noexcept -> const char*
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
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto signed_underflow_mul_msg() noexcept -> const char*
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

// clang lowers signed __builtin_mul_overflow on __int128 to __muloti4 (compiler-rt
// only), which is missing when linking libgcc on clang-13. Limit the fast path to
// GCC and clang >= 14; everything else uses signed_no_intrin_mul for int128.
#if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_mul_overflow) && defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) && (!defined(__clang__) || __clang_major__ >= 14)
#  define BOOST_SAFE_NUMBERS_HAS_INT128_SIGNED_INTRIN_MUL
#endif

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

#ifdef BOOST_SAFE_NUMBERS_HAS_INT128_SIGNED_INTRIN_MUL

inline auto signed_intrin_mul(const int128::int128_t lhs, const int128::int128_t rhs, int128::int128_t& result) -> signed_overflow_status
{
    __int128_t builtin_result;
    const auto overflow {__builtin_mul_overflow(static_cast<__int128_t>(lhs), static_cast<__int128_t>(rhs), &builtin_result)};
    result = builtin_result;

    if (overflow)
    {
        return (lhs >= 0) == (rhs >= 0) ? signed_overflow_status::overflow : signed_overflow_status::underflow;
    }

    return signed_overflow_status::no_error;
}

#endif // BOOST_SAFE_NUMBERS_HAS_INT128_SIGNED_INTRIN_MUL

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
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto signed_no_intrin_mul(const T lhs, const T rhs, T& result) noexcept -> signed_overflow_status
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

template <auto Policy, fundamental_signed_integral BasisType>
struct signed_mul_helper
{
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType, TypePolicy> lhs,
                                              const signed_integer_basis<BasisType, TypePolicy> rhs)
        noexcept(policy_is_nothrow_arith<Policy, BasisType>())
        -> signed_integer_basis<BasisType, TypePolicy>
    {
        using result_type = signed_integer_basis<BasisType, TypePolicy>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType result {};

        auto handle_error = [&result](signed_overflow_status status)
        {
            #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
            // Saturation produces a defined value, so it must keep evaluating at
            // constant evaluation time instead of failing the build with a throw
            if (std::is_constant_evaluated() && !policy_equals<Policy>(overflow_policy::saturate) && !is_user_handler_v<Policy>)
            {
                if (status == signed_overflow_status::overflow)
                {
                    if constexpr (std::is_same_v<BasisType, std::int8_t>)
                    {
                        throw std::overflow_error("Overflow detected in i8 multiplication"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
                    {
                        throw std::overflow_error("Overflow detected in i16 multiplication"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
                    {
                        throw std::overflow_error("Overflow detected in i32 multiplication"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
                    {
                        throw std::overflow_error("Overflow detected in i64 multiplication"); // LCOV_EXCL_LINE
                    }
                    else
                    {
                        throw std::overflow_error("Overflow detected in i128 multiplication"); // LCOV_EXCL_LINE
                    }
                }
                else
                {
                    if constexpr (std::is_same_v<BasisType, std::int8_t>)
                    {
                        throw std::underflow_error("Underflow detected in i8 multiplication"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
                    {
                        throw std::underflow_error("Underflow detected in i16 multiplication"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
                    {
                        throw std::underflow_error("Underflow detected in i32 multiplication"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
                    {
                        throw std::underflow_error("Underflow detected in i64 multiplication"); // LCOV_EXCL_LINE
                    }
                    else
                    {
                        throw std::underflow_error("Underflow detected in i128 multiplication"); // LCOV_EXCL_LINE
                    }
                }
            }
            else
            #endif
            {
                if constexpr (policy_equals<Policy>(overflow_policy::throw_exception))
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
                else if constexpr (policy_equals<Policy>(overflow_policy::saturate))
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
                else if constexpr (policy_equals<Policy>(overflow_policy::strict))
                {
                    static_cast<void>(result);
                    std::exit(EXIT_FAILURE);
                }
                else if constexpr (is_user_handler_v<Policy>)
                {
                    if (status == signed_overflow_status::overflow)
                    {
                        result = Policy.on_error(error_kind::overflow, result, signed_overflow_mul_msg<BasisType>());
                    }
                    else
                    {
                        result = Policy.on_error(error_kind::underflow, result, signed_underflow_mul_msg<BasisType>());
                    }
                }
                else
                {
                    static_cast<void>(result);
                    BOOST_SAFE_NUMBERS_UNREACHABLE;
                }
            }
        };

        // A constant operation that overflows is a build error under the error policies
        // (throw_exception, strict); saturate/checked/overflow_tuple return a defined value
        // and are excluded. The assert reuses the status the runtime path already computes.
        #if BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_mul_overflow) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

        // Route int128 through the intrin path only where the signed 128-bit
        // fast path links (GCC, clang >= 14); elsewhere it uses no_intrin.
        #if !defined(BOOST_SAFE_NUMBERS_HAS_INT128_SIGNED_INTRIN_MUL)
        if constexpr (!std::is_same_v<BasisType, int128::int128_t>)
        #endif
        {
            #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)

            if (!std::is_constant_evaluated())
            {
                const auto status {impl::signed_intrin_mul(lhs_basis, rhs_basis, result)};

                #ifdef BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT
                if constexpr (policy_equals<Policy>(overflow_policy::throw_exception) || policy_equals<Policy>(overflow_policy::strict))
                {
                    BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(status == signed_overflow_status::no_error, "signed multiplication overflow");
                }
                #endif

                if (status != signed_overflow_status::no_error)
                {
                    handle_error(status);
                }

                return result_type{result};
            }

            #endif
        }

        #endif // BOOST_SAFE_NUMBERS_HAS_BUILTIN(__builtin_mul_overflow) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X64_INTRIN) || defined(BOOST_SAFENUMBERS_HAS_WINDOWS_X86_INTRIN)

        const auto status {impl::signed_no_intrin_mul(lhs_basis, rhs_basis, result)};

        #ifdef BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT
        if constexpr (policy_equals<Policy>(overflow_policy::throw_exception) || policy_equals<Policy>(overflow_policy::strict))
        {
            BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(status == signed_overflow_status::no_error, "signed multiplication overflow");
        }
        #endif

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
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType, TypePolicy> lhs,
                                              const signed_integer_basis<BasisType, TypePolicy> rhs) noexcept
        -> std::pair<signed_integer_basis<BasisType, TypePolicy>, bool>
    {
        using result_type = signed_integer_basis<BasisType, TypePolicy>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType result {};

        // No overflow intrinsic here on purpose: the *.with.overflow intrinsics
        // block loop vectorization when the flag is consumed as a value, while
        // this compare form vectorizes and emits the same scalar code.
        const auto status {impl::signed_no_intrin_mul(lhs_basis, rhs_basis, result)};
        return std::make_pair(result_type{result}, status != signed_overflow_status::no_error);
    }
};

// Partial specialization for checked policy
template <fundamental_signed_integral BasisType>
struct signed_mul_helper<overflow_policy::checked, BasisType>
{
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType, TypePolicy> lhs,
                                              const signed_integer_basis<BasisType, TypePolicy> rhs) noexcept
        -> std::optional<signed_integer_basis<BasisType, TypePolicy>>
    {
        using result_type = signed_integer_basis<BasisType, TypePolicy>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};
        BasisType result {};

        // No overflow intrinsic here on purpose: the *.with.overflow intrinsics
        // block loop vectorization when the flag is consumed as a value, while
        // this compare form vectorizes and emits the same scalar code.
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
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType, TypePolicy> lhs,
                                              const signed_integer_basis<BasisType, TypePolicy> rhs) noexcept
    {
        using promoted = signed_promoted_type<BasisType>;
        static_assert(!std::is_same_v<promoted, bool>, "Widening policy with int128_t is not supported");

        using result_type = signed_integer_basis<promoted, TypePolicy>;
        return result_type{static_cast<promoted>(static_cast<promoted>(static_cast<BasisType>(lhs)) * static_cast<BasisType>(rhs))};
    }
};

template <auto Policy, fundamental_signed_integral BasisType, auto TypePolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto mul_impl(const signed_integer_basis<BasisType, TypePolicy> lhs,
                                      const signed_integer_basis<BasisType, TypePolicy> rhs)
    noexcept(policy_is_nothrow_arith<Policy, BasisType>())
{
    return signed_mul_helper<Policy, BasisType>::apply(lhs, rhs);
}

} // namespace impl

template <fundamental_signed_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto operator*(const signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                       const signed_integer_basis<BasisType, ErrorPolicy> rhs)
    noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> signed_integer_basis<BasisType, ErrorPolicy>
{
    #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)

    if constexpr (policy_equals<ErrorPolicy>(overflow_policy::throw_exception))
    {
        if (std::is_constant_evaluated())
        {
            BasisType res {};
            const auto status {impl::signed_no_intrin_mul(static_cast<BasisType>(lhs), static_cast<BasisType>(rhs), res)};
            if (status == impl::signed_overflow_status::overflow)
            {
                if constexpr (std::is_same_v<BasisType, std::int8_t>)
                {
                    throw std::overflow_error("Overflow detected in i8 multiplication"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::int16_t>)
                {
                    throw std::overflow_error("Overflow detected in i16 multiplication"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::int32_t>)
                {
                    throw std::overflow_error("Overflow detected in i32 multiplication"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::int64_t>)
                {
                    throw std::overflow_error("Overflow detected in i64 multiplication"); // LCOV_EXCL_LINE
                }
                else
                {
                    throw std::overflow_error("Overflow detected in i128 multiplication"); // LCOV_EXCL_LINE
                }
            }
            else if (status == impl::signed_overflow_status::underflow)
            {
                if constexpr (std::is_same_v<BasisType, std::int8_t>)
                {
                    throw std::underflow_error("Underflow detected in i8 multiplication"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::int16_t>)
                {
                    throw std::underflow_error("Underflow detected in i16 multiplication"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::int32_t>)
                {
                    throw std::underflow_error("Underflow detected in i32 multiplication"); // LCOV_EXCL_LINE
                }
                else if constexpr (std::is_same_v<BasisType, std::int64_t>)
                {
                    throw std::underflow_error("Underflow detected in i64 multiplication"); // LCOV_EXCL_LINE
                }
                else
                {
                    throw std::underflow_error("Underflow detected in i128 multiplication"); // LCOV_EXCL_LINE
                }
            }

            return signed_integer_basis<BasisType, ErrorPolicy>{res};
        }
    }

    #endif

    return impl::signed_mul_helper<ErrorPolicy, BasisType>::apply(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("multiplication", operator*)

template <fundamental_signed_integral BasisType, auto ErrorPolicy>
template <fundamental_signed_integral OtherBasisType, auto OtherPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr auto signed_integer_basis<BasisType, ErrorPolicy>::operator*=(const signed_integer_basis<OtherBasisType, OtherPolicy> rhs)
    noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> signed_integer_basis&
{
    *this = *this * rhs;
    return *this;
}

// ------------------------------
// Division
// ------------------------------

namespace impl {

template <fundamental_signed_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto signed_div_by_zero_msg() noexcept -> const char*
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
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto signed_overflow_div_msg() noexcept -> const char*
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

template <auto Policy, fundamental_signed_integral BasisType>
struct signed_div_helper
{
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType, TypePolicy> lhs,
                                              const signed_integer_basis<BasisType, TypePolicy> rhs)
        noexcept(policy_is_nothrow_div<Policy, BasisType>())
        -> signed_integer_basis<BasisType, TypePolicy>
    {
        using result_type = signed_integer_basis<BasisType, TypePolicy>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};

        #ifdef BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT
        // Divide-by-zero throws or exits under every policy that reaches this template
        // (throw_exception, saturate, strict), so a constant zero divisor is always a bug.
        BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(rhs_basis != BasisType{0}, "signed division by zero", rhs_basis);

        // min / -1 overflow is an error under throw_exception and strict; saturate returns max.
        if constexpr (policy_equals<Policy>(overflow_policy::throw_exception) || policy_equals<Policy>(overflow_policy::strict))
        {
            BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(!(lhs_basis == std::numeric_limits<BasisType>::min() && rhs_basis == static_cast<BasisType>(-1)), "signed division overflow (min / -1)");
        }
        #endif

        if (rhs_basis == BasisType{0}) [[unlikely]]
        {
            if constexpr (policy_equals<Policy>(overflow_policy::strict))
            {
                std::exit(EXIT_FAILURE);
            }
            else if constexpr (is_user_handler_v<Policy>)
            {
                return result_type{Policy.on_error(error_kind::divide_by_zero, lhs_basis, signed_div_by_zero_msg<BasisType>())};
            }
            else
            {
                #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
                if (std::is_constant_evaluated())
                {
                    if constexpr (std::is_same_v<BasisType, std::int8_t>)
                    {
                        throw std::domain_error("Division by zero in i8 division"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
                    {
                        throw std::domain_error("Division by zero in i16 division"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
                    {
                        throw std::domain_error("Division by zero in i32 division"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
                    {
                        throw std::domain_error("Division by zero in i64 division"); // LCOV_EXCL_LINE
                    }
                    else
                    {
                        throw std::domain_error("Division by zero in i128 division"); // LCOV_EXCL_LINE
                    }
                }
                else
                #endif
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
            if constexpr (policy_equals<Policy>(overflow_policy::strict))
            {
                std::exit(EXIT_FAILURE);
            }
            else if constexpr (policy_equals<Policy>(overflow_policy::saturate))
            {
                return result_type{std::numeric_limits<BasisType>::max()};
            }
            else if constexpr (is_user_handler_v<Policy>)
            {
                return result_type{Policy.on_error(error_kind::overflow, std::numeric_limits<BasisType>::min(), signed_overflow_div_msg<BasisType>())};
            }
            else
            {
                #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
                if (std::is_constant_evaluated())
                {
                    if constexpr (std::is_same_v<BasisType, std::int8_t>)
                    {
                        throw std::overflow_error("Overflow detected in i8 division"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
                    {
                        throw std::overflow_error("Overflow detected in i16 division"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
                    {
                        throw std::overflow_error("Overflow detected in i32 division"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
                    {
                        throw std::overflow_error("Overflow detected in i64 division"); // LCOV_EXCL_LINE
                    }
                    else
                    {
                        throw std::overflow_error("Overflow detected in i128 division"); // LCOV_EXCL_LINE
                    }
                }
                else
                #endif
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
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType, TypePolicy> lhs,
                                              const signed_integer_basis<BasisType, TypePolicy> rhs)
        -> std::pair<signed_integer_basis<BasisType, TypePolicy>, bool>
    {
        using result_type = signed_integer_basis<BasisType, TypePolicy>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};

        // overflow_tuple divide-by-zero throws, so a constant zero divisor is a build error
        #ifdef BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT
        BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(rhs_basis != BasisType{0}, "signed division by zero");
        #endif

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
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType, TypePolicy> lhs,
                                              const signed_integer_basis<BasisType, TypePolicy> rhs) noexcept
        -> std::optional<signed_integer_basis<BasisType, TypePolicy>>
    {
        using result_type = signed_integer_basis<BasisType, TypePolicy>;

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

template <auto Policy, fundamental_signed_integral BasisType, auto TypePolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto div_impl(const signed_integer_basis<BasisType, TypePolicy> lhs,
                                      const signed_integer_basis<BasisType, TypePolicy> rhs)
    noexcept(policy_equals<Policy>(overflow_policy::checked) || policy_is_nothrow_div<Policy, BasisType>())
{
    return signed_div_helper<Policy, BasisType>::apply(lhs, rhs);
}

} // namespace impl

template <fundamental_signed_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto operator/(const signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                       const signed_integer_basis<BasisType, ErrorPolicy> rhs)
    noexcept(policy_is_nothrow_div<ErrorPolicy, BasisType>()) -> signed_integer_basis<BasisType, ErrorPolicy>
{
    return impl::signed_div_helper<ErrorPolicy, BasisType>::apply(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("division", operator/)

template <fundamental_signed_integral BasisType, auto ErrorPolicy>
template <fundamental_signed_integral OtherBasisType, auto OtherPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr auto signed_integer_basis<BasisType, ErrorPolicy>::operator/=(const signed_integer_basis<OtherBasisType, OtherPolicy> rhs)
    noexcept(policy_is_nothrow_div<ErrorPolicy, BasisType>()) -> signed_integer_basis&
{
    *this = *this / rhs;
    return *this;
}

// ------------------------------
// Modulo
// ------------------------------

namespace impl {

template <fundamental_signed_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto signed_mod_by_zero_msg() noexcept -> const char*
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
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto signed_overflow_mod_msg() noexcept -> const char*
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

template <auto Policy, fundamental_signed_integral BasisType>
struct signed_mod_helper
{
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType, TypePolicy> lhs,
                                              const signed_integer_basis<BasisType, TypePolicy> rhs)
        noexcept(policy_is_nothrow_div<Policy, BasisType>())
        -> signed_integer_basis<BasisType, TypePolicy>
    {
        using result_type = signed_integer_basis<BasisType, TypePolicy>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};

        #ifdef BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT
        // Modulo-by-zero throws or exits under every policy that reaches this template
        // (throw_exception, saturate, strict), so a constant zero divisor is always a bug.
        BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(rhs_basis != BasisType{0}, "signed modulo by zero");

        // min % -1 overflow is an error under throw_exception and strict; saturate returns 0.
        if constexpr (policy_equals<Policy>(overflow_policy::throw_exception) || policy_equals<Policy>(overflow_policy::strict))
        {
            BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(!(lhs_basis == std::numeric_limits<BasisType>::min() && rhs_basis == static_cast<BasisType>(-1)), "signed modulo overflow (min % -1)");
        }
        #endif

        if (rhs_basis == BasisType{0}) [[unlikely]]
        {
            if constexpr (policy_equals<Policy>(overflow_policy::strict))
            {
                std::exit(EXIT_FAILURE);
            }
            else if constexpr (is_user_handler_v<Policy>)
            {
                return result_type{Policy.on_error(error_kind::divide_by_zero, lhs_basis, signed_mod_by_zero_msg<BasisType>())};
            }
            else
            {
                #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
                if (std::is_constant_evaluated())
                {
                    if constexpr (std::is_same_v<BasisType, std::int8_t>)
                    {
                        throw std::domain_error("Division by zero in i8 modulo"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
                    {
                        throw std::domain_error("Division by zero in i16 modulo"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
                    {
                        throw std::domain_error("Division by zero in i32 modulo"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
                    {
                        throw std::domain_error("Division by zero in i64 modulo"); // LCOV_EXCL_LINE
                    }
                    else
                    {
                        throw std::domain_error("Division by zero in i128 modulo"); // LCOV_EXCL_LINE
                    }
                }
                else
                #endif
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
            if constexpr (policy_equals<Policy>(overflow_policy::strict))
            {
                std::exit(EXIT_FAILURE);
            }
            else if constexpr (policy_equals<Policy>(overflow_policy::saturate))
            {
                // The mathematical result of min % -1 is 0
                return result_type{BasisType{0}};
            }
            else if constexpr (is_user_handler_v<Policy>)
            {
                // The mathematical result of min % -1 is 0
                return result_type{Policy.on_error(error_kind::overflow, BasisType{0}, signed_overflow_mod_msg<BasisType>())};
            }
            else
            {
                #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
                if (std::is_constant_evaluated())
                {
                    if constexpr (std::is_same_v<BasisType, std::int8_t>)
                    {
                        throw std::overflow_error("Overflow detected in i8 modulo"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int16_t>)
                    {
                        throw std::overflow_error("Overflow detected in i16 modulo"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int32_t>)
                    {
                        throw std::overflow_error("Overflow detected in i32 modulo"); // LCOV_EXCL_LINE
                    }
                    else if constexpr (std::is_same_v<BasisType, std::int64_t>)
                    {
                        throw std::overflow_error("Overflow detected in i64 modulo"); // LCOV_EXCL_LINE
                    }
                    else
                    {
                        throw std::overflow_error("Overflow detected in i128 modulo"); // LCOV_EXCL_LINE
                    }
                }
                else
                #endif
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
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType, TypePolicy> lhs,
                                              const signed_integer_basis<BasisType, TypePolicy> rhs)
        -> std::pair<signed_integer_basis<BasisType, TypePolicy>, bool>
    {
        using result_type = signed_integer_basis<BasisType, TypePolicy>;

        const auto lhs_basis {static_cast<BasisType>(lhs)};
        const auto rhs_basis {static_cast<BasisType>(rhs)};

        // overflow_tuple modulo-by-zero throws, so a constant zero divisor is a build error
        #ifdef BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT
        BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(rhs_basis != BasisType{0}, "signed modulo by zero");
        #endif

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
    template <auto TypePolicy>
    BOOST_SAFE_NUMBERS_HOST_DEVICE
    [[nodiscard]] static constexpr auto apply(const signed_integer_basis<BasisType, TypePolicy> lhs,
                                              const signed_integer_basis<BasisType, TypePolicy> rhs) noexcept
        -> std::optional<signed_integer_basis<BasisType, TypePolicy>>
    {
        using result_type = signed_integer_basis<BasisType, TypePolicy>;

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

template <auto Policy, fundamental_signed_integral BasisType, auto TypePolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto mod_impl(const signed_integer_basis<BasisType, TypePolicy> lhs,
                                      const signed_integer_basis<BasisType, TypePolicy> rhs)
    noexcept(policy_equals<Policy>(overflow_policy::checked) || policy_is_nothrow_div<Policy, BasisType>())
{
    return signed_mod_helper<Policy, BasisType>::apply(lhs, rhs);
}

} // namespace impl

template <fundamental_signed_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto operator%(const signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                       const signed_integer_basis<BasisType, ErrorPolicy> rhs)
    noexcept(policy_is_nothrow_div<ErrorPolicy, BasisType>()) -> signed_integer_basis<BasisType, ErrorPolicy>
{
    return impl::signed_mod_helper<ErrorPolicy, BasisType>::apply(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("modulo", operator%)

template <fundamental_signed_integral BasisType, auto ErrorPolicy>
template <fundamental_signed_integral OtherBasisType, auto OtherPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr auto signed_integer_basis<BasisType, ErrorPolicy>::operator%=(const signed_integer_basis<OtherBasisType, OtherPolicy> rhs)
    noexcept(policy_is_nothrow_div<ErrorPolicy, BasisType>()) -> signed_integer_basis&
{
    *this = *this % rhs;
    return *this;
}

// ------------------------------
// Increment / Decrement error messages
// ------------------------------

template <fundamental_signed_integral BasisType>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto signed_overflow_inc_msg() noexcept -> const char*
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
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto signed_underflow_dec_msg() noexcept -> const char*
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

template <fundamental_signed_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr auto signed_integer_basis<BasisType, ErrorPolicy>::operator++()
    noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> signed_integer_basis&
{
    if (this->basis_ == std::numeric_limits<BasisType>::max()) [[unlikely]]
    {
        if constexpr (policy_equals<ErrorPolicy>(overflow_policy::throw_exception))
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, signed_overflow_inc_msg<BasisType>());
        }
        else if constexpr (policy_equals<ErrorPolicy>(overflow_policy::saturate))
        {
            return *this;
        }
        else if constexpr (policy_equals<ErrorPolicy>(overflow_policy::strict))
        {
            std::exit(EXIT_FAILURE);
        }
        else
        {
            this->basis_ = ErrorPolicy.on_error(error_kind::overflow, std::numeric_limits<BasisType>::min(), signed_overflow_inc_msg<BasisType>());
            return *this;
        }
    }

    ++this->basis_;
    return *this;
}

template <fundamental_signed_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr auto signed_integer_basis<BasisType, ErrorPolicy>::operator++(int)
    noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> signed_integer_basis
{
    if (this->basis_ == std::numeric_limits<BasisType>::max()) [[unlikely]]
    {
        if constexpr (policy_equals<ErrorPolicy>(overflow_policy::throw_exception))
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, signed_overflow_inc_msg<BasisType>());
        }
        else if constexpr (policy_equals<ErrorPolicy>(overflow_policy::saturate))
        {
            return *this;
        }
        else if constexpr (policy_equals<ErrorPolicy>(overflow_policy::strict))
        {
            std::exit(EXIT_FAILURE);
        }
        else
        {
            const auto temp {*this};
            this->basis_ = ErrorPolicy.on_error(error_kind::overflow, std::numeric_limits<BasisType>::min(), signed_overflow_inc_msg<BasisType>());
            return temp;
        }
    }

    const auto temp {*this};
    ++this->basis_;
    return temp;
}

// ------------------------------
// Pre and post decrement
// ------------------------------

template <fundamental_signed_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr auto signed_integer_basis<BasisType, ErrorPolicy>::operator--()
    noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> signed_integer_basis&
{
    if (this->basis_ == std::numeric_limits<BasisType>::min()) [[unlikely]]
    {
        if constexpr (policy_equals<ErrorPolicy>(overflow_policy::throw_exception))
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, signed_underflow_dec_msg<BasisType>());
        }
        else if constexpr (policy_equals<ErrorPolicy>(overflow_policy::saturate))
        {
            return *this;
        }
        else if constexpr (policy_equals<ErrorPolicy>(overflow_policy::strict))
        {
            std::exit(EXIT_FAILURE);
        }
        else
        {
            this->basis_ = ErrorPolicy.on_error(error_kind::underflow, std::numeric_limits<BasisType>::max(), signed_underflow_dec_msg<BasisType>());
            return *this;
        }
    }

    --this->basis_;
    return *this;
}

template <fundamental_signed_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
constexpr auto signed_integer_basis<BasisType, ErrorPolicy>::operator--(int)
    noexcept(policy_is_nothrow_arith<ErrorPolicy, BasisType>()) -> signed_integer_basis
{
    if (this->basis_ == std::numeric_limits<BasisType>::min()) [[unlikely]]
    {
        if constexpr (policy_equals<ErrorPolicy>(overflow_policy::throw_exception))
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, signed_underflow_dec_msg<BasisType>());
        }
        else if constexpr (policy_equals<ErrorPolicy>(overflow_policy::saturate))
        {
            return *this;
        }
        else if constexpr (policy_equals<ErrorPolicy>(overflow_policy::strict))
        {
            std::exit(EXIT_FAILURE);
        }
        else
        {
            const auto temp {*this};
            this->basis_ = ErrorPolicy.on_error(error_kind::underflow, std::numeric_limits<BasisType>::max(), signed_underflow_dec_msg<BasisType>());
            return temp;
        }
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

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto saturating_add(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                            const detail::signed_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> detail::signed_integer_basis<BasisType, ErrorPolicy>
{
    return detail::impl::add_impl<overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("saturating addition", saturating_add)

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto saturating_sub(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                            const detail::signed_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> detail::signed_integer_basis<BasisType, ErrorPolicy>
{
    return detail::impl::sub_impl<overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("saturating subtraction", saturating_sub)

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto saturating_mul(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                            const detail::signed_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> detail::signed_integer_basis<BasisType, ErrorPolicy>
{
    return detail::impl::mul_impl<overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("saturating multiplication", saturating_mul)

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto saturating_div(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                            const detail::signed_integer_basis<BasisType, ErrorPolicy> rhs)
    -> detail::signed_integer_basis<BasisType, ErrorPolicy>
{
    return detail::impl::div_impl<overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("saturating division", saturating_div)

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto saturating_mod(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                            const detail::signed_integer_basis<BasisType, ErrorPolicy> rhs)
    -> detail::signed_integer_basis<BasisType, ErrorPolicy>
{
    return detail::impl::mod_impl<overflow_policy::saturate>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("saturating modulo", saturating_mod)

// ------------------------------
// Overflowing Math
// ------------------------------

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto overflowing_add(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                             const detail::signed_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> std::pair<detail::signed_integer_basis<BasisType, ErrorPolicy>, bool>
{
    return detail::impl::add_impl<overflow_policy::overflow_tuple>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("overflowing addition", overflowing_add)

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto overflowing_sub(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                             const detail::signed_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> std::pair<detail::signed_integer_basis<BasisType, ErrorPolicy>, bool>
{
    return detail::impl::sub_impl<overflow_policy::overflow_tuple>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("overflowing subtraction", overflowing_sub)

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto overflowing_mul(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                             const detail::signed_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> std::pair<detail::signed_integer_basis<BasisType, ErrorPolicy>, bool>
{
    return detail::impl::mul_impl<overflow_policy::overflow_tuple>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("overflowing multiplication", overflowing_mul)

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto overflowing_div(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                             const detail::signed_integer_basis<BasisType, ErrorPolicy> rhs)
    -> std::pair<detail::signed_integer_basis<BasisType, ErrorPolicy>, bool>
{
    return detail::impl::div_impl<overflow_policy::overflow_tuple>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("overflowing division", overflowing_div)

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto overflowing_mod(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                             const detail::signed_integer_basis<BasisType, ErrorPolicy> rhs)
    -> std::pair<detail::signed_integer_basis<BasisType, ErrorPolicy>, bool>
{
    return detail::impl::mod_impl<overflow_policy::overflow_tuple>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("overflowing modulo", overflowing_mod)

// ------------------------------
// Checked Math
// ------------------------------

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto checked_add(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                         const detail::signed_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> std::optional<detail::signed_integer_basis<BasisType, ErrorPolicy>>
{
    return detail::impl::add_impl<overflow_policy::checked>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("checked addition", checked_add)

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto checked_sub(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                         const detail::signed_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> std::optional<detail::signed_integer_basis<BasisType, ErrorPolicy>>
{
    return detail::impl::sub_impl<overflow_policy::checked>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("checked subtraction", checked_sub)

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto checked_mul(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                         const detail::signed_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> std::optional<detail::signed_integer_basis<BasisType, ErrorPolicy>>
{
    return detail::impl::mul_impl<overflow_policy::checked>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("checked multiplication", checked_mul)

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto checked_div(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                         const detail::signed_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> std::optional<detail::signed_integer_basis<BasisType, ErrorPolicy>>
{
    return detail::impl::div_impl<overflow_policy::checked>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("checked division", checked_div)

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
BOOST_SAFE_NUMBERS_HOST_DEVICE
[[nodiscard]] constexpr auto checked_mod(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                         const detail::signed_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> std::optional<detail::signed_integer_basis<BasisType, ErrorPolicy>>
{
    return detail::impl::mod_impl<overflow_policy::checked>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("checked modulo", checked_mod)

// ------------------------------
// Strict Math
// ------------------------------

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto strict_add(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                        const detail::signed_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> detail::signed_integer_basis<BasisType, ErrorPolicy>
{
    return detail::impl::add_impl<overflow_policy::strict>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("strict addition", strict_add)

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto strict_sub(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                        const detail::signed_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> detail::signed_integer_basis<BasisType, ErrorPolicy>
{
    return detail::impl::sub_impl<overflow_policy::strict>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("strict subtraction", strict_sub)

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto strict_mul(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                        const detail::signed_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> detail::signed_integer_basis<BasisType, ErrorPolicy>
{
    return detail::impl::mul_impl<overflow_policy::strict>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("strict multiplication", strict_mul)

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto strict_div(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                        const detail::signed_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> detail::signed_integer_basis<BasisType, ErrorPolicy>
{
    return detail::impl::div_impl<overflow_policy::strict>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("strict division", strict_div)

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto strict_mod(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                        const detail::signed_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
    -> detail::signed_integer_basis<BasisType, ErrorPolicy>
{
    return detail::impl::mod_impl<overflow_policy::strict>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("strict modulo", strict_mod)

// ------------------------------
// Widening Math
// ------------------------------

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto widening_add(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                          const detail::signed_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
{
    return detail::impl::add_impl<overflow_policy::widen>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("widening add", widening_add)

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto widening_mul(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                          const detail::signed_integer_basis<BasisType, ErrorPolicy> rhs) noexcept
{
    return detail::impl::mul_impl<overflow_policy::widen>(lhs, rhs);
}

BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP("widening mul", widening_mul)

// ------------------------------
// Generic policy-parameterized functions
// ------------------------------

template <overflow_policy Policy, detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto add(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                 const detail::signed_integer_basis<BasisType, ErrorPolicy> rhs)
    noexcept(Policy != overflow_policy::throw_exception)
{
    if constexpr (Policy == overflow_policy::throw_exception)
    {
        if constexpr (detail::policy_equals<ErrorPolicy>(overflow_policy::throw_exception))
        {
            return lhs + rhs;
        }
        else
        {
            return detail::impl::add_impl<overflow_policy::throw_exception>(lhs, rhs);
        }
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

template <overflow_policy Policy, detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto sub(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                 const detail::signed_integer_basis<BasisType, ErrorPolicy> rhs)
    noexcept(Policy != overflow_policy::throw_exception)
{
    if constexpr (Policy == overflow_policy::throw_exception)
    {
        if constexpr (detail::policy_equals<ErrorPolicy>(overflow_policy::throw_exception))
        {
            return lhs - rhs;
        }
        else
        {
            return detail::impl::sub_impl<overflow_policy::throw_exception>(lhs, rhs);
        }
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

template <overflow_policy Policy, detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto mul(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                 const detail::signed_integer_basis<BasisType, ErrorPolicy> rhs)
    noexcept(Policy != overflow_policy::throw_exception)
{
    if constexpr (Policy == overflow_policy::throw_exception)
    {
        if constexpr (detail::policy_equals<ErrorPolicy>(overflow_policy::throw_exception))
        {
            return lhs * rhs;
        }
        else
        {
            return detail::impl::mul_impl<overflow_policy::throw_exception>(lhs, rhs);
        }
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

template <overflow_policy Policy, detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto div(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                 const detail::signed_integer_basis<BasisType, ErrorPolicy> rhs)
    noexcept(Policy == overflow_policy::checked || Policy == overflow_policy::strict)
{
    if constexpr (Policy == overflow_policy::throw_exception)
    {
        if constexpr (detail::policy_equals<ErrorPolicy>(overflow_policy::throw_exception))
        {
            return lhs / rhs;
        }
        else
        {
            return detail::impl::div_impl<overflow_policy::throw_exception>(lhs, rhs);
        }
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

template <overflow_policy Policy, detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
[[nodiscard]] constexpr auto mod(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                                 const detail::signed_integer_basis<BasisType, ErrorPolicy> rhs)
    noexcept(Policy == overflow_policy::checked || Policy == overflow_policy::strict)
{
    if constexpr (Policy == overflow_policy::throw_exception)
    {
        if constexpr (detail::policy_equals<ErrorPolicy>(overflow_policy::throw_exception))
        {
            return lhs % rhs;
        }
        else
        {
            return detail::impl::mod_impl<overflow_policy::throw_exception>(lhs, rhs);
        }
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

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
constexpr auto operator~(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs) noexcept
{
    static_assert(detail::dependent_false<BasisType>, "Bitwise NOT is deliberately disabled for signed safe integers (see Ada)");
    return lhs; // LCOV_EXCL_LINE : deliberately unreachable
}

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
constexpr auto operator&(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                         const detail::signed_integer_basis<BasisType, ErrorPolicy>) noexcept
{
    static_assert(detail::dependent_false<BasisType>, "Bitwise AND is deliberately disabled for signed safe integers (see Ada)");
    return lhs; // LCOV_EXCL_LINE : deliberately unreachable
}

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
constexpr auto operator|(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                         const detail::signed_integer_basis<BasisType, ErrorPolicy>) noexcept
{
    static_assert(detail::dependent_false<BasisType>, "Bitwise OR is deliberately disabled for signed safe integers (see Ada)");
    return lhs; // LCOV_EXCL_LINE : deliberately unreachable
}

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
constexpr auto operator^(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                         const detail::signed_integer_basis<BasisType, ErrorPolicy>) noexcept
{
    static_assert(detail::dependent_false<BasisType>, "Bitwise XOR is deliberately disabled for signed safe integers (see Ada)");
    return lhs; // LCOV_EXCL_LINE : deliberately unreachable
}

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
constexpr auto operator<<(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                          const detail::signed_integer_basis<BasisType, ErrorPolicy>) noexcept
{
    static_assert(detail::dependent_false<BasisType>, "Left shift is deliberately disabled for signed safe integers (see Ada)");
    return lhs; // LCOV_EXCL_LINE : deliberately unreachable
}

template <detail::fundamental_signed_integral BasisType, auto ErrorPolicy>
constexpr auto operator>>(const detail::signed_integer_basis<BasisType, ErrorPolicy> lhs,
                          const detail::signed_integer_basis<BasisType, ErrorPolicy>) noexcept
{
    static_assert(detail::dependent_false<BasisType>, "Right shift is deliberately disabled for signed safe integers (see Ada)");
    return lhs; // LCOV_EXCL_LINE : deliberately unreachable
}

} // namespace boost::safe_numbers

#undef BOOST_SAFE_NUMBERS_DEFINE_MIXED_SIGNED_INTEGER_OP

#endif //BOOST_SAFE_NUMBERS_SIGNED_INTEGER_BASIS_HPP
// ===== END boost/safe_numbers/detail/signed_integer_basis.hpp =====
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/int128.hpp
// ===== BEGIN boost/safe_numbers/detail/int128/random.hpp =====
// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_RANDOM_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_RANDOM_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/int128.hpp

namespace boost {
namespace random {
namespace traits {

template <class T, bool intrinsic>
struct make_unsigned_imp;

template <>
struct make_unsigned_imp<int128::uint128_t, false>
{
    using type = int128::uint128_t;
};

template <>
struct make_unsigned_imp<int128::int128_t, false>
{
    using type = int128::uint128_t;
};

template <class T>
struct make_unsigned;

template <>
struct make_unsigned<int128::uint128_t>
{
    using type = int128::uint128_t;
};

template <>
struct make_unsigned<int128::int128_t>
{
    using type = int128::int128_t;
};

template <class T, bool intrinsic>
struct make_unsigned_or_unbounded_imp;

template <>
struct make_unsigned_or_unbounded_imp<int128::uint128_t, false>
{
    using type = int128::uint128_t;
};

template <>
struct make_unsigned_or_unbounded_imp<int128::int128_t, false>
{
    using type = int128::uint128_t;
};

template <class T>
struct make_unsigned_or_unbounded;

template <>
struct make_unsigned_or_unbounded<int128::uint128_t>
{
    using type = int128::uint128_t;
};

template <>
struct make_unsigned_or_unbounded<int128::int128_t>
{
    using type = int128::uint128_t;
};

template <class T>
struct is_integral;

template <>
struct is_integral<int128::uint128_t> : std::true_type {};

template <>
struct is_integral<int128::int128_t> : std::true_type {};

template <class T>
struct is_signed;

template <>
struct is_signed<int128::uint128_t> : std::false_type {};

template <>
struct is_signed<int128::int128_t> : std::true_type {};

} // namespace traits
} // namespace random
} // namespace boost

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_RANDOM_HPP
// ===== END boost/safe_numbers/detail/int128/random.hpp =====

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <cstdint>

#endif

namespace boost::safe_numbers {

BOOST_SAFE_NUMBERS_EXPORT using i8 = detail::signed_integer_basis<std::int8_t>;

BOOST_SAFE_NUMBERS_EXPORT using i16 = detail::signed_integer_basis<std::int16_t>;

BOOST_SAFE_NUMBERS_EXPORT using i32 = detail::signed_integer_basis<std::int32_t>;

BOOST_SAFE_NUMBERS_EXPORT using i64 = detail::signed_integer_basis<std::int64_t>;

BOOST_SAFE_NUMBERS_EXPORT using i128 = detail::signed_integer_basis<int128::int128_t>;

// Saturating counterparts: overflow and underflow clamp to the numeric limits
// instead of throwing (including MIN / -1 and negation of MIN).
// Division and modulo by zero still throw std::domain_error.

BOOST_SAFE_NUMBERS_EXPORT using sat_i8 = detail::signed_integer_basis<std::int8_t, overflow_policy::saturate>;

BOOST_SAFE_NUMBERS_EXPORT using sat_i16 = detail::signed_integer_basis<std::int16_t, overflow_policy::saturate>;

BOOST_SAFE_NUMBERS_EXPORT using sat_i32 = detail::signed_integer_basis<std::int32_t, overflow_policy::saturate>;

BOOST_SAFE_NUMBERS_EXPORT using sat_i64 = detail::signed_integer_basis<std::int64_t, overflow_policy::saturate>;

BOOST_SAFE_NUMBERS_EXPORT using sat_i128 = detail::signed_integer_basis<int128::int128_t, overflow_policy::saturate>;

// Strict counterparts: any error terminates via std::exit(EXIT_FAILURE). Host only.

BOOST_SAFE_NUMBERS_EXPORT using strict_i8 = detail::signed_integer_basis<std::int8_t, overflow_policy::strict>;

BOOST_SAFE_NUMBERS_EXPORT using strict_i16 = detail::signed_integer_basis<std::int16_t, overflow_policy::strict>;

BOOST_SAFE_NUMBERS_EXPORT using strict_i32 = detail::signed_integer_basis<std::int32_t, overflow_policy::strict>;

BOOST_SAFE_NUMBERS_EXPORT using strict_i64 = detail::signed_integer_basis<std::int64_t, overflow_policy::strict>;

BOOST_SAFE_NUMBERS_EXPORT using strict_i128 = detail::signed_integer_basis<int128::int128_t, overflow_policy::strict>;

// Width-named alias templates selecting the policy by type: basic_i8<> is i8,
// basic_i8<saturating> is sat_i8, and basic_i8<my_handler> carries a user
// defined handler (see the error_handler_for concept).

BOOST_SAFE_NUMBERS_EXPORT template <typename ErrorHandler = throwing>
using basic_i8 = detail::signed_integer_basis<std::int8_t, detail::type_policy_v<ErrorHandler>>;

BOOST_SAFE_NUMBERS_EXPORT template <typename ErrorHandler = throwing>
using basic_i16 = detail::signed_integer_basis<std::int16_t, detail::type_policy_v<ErrorHandler>>;

BOOST_SAFE_NUMBERS_EXPORT template <typename ErrorHandler = throwing>
using basic_i32 = detail::signed_integer_basis<std::int32_t, detail::type_policy_v<ErrorHandler>>;

BOOST_SAFE_NUMBERS_EXPORT template <typename ErrorHandler = throwing>
using basic_i64 = detail::signed_integer_basis<std::int64_t, detail::type_policy_v<ErrorHandler>>;

BOOST_SAFE_NUMBERS_EXPORT template <typename ErrorHandler = throwing>
using basic_i128 = detail::signed_integer_basis<int128::int128_t, detail::type_policy_v<ErrorHandler>>;

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_SIGNED_INTEGERS_HPP
// ===== END boost/safe_numbers/signed_integers.hpp =====
// ===== BEGIN boost/safe_numbers/bounded_integers.hpp =====
// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_BOUNDED_INTEGERS_HPP
#define BOOST_SAFE_NUMBERS_BOUNDED_INTEGERS_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/config.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/type_traits.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/throw_exception.hpp
// ===== BEGIN boost/safe_numbers/detail/int128/string.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_STRING_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_STRING_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/int128.hpp
// ===== BEGIN boost/safe_numbers/detail/int128/detail/mini_to_chars.hpp =====
// Copyright 2022 Peter Dimov
// Copyright 2023 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_MINI_TO_CHARS_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_MINI_TO_CHARS_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/int128.hpp

namespace boost {
namespace int128 {
namespace detail {

// A 128-bit integer needs up to 128 binary digits (base 2); allow for a leading sign and a
// null terminator so mini_to_chars is safe for every supported base (2, 8, 10, 16).
constexpr std::size_t mini_to_chars_buffer_size = 130;

#if !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_GPU_SUPPORT)

BOOST_SAFE_NUMBERS_DETAIL_INT128_INLINE_CONSTEXPR char lower_case_digit_table[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'a', 'b', 'c', 'd', 'e', 'f'
};

static_assert(sizeof(lower_case_digit_table) == sizeof(char) * 16, "10 numbers, and 6 letters");

BOOST_SAFE_NUMBERS_DETAIL_INT128_INLINE_CONSTEXPR char upper_case_digit_table[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F'
};

static_assert(sizeof(upper_case_digit_table) == sizeof(char) * 16, "10 numbers, and 6 letters");

#endif // !__NVCC__

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr char* mini_to_chars(char (&buffer)[mini_to_chars_buffer_size], uint128_t v, const int base, const bool uppercase) noexcept
{
    #if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_GPU_SUPPORT)
    constexpr char lower_case_digit_table[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'a', 'b', 'c', 'd', 'e', 'f'
    };

    constexpr char upper_case_digit_table[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'A', 'B', 'C', 'D', 'E', 'F'
    };
    #endif

    char* last {buffer + sizeof(buffer)};
    *--last = '\0';

    if (v == 0U)
    {
        *--last = '0';
        return last;
    }

    const auto digit_table {uppercase ? upper_case_digit_table : lower_case_digit_table};

    switch (base)
    {
        case 2:
            while (v != 0U)
            {
                *--last = v.low & 1U ? '1' : '0';
                v >>= 1U;
            }
            break;

        case 8:
            while (v != 0U)
            {
                constexpr unsigned zero {48U};
                *--last = static_cast<char>(zero + (v & 7U));
                v >>= 3U;
            }
            break;

        case 10:
            while (v != 0U)
            {
                *--last = digit_table[static_cast<std::size_t>(v % 10U)];
                v /= 10U;
            }
            break;

        case 16:
            while (v != 0U)
            {
                *--last = digit_table[static_cast<std::size_t>(v & 15U)];
                v >>= 4U;
            }
            break;

        default:                        // LCOV_EXCL_LINE
            BOOST_SAFE_NUMBERS_DETAIL_INT128_UNREACHABLE;   // LCOV_EXCL_LINE
    }

    return last;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr char* mini_to_chars(char (&buffer)[mini_to_chars_buffer_size], const int128_t v, const int base, const bool uppercase) noexcept
{
    char* p {nullptr};

    if (v < 0)
    {
        // We cant negate the min value inside the signed type, but we know what the result will be
        if (v == (std::numeric_limits<int128_t>::min)())
        {
            p = mini_to_chars(buffer, uint128_t{UINT64_C(0x8000000000000000), 0}, base, uppercase);
        }
        else
        {
            const auto neg_v {-v};
            p = mini_to_chars(buffer, static_cast<uint128_t>(neg_v), base, uppercase);
        }

        *--p = '-';
    }
    else
    {
        p = mini_to_chars(buffer, static_cast<uint128_t>(v), base, uppercase);
    }

    return p;
}

} // namespace detail
} // namespace int128
} // namespace boost

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_MINI_TO_CHARS_HPP
// ===== END boost/safe_numbers/detail/int128/detail/mini_to_chars.hpp =====

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILD_MODULE

#include <string>

#endif

namespace boost {
namespace int128 {

template <typename T>
auto to_string(const T& value) -> std::enable_if_t<(std::is_same<T, int128_t>::value || std::is_same<T, uint128_t>::value), std::string>
{
    char buffer[detail::mini_to_chars_buffer_size];
    const auto last {detail::mini_to_chars(buffer, value, 10, false)};
    return std::string{last, buffer + sizeof(buffer) - 1};
}

} // namespace int128
} // namespace boost

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_STRING_HPP
// ===== END boost/safe_numbers/detail/int128/string.hpp =====
// [amalgamate] skipped duplicate include of boost/safe_numbers/overflow_policy.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/unsigned_integers.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/signed_integers.hpp

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
#include <string>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers {

namespace detail {

template <fundamental_unsigned_integral T>
auto to_string_val(T val) -> std::string
{
    using std::to_string;
    using boost::int128::to_string;
    return to_string(val);
}

template <fundamental_signed_integral T>
auto to_string_val(T val) -> std::string
{
    using std::to_string;
    using boost::int128::to_string;
    return to_string(val);
}

// Range checks that take the bounds as runtime parameters. A bound of 0 reaches
// the comparison as a parameter rather than a literal, so an unsigned check does
// not trip the "pointless comparison of unsigned integer with zero" diagnostic
// (for example NVCC warning 186) when Min is 0.
template <typename U>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto out_of_bounds(const U value, const U min_val, const U max_val) noexcept -> bool
{
    return value < min_val || value > max_val;
}

template <typename U>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto below_bound(const U value, const U min_val) noexcept -> bool
{
    return value < min_val;
}

} // namespace detail

template <auto Min, auto Max>
    requires (detail::valid_bound<decltype(Min)> &&
              detail::valid_bound<decltype(Max)> &&
              detail::raw_value(Max) > detail::raw_value(Min))
class bounded_uint
{
public:

    using basis_type = std::conditional_t<(std::numeric_limits<std::uint8_t>::max() >= detail::raw_value(Max)), u8,
                           std::conditional_t<(std::numeric_limits<std::uint16_t>::max() >= detail::raw_value(Max)), u16,
                               std::conditional_t<(std::numeric_limits<std::uint32_t>::max() >= detail::raw_value(Max)), u32,
                                   std::conditional_t<(std::numeric_limits<std::uint64_t>::max() >= detail::raw_value(Max)), u64, u128>>>>;

private:

    using underlying_type = detail::underlying_type_t<basis_type>;
    basis_type basis_ {};

public:

    BOOST_SAFE_NUMBERS_HOST_DEVICE explicit constexpr bounded_uint(const basis_type val)
    {
        constexpr auto min_raw {static_cast<underlying_type>(detail::raw_value(Min))};
        constexpr auto max_raw {static_cast<underlying_type>(detail::raw_value(Max))};
        constexpr auto min_val {basis_type{min_raw}};
        constexpr auto max_val {basis_type{max_raw}};

        if (val < min_val || val > max_val)
        {
            #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
            if (std::is_constant_evaluated())
            {
                throw std::domain_error("bounded_uint value out of range"); // LCOV_EXCL_LINE
            }
            else
            #endif
            {
                BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_uint value out of range");
            }
        }

        basis_ = val;
    }

    BOOST_SAFE_NUMBERS_HOST_DEVICE explicit constexpr bounded_uint(const underlying_type val) : bounded_uint{basis_type{val}} {}

    template <typename OtherBasis>
        requires (detail::is_unsigned_library_type_v<OtherBasis> || detail::is_fundamental_unsigned_integral_v<OtherBasis>)
    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] explicit constexpr operator OtherBasis() const
    {
        const auto raw {static_cast<detail::underlying_type_t<basis_type>>(basis_)};

        if constexpr (sizeof(OtherBasis) < sizeof(basis_type))
        {
            using raw_other = detail::underlying_type_t<OtherBasis>;
            if (raw > static_cast<detail::underlying_type_t<basis_type>>(std::numeric_limits<raw_other>::max()))
            {
                #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
                if (std::is_constant_evaluated())
                {
                    throw std::domain_error("bounded_uint conversion overflow"); // LCOV_EXCL_LINE
                }
                else
                #endif
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_uint conversion overflow");
                }
            }

            return static_cast<OtherBasis>(static_cast<raw_other>(raw));
        }
        else
        {
            return static_cast<OtherBasis>(raw);
        }
    }

    template <auto Min2, auto Max2>
    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] explicit constexpr operator bounded_uint<Min2, Max2>() const
    {
        using target_basis = typename bounded_uint<Min2, Max2>::basis_type;
        const auto raw {static_cast<detail::underlying_type_t<basis_type>>(basis_)};
        return bounded_uint<Min2, Max2>{static_cast<target_basis>(raw)};
    }

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] explicit constexpr operator basis_type() const noexcept { return basis_; }

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] explicit constexpr operator underlying_type() const noexcept { return static_cast<underlying_type>(basis_); }

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] friend constexpr auto operator<=>(bounded_uint lhs, bounded_uint rhs) noexcept
        -> std::strong_ordering = default;


    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator+=(bounded_uint<Min, Max> rhs) -> bounded_uint&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator-=(bounded_uint<Min, Max> rhs) -> bounded_uint&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator*=(bounded_uint<Min, Max> rhs) -> bounded_uint&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator/=(bounded_uint<Min, Max> rhs) -> bounded_uint&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator++() -> bounded_uint&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator++(int) -> bounded_uint;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator--() -> bounded_uint&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator--(int) -> bounded_uint;
};

template <auto Min, auto Max>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto operator+(const bounded_uint<Min, Max> lhs,
                                       const bounded_uint<Min, Max> rhs) -> bounded_uint<Min, Max>
{
    using basis = typename bounded_uint<Min, Max>::basis_type;
    using underlying = detail::underlying_type_t<basis>;
    constexpr auto min_raw {static_cast<underlying>(detail::raw_value(Min))};
    constexpr auto max_raw {static_cast<underlying>(detail::raw_value(Max))};
    const auto lhs_raw {static_cast<underlying>(static_cast<basis>(lhs))};
    const auto rhs_raw {static_cast<underlying>(static_cast<basis>(rhs))};

    underlying res {};
    if (detail::impl::unsigned_no_intrin_add(lhs_raw, rhs_raw, res))
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::overflow_error("bounded_uint addition overflow"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "bounded_uint addition overflow");
        }
    }

    if (detail::out_of_bounds(res, min_raw, max_raw))
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_uint addition result out of range"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_uint addition result out of range");
        }
    }

    return bounded_uint<Min, Max>{basis{res}};
}

template <auto Min, auto Max>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto operator-(const bounded_uint<Min, Max> lhs,
                                       const bounded_uint<Min, Max> rhs) -> bounded_uint<Min, Max>
{
    using basis = typename bounded_uint<Min, Max>::basis_type;
    using underlying = detail::underlying_type_t<basis>;
    constexpr auto min_raw {static_cast<underlying>(detail::raw_value(Min))};
    constexpr auto max_raw {static_cast<underlying>(detail::raw_value(Max))};
    const auto lhs_raw {static_cast<underlying>(static_cast<basis>(lhs))};
    const auto rhs_raw {static_cast<underlying>(static_cast<basis>(rhs))};

    underlying res {};
    if (detail::impl::unsigned_no_intrin_sub(lhs_raw, rhs_raw, res))
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::underflow_error("bounded_uint subtraction underflow"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, "bounded_uint subtraction underflow");
        }
    }

    if (detail::out_of_bounds(res, min_raw, max_raw))
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_uint subtraction result out of range"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_uint subtraction result out of range");
        }
    }

    return bounded_uint<Min, Max>{basis{res}};
}

template <auto Min, auto Max>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto operator*(const bounded_uint<Min, Max> lhs,
                                       const bounded_uint<Min, Max> rhs) -> bounded_uint<Min, Max>
{
    using basis = typename bounded_uint<Min, Max>::basis_type;
    using underlying = detail::underlying_type_t<basis>;
    constexpr auto min_raw {static_cast<underlying>(detail::raw_value(Min))};
    constexpr auto max_raw {static_cast<underlying>(detail::raw_value(Max))};
    const auto lhs_raw {static_cast<underlying>(static_cast<basis>(lhs))};
    const auto rhs_raw {static_cast<underlying>(static_cast<basis>(rhs))};

    underlying res {};
    if (detail::impl::no_intrin_mul(lhs_raw, rhs_raw, res))
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::overflow_error("bounded_uint multiplication overflow"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "bounded_uint multiplication overflow");
        }
    }

    if (detail::out_of_bounds(res, min_raw, max_raw))
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_uint multiplication result out of range"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_uint multiplication result out of range");
        }
    }

    return bounded_uint<Min, Max>{basis{res}};
}

template <auto Min, auto Max>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto operator/(const bounded_uint<Min, Max> lhs,
                                       const bounded_uint<Min, Max> rhs) -> bounded_uint<Min, Max>
{
    using basis = typename bounded_uint<Min, Max>::basis_type;
    using underlying = detail::underlying_type_t<basis>;
    constexpr auto min_raw {static_cast<underlying>(detail::raw_value(Min))};
    constexpr auto max_raw {static_cast<underlying>(detail::raw_value(Max))};
    const auto lhs_raw {static_cast<underlying>(static_cast<basis>(lhs))};
    const auto rhs_raw {static_cast<underlying>(static_cast<basis>(rhs))};

    if (rhs_raw == 0U) [[unlikely]]
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_uint division by zero"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_uint division by zero");
        }
    }

    underlying res {};
    if constexpr (std::is_same_v<underlying, std::uint8_t> || std::is_same_v<underlying, std::uint16_t>)
    {
        res = static_cast<underlying>(lhs_raw / rhs_raw);
    }
    else
    {
        res = lhs_raw / rhs_raw;
    }

    if (detail::out_of_bounds(res, min_raw, max_raw))
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_uint division result out of range"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_uint division result out of range");
        }
    }

    return bounded_uint<Min, Max>{basis{res}};
}

template <auto Min, auto Max>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto operator%(const bounded_uint<Min, Max> lhs,
                                       const bounded_uint<Min, Max> rhs) -> bounded_uint<Min, Max>
{
    using basis = typename bounded_uint<Min, Max>::basis_type;
    using underlying = detail::underlying_type_t<basis>;
    constexpr auto min_raw {static_cast<underlying>(detail::raw_value(Min))};
    constexpr auto max_raw {static_cast<underlying>(detail::raw_value(Max))};
    const auto lhs_raw {static_cast<underlying>(static_cast<basis>(lhs))};
    const auto rhs_raw {static_cast<underlying>(static_cast<basis>(rhs))};

    if (rhs_raw == 0U) [[unlikely]]
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_uint modulo by zero"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_uint modulo by zero");
        }
    }

    underlying res {};
    if constexpr (std::is_same_v<underlying, std::uint8_t> || std::is_same_v<underlying, std::uint16_t>)
    {
        res = static_cast<underlying>(lhs_raw % rhs_raw);
    }
    else
    {
        res = lhs_raw % rhs_raw;
    }

    if (detail::out_of_bounds(res, min_raw, max_raw))
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_uint modulo result out of range"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_uint modulo result out of range");
        }
    }

    return bounded_uint<Min, Max>{basis{res}};
}

template <auto Min, auto Max>
    requires (detail::valid_bound<decltype(Min)> &&
              detail::valid_bound<decltype(Max)> &&
              detail::raw_value(Max) > detail::raw_value(Min))
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto bounded_uint<Min, Max>::operator+=(bounded_uint<Min, Max> rhs) -> bounded_uint&
{
    *this = *this + rhs;
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_bound<decltype(Min)> &&
              detail::valid_bound<decltype(Max)> &&
              detail::raw_value(Max) > detail::raw_value(Min))
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto bounded_uint<Min, Max>::operator-=(bounded_uint<Min, Max> rhs) -> bounded_uint&
{
    *this = *this - rhs;
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_bound<decltype(Min)> &&
              detail::valid_bound<decltype(Max)> &&
              detail::raw_value(Max) > detail::raw_value(Min))
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto bounded_uint<Min, Max>::operator*=(bounded_uint<Min, Max> rhs) -> bounded_uint&
{
    *this = *this * rhs;
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_bound<decltype(Min)> &&
              detail::valid_bound<decltype(Max)> &&
              detail::raw_value(Max) > detail::raw_value(Min))
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto bounded_uint<Min, Max>::operator/=(bounded_uint<Min, Max> rhs) -> bounded_uint&
{
    *this = *this / rhs;
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_bound<decltype(Min)> &&
              detail::valid_bound<decltype(Max)> &&
              detail::raw_value(Max) > detail::raw_value(Min))
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto bounded_uint<Min, Max>::operator++() -> bounded_uint&
{
    using underlying = detail::underlying_type_t<basis_type>;
    constexpr auto max_raw {static_cast<underlying>(detail::raw_value(Max))};
    const auto raw {static_cast<underlying>(static_cast<basis_type>(*this))};

    underlying res {};
    if (detail::impl::unsigned_no_intrin_add(raw, static_cast<underlying>(1U), res))
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::overflow_error("bounded_uint increment overflow"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "bounded_uint increment overflow");
        }
    }

    if (res > max_raw)
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_uint increment result out of range"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_uint increment result out of range");
        }
    }

    *this = bounded_uint{basis_type{res}};
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_bound<decltype(Min)> &&
              detail::valid_bound<decltype(Max)> &&
              detail::raw_value(Max) > detail::raw_value(Min))
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto bounded_uint<Min, Max>::operator++(int) -> bounded_uint
{
    auto tmp {*this};
    ++(*this);
    return tmp;
}

template <auto Min, auto Max>
    requires (detail::valid_bound<decltype(Min)> &&
              detail::valid_bound<decltype(Max)> &&
              detail::raw_value(Max) > detail::raw_value(Min))
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto bounded_uint<Min, Max>::operator--() -> bounded_uint&
{
    using underlying = detail::underlying_type_t<basis_type>;
    constexpr auto min_raw {static_cast<underlying>(detail::raw_value(Min))};
    const auto raw {static_cast<underlying>(static_cast<basis_type>(*this))};

    underlying res {};
    if (detail::impl::unsigned_no_intrin_sub(raw, static_cast<underlying>(1U), res))
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::underflow_error("bounded_uint decrement underflow"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, "bounded_uint decrement underflow");
        }
    }

    if (detail::below_bound(res, min_raw))
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_uint decrement result out of range"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_uint decrement result out of range");
        }
    }

    *this = bounded_uint{basis_type{res}};
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_bound<decltype(Min)> &&
              detail::valid_bound<decltype(Max)> &&
              detail::raw_value(Max) > detail::raw_value(Min))
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto bounded_uint<Min, Max>::operator--(int) -> bounded_uint
{
    auto tmp {*this};
    --(*this);
    return tmp;
}

} // namespace boost::safe_numbers

// ============================================================
// bounded_int
// ============================================================

namespace boost::safe_numbers {

template <auto Min, auto Max>
    requires (detail::valid_signed_bound<decltype(Min)> &&
              detail::valid_signed_bound<decltype(Max)> &&
              detail::signed_raw_value(Max) > detail::signed_raw_value(Min))
class bounded_int
{
public:

    using basis_type = std::conditional_t<
        (detail::signed_raw_value(Min) >= std::numeric_limits<std::int8_t>::min() &&
         detail::signed_raw_value(Max) <= std::numeric_limits<std::int8_t>::max()), i8,
        std::conditional_t<
            (detail::signed_raw_value(Min) >= std::numeric_limits<std::int16_t>::min() &&
             detail::signed_raw_value(Max) <= std::numeric_limits<std::int16_t>::max()), i16,
            std::conditional_t<
                (detail::signed_raw_value(Min) >= std::numeric_limits<std::int32_t>::min() &&
                 detail::signed_raw_value(Max) <= std::numeric_limits<std::int32_t>::max()), i32,
                std::conditional_t<
                    (detail::signed_raw_value(Min) >= std::numeric_limits<std::int64_t>::min() &&
                     detail::signed_raw_value(Max) <= std::numeric_limits<std::int64_t>::max()), i64, i128>>>>;

private:

    using underlying_type = detail::underlying_type_t<basis_type>;
    basis_type basis_ {};

public:

    BOOST_SAFE_NUMBERS_HOST_DEVICE explicit constexpr bounded_int(const basis_type val)
    {
        constexpr auto min_raw {static_cast<underlying_type>(detail::signed_raw_value(Min))};
        constexpr auto max_raw {static_cast<underlying_type>(detail::signed_raw_value(Max))};
        constexpr auto min_val {basis_type{min_raw}};
        constexpr auto max_val {basis_type{max_raw}};

        if (val < min_val || val > max_val)
        {
            #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
            if (std::is_constant_evaluated())
            {
                throw std::domain_error("bounded_int value out of range"); // LCOV_EXCL_LINE
            }
            else
            #endif
            {
                BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_int value out of range");
            }
        }

        basis_ = val;
    }

    BOOST_SAFE_NUMBERS_HOST_DEVICE explicit constexpr bounded_int(const underlying_type val) : bounded_int{basis_type{val}} {}

    template <typename OtherBasis>
        requires (detail::is_signed_library_type_v<OtherBasis> || detail::is_fundamental_signed_integral_v<OtherBasis>)
    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] explicit constexpr operator OtherBasis() const
    {
        const auto raw {static_cast<detail::underlying_type_t<basis_type>>(basis_)};

        if constexpr (sizeof(OtherBasis) < sizeof(basis_type))
        {
            using raw_other = detail::underlying_type_t<OtherBasis>;
            if (raw > static_cast<detail::underlying_type_t<basis_type>>(std::numeric_limits<raw_other>::max()) ||
                raw < static_cast<detail::underlying_type_t<basis_type>>(std::numeric_limits<raw_other>::min()))
            {
                #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
                if (std::is_constant_evaluated())
                {
                    throw std::domain_error("bounded_int conversion overflow"); // LCOV_EXCL_LINE
                }
                else
                #endif
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_int conversion overflow");
                }
            }

            return static_cast<OtherBasis>(static_cast<raw_other>(raw));
        }
        else
        {
            return static_cast<OtherBasis>(raw);
        }
    }

    template <auto Min2, auto Max2>
    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] explicit constexpr operator bounded_int<Min2, Max2>() const
    {
        using target_basis = typename bounded_int<Min2, Max2>::basis_type;
        using target_underlying = detail::underlying_type_t<target_basis>;
        const auto raw {static_cast<detail::underlying_type_t<basis_type>>(basis_)};
        return bounded_int<Min2, Max2>{target_basis{static_cast<target_underlying>(raw)}};
    }

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] explicit constexpr operator basis_type() const noexcept { return basis_; }

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] explicit constexpr operator underlying_type() const noexcept { return static_cast<underlying_type>(basis_); }

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] friend constexpr auto operator<=>(bounded_int lhs, bounded_int rhs) noexcept
        -> std::strong_ordering = default;

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto operator+() const noexcept -> bounded_int { return *this; }

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto operator-() const -> bounded_int;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator+=(bounded_int<Min, Max> rhs) -> bounded_int&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator-=(bounded_int<Min, Max> rhs) -> bounded_int&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator*=(bounded_int<Min, Max> rhs) -> bounded_int&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator/=(bounded_int<Min, Max> rhs) -> bounded_int&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator++() -> bounded_int&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator++(int) -> bounded_int;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator--() -> bounded_int&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator--(int) -> bounded_int;
};

// ------------------------------
// Unary negation
// ------------------------------

template <auto Min, auto Max>
    requires (detail::valid_signed_bound<decltype(Min)> &&
              detail::valid_signed_bound<decltype(Max)> &&
              detail::signed_raw_value(Max) > detail::signed_raw_value(Min))
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto bounded_int<Min, Max>::operator-() const -> bounded_int
{
    using underlying = detail::underlying_type_t<basis_type>;
    const auto raw {static_cast<underlying>(basis_)};

    if (raw == std::numeric_limits<underlying>::min()) [[unlikely]]
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::overflow_error("bounded_int negation overflow"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "bounded_int negation overflow");
        }
    }

    const auto negated {static_cast<underlying>(-raw)};
    return bounded_int{basis_type{negated}};
}

// ------------------------------
// Addition
// ------------------------------

template <auto Min, auto Max>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto operator+(const bounded_int<Min, Max> lhs,
                                       const bounded_int<Min, Max> rhs) -> bounded_int<Min, Max>
{
    using basis = typename bounded_int<Min, Max>::basis_type;
    using underlying = detail::underlying_type_t<basis>;
    constexpr auto min_raw {static_cast<underlying>(detail::signed_raw_value(Min))};
    constexpr auto max_raw {static_cast<underlying>(detail::signed_raw_value(Max))};
    const auto lhs_raw {static_cast<underlying>(static_cast<basis>(lhs))};
    const auto rhs_raw {static_cast<underlying>(static_cast<basis>(rhs))};

    underlying res {};
    const auto status {detail::impl::signed_no_intrin_add(lhs_raw, rhs_raw, res)};
    if (status == detail::impl::signed_overflow_status::overflow)
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::overflow_error("bounded_int addition overflow"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "bounded_int addition overflow");
        }
    }
    else if (status == detail::impl::signed_overflow_status::underflow)
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::underflow_error("bounded_int addition underflow"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, "bounded_int addition underflow");
        }
    }

    if (detail::out_of_bounds(res, min_raw, max_raw))
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_int addition result out of range"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_int addition result out of range");
        }
    }

    return bounded_int<Min, Max>{basis{res}};
}

// ------------------------------
// Subtraction
// ------------------------------

template <auto Min, auto Max>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto operator-(const bounded_int<Min, Max> lhs,
                                       const bounded_int<Min, Max> rhs) -> bounded_int<Min, Max>
{
    using basis = typename bounded_int<Min, Max>::basis_type;
    using underlying = detail::underlying_type_t<basis>;
    constexpr auto min_raw {static_cast<underlying>(detail::signed_raw_value(Min))};
    constexpr auto max_raw {static_cast<underlying>(detail::signed_raw_value(Max))};
    const auto lhs_raw {static_cast<underlying>(static_cast<basis>(lhs))};
    const auto rhs_raw {static_cast<underlying>(static_cast<basis>(rhs))};

    underlying res {};
    const auto status {detail::impl::signed_no_intrin_sub(lhs_raw, rhs_raw, res)};
    if (status == detail::impl::signed_overflow_status::overflow)
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::overflow_error("bounded_int subtraction overflow"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "bounded_int subtraction overflow");
        }
    }
    else if (status == detail::impl::signed_overflow_status::underflow)
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::underflow_error("bounded_int subtraction underflow"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, "bounded_int subtraction underflow");
        }
    }

    if (detail::out_of_bounds(res, min_raw, max_raw))
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_int subtraction result out of range"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_int subtraction result out of range");
        }
    }

    return bounded_int<Min, Max>{basis{res}};
}

// ------------------------------
// Multiplication
// ------------------------------

template <auto Min, auto Max>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto operator*(const bounded_int<Min, Max> lhs,
                                       const bounded_int<Min, Max> rhs) -> bounded_int<Min, Max>
{
    using basis = typename bounded_int<Min, Max>::basis_type;
    using underlying = detail::underlying_type_t<basis>;
    constexpr auto min_raw {static_cast<underlying>(detail::signed_raw_value(Min))};
    constexpr auto max_raw {static_cast<underlying>(detail::signed_raw_value(Max))};
    const auto lhs_raw {static_cast<underlying>(static_cast<basis>(lhs))};
    const auto rhs_raw {static_cast<underlying>(static_cast<basis>(rhs))};

    underlying res {};
    const auto status {detail::impl::signed_no_intrin_mul(lhs_raw, rhs_raw, res)};
    if (status == detail::impl::signed_overflow_status::overflow)
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::overflow_error("bounded_int multiplication overflow"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "bounded_int multiplication overflow");
        }
    }
    else if (status == detail::impl::signed_overflow_status::underflow)
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::underflow_error("bounded_int multiplication underflow"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, "bounded_int multiplication underflow");
        }
    }

    if (detail::out_of_bounds(res, min_raw, max_raw))
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_int multiplication result out of range"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_int multiplication result out of range");
        }
    }

    return bounded_int<Min, Max>{basis{res}};
}

// ------------------------------
// Division
// ------------------------------

template <auto Min, auto Max>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto operator/(const bounded_int<Min, Max> lhs,
                                       const bounded_int<Min, Max> rhs) -> bounded_int<Min, Max>
{
    using basis = typename bounded_int<Min, Max>::basis_type;
    using underlying = detail::underlying_type_t<basis>;
    constexpr auto min_raw {static_cast<underlying>(detail::signed_raw_value(Min))};
    constexpr auto max_raw {static_cast<underlying>(detail::signed_raw_value(Max))};
    const auto lhs_raw {static_cast<underlying>(static_cast<basis>(lhs))};
    const auto rhs_raw {static_cast<underlying>(static_cast<basis>(rhs))};

    if (rhs_raw == static_cast<underlying>(0)) [[unlikely]]
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_int division by zero"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_int division by zero");
        }
    }

    if (lhs_raw == std::numeric_limits<underlying>::min() &&
        rhs_raw == static_cast<underlying>(-1)) [[unlikely]]
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::overflow_error("bounded_int division overflow"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "bounded_int division overflow");
        }
    }

    // Fast path: 0 / x = 0
    if (lhs_raw == static_cast<underlying>(0))
    {
        return bounded_int<Min, Max>{basis{static_cast<underlying>(0)}};
    }

    underlying res {};
    if constexpr (std::is_same_v<underlying, std::int8_t> || std::is_same_v<underlying, std::int16_t>)
    {
        res = static_cast<underlying>(lhs_raw / rhs_raw);
    }
    else
    {
        res = lhs_raw / rhs_raw;
    }

    if (detail::out_of_bounds(res, min_raw, max_raw))
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_int division result out of range"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_int division result out of range");
        }
    }

    return bounded_int<Min, Max>{basis{res}};
}

// ------------------------------
// Modulo
// ------------------------------

template <auto Min, auto Max>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto operator%(const bounded_int<Min, Max> lhs,
                                       const bounded_int<Min, Max> rhs) -> bounded_int<Min, Max>
{
    using basis = typename bounded_int<Min, Max>::basis_type;
    using underlying = detail::underlying_type_t<basis>;
    constexpr auto min_raw {static_cast<underlying>(detail::signed_raw_value(Min))};
    constexpr auto max_raw {static_cast<underlying>(detail::signed_raw_value(Max))};
    const auto lhs_raw {static_cast<underlying>(static_cast<basis>(lhs))};
    const auto rhs_raw {static_cast<underlying>(static_cast<basis>(rhs))};

    if (rhs_raw == static_cast<underlying>(0)) [[unlikely]]
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_int modulo by zero"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_int modulo by zero");
        }
    }

    if (lhs_raw == std::numeric_limits<underlying>::min() &&
        rhs_raw == static_cast<underlying>(-1)) [[unlikely]]
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::overflow_error("bounded_int modulo overflow"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "bounded_int modulo overflow");
        }
    }

    // Fast path: 0 % x = 0
    if (lhs_raw == static_cast<underlying>(0))
    {
        return bounded_int<Min, Max>{basis{static_cast<underlying>(0)}};
    }

    underlying res {};
    if constexpr (std::is_same_v<underlying, std::int8_t> || std::is_same_v<underlying, std::int16_t>)
    {
        res = static_cast<underlying>(lhs_raw % rhs_raw);
    }
    else
    {
        res = lhs_raw % rhs_raw;
    }

    if (detail::out_of_bounds(res, min_raw, max_raw))
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_int modulo result out of range"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_int modulo result out of range");
        }
    }

    return bounded_int<Min, Max>{basis{res}};
}

// ------------------------------
// Compound assignment
// ------------------------------

template <auto Min, auto Max>
    requires (detail::valid_signed_bound<decltype(Min)> &&
              detail::valid_signed_bound<decltype(Max)> &&
              detail::signed_raw_value(Max) > detail::signed_raw_value(Min))
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto bounded_int<Min, Max>::operator+=(bounded_int<Min, Max> rhs) -> bounded_int&
{
    *this = *this + rhs;
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_signed_bound<decltype(Min)> &&
              detail::valid_signed_bound<decltype(Max)> &&
              detail::signed_raw_value(Max) > detail::signed_raw_value(Min))
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto bounded_int<Min, Max>::operator-=(bounded_int<Min, Max> rhs) -> bounded_int&
{
    *this = *this - rhs;
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_signed_bound<decltype(Min)> &&
              detail::valid_signed_bound<decltype(Max)> &&
              detail::signed_raw_value(Max) > detail::signed_raw_value(Min))
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto bounded_int<Min, Max>::operator*=(bounded_int<Min, Max> rhs) -> bounded_int&
{
    *this = *this * rhs;
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_signed_bound<decltype(Min)> &&
              detail::valid_signed_bound<decltype(Max)> &&
              detail::signed_raw_value(Max) > detail::signed_raw_value(Min))
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto bounded_int<Min, Max>::operator/=(bounded_int<Min, Max> rhs) -> bounded_int&
{
    *this = *this / rhs;
    return *this;
}

// ------------------------------
// Increment / Decrement
// ------------------------------

template <auto Min, auto Max>
    requires (detail::valid_signed_bound<decltype(Min)> &&
              detail::valid_signed_bound<decltype(Max)> &&
              detail::signed_raw_value(Max) > detail::signed_raw_value(Min))
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto bounded_int<Min, Max>::operator++() -> bounded_int&
{
    using underlying = detail::underlying_type_t<basis_type>;
    constexpr auto max_raw {static_cast<underlying>(detail::signed_raw_value(Max))};
    const auto raw {static_cast<underlying>(static_cast<basis_type>(*this))};

    underlying res {};
    const auto status {detail::impl::signed_no_intrin_add(raw, static_cast<underlying>(1), res)};
    if (status != detail::impl::signed_overflow_status::no_error)
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::overflow_error("bounded_int increment overflow"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "bounded_int increment overflow");
        }
    }

    if (res > max_raw)
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_int increment result out of range"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_int increment result out of range");
        }
    }

    *this = bounded_int{basis_type{res}};
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_signed_bound<decltype(Min)> &&
              detail::valid_signed_bound<decltype(Max)> &&
              detail::signed_raw_value(Max) > detail::signed_raw_value(Min))
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto bounded_int<Min, Max>::operator++(int) -> bounded_int
{
    auto tmp {*this};
    ++(*this);
    return tmp;
}

template <auto Min, auto Max>
    requires (detail::valid_signed_bound<decltype(Min)> &&
              detail::valid_signed_bound<decltype(Max)> &&
              detail::signed_raw_value(Max) > detail::signed_raw_value(Min))
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto bounded_int<Min, Max>::operator--() -> bounded_int&
{
    using underlying = detail::underlying_type_t<basis_type>;
    constexpr auto min_raw {static_cast<underlying>(detail::signed_raw_value(Min))};
    const auto raw {static_cast<underlying>(static_cast<basis_type>(*this))};

    underlying res {};
    const auto status {detail::impl::signed_no_intrin_sub(raw, static_cast<underlying>(1), res)};
    if (status != detail::impl::signed_overflow_status::no_error)
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::underflow_error("bounded_int decrement underflow"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::underflow_error, "bounded_int decrement underflow");
        }
    }

    if (detail::below_bound(res, min_raw))
    {
        #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
        if (std::is_constant_evaluated())
        {
            throw std::domain_error("bounded_int decrement result out of range"); // LCOV_EXCL_LINE
        }
        else
        #endif
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_int decrement result out of range");
        }
    }

    *this = bounded_int{basis_type{res}};
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_signed_bound<decltype(Min)> &&
              detail::valid_signed_bound<decltype(Max)> &&
              detail::signed_raw_value(Max) > detail::signed_raw_value(Min))
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto bounded_int<Min, Max>::operator--(int) -> bounded_int
{
    auto tmp {*this};
    --(*this);
    return tmp;
}

} // namespace boost::safe_numbers

// Mixed-bounds blocking for bounded_int

#define BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_INT_OP(OP_NAME, OP_SYMBOL)                                                              \
template <auto LHSMin, auto LHSMax, auto RHSMin, auto RHSMax>                                                                          \
    requires (LHSMin != RHSMin || LHSMax != RHSMax)                                                                                     \
constexpr auto OP_SYMBOL(const boost::safe_numbers::bounded_int<LHSMin, LHSMax>,                                                       \
                         const boost::safe_numbers::bounded_int<RHSMin, RHSMax>)                                                        \
{                                                                                                                                       \
    static_assert(boost::safe_numbers::detail::dependent_false<                                                                         \
                      boost::safe_numbers::bounded_int<LHSMin, LHSMax>,                                                                 \
                      boost::safe_numbers::bounded_int<RHSMin, RHSMax>>,                                                                \
                  "Can not perform " OP_NAME " between bounded_int types with different bounds. "                                        \
                  "Both operands must have the same Min and Max.");                                                                      \
                                                                                                                                        \
    return boost::safe_numbers::bounded_int<LHSMin, LHSMax>(                                                                            \
        typename boost::safe_numbers::bounded_int<LHSMin, LHSMax>::basis_type{0});                                                      \
}

namespace boost::safe_numbers {

BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_INT_OP("comparison", operator<=>)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_INT_OP("equality", operator==)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_INT_OP("addition", operator+)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_INT_OP("subtraction", operator-)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_INT_OP("multiplication", operator*)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_INT_OP("division", operator/)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_INT_OP("modulo", operator%)

} // namespace boost::safe_numbers

#undef BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_INT_OP

// Bitwise operations blocking for bounded_int

#define BOOST_SAFE_NUMBERS_DEFINE_BOUNDED_INT_BITWISE_OP(OP_NAME, OP_SYMBOL)                                                   \
template <auto Min, auto Max>                                                                                                  \
constexpr auto OP_SYMBOL(const boost::safe_numbers::bounded_int<Min, Max>,                                                     \
                         const boost::safe_numbers::bounded_int<Min, Max>)                                                     \
{                                                                                                                              \
    static_assert(boost::safe_numbers::detail::dependent_false<boost::safe_numbers::bounded_int<Min, Max>>,                    \
                  "Can not perform " OP_NAME " between bounded_int types as the results would be non-sensical");               \
                                                                                                                               \
    return boost::safe_numbers::bounded_int<Min, Max>(                                                                         \
        typename boost::safe_numbers::bounded_int<Min, Max>::basis_type{0});                                                   \
}

namespace boost::safe_numbers {

BOOST_SAFE_NUMBERS_DEFINE_BOUNDED_INT_BITWISE_OP("and", operator&)
BOOST_SAFE_NUMBERS_DEFINE_BOUNDED_INT_BITWISE_OP("or", operator|)
BOOST_SAFE_NUMBERS_DEFINE_BOUNDED_INT_BITWISE_OP("xor", operator^)
BOOST_SAFE_NUMBERS_DEFINE_BOUNDED_INT_BITWISE_OP("right shift", operator>>)
BOOST_SAFE_NUMBERS_DEFINE_BOUNDED_INT_BITWISE_OP("left shift", operator<<)

} // namespace boost::safe_numbers

#undef BOOST_SAFE_NUMBERS_DEFINE_BOUNDED_INT_BITWISE_OP

#define BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP(OP_NAME, OP_SYMBOL)                                                             \
template <auto LHSMin, auto LHSMax, auto RHSMin, auto RHSMax>                                                                          \
    requires (LHSMin != RHSMin || LHSMax != RHSMax)                                                                                     \
constexpr auto OP_SYMBOL(const boost::safe_numbers::bounded_uint<LHSMin, LHSMax>,                                                      \
                         const boost::safe_numbers::bounded_uint<RHSMin, RHSMax>)                                                       \
{                                                                                                                                       \
    static_assert(boost::safe_numbers::detail::dependent_false<                                                                         \
                      boost::safe_numbers::bounded_uint<LHSMin, LHSMax>,                                                                \
                      boost::safe_numbers::bounded_uint<RHSMin, RHSMax>>,                                                               \
                  "Can not perform " OP_NAME " between bounded_uint types with different bounds. "                                       \
                  "Both operands must have the same Min and Max.");                                                                      \
                                                                                                                                        \
    return boost::safe_numbers::bounded_uint<LHSMin, LHSMax>(                                                                           \
        typename boost::safe_numbers::bounded_uint<LHSMin, LHSMax>::basis_type{0});                                                     \
}

namespace boost::safe_numbers {

BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("comparison", operator<=>)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("equality", operator==)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("addition", operator+)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("subtraction", operator-)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("multiplication", operator*)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("division", operator/)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("modulo", operator%)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("and", operator&)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("or", operator|)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("xor", operator^)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("right shift", operator>>)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP("left shift", operator<<)

} // namespace boost::safe_numbers

#undef BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_UINT_OP

#define BOOST_SAFE_NUMBERS_DEFINE_BITWISE_OP(OP_NAME, OP_SYMBOL)                                                    \
template <auto Min, auto Max>                                                                                       \
constexpr auto OP_SYMBOL(const boost::safe_numbers::bounded_uint<Min, Max>,                                         \
                         const boost::safe_numbers::bounded_uint<Min, Max>)                                         \
{                                                                                                                   \
    static_assert(boost::safe_numbers::detail::dependent_false<boost::safe_numbers::bounded_uint<Min, Max>>,        \
                  "Can not perform " OP_NAME "between bounded_uint types as the results would be non-sensical");    \
                                                                                                                    \
    return boost::safe_numbers::bounded_uint<Min, Max>(                                                             \
        typename boost::safe_numbers::bounded_uint<Min, Max>::basis_type{0});                                       \
}                                                                                                                   \

namespace boost::safe_numbers {

BOOST_SAFE_NUMBERS_DEFINE_BITWISE_OP("and", operator&)
BOOST_SAFE_NUMBERS_DEFINE_BITWISE_OP("or", operator|)
BOOST_SAFE_NUMBERS_DEFINE_BITWISE_OP("xor", operator^)
BOOST_SAFE_NUMBERS_DEFINE_BITWISE_OP("right shift", operator>>)
BOOST_SAFE_NUMBERS_DEFINE_BITWISE_OP("left shift", operator<<)

}

#undef BOOST_SAFE_NUMBERS_DEFINE_BITWISE_OP

#endif // BOOST_SAFE_NUMBERS_BOUNDED_INTEGERS_HPP
// ===== END boost/safe_numbers/bounded_integers.hpp =====
// ===== BEGIN boost/safe_numbers/floats.hpp =====
// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_FLOATS_HPP
#define BOOST_SAFE_NUMBERS_FLOATS_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/config.hpp
// ===== BEGIN boost/safe_numbers/detail/float_basis.hpp =====
// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BASIS_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BASIS_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/config.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/type_traits.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/throw_exception.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/overflow_policy.hpp

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

// The saturating policy is raw IEEE 754, so overflow to infinity is the intended result
#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable:4756)
#endif

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

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

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

// The saturating policy is raw IEEE 754, so overflow to infinity is the intended result
#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable:4756)
#endif

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

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

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

// The saturating policy is raw IEEE 754, so overflow to infinity is the intended result
#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable:4756)
#endif

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

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

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

// The saturating policy is raw IEEE 754, so overflow to infinity is the intended result
#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable:4756)
#endif

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

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

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

// These return the raw IEEE result alongside the flag, so overflow is expected here
#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable:4756)
#endif

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

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

} // namespace boost::safe_numbers

#undef BOOST_SAFE_NUMBERS_DEFINE_MIXED_FLOAT_OP

#endif // BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BASIS_HPP
// ===== END boost/safe_numbers/detail/float_basis.hpp =====

namespace boost::safe_numbers {

BOOST_SAFE_NUMBERS_EXPORT using f32 = detail::float_basis<float>;

BOOST_SAFE_NUMBERS_EXPORT using f64 = detail::float_basis<double>;

// Saturating counterparts with raw IEEE 754 semantics: overflow saturates to
// infinity, NaN propagates, and division by zero yields infinity. No checks run.
// basic_f32/basic_f64 below select the policy by type, including user handlers.

BOOST_SAFE_NUMBERS_EXPORT using sat_f32 = detail::float_basis<float, overflow_policy::saturate>;

BOOST_SAFE_NUMBERS_EXPORT using sat_f64 = detail::float_basis<double, overflow_policy::saturate>;

// Width-named alias templates selecting the policy by type: basic_f32<> is f32,
// basic_f32<saturating> is sat_f32, and basic_f32<my_handler> carries a user
// defined handler (see the error_handler_for concept).

BOOST_SAFE_NUMBERS_EXPORT template <typename ErrorHandler = throwing>
using basic_f32 = detail::float_basis<float, detail::type_policy_v<ErrorHandler>>;

BOOST_SAFE_NUMBERS_EXPORT template <typename ErrorHandler = throwing>
using basic_f64 = detail::float_basis<double, detail::type_policy_v<ErrorHandler>>;

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_FLOATS_HPP
// ===== END boost/safe_numbers/floats.hpp =====
// ===== BEGIN boost/safe_numbers/iostream.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_IOSTREAM_HPP
#define BOOST_SAFE_NUMBERS_IOSTREAM_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/config.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/type_traits.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/throw_exception.hpp
// ===== BEGIN boost/safe_numbers/detail/int128/iostream.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_IOSTREAM_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_IOSTREAM_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/int128.hpp
// ===== BEGIN boost/safe_numbers/detail/int128/detail/mini_from_chars.hpp =====
// Copyright 2022 Peter Dimov
// Copyright 2023 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef MINI_FROM_CHARS_HPP
#define MINI_FROM_CHARS_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/uint128_imp.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/int128_imp.hpp

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILD_MODULE

#include <cerrno>
#include <limits>
#include <cstddef>

#endif

namespace boost {
namespace int128 {
namespace detail {

namespace impl {

#if !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_GPU_SUPPORT)

BOOST_SAFE_NUMBERS_DETAIL_INT128_INLINE_CONSTEXPR unsigned char uchar_values[] =
     {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        0,   1,   2,   3,   4,   5,   6,   7,   8,   9, 255, 255, 255, 255, 255, 255,
      255,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
       25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35, 255, 255, 255, 255, 255,
      255,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
       25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

static_assert(sizeof(uchar_values) == 256, "uchar_values should represent all 256 values of unsigned char");

#endif // __NVCC__

// Convert characters for 0-9, A-Z, a-z to 0-35. Anything else is 255
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_SAFE_NUMBERS_DETAIL_INT128_FORCE_INLINE constexpr auto digit_from_char(char val) noexcept -> unsigned char
{
    #if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_GPU_SUPPORT)

    constexpr unsigned char uchar_values[] =
    {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
       0,   1,   2,   3,   4,   5,   6,   7,   8,   9, 255, 255, 255, 255, 255, 255,
     255,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
      25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35, 255, 255, 255, 255, 255,
     255,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
      25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

    static_assert(sizeof(uchar_values) == 256, "uchar_values should represent all 256 values of unsigned char");

    #endif // __NVCC__

    return uchar_values[static_cast<unsigned char>(val)];
}

template <typename Integer, typename Unsigned_Integer>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int from_chars_integer_impl(const char* first, const char* last, Integer& value, int base) noexcept
{
    if (first >= last)
    {
        return EINVAL;
    }

    Unsigned_Integer result {};
    Unsigned_Integer overflow_value {};
    Unsigned_Integer max_digit {};

    const auto unsigned_base = static_cast<Unsigned_Integer>(base);

    // Strip sign if the type is signed
    // Negative sign will be appended at the end of parsing
    bool is_negative = false;
    static_cast<void>(is_negative);
    auto next = first;

    BOOST_SAFE_NUMBERS_DETAIL_INT128_IF_CONSTEXPR (std::numeric_limits<Integer>::is_signed)
    {
        if (*next == '-')
        {
            is_negative = true;
            ++next;
        }

        overflow_value = static_cast<Unsigned_Integer>((std::numeric_limits<Integer>::max)());
        max_digit = static_cast<Unsigned_Integer>((std::numeric_limits<Integer>::max)());

        if (is_negative)
        {
            ++overflow_value;
            ++max_digit;
        }
    }
    else
    {
        if (*next == '-' || *next == '+')
        {
            return EINVAL;
        }

        overflow_value = (std::numeric_limits<Unsigned_Integer>::max)();
        max_digit = (std::numeric_limits<Unsigned_Integer>::max)();
    }


    overflow_value /= unsigned_base;
    max_digit %= unsigned_base;

    // If the only character was a sign abort now
    if (next == last)
    {
        return EINVAL;
    }

    bool overflowed = false;

    const std::ptrdiff_t nc = last - next;

    // For bases 2..10 the first digits10 characters always fit in the unsigned
    // For bases above 10, the safe window is shorter, so we must check with each iteration
    const std::ptrdiff_t nd {
        base <= 10
            ? static_cast<std::ptrdiff_t>(std::numeric_limits<Integer>::digits10)
            : std::ptrdiff_t{0}
    };

    const std::ptrdiff_t fast_limit {nd < nc ? nd : nc};
    std::ptrdiff_t i = 0;

    for (; i < fast_limit; ++i)
    {
        const auto current_digit = static_cast<Unsigned_Integer>(digit_from_char(*next));

        if (current_digit >= unsigned_base)
        {
            break;
        }

        result = static_cast<Unsigned_Integer>(result * unsigned_base + current_digit);
        ++next;
    }

    for (; i < nc; ++i)
    {
        const auto current_digit = static_cast<Unsigned_Integer>(digit_from_char(*next));

        if (current_digit >= unsigned_base)
        {
            break;
        }

        if (result < overflow_value || (result == overflow_value && current_digit <= max_digit))
        {
            result = static_cast<Unsigned_Integer>(result * unsigned_base + current_digit);
        }
        else
        {
            overflowed = true;
            break;
        }

        ++next;
    }

    // Return the parsed value, adding the sign back if applicable
    // If we have overflowed, then we do not return the result
    if (overflowed)
    {
        return EDOM;
    }

    value = static_cast<Integer>(result);

    BOOST_SAFE_NUMBERS_DETAIL_INT128_IF_CONSTEXPR (std::numeric_limits<Integer>::is_signed)
    {
        if (is_negative)
        {
            value = static_cast<Integer>(-(static_cast<Unsigned_Integer>(value)));
        }
    }

    // This value will be negative to differentiate from errno values
    // since they are in the range of acceptable distances

    // This cast is useless on 32-bit platforms
    #if defined(__GNUC__) && !defined(__clang__)
    #  pragma GCC diagnostic push
    #  pragma GCC diagnostic ignored "-Wuseless-cast"
    #endif

    return static_cast<int>(first - next);

    #if defined(__GNUC__) && !defined(__clang__)
    #  pragma GCC diagnostic pop
    #endif
}
} // namespace impl

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int from_chars(const char* first, const char* last, uint128_t& value, int base = 10) noexcept
{
    return impl::from_chars_integer_impl<uint128_t, uint128_t>(first, last, value, base);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int from_chars(const char* first, const char* last, int128_t& value, int base = 10) noexcept
{
    return impl::from_chars_integer_impl<int128_t, uint128_t>(first, last, value, base);
}

} // namespace detail
} // namespace int128
} // namespace boost

#endif //MINI_FROM_CHARS_HPP
// ===== END boost/safe_numbers/detail/int128/detail/mini_from_chars.hpp =====
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/mini_to_chars.hpp
// ===== BEGIN boost/safe_numbers/detail/int128/detail/utilities.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_UTILITIES_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_UTILITIES_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/config.hpp

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILD_MODULE

#include <cstddef>

#endif

namespace boost {
namespace int128 {
namespace detail {

template <typename T>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr std::size_t strlen(const T* str) noexcept
{
    std::size_t i {};
    while (*str != '\0')
    {
        ++str;
        ++i;
    }

    return i;
}

} // namespace detail
} // namespace int128
} // namespace boost

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_DETAIL_UTILITIES_HPP
// ===== END boost/safe_numbers/detail/int128/detail/utilities.hpp =====
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/config.hpp

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILD_MODULE

#include <type_traits>
#include <iostream>
#include <iomanip>
#include <cstring>

#endif

namespace boost {
namespace int128 {

namespace detail {

template <typename T>
struct streamable_overload
{
    static constexpr bool value = std::is_same<T, uint128_t>::value || std::is_same<T, int128_t>::value;
};

template <typename T>
BOOST_SAFE_NUMBERS_DETAIL_INT128_INLINE_CONSTEXPR bool is_streamable_overload_v = streamable_overload<T>::value;

} // namespace detail

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <typename charT, typename traits, typename LibIntegerType>
auto operator>>(std::basic_istream<charT, traits>& is, LibIntegerType& v)
    -> std::enable_if_t<detail::is_streamable_overload_v<LibIntegerType>, std::basic_istream<charT, traits>&>
{
    charT t_buffer[64] {};
    is >> std::ws >> std::setw(63) >> t_buffer;

    const auto t_buffer_len {std::char_traits<charT>::length(t_buffer)};

    char buffer[64] {};
    auto buffer_start {buffer};

    BOOST_SAFE_NUMBERS_DETAIL_INT128_IF_CONSTEXPR (!std::is_same<charT, char>::value)
    {
        auto first {buffer};
        auto t_first {t_buffer};
        const auto t_buffer_end {t_buffer + detail::strlen(t_buffer)};

        while (t_first != t_buffer_end)
        {
            *first++ = static_cast<char>(*t_first++);
        }
    }
    else
    {
        std::memcpy(buffer, t_buffer, sizeof(t_buffer));
    }

    const auto flags {is.flags()};
    int base {10};
    if (flags & std::ios_base::oct)
    {
        base = 8;
        if (*buffer_start == '0')
        {
            ++buffer_start;
        }
    }
    else if (flags & std::ios_base::hex)
    {
        base = 16;
        if (*buffer_start == '0')
        {
            buffer_start += 2;
        }
    }

    const auto r {detail::from_chars(buffer_start, buffer + detail::strlen(buffer), v, base)};

    // Put back unconsumed characters
    // If r is greater than 0 then an errno values has been hit
    const auto consumed {static_cast<std::size_t>(r > 0 ? 0 : -r)};
    BOOST_SAFE_NUMBERS_DETAIL_INT128_ASSERT(t_buffer_len >= consumed);
    const auto return_chars {static_cast<std::size_t>(t_buffer_len - consumed)};

    for (std::size_t i {}; i < return_chars; ++i)
    {
        is.putback(t_buffer[t_buffer_len - i - 1]);
    }

    return is;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <typename charT, typename traits, typename LibIntegerType>
auto operator<<(std::basic_ostream<charT, traits>& os, const LibIntegerType& v)
    -> std::enable_if_t<detail::is_streamable_overload_v<LibIntegerType>, std::basic_ostream<charT, traits>&>
{
    char buffer[detail::mini_to_chars_buffer_size] {};

    const auto flags {os.flags()};
    int base {10};
    bool uppercase {false};
    if (flags & std::ios_base::oct)
    {
        base = 8;
    }
    else if (flags & std::ios_base::hex)
    {
        base = 16;
    }

    if (flags & std::ios_base::uppercase)
    {
        uppercase = true;
    }

    auto first {detail::mini_to_chars(buffer, v, base, uppercase)};

    if (flags & std::ios_base::showbase)
    {
        if (base == 8)
        {
            *--first = '0';
        }
        else if (base == 16)
        {
            *--first = uppercase ? 'X' : 'x';
            *--first = '0';
        }
    }

    BOOST_SAFE_NUMBERS_DETAIL_INT128_IF_CONSTEXPR (!std::is_same<charT, char>::value)
    {
        charT t_buffer[64U] {};

        auto t_first {t_buffer};
        while (*first != '\0')
        {
            *t_first++ = static_cast<charT>(*first++);
        }

        os << t_buffer;
    }
    else
    {
        os << first;
    }

    return os;
}

} // namespace int128
} // namespace boost

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_IOSTREAM_HPP
// ===== END boost/safe_numbers/detail/int128/iostream.hpp =====

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <iostream>
#include <cstdint>

#endif // ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers::detail {

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable:4702) // Unreachable code is only true with exceptions enabled
#endif

BOOST_SAFE_NUMBERS_EXPORT template <typename charT, typename traits, library_type LibType>
auto operator>>(std::basic_istream<charT, traits>& is, LibType& v) -> std::basic_istream<charT, traits>&
{
    using underlying_type = underlying_type_t<LibType>;

    if constexpr (is_unsigned_library_type_v<LibType>)
    {
        if (is.peek() == static_cast<charT>('-'))
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "Attempting to construct negative value with unsigned safe integer");
        }
    }

    if constexpr (std::is_same_v<underlying_type, std::uint8_t>)
    {
        std::uint32_t temp;
        is >> temp;
        v = static_cast<LibType>(static_cast<std::uint8_t>(temp));
    }
    else if constexpr (std::is_same_v<underlying_type, std::int8_t>)
    {
        std::int32_t temp;
        is >> temp;
        v = static_cast<LibType>(static_cast<std::int8_t>(temp));
    }
    else
    {
        underlying_type temp;
        is >> temp;
        v = static_cast<LibType>(temp);
    }

    return is;
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

BOOST_SAFE_NUMBERS_EXPORT template <typename charT, typename traits, library_type LibType>
auto operator<<(std::basic_ostream<charT, traits>& os, const LibType& v) -> std::basic_ostream<charT, traits>&
{
    using underlying_type = underlying_type_t<LibType>;

    const auto temp {static_cast<underlying_type>(v)};

    if constexpr (std::is_same_v<underlying_type, std::uint8_t>)
    {
        // Display the value, not the underlying representation like a carriage return
        os << static_cast<std::uint32_t>(temp);
    }
    else if constexpr (std::is_same_v<underlying_type, std::int8_t>)
    {
        os << static_cast<std::int32_t>(temp);
    }
    else
    {
        os << temp;
    }

    return os;
}

}  // namespace boost::safe_numbers::detail

namespace boost::safe_numbers {

using detail::operator<<;
using detail::operator>>;

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_IOSTREAM_HPP
// ===== END boost/safe_numbers/iostream.hpp =====
// ===== BEGIN boost/safe_numbers/limits.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_LIMITS_HPP
#define BOOST_SAFE_NUMBERS_LIMITS_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/config.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/type_traits.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/unsigned_integers.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/signed_integers.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/bounded_integers.hpp
// ===== BEGIN boost/safe_numbers/bounded_floats.hpp =====
// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_BOUNDED_FLOATS_HPP
#define BOOST_SAFE_NUMBERS_BOUNDED_FLOATS_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/config.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/type_traits.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/throw_exception.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/float_basis.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/floats.hpp

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/throw_exception.hpp>
#include <concepts>
#include <compare>
#include <limits>
#include <stdexcept>
#include <type_traits>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

// bounded_float requires C++20 floating-point non-type template parameters (P1907R1).
// On compilers without that support (notably Clang 13-15) this header expands to
// nothing, and any reference to bounded_float will fail with a "no such type" error.
// Users can test for availability with BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT.
#if BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT

namespace boost::safe_numbers {

template <auto Min, auto Max>
    requires (detail::valid_float_bound<decltype(Min)> &&
              detail::valid_float_bound<decltype(Max)> &&
              std::is_same_v<decltype(Min), decltype(Max)> &&
              detail::float_raw_value(Min) == detail::float_raw_value(Min) &&
              detail::float_raw_value(Max) == detail::float_raw_value(Max) &&
              detail::float_raw_value(Max) > detail::float_raw_value(Min))
class bounded_float
{
public:

    // basis_type follows the bound's underlying type rather than fitting smallest type like for integers
    // This preserves the precision that the user may want
    using basis_type = std::conditional_t<
        std::is_same_v<detail::underlying_type_t<decltype(Min)>, float>,
        f32, f64>;

private:

    using underlying_type = detail::underlying_type_t<basis_type>;
    basis_type basis_ {};

public:

    BOOST_SAFE_NUMBERS_HOST_DEVICE explicit constexpr bounded_float(const basis_type val)
    {
        const auto raw {static_cast<underlying_type>(val)};

        // NaN comparisons are unordered: a naked range check would silently accept NaN.
        if (detail::impl::constexpr_isnan(raw))
        {
            #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
            if (std::is_constant_evaluated())
            {
                throw std::domain_error("bounded_float NaN value"); // LCOV_EXCL_LINE
            }
            else
            #endif
            {
                BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_float NaN value");
            }
        }

        constexpr auto min_raw {static_cast<underlying_type>(detail::float_raw_value(Min))};
        constexpr auto max_raw {static_cast<underlying_type>(detail::float_raw_value(Max))};

        if (raw < min_raw || raw > max_raw)
        {
            #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
            if (std::is_constant_evaluated())
            {
                throw std::domain_error("bounded_float value out of range"); // LCOV_EXCL_LINE
            }
            else
            #endif
            {
                BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "bounded_float value out of range");
            }
        }

        basis_ = val;
    }

    BOOST_SAFE_NUMBERS_HOST_DEVICE explicit constexpr bounded_float(const underlying_type val) : bounded_float{basis_type{val}} {}

    template <typename OtherBasis>
        requires (detail::is_compatible_float_type<OtherBasis>)
    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] explicit constexpr operator OtherBasis() const
    {
        const auto raw {static_cast<underlying_type>(basis_)};

        if constexpr (sizeof(OtherBasis) < sizeof(underlying_type))
        {
            const auto result {static_cast<OtherBasis>(raw)};

            if (detail::impl::constexpr_isinf(result) && !detail::impl::constexpr_isinf(raw))
            {
                #if !defined(BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT)
                if (std::is_constant_evaluated())
                {
                    throw std::overflow_error("bounded_float narrowing conversion overflow"); // LCOV_EXCL_LINE
                }
                else
                #endif
                {
                    BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "bounded_float narrowing conversion overflow");
                }
            }

            return result;
        }
        else
        {
            return static_cast<OtherBasis>(raw);
        }
    }

    template <auto Min2, auto Max2>
    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] explicit constexpr operator bounded_float<Min2, Max2>() const
    {
        using target_basis = typename bounded_float<Min2, Max2>::basis_type;
        using target_underlying = detail::underlying_type_t<target_basis>;
        const auto raw {static_cast<underlying_type>(basis_)};
        return bounded_float<Min2, Max2>{target_basis{static_cast<target_underlying>(raw)}};
    }

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto to_basis() const noexcept -> basis_type { return basis_; }

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] friend constexpr auto operator==(bounded_float lhs, bounded_float rhs) noexcept -> bool = default;

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] friend constexpr auto operator<=>(bounded_float lhs, bounded_float rhs) noexcept
        -> std::partial_ordering = default;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator+=(bounded_float<Min, Max> rhs) -> bounded_float&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator-=(bounded_float<Min, Max> rhs) -> bounded_float&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator*=(bounded_float<Min, Max> rhs) -> bounded_float&;

    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto operator/=(bounded_float<Min, Max> rhs) -> bounded_float&;
};

// ------------------------------
// Free-function arithmetic
// ------------------------------
//
// Each operator delegates to the underlying float_basis arithmetic, which already
// throws on IEEE 754 issues (overflow, underflow, NaN, invalid_op, divide_by_zero).
// The bounded_float constructor then re-validates the result against [Min, Max].

template <auto Min, auto Max>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto operator+(const bounded_float<Min, Max> lhs,
                                       const bounded_float<Min, Max> rhs) -> bounded_float<Min, Max>
{
    using basis = typename bounded_float<Min, Max>::basis_type;
    using underlying = detail::underlying_type_t<basis>;
    const basis lhs_b {static_cast<underlying>(lhs)};
    const basis rhs_b {static_cast<underlying>(rhs)};
    return bounded_float<Min, Max>{lhs_b + rhs_b};
}

template <auto Min, auto Max>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto operator-(const bounded_float<Min, Max> lhs,
                                       const bounded_float<Min, Max> rhs) -> bounded_float<Min, Max>
{
    using basis = typename bounded_float<Min, Max>::basis_type;
    using underlying = detail::underlying_type_t<basis>;
    const basis lhs_b {static_cast<underlying>(lhs)};
    const basis rhs_b {static_cast<underlying>(rhs)};
    return bounded_float<Min, Max>{lhs_b - rhs_b};
}

template <auto Min, auto Max>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto operator*(const bounded_float<Min, Max> lhs,
                                       const bounded_float<Min, Max> rhs) -> bounded_float<Min, Max>
{
    using basis = typename bounded_float<Min, Max>::basis_type;
    using underlying = detail::underlying_type_t<basis>;
    const basis lhs_b {static_cast<underlying>(lhs)};
    const basis rhs_b {static_cast<underlying>(rhs)};
    return bounded_float<Min, Max>{lhs_b * rhs_b};
}

template <auto Min, auto Max>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto operator/(const bounded_float<Min, Max> lhs,
                                       const bounded_float<Min, Max> rhs) -> bounded_float<Min, Max>
{
    using basis = typename bounded_float<Min, Max>::basis_type;
    using underlying = detail::underlying_type_t<basis>;
    const basis lhs_b {static_cast<underlying>(lhs)};
    const basis rhs_b {static_cast<underlying>(rhs)};
    return bounded_float<Min, Max>{lhs_b / rhs_b};
}

// ------------------------------
// Compound assignment
// ------------------------------

template <auto Min, auto Max>
    requires (detail::valid_float_bound<decltype(Min)> &&
              detail::valid_float_bound<decltype(Max)> &&
              std::is_same_v<decltype(Min), decltype(Max)> &&
              detail::float_raw_value(Min) == detail::float_raw_value(Min) &&
              detail::float_raw_value(Max) == detail::float_raw_value(Max) &&
              detail::float_raw_value(Max) > detail::float_raw_value(Min))
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto bounded_float<Min, Max>::operator+=(bounded_float<Min, Max> rhs) -> bounded_float&
{
    *this = *this + rhs;
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_float_bound<decltype(Min)> &&
              detail::valid_float_bound<decltype(Max)> &&
              std::is_same_v<decltype(Min), decltype(Max)> &&
              detail::float_raw_value(Min) == detail::float_raw_value(Min) &&
              detail::float_raw_value(Max) == detail::float_raw_value(Max) &&
              detail::float_raw_value(Max) > detail::float_raw_value(Min))
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto bounded_float<Min, Max>::operator-=(bounded_float<Min, Max> rhs) -> bounded_float&
{
    *this = *this - rhs;
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_float_bound<decltype(Min)> &&
              detail::valid_float_bound<decltype(Max)> &&
              std::is_same_v<decltype(Min), decltype(Max)> &&
              detail::float_raw_value(Min) == detail::float_raw_value(Min) &&
              detail::float_raw_value(Max) == detail::float_raw_value(Max) &&
              detail::float_raw_value(Max) > detail::float_raw_value(Min))
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto bounded_float<Min, Max>::operator*=(bounded_float<Min, Max> rhs) -> bounded_float&
{
    *this = *this * rhs;
    return *this;
}

template <auto Min, auto Max>
    requires (detail::valid_float_bound<decltype(Min)> &&
              detail::valid_float_bound<decltype(Max)> &&
              std::is_same_v<decltype(Min), decltype(Max)> &&
              detail::float_raw_value(Min) == detail::float_raw_value(Min) &&
              detail::float_raw_value(Max) == detail::float_raw_value(Max) &&
              detail::float_raw_value(Max) > detail::float_raw_value(Min))
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto bounded_float<Min, Max>::operator/=(bounded_float<Min, Max> rhs) -> bounded_float&
{
    *this = *this / rhs;
    return *this;
}

} // namespace boost::safe_numbers

// Mixed-bounds blocking for bounded_float

#define BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_FLOAT_OP(OP_NAME, OP_SYMBOL)                                                            \
template <auto LHSMin, auto LHSMax, auto RHSMin, auto RHSMax>                                                                            \
    requires (LHSMin != RHSMin || LHSMax != RHSMax)                                                                                      \
constexpr auto OP_SYMBOL(const boost::safe_numbers::bounded_float<LHSMin, LHSMax>,                                                       \
                         const boost::safe_numbers::bounded_float<RHSMin, RHSMax>)                                                       \
{                                                                                                                                         \
    static_assert(boost::safe_numbers::detail::dependent_false<                                                                          \
                      boost::safe_numbers::bounded_float<LHSMin, LHSMax>,                                                                \
                      boost::safe_numbers::bounded_float<RHSMin, RHSMax>>,                                                               \
                  "Can not perform " OP_NAME " between bounded_float types with different bounds. "                                      \
                  "Both operands must have the same Min and Max.");                                                                      \
                                                                                                                                          \
    return boost::safe_numbers::bounded_float<LHSMin, LHSMax>(                                                                           \
        typename boost::safe_numbers::bounded_float<LHSMin, LHSMax>::basis_type{0});                                                     \
}

namespace boost::safe_numbers {

BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_FLOAT_OP("comparison", operator<=>)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_FLOAT_OP("equality", operator==)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_FLOAT_OP("addition", operator+)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_FLOAT_OP("subtraction", operator-)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_FLOAT_OP("multiplication", operator*)
BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_FLOAT_OP("division", operator/)

} // namespace boost::safe_numbers

#undef BOOST_SAFE_NUMBERS_DEFINE_MIXED_BOUNDED_FLOAT_OP

#endif // BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT

#endif // BOOST_SAFE_NUMBERS_BOUNDED_FLOATS_HPP
// ===== END boost/safe_numbers/bounded_floats.hpp =====
// [amalgamate] skipped duplicate include of boost/safe_numbers/floats.hpp

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <limits>

#endif // ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers::detail {

template <typename T>
struct numeric_limits_impl
{
    using basis_type = underlying_type_t<T>;

    static constexpr bool is_specialized = std::numeric_limits<basis_type>::is_specialized;
    static constexpr bool is_signed = std::numeric_limits<basis_type>::is_signed;
    static constexpr bool is_integer = std::numeric_limits<basis_type>::is_integer;
    static constexpr bool is_exact = std::numeric_limits<basis_type>::is_exact;
    static constexpr bool has_infinity = std::numeric_limits<basis_type>::has_infinity;
    static constexpr bool has_quiet_NaN = std::numeric_limits<basis_type>::has_quiet_NaN;
    static constexpr bool has_signaling_NaN = std::numeric_limits<basis_type>::has_signaling_NaN;

    #if ((!defined(_MSC_VER) && (__cplusplus <= 202002L)) || (defined(_MSC_VER) && (_MSVC_LANG <= 202002L)))
    static constexpr std::float_denorm_style has_denorm = std::numeric_limits<basis_type>::has_denorm;
    static constexpr bool has_denorm_loss = std::numeric_limits<basis_type>::has_denorm_loss;
    #endif

    static constexpr std::float_round_style round_style = std::numeric_limits<basis_type>::round_style;
    static constexpr bool is_iec559 = std::numeric_limits<basis_type>::is_iec559;
    static constexpr bool is_bounded = std::numeric_limits<basis_type>::is_bounded;
    static constexpr bool is_modulo = std::numeric_limits<basis_type>::is_modulo;
    static constexpr int digits = std::numeric_limits<basis_type>::digits;
    static constexpr int digits10 = std::numeric_limits<basis_type>::digits10;
    static constexpr int max_digits10 = std::numeric_limits<basis_type>::max_digits10;
    static constexpr int radix = std::numeric_limits<basis_type>::radix;
    static constexpr int min_exponent = std::numeric_limits<basis_type>::min_exponent;
    static constexpr int min_exponent10 = std::numeric_limits<basis_type>::min_exponent10;
    static constexpr int max_exponent = std::numeric_limits<basis_type>::max_exponent;
    static constexpr int max_exponent10 = std::numeric_limits<basis_type>::max_exponent10;
    static constexpr bool traps = std::numeric_limits<basis_type>::traps;
    static constexpr bool tinyness_before = std::numeric_limits<basis_type>::tinyness_before;

    BOOST_SAFE_NUMBERS_HOST_DEVICE static constexpr T min() { return T{std::numeric_limits<basis_type>::min()}; }
    BOOST_SAFE_NUMBERS_HOST_DEVICE static constexpr T max() { return T{std::numeric_limits<basis_type>::max()}; }
    BOOST_SAFE_NUMBERS_HOST_DEVICE static constexpr T lowest() { return T{std::numeric_limits<basis_type>::lowest()}; }
    BOOST_SAFE_NUMBERS_HOST_DEVICE static constexpr T epsilon() { return T{std::numeric_limits<basis_type>::epsilon()}; }
    BOOST_SAFE_NUMBERS_HOST_DEVICE static constexpr T round_error() { return T{std::numeric_limits<basis_type>::round_error() }; }
    BOOST_SAFE_NUMBERS_HOST_DEVICE static constexpr T infinity() { return T{std::numeric_limits<basis_type>::infinity()}; }
    BOOST_SAFE_NUMBERS_HOST_DEVICE static constexpr T quiet_NaN() { return T{std::numeric_limits<basis_type>::quiet_NaN()}; }
    BOOST_SAFE_NUMBERS_HOST_DEVICE static constexpr T signaling_NaN() { return T{std::numeric_limits<basis_type>::signaling_NaN()}; }
    BOOST_SAFE_NUMBERS_HOST_DEVICE static constexpr T denorm_min() { return T{std::numeric_limits<basis_type>::denorm_min()}; }
};

} // namespace boost::safe_numbers::detail

namespace std {

#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wmismatched-tags"
#endif

template <typename BasisType, auto ErrorPolicy>
class numeric_limits<boost::safe_numbers::detail::unsigned_integer_basis<BasisType, ErrorPolicy>> :
    public boost::safe_numbers::detail::numeric_limits_impl<boost::safe_numbers::detail::unsigned_integer_basis<BasisType, ErrorPolicy>> {};

template <typename BasisType, auto ErrorPolicy>
class numeric_limits<boost::safe_numbers::detail::signed_integer_basis<BasisType, ErrorPolicy>> :
    public boost::safe_numbers::detail::numeric_limits_impl<boost::safe_numbers::detail::signed_integer_basis<BasisType, ErrorPolicy>> {};

template <typename BasisType, auto ErrorPolicy>
class numeric_limits<boost::safe_numbers::detail::float_basis<BasisType, ErrorPolicy>> :
    public boost::safe_numbers::detail::numeric_limits_impl<boost::safe_numbers::detail::float_basis<BasisType, ErrorPolicy>> {};

template <auto Min, auto Max>
class numeric_limits<boost::safe_numbers::bounded_uint<Min, Max>>
{
    using type = boost::safe_numbers::bounded_uint<Min, Max>;
    using underlying_type = boost::safe_numbers::detail::underlying_type_t<type>;

public:
    static constexpr bool is_specialized = std::numeric_limits<underlying_type>::is_specialized;
    static constexpr bool is_signed = std::numeric_limits<underlying_type>::is_signed;
    static constexpr bool is_integer = std::numeric_limits<underlying_type>::is_integer;
    static constexpr bool is_exact = std::numeric_limits<underlying_type>::is_exact;
    static constexpr bool has_infinity = std::numeric_limits<underlying_type>::has_infinity;
    static constexpr bool has_quiet_NaN = std::numeric_limits<underlying_type>::has_quiet_NaN;
    static constexpr bool has_signaling_NaN = std::numeric_limits<underlying_type>::has_signaling_NaN;

    #if ((!defined(_MSC_VER) && (__cplusplus <= 202002L)) || (defined(_MSC_VER) && (_MSVC_LANG <= 202002L)))
    static constexpr std::float_denorm_style has_denorm = std::numeric_limits<underlying_type>::has_denorm;
    static constexpr bool has_denorm_loss = std::numeric_limits<underlying_type>::has_denorm_loss;
    #endif

    static constexpr std::float_round_style round_style = std::numeric_limits<underlying_type>::round_style;
    static constexpr bool is_iec559 = std::numeric_limits<underlying_type>::is_iec559;
    static constexpr bool is_bounded = std::numeric_limits<underlying_type>::is_bounded;
    static constexpr bool is_modulo = std::numeric_limits<underlying_type>::is_modulo;
    static constexpr int digits = std::numeric_limits<underlying_type>::digits;
    static constexpr int digits10 = std::numeric_limits<underlying_type>::digits10;
    static constexpr int max_digits10 = std::numeric_limits<underlying_type>::max_digits10;
    static constexpr int radix = std::numeric_limits<underlying_type>::radix;
    static constexpr int min_exponent = std::numeric_limits<underlying_type>::min_exponent;
    static constexpr int min_exponent10 = std::numeric_limits<underlying_type>::min_exponent10;
    static constexpr int max_exponent = std::numeric_limits<underlying_type>::max_exponent;
    static constexpr int max_exponent10 = std::numeric_limits<underlying_type>::max_exponent10;
    static constexpr bool traps = std::numeric_limits<underlying_type>::traps;
    static constexpr bool tinyness_before = std::numeric_limits<underlying_type>::tinyness_before;

    static constexpr type min() { return type{static_cast<underlying_type>(boost::safe_numbers::detail::raw_value(Min))}; }
    static constexpr type max() { return type{static_cast<underlying_type>(boost::safe_numbers::detail::raw_value(Max))}; }
    static constexpr type lowest() { return min(); }
    static constexpr type epsilon() { return min(); }
    static constexpr type round_error() { return min(); }
    static constexpr type infinity() { return min(); }
    static constexpr type quiet_NaN() { return min(); }
    static constexpr type signaling_NaN() { return min(); }
    static constexpr type denorm_min() { return min(); }
};

template <auto Min, auto Max>
class numeric_limits<boost::safe_numbers::bounded_int<Min, Max>>
{
    using type = boost::safe_numbers::bounded_int<Min, Max>;
    using underlying_type = boost::safe_numbers::detail::underlying_type_t<type>;

public:
    static constexpr bool is_specialized = std::numeric_limits<underlying_type>::is_specialized;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = std::numeric_limits<underlying_type>::is_exact;
    static constexpr bool has_infinity = std::numeric_limits<underlying_type>::has_infinity;
    static constexpr bool has_quiet_NaN = std::numeric_limits<underlying_type>::has_quiet_NaN;
    static constexpr bool has_signaling_NaN = std::numeric_limits<underlying_type>::has_signaling_NaN;

    #if ((!defined(_MSC_VER) && (__cplusplus <= 202002L)) || (defined(_MSC_VER) && (_MSVC_LANG <= 202002L)))
    static constexpr std::float_denorm_style has_denorm = std::numeric_limits<underlying_type>::has_denorm;
    static constexpr bool has_denorm_loss = std::numeric_limits<underlying_type>::has_denorm_loss;
    #endif

    static constexpr std::float_round_style round_style = std::numeric_limits<underlying_type>::round_style;
    static constexpr bool is_iec559 = std::numeric_limits<underlying_type>::is_iec559;
    static constexpr bool is_bounded = std::numeric_limits<underlying_type>::is_bounded;
    static constexpr bool is_modulo = std::numeric_limits<underlying_type>::is_modulo;
    static constexpr int digits = std::numeric_limits<underlying_type>::digits;
    static constexpr int digits10 = std::numeric_limits<underlying_type>::digits10;
    static constexpr int max_digits10 = std::numeric_limits<underlying_type>::max_digits10;
    static constexpr int radix = std::numeric_limits<underlying_type>::radix;
    static constexpr int min_exponent = std::numeric_limits<underlying_type>::min_exponent;
    static constexpr int min_exponent10 = std::numeric_limits<underlying_type>::min_exponent10;
    static constexpr int max_exponent = std::numeric_limits<underlying_type>::max_exponent;
    static constexpr int max_exponent10 = std::numeric_limits<underlying_type>::max_exponent10;
    static constexpr bool traps = std::numeric_limits<underlying_type>::traps;
    static constexpr bool tinyness_before = std::numeric_limits<underlying_type>::tinyness_before;

    static constexpr type min() { return type{static_cast<underlying_type>(boost::safe_numbers::detail::signed_raw_value(Min))}; }
    static constexpr type max() { return type{static_cast<underlying_type>(boost::safe_numbers::detail::signed_raw_value(Max))}; }
    static constexpr type lowest() { return min(); }
    static constexpr type epsilon() { return min(); }
    static constexpr type round_error() { return min(); }
    static constexpr type infinity() { return min(); }
    static constexpr type quiet_NaN() { return min(); }
    static constexpr type signaling_NaN() { return min(); }
    static constexpr type denorm_min() { return min(); }
};

#if BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT

template <auto Min, auto Max>
class numeric_limits<boost::safe_numbers::bounded_float<Min, Max>>
{
    using type = boost::safe_numbers::bounded_float<Min, Max>;
    using underlying_type = boost::safe_numbers::detail::underlying_type_t<type>;

public:
    static constexpr bool is_specialized = std::numeric_limits<underlying_type>::is_specialized;
    static constexpr bool is_signed = std::numeric_limits<underlying_type>::is_signed;
    static constexpr bool is_integer = false;
    static constexpr bool is_exact = false;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;

    #if ((!defined(_MSC_VER) && (__cplusplus <= 202002L)) || (defined(_MSC_VER) && (_MSVC_LANG <= 202002L)))
    static constexpr std::float_denorm_style has_denorm = std::numeric_limits<underlying_type>::has_denorm;
    static constexpr bool has_denorm_loss = std::numeric_limits<underlying_type>::has_denorm_loss;
    #endif

    static constexpr std::float_round_style round_style = std::numeric_limits<underlying_type>::round_style;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = false;
    static constexpr int digits = std::numeric_limits<underlying_type>::digits;
    static constexpr int digits10 = std::numeric_limits<underlying_type>::digits10;
    static constexpr int max_digits10 = std::numeric_limits<underlying_type>::max_digits10;
    static constexpr int radix = std::numeric_limits<underlying_type>::radix;
    static constexpr int min_exponent = std::numeric_limits<underlying_type>::min_exponent;
    static constexpr int min_exponent10 = std::numeric_limits<underlying_type>::min_exponent10;
    static constexpr int max_exponent = std::numeric_limits<underlying_type>::max_exponent;
    static constexpr int max_exponent10 = std::numeric_limits<underlying_type>::max_exponent10;
    static constexpr bool traps = std::numeric_limits<underlying_type>::traps;
    static constexpr bool tinyness_before = std::numeric_limits<underlying_type>::tinyness_before;

    static constexpr type min() { return type{static_cast<underlying_type>(boost::safe_numbers::detail::float_raw_value(Min))}; }
    static constexpr type max() { return type{static_cast<underlying_type>(boost::safe_numbers::detail::float_raw_value(Max))}; }
    static constexpr type lowest() { return min(); }
    static constexpr type epsilon() { return min(); }
    static constexpr type round_error() { return min(); }
    static constexpr type infinity() { return min(); }
    static constexpr type quiet_NaN() { return min(); }
    static constexpr type signaling_NaN() { return min(); }
    static constexpr type denorm_min() { return min(); }
};

#endif // BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT

#ifdef __clang__
#  pragma clang diagnostic pop
#endif

} // namespace std

#endif // BOOST_SAFE_NUMBERS_LIMITS_HPP
// ===== END boost/safe_numbers/limits.hpp =====
// ===== BEGIN boost/safe_numbers/literals.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_LITERALS_HPP
#define BOOST_SAFE_NUMBERS_LITERALS_HPP

// ===== BEGIN boost/safe_numbers/detail/int128/literals.hpp =====
// Copyright 2022 Peter Dimov
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_LITERALS_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_LITERALS_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/int128.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/mini_from_chars.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/utilities.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/config.hpp

namespace boost {
namespace int128 {
namespace literals {

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator ""_u128(const char* str) noexcept
{
    uint128_t result {};
    detail::from_chars(str, str + detail::strlen(str), result);
    return result;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator ""_U128(const char* str) noexcept
{
    uint128_t result {};
    detail::from_chars(str, str + detail::strlen(str), result);
    return result;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator ""_u128(const char* str, std::size_t len) noexcept
{
    uint128_t result {};
    detail::from_chars(str, str + len, result);
    return result;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t operator ""_U128(const char* str, std::size_t len) noexcept
{
    uint128_t result {};
    detail::from_chars(str, str + len, result);
    return result;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator ""_i128(const char* str) noexcept
{
    int128_t result {};
    detail::from_chars(str, str + detail::strlen(str), result);
    return result;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator ""_I128(const char* str) noexcept
{
    int128_t result {};
    detail::from_chars(str, str + detail::strlen(str), result);
    return result;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator ""_i128(const char* str, std::size_t len) noexcept
{
    int128_t result {};
    detail::from_chars(str, str + len, result);
    return result;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t operator ""_I128(const char* str, std::size_t len) noexcept
{
    int128_t result {};
    detail::from_chars(str, str + len, result);
    return result;
}

} // namespace literals
} // namespace int128
} // namespace boost

#define BOOST_SAFE_NUMBERS_DETAIL_INT128_STRINGIFY(x) #x
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_UINT128_C(x) boost::int128::literals::operator""_u128(BOOST_SAFE_NUMBERS_DETAIL_INT128_STRINGIFY(x))
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_INT128_C(x) boost::int128::literals::operator""_i128(BOOST_SAFE_NUMBERS_DETAIL_INT128_STRINGIFY(x))

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_LITERALS_HPP
// ===== END boost/safe_numbers/detail/int128/literals.hpp =====
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/throw_exception.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/unsigned_integers.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/signed_integers.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/floats.hpp

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <limits>
#include <cstdint>

#endif

namespace boost::safe_numbers::literals {

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable:4702) // Unreachable code is only true with exceptions enabled
#endif

BOOST_SAFE_NUMBERS_EXPORT constexpr auto operator ""_u8(const unsigned long long int val) -> u8
{
    if (constexpr unsigned long long int max_value {std::numeric_limits<std::uint8_t>::max()}; val > max_value)
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "Overflow detected in literal construction");
    }

    return static_cast<u8>(static_cast<std::uint8_t>(val));
}

BOOST_SAFE_NUMBERS_EXPORT constexpr auto operator ""_u16(const unsigned long long int val) -> u16
{
    if (constexpr unsigned long long int max_value {std::numeric_limits<std::uint16_t>::max()}; val > max_value)
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "Overflow detected in literal construction");
    }

    return static_cast<u16>(static_cast<std::uint16_t>(val));
}

BOOST_SAFE_NUMBERS_EXPORT constexpr auto operator ""_u32(const unsigned long long int val) -> u32
{
    if (constexpr unsigned long long int max_value {std::numeric_limits<std::uint32_t>::max()}; val > max_value)
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "Overflow detected in literal construction");
    }

    return static_cast<u32>(static_cast<std::uint32_t>(val));
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

BOOST_SAFE_NUMBERS_EXPORT constexpr auto operator ""_u64(const unsigned long long int val) noexcept -> u64
{
    static_assert(std::numeric_limits<std::uint64_t>::max() == std::numeric_limits<unsigned long long int>::max());
    return static_cast<u64>(static_cast<std::uint64_t>(val));
}

BOOST_SAFE_NUMBERS_EXPORT constexpr auto operator ""_u128(const char* str) -> u128
{
    int128::uint128_t result;
    const auto r {int128::detail::from_chars(str, str + int128::detail::strlen(str), result)};

    switch (r)
    {
        case EDOM:
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "Overflow detected in literal construction");
        case EINVAL:
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::invalid_argument, "Invalid conversion from literal");
        default:
            static_cast<void>(r);
    }

    return u128{result};
}

BOOST_SAFE_NUMBERS_EXPORT constexpr auto operator ""_i8(const unsigned long long int val) -> i8
{
    if (constexpr unsigned long long int max_value {std::numeric_limits<std::int8_t>::max()}; val > max_value)
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "Overflow detected in literal construction");
    }

    return static_cast<i8>(static_cast<std::int8_t>(val));
}

BOOST_SAFE_NUMBERS_EXPORT constexpr auto operator ""_i16(const unsigned long long int val) -> i16
{
    if (constexpr unsigned long long int max_value {std::numeric_limits<std::int16_t>::max()}; val > max_value)
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "Overflow detected in literal construction");
    }

    return static_cast<i16>(static_cast<std::int16_t>(val));
}

BOOST_SAFE_NUMBERS_EXPORT constexpr auto operator ""_i32(const unsigned long long int val) -> i32
{
    if (constexpr unsigned long long int max_value {std::numeric_limits<std::int32_t>::max()}; val > max_value)
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "Overflow detected in literal construction");
    }

    return static_cast<i32>(static_cast<std::int32_t>(val));
}

BOOST_SAFE_NUMBERS_EXPORT constexpr auto operator ""_i64(const unsigned long long int val) -> i64
{
    if (constexpr unsigned long long int max_value {std::numeric_limits<std::int64_t>::max()}; val > max_value)
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "Overflow detected in literal construction");
    }

    return static_cast<i64>(static_cast<std::int64_t>(val));
}

BOOST_SAFE_NUMBERS_EXPORT constexpr auto operator ""_i128(const char* str) -> i128
{
    int128::int128_t result;
    const auto r {int128::detail::from_chars(str, str + int128::detail::strlen(str), result)};

    switch (r)
    {
        case EDOM:
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "Overflow detected in literal construction");
        case EINVAL:
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::invalid_argument, "Invalid conversion from literal");
        default:
            static_cast<void>(r);
    }

    return i128{result};
}

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4756)
#endif

BOOST_SAFE_NUMBERS_EXPORT constexpr auto operator ""_f32(const long double val) -> f32
{
    if (constexpr auto max_value {static_cast<long double>(std::numeric_limits<float>::max())}; val > max_value)
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "Overflow detected in literal construction");
    }

    return static_cast<f32>(static_cast<float>(val));
}

BOOST_SAFE_NUMBERS_EXPORT constexpr auto operator ""_f64(const long double val) -> f64
{
    if (constexpr auto max_value {static_cast<long double>(std::numeric_limits<double>::max())}; val > max_value)
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::overflow_error, "Overflow detected in literal construction");
    }

    return static_cast<f64>(static_cast<double>(val));
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

}  // boost::safe_numbers::literals

#endif // BOOST_SAFE_NUMBERS_LITERALS_HPP
// ===== END boost/safe_numbers/literals.hpp =====
// ===== BEGIN boost/safe_numbers/format.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_FORMAT_HPP
#define BOOST_SAFE_NUMBERS_FORMAT_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/unsigned_integer_basis.hpp
// ===== BEGIN boost/safe_numbers/detail/int128/format.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_FORMAT_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_FORMAT_HPP

#if __has_include(<format>) && defined(__cpp_lib_format) && __cpp_lib_format >= 201907L && !defined(BOOST_DECIMAL_DISABLE_CLIB)

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/mini_to_chars.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/config.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/int128.hpp
#include <string>
#include <format>
#include <tuple>

#define BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_FORMAT

namespace boost::int128::detail {

enum class sign_option
{
    plus,
    negative,
    space
};

enum class alignment
{
    none,
    left,   // <
    right,  // >
    center  // ^
};

template <typename ParseContext>
constexpr auto parse_impl(ParseContext& ctx)
{
    auto it {ctx.begin()};
    int base = 10;
    bool is_upper = false;
    int padding_digits = 0;
    auto sign = sign_option::negative;
    bool prefix = false;
    bool zero_pad = false;
    char fill_char = ' ';
    auto align = alignment::none;

    // Parse fill and alignment: [[fill]align]
    // Alignment characters are: < (left), > (right), ^ (center)
    if (it != ctx.end())
    {
        // Check if we have [fill]align (fill char followed by alignment)
        auto next = it;
        ++next;
        if (next != ctx.end() && (*next == '<' || *next == '>' || *next == '^'))
        {
            fill_char = *it;
            it = next;
            switch (*it)
            {
                case '<':
                    align = alignment::left;
                    break;
                case '>':
                    align = alignment::right;
                    break;
                case '^':
                    align = alignment::center;
                    break;
                default:                        // LCOV_EXCL_LINE
                    BOOST_SAFE_NUMBERS_DETAIL_INT128_UNREACHABLE;   // LCOV_EXCL_LINE
            }
            ++it;
        }
        // Check if we just have align (no fill char)
        else if (*it == '<' || *it == '>' || *it == '^')
        {
            switch (*it)
            {
                case '<':
                    align = alignment::left;
                    break;
                case '>':
                    align = alignment::right;
                    break;
                case '^':
                    align = alignment::center;
                    break;
                default:                        // LCOV_EXCL_LINE
                    BOOST_SAFE_NUMBERS_DETAIL_INT128_UNREACHABLE;   // LCOV_EXCL_LINE
            }
            ++it;
        }
    }

    // Handle sign or space
    if (it != ctx.end())
    {
        switch (*it) {
            case ' ':
                sign = sign_option::space;
                ++it;
                break;
            case '+':
                sign = sign_option::plus;
                ++it;
                break;
            case '-':
                sign = sign_option::negative;
                ++it;
                break;
            default:
                break;
        }
    }

    // Alternate form option
    if (it != ctx.end() && *it == '#')
    {
        prefix = true;
        ++it;
    }

    // Zero-pad flag (std::format places '0' before the width)
    if (it != ctx.end() && *it == '0')
    {
        zero_pad = true;
        ++it;
    }

    // Parse the width
    while (it != ctx.end() && *it >= '0' && *it <= '9')
    {
        padding_digits = padding_digits * 10 + (*it - '0');
        ++it;
    }

    // Integer presentation
    if (it != ctx.end() && *it != '}')
    {
        switch (*it++)
        {
            case 'b':
                base = 2;
                break;
            case 'B':
                base = 2;
                is_upper = true;
                break;

            case 'o':
                base = 8;
                break;

            case 'd':
                base = 10;
                break;

            case 'x':
                base = 16;
                break;
            case 'X':
                base = 16;
                is_upper = true;
                break;
            default:                                                                                // LCOV_EXCL_LINE
                BOOST_SAFE_NUMBERS_DETAIL_INT128_THROW_EXCEPTION(std::format_error("Unsupported format specifier"));    // LCOV_EXCL_LINE
        }
    }

    // Verify we're at the closing brace
    if (it != ctx.end() && *it != '}')
    {
        BOOST_SAFE_NUMBERS_DETAIL_INT128_THROW_EXCEPTION(std::format_error("Expected '}' in format string")); // LCOV_EXCL_LINE
    }

    return std::make_tuple(base, padding_digits, sign, is_upper, prefix, zero_pad, fill_char, align, it);
}

template <typename T>
struct is_library_type_impl
{
    static constexpr bool value {std::is_same_v<T, boost::int128::uint128_t> || std::is_same_v<T, boost::int128::int128_t>};
};

template <typename T>
BOOST_SAFE_NUMBERS_DETAIL_INT128_INLINE_CONSTEXPR bool is_library_type_v = is_library_type_impl<T>::value;

template <typename T>
concept is_library_type = is_library_type_v<T>;

} // namespace boost::int128::detail

namespace std {

template <boost::int128::detail::is_library_type T>
struct formatter<T>
{
    int base;
    int padding_digits;
    boost::int128::detail::sign_option sign;
    bool is_upper;
    bool prefix;
    bool zero_pad;
    char fill_char;
    boost::int128::detail::alignment align;

    constexpr formatter() : base {10},
                            padding_digits {0},
                            sign {boost::int128::detail::sign_option::negative},
                            is_upper {false},
                            prefix {false},
                            zero_pad {false},
                            fill_char {' '},
                            align {boost::int128::detail::alignment::none}
    {}

    constexpr auto parse(format_parse_context& ctx)
    {
        const auto res {boost::int128::detail::parse_impl(ctx)};

        base = std::get<0>(res);
        padding_digits = std::get<1>(res);
        sign = std::get<2>(res);
        is_upper = std::get<3>(res);
        prefix = std::get<4>(res);
        zero_pad = std::get<5>(res);
        fill_char = std::get<6>(res);
        align = std::get<7>(res);

        return std::get<8>(res);
    }

    template <typename FormatContext>
    auto format(T v, FormatContext& ctx) const
    {
        char buffer[boost::int128::detail::mini_to_chars_buffer_size];
        bool isneg {false};
        boost::int128::uint128_t abs_v {};

        if constexpr (std::is_same_v<T, boost::int128::int128_t>)
        {
            if (v < 0)
            {
                isneg = true;
                // Can't negate int128_t::min(), handle specially
                if (v == (std::numeric_limits<T>::min)())
                {
                    abs_v = boost::int128::uint128_t{UINT64_C(0x8000000000000000), 0};
                }
                else
                {
                    abs_v = static_cast<boost::int128::uint128_t>(-v);
                }
            }
            else
            {
                abs_v = static_cast<boost::int128::uint128_t>(v);
            }
        }
        else
        {
            abs_v = static_cast<boost::int128::uint128_t>(v);
        }

        const auto end = boost::int128::detail::mini_to_chars(buffer, abs_v, base, is_upper);
        std::string s(end, buffer + sizeof(buffer));

        // Calculate prefix length that will be added later
        std::size_t prefix_len {0};
        if (prefix)
        {
            switch (base)
            {
                case 2:
                case 16:
                    prefix_len = 2;  // "0b", "0B", "0x", or "0X"
                    break;
                case 8:
                    prefix_len = 1;  // "0"
                    break;
                default:
                    break;
            }
        }

        // Calculate sign length that will be added later
        std::size_t sign_len {0};
        if (sign == boost::int128::detail::sign_option::plus || sign == boost::int128::detail::sign_option::space || isneg)
        {
            sign_len = 1;
        }

        // Zero-padding applies only with the '0' flag and no explicit alignment.
        // Account for prefix and sign in the padding calculation.
        if (zero_pad && align == boost::int128::detail::alignment::none && padding_digits > 0)
        {
            auto target_digit_width {static_cast<std::size_t>(padding_digits)};
            if (target_digit_width > prefix_len + sign_len)
            {
                target_digit_width -= prefix_len + sign_len;
            }
            else
            {
                target_digit_width = 0;
            }

            if (s.size() - 1u < target_digit_width)
            {
                s.insert(s.begin(), target_digit_width - s.size() + 1u, '0');
            }
        }

        if (prefix)
        {
            switch (base)
            {
                case 2:
                    if (is_upper)
                    {
                        s.insert(s.begin(), 'B');
                    }
                    else
                    {
                        s.insert(s.begin(), 'b');
                    }
                    s.insert(s.begin(), '0');
                    break;
                case 8:
                    s.insert(s.begin(), '0');
                    break;
                case 16:
                    if (is_upper)
                    {
                        s.insert(s.begin(), 'X');
                    }
                    else
                    {
                        s.insert(s.begin(), 'x');
                    }
                    s.insert(s.begin(), '0');
                    break;
                default:
                    // Nothing to do
                    break;
            }
        }

        // Insert our sign
        switch (sign)
        {
            case boost::int128::detail::sign_option::plus:
                if (isneg)
                {
                    s.insert(s.begin(), '-');
                }
                else
                {
                    s.insert(s.begin(), '+');
                }
                break;
            case boost::int128::detail::sign_option::space:
                if (!isneg)
                {
                    s.insert(s.begin(), ' ');
                }
                if constexpr (std::is_same_v<T, boost::int128::int128_t>)
                {
                    if (isneg)
                    {
                        s.insert(s.begin(), '-');
                    }
                }
                break;
            case boost::int128::detail::sign_option::negative:
                if constexpr (std::is_same_v<T, boost::int128::int128_t>)
                {
                    if (isneg)
                    {
                        s.insert(s.begin(), '-');
                    }
                }
                break;
            // LCOV_EXCL_START
            default:
                BOOST_SAFE_NUMBERS_DETAIL_INT128_UNREACHABLE;
            // LCOV_EXCL_STOP
        }

        s.erase(0, s.find_first_not_of('\0'));
        s.erase(s.find_last_not_of('\0') + 1);

        // Apply alignment. An explicit alignment uses fill_char; with no explicit
        // alignment and no zero-padding, integer types default to right alignment
        // with the fill character (matching std::format).
        if (s.size() < static_cast<std::size_t>(padding_digits) &&
            (align != boost::int128::detail::alignment::none || !zero_pad))
        {
            auto fill_count = static_cast<std::size_t>(padding_digits) - s.size();
            const auto effective_align = (align == boost::int128::detail::alignment::none)
                                             ? boost::int128::detail::alignment::right
                                             : align;
            switch (effective_align)
            {
                case boost::int128::detail::alignment::left:
                    s.append(fill_count, fill_char);
                    break;
                case boost::int128::detail::alignment::right:
                    s.insert(s.begin(), fill_count, fill_char);
                    break;
                case boost::int128::detail::alignment::center:
                {
                    auto left_fill = fill_count / 2;
                    auto right_fill = fill_count - left_fill;
                    s.insert(s.begin(), left_fill, fill_char);
                    s.append(right_fill, fill_char);
                    break;
                }
                    // LCOV_EXCL_START
                default:
                    break;
                    // LCOV_EXCL_STOP
            }
        }

        return std::format_to(ctx.out(), "{}", s);
    }
};

} // namespace std

#endif

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_FORMAT_HPP
// ===== END boost/safe_numbers/detail/int128/format.hpp =====
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/type_traits.hpp

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_FORMAT

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <format>
#include <concepts>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

template <boost::safe_numbers::detail::library_type T>
struct std::formatter<T>
    : std::formatter<boost::safe_numbers::detail::underlying_type_t<T>>
{
    using underlying_type = boost::safe_numbers::detail::underlying_type_t<T>;

    auto format(const T& val, std::format_context& ctx) const
    {
        return std::formatter<underlying_type>::format(static_cast<underlying_type>(val), ctx);
    }
};

#endif // Has format

#endif // BOOST_SAFE_NUMBERS_FORMAT_HPP
// ===== END boost/safe_numbers/format.hpp =====
// ===== BEGIN boost/safe_numbers/charconv.hpp =====
// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_CHARCONV_HPP
#define BOOST_SAFE_NUMBERS_CHARCONV_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/config.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/type_traits.hpp
// ===== BEGIN boost/safe_numbers/detail/int128/charconv.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// This header injects additional functionality into the Boost.Charconv namespace
// If this library is accepted into boost this functionality could be moved their

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_CHARCONV_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_CHARCONV_HPP

#if __has_include(<boost/charconv.hpp>)

// Define for the user automatically,
// otherwise we'll have an ever-increasing number of these required as we go down the dependency chain
#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_CUDA) && !defined(BOOST_CHARCONV_ENABLE_CUDA)
#  define BOOST_CHARCONV_ENABLE_CUDA
#endif

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_ENABLE_SYCL) && !defined(BOOST_CHARCONV_ENABLE_SYCL)
#  define BOOST_CHARCONV_ENABLE_SYCL
#endif

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/int128.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/literals.hpp
#include <boost/charconv.hpp>
#include <boost/core/detail/string_view.hpp>

namespace boost {
namespace charconv {

namespace detail {

template <>
struct is_signed<int128::uint128_t> { static constexpr bool value = false; };

template <>
struct is_signed<int128::int128_t> { static constexpr bool value = true; };

template <>
struct make_unsigned<int128::uint128_t> { using type = int128::uint128_t; };

template <>
struct make_unsigned<int128::int128_t> { using type = int128::uint128_t; };

template <>
struct make_signed<int128::uint128_t> { using type = int128::int128_t; };

template <>
struct make_signed<int128::int128_t> { using type = int128::int128_t; };

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_GPU_SUPPORT)

template <>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128::uint128_t get_max_value<int128::uint128_t>()
{
    return std::numeric_limits<int128::uint128_t>::max();
}

template <>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128::int128_t get_max_value<int128::int128_t>()
{
    return std::numeric_limits<int128::int128_t>::max();
}

#endif

#if !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_GPU_SUPPORT)

BOOST_SAFE_NUMBERS_DETAIL_INT128_INLINE_CONSTEXPR int128::uint128_t int128_pow10[39] =
{
    int128::uint128_t{UINT64_C(0x0), UINT64_C(0x1)},
    int128::uint128_t{UINT64_C(0x0), UINT64_C(0xa)},
    int128::uint128_t{UINT64_C(0x0), UINT64_C(0x64)},
    int128::uint128_t{UINT64_C(0x0), UINT64_C(0x3e8)},
    int128::uint128_t{UINT64_C(0x0), UINT64_C(0x2710)},
    int128::uint128_t{UINT64_C(0x0), UINT64_C(0x186a0)},
    int128::uint128_t{UINT64_C(0x0), UINT64_C(0xf4240)},
    int128::uint128_t{UINT64_C(0x0), UINT64_C(0x989680)},
    int128::uint128_t{UINT64_C(0x0), UINT64_C(0x5f5e100)},
    int128::uint128_t{UINT64_C(0x0), UINT64_C(0x3b9aca00)},
    int128::uint128_t{UINT64_C(0x0), UINT64_C(0x2540be400)},
    int128::uint128_t{UINT64_C(0x0), UINT64_C(0x174876e800)},
    int128::uint128_t{UINT64_C(0x0), UINT64_C(0xe8d4a51000)},
    int128::uint128_t{UINT64_C(0x0), UINT64_C(0x9184e72a000)},
    int128::uint128_t{UINT64_C(0x0), UINT64_C(0x5af3107a4000)},
    int128::uint128_t{UINT64_C(0x0), UINT64_C(0x38d7ea4c68000)},
    int128::uint128_t{UINT64_C(0x0), UINT64_C(0x2386f26fc10000)},
    int128::uint128_t{UINT64_C(0x0), UINT64_C(0x16345785d8a0000)},
    int128::uint128_t{UINT64_C(0x0), UINT64_C(0xde0b6b3a7640000)},
    int128::uint128_t{UINT64_C(0x0), UINT64_C(0x8ac7230489e80000)},
    int128::uint128_t{UINT64_C(0x5), UINT64_C(0x6bc75e2d63100000)},
    int128::uint128_t{UINT64_C(0x36), UINT64_C(0x35c9adc5dea00000)},
    int128::uint128_t{UINT64_C(0x21e), UINT64_C(0x19e0c9bab2400000)},
    int128::uint128_t{UINT64_C(0x152d), UINT64_C(0x2c7e14af6800000)},
    int128::uint128_t{UINT64_C(0xd3c2), UINT64_C(0x1bcecceda1000000)},
    int128::uint128_t{UINT64_C(0x84595), UINT64_C(0x161401484a000000)},
    int128::uint128_t{UINT64_C(0x52b7d2), UINT64_C(0xdcc80cd2e4000000)},
    int128::uint128_t{UINT64_C(0x33b2e3c), UINT64_C(0x9fd0803ce8000000)},
    int128::uint128_t{UINT64_C(0x204fce5e), UINT64_C(0x3e25026110000000)},
    int128::uint128_t{UINT64_C(0x1431e0fae), UINT64_C(0x6d7217caa0000000)},
    int128::uint128_t{UINT64_C(0xc9f2c9cd0), UINT64_C(0x4674edea40000000)},
    int128::uint128_t{UINT64_C(0x7e37be2022), UINT64_C(0xc0914b2680000000)},
    int128::uint128_t{UINT64_C(0x4ee2d6d415b), UINT64_C(0x85acef8100000000)},
    int128::uint128_t{UINT64_C(0x314dc6448d93), UINT64_C(0x38c15b0a00000000)},
    int128::uint128_t{UINT64_C(0x1ed09bead87c0), UINT64_C(0x378d8e6400000000)},
    int128::uint128_t{UINT64_C(0x13426172c74d82), UINT64_C(0x2b878fe800000000)},
    int128::uint128_t{UINT64_C(0xc097ce7bc90715), UINT64_C(0xb34b9f1000000000)},
    int128::uint128_t{UINT64_C(0x785ee10d5da46d9), UINT64_C(0xf436a000000000)},
    int128::uint128_t{UINT64_C(0x4b3b4ca85a86c47a), UINT64_C(0x98a224000000000)}
};

#endif // __NVCC__

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int num_digits(const int128::uint128_t& x) noexcept
{
    #if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_GPU_SUPPORT)

    constexpr int128::uint128_t int128_pow10[39] =
    {
        int128::uint128_t{UINT64_C(0x0), UINT64_C(0x1)},
        int128::uint128_t{UINT64_C(0x0), UINT64_C(0xa)},
        int128::uint128_t{UINT64_C(0x0), UINT64_C(0x64)},
        int128::uint128_t{UINT64_C(0x0), UINT64_C(0x3e8)},
        int128::uint128_t{UINT64_C(0x0), UINT64_C(0x2710)},
        int128::uint128_t{UINT64_C(0x0), UINT64_C(0x186a0)},
        int128::uint128_t{UINT64_C(0x0), UINT64_C(0xf4240)},
        int128::uint128_t{UINT64_C(0x0), UINT64_C(0x989680)},
        int128::uint128_t{UINT64_C(0x0), UINT64_C(0x5f5e100)},
        int128::uint128_t{UINT64_C(0x0), UINT64_C(0x3b9aca00)},
        int128::uint128_t{UINT64_C(0x0), UINT64_C(0x2540be400)},
        int128::uint128_t{UINT64_C(0x0), UINT64_C(0x174876e800)},
        int128::uint128_t{UINT64_C(0x0), UINT64_C(0xe8d4a51000)},
        int128::uint128_t{UINT64_C(0x0), UINT64_C(0x9184e72a000)},
        int128::uint128_t{UINT64_C(0x0), UINT64_C(0x5af3107a4000)},
        int128::uint128_t{UINT64_C(0x0), UINT64_C(0x38d7ea4c68000)},
        int128::uint128_t{UINT64_C(0x0), UINT64_C(0x2386f26fc10000)},
        int128::uint128_t{UINT64_C(0x0), UINT64_C(0x16345785d8a0000)},
        int128::uint128_t{UINT64_C(0x0), UINT64_C(0xde0b6b3a7640000)},
        int128::uint128_t{UINT64_C(0x0), UINT64_C(0x8ac7230489e80000)},
        int128::uint128_t{UINT64_C(0x5), UINT64_C(0x6bc75e2d63100000)},
        int128::uint128_t{UINT64_C(0x36), UINT64_C(0x35c9adc5dea00000)},
        int128::uint128_t{UINT64_C(0x21e), UINT64_C(0x19e0c9bab2400000)},
        int128::uint128_t{UINT64_C(0x152d), UINT64_C(0x2c7e14af6800000)},
        int128::uint128_t{UINT64_C(0xd3c2), UINT64_C(0x1bcecceda1000000)},
        int128::uint128_t{UINT64_C(0x84595), UINT64_C(0x161401484a000000)},
        int128::uint128_t{UINT64_C(0x52b7d2), UINT64_C(0xdcc80cd2e4000000)},
        int128::uint128_t{UINT64_C(0x33b2e3c), UINT64_C(0x9fd0803ce8000000)},
        int128::uint128_t{UINT64_C(0x204fce5e), UINT64_C(0x3e25026110000000)},
        int128::uint128_t{UINT64_C(0x1431e0fae), UINT64_C(0x6d7217caa0000000)},
        int128::uint128_t{UINT64_C(0xc9f2c9cd0), UINT64_C(0x4674edea40000000)},
        int128::uint128_t{UINT64_C(0x7e37be2022), UINT64_C(0xc0914b2680000000)},
        int128::uint128_t{UINT64_C(0x4ee2d6d415b), UINT64_C(0x85acef8100000000)},
        int128::uint128_t{UINT64_C(0x314dc6448d93), UINT64_C(0x38c15b0a00000000)},
        int128::uint128_t{UINT64_C(0x1ed09bead87c0), UINT64_C(0x378d8e6400000000)},
        int128::uint128_t{UINT64_C(0x13426172c74d82), UINT64_C(0x2b878fe800000000)},
        int128::uint128_t{UINT64_C(0xc097ce7bc90715), UINT64_C(0xb34b9f1000000000)},
        int128::uint128_t{UINT64_C(0x785ee10d5da46d9), UINT64_C(0xf436a000000000)},
        int128::uint128_t{UINT64_C(0x4b3b4ca85a86c47a), UINT64_C(0x98a224000000000)}
    };

    #endif // __NVCC__

    if (x.high == UINT64_C(0))
    {
        return num_digits(x.low);
    }

    // Use the most significant bit position to approximate log10
    // log10(x) ~= log2(x) / log2(10) ~= log2(x) / 3.32

    const auto msb {64 + (63 - int128::detail::countl_zero(x.high))};
    
    // Approximate log10
    const auto estimated_digits {(msb * 1000) / 3322 + 1};

    if (estimated_digits < 39 && x >= int128_pow10[static_cast<std::size_t>(estimated_digits)])
    {
        return estimated_digits + 1;
    }

    // Estimated digits can't be less than 20 digits (65-bits minimum)
    if (x < int128_pow10[estimated_digits - 1])
    {
        return estimated_digits - 1;
    }

    return estimated_digits;
}

} // namespace detail

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_CHARCONV_CONSTEXPR to_chars_result to_chars(char* first, char* last, const int128::uint128_t value, const int base = 10) noexcept
{
    #if !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_GPU_SUPPORT)

    if (base == 10)
    {
        return detail::to_chars_128integer_impl<int128::uint128_t, int128::uint128_t>(first, last, value);
    }

    #endif // __NVCC__

    return detail::to_chars_integer_impl<int128::uint128_t, int128::uint128_t>(first, last, value, base);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_CHARCONV_CONSTEXPR to_chars_result to_chars(char* first, char* last, const int128::int128_t value, const int base = 10) noexcept
{
    #if !defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_GPU_SUPPORT)

    if (base == 10)
    {
        return detail::to_chars_128integer_impl<int128::int128_t, int128::uint128_t>(first, last, value);
    }

    #endif // __NVCC__

    return detail::to_chars_integer_impl<int128::int128_t, int128::uint128_t>(first, last, value, base);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_CHARCONV_GCC5_CONSTEXPR from_chars_result from_chars(const char* first, const char* last, int128::uint128_t& value, const int base = 10) noexcept
{
    return detail::from_chars_integer_impl<int128::uint128_t, int128::uint128_t>(first, last, value, base);
}

BOOST_CHARCONV_GCC5_CONSTEXPR from_chars_result from_chars(core::string_view sv, int128::uint128_t& value, const int base = 10) noexcept
{
    return detail::from_chars_integer_impl<int128::uint128_t, int128::uint128_t>(sv.data(), sv.data() + sv.size(), value, base);
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE BOOST_CHARCONV_GCC5_CONSTEXPR from_chars_result from_chars(const char* first, const char* last, int128::int128_t& value, const int base = 10) noexcept
{
    return detail::from_chars_integer_impl<int128::int128_t, int128::uint128_t>(first, last, value, base);
}

BOOST_CHARCONV_GCC5_CONSTEXPR from_chars_result from_chars(core::string_view sv, int128::int128_t& value, int base = 10) noexcept
{
    return detail::from_chars_integer_impl<int128::int128_t, int128::uint128_t>(sv.data(), sv.data() + sv.size(), value, base);
}

} // namespace charconv
} // namespace boost

#else

#error "This header requires Boost.Charconv to be present"

#endif // __has_include(<boost/charconv.hpp>)

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_CHARCONV_HPP
// ===== END boost/safe_numbers/detail/int128/charconv.hpp =====

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/charconv.hpp>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::charconv {

template <safe_numbers::detail::integral_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto from_chars(const char* first, const char* last, T& value, int base = 10)
    -> charconv::from_chars_result
{
    using underlying_type = safe_numbers::detail::underlying_type_t<T>;

    underlying_type result {};
    const auto r {charconv::from_chars(first, last, result, base)};
    value = T{result};

    return r;
}

template <safe_numbers::detail::integral_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto to_chars(char* first, char* last, const T value, int base = 10)
    -> charconv::to_chars_result
{
    using underlying_type = safe_numbers::detail::underlying_type_t<T>;
    return charconv::to_chars(first, last, static_cast<underlying_type>(value), base);
}

template <safe_numbers::detail::float_library_type T>
auto from_chars(const char* first, const char* last, T& value, chars_format fmt = chars_format::general)
    -> charconv::from_chars_result
{
    using underlying_type = safe_numbers::detail::underlying_type_t<T>;

    underlying_type result {};
    const auto r {charconv::from_chars(first, last, result, fmt)};
    value = T{result};

    return r;
}

template <safe_numbers::detail::float_library_type T>
auto to_chars(char* first, char* last, const T value, chars_format fmt = chars_format::general)
    -> charconv::to_chars_result
{
    using underlying_type = safe_numbers::detail::underlying_type_t<T>;
    return charconv::to_chars(first, last, static_cast<underlying_type>(value), fmt);
}

template <safe_numbers::detail::float_library_type T>
auto to_chars(char* first, char* last, const T value, chars_format fmt, int precision)
    -> charconv::to_chars_result
{
    using underlying_type = safe_numbers::detail::underlying_type_t<T>;
    return charconv::to_chars(first, last, static_cast<underlying_type>(value), fmt, precision);
}

} // namespace boost::charconv

#endif // BOOST_SAFE_NUMBERS_CHARCONV_HPP
// ===== END boost/safe_numbers/charconv.hpp =====
// ===== BEGIN boost/safe_numbers/bit.hpp =====
// Copyright 2020 Peter Dimov
// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_BIT_HPP
#define BOOST_SAFE_NUMBERS_BIT_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/unsigned_integer_basis.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/type_traits.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/bit.hpp

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <array>

#if (defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))

#include <cuda/std/bit>

#else

#include <boost/core/bit.hpp>
#include <bit>

#endif

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers {

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto has_single_bit(const UnsignedInt x) noexcept -> bool
{
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    #if !(defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))

    using boost::core::has_single_bit;
    return has_single_bit(static_cast<underlying_type>(x));

    #else

    if constexpr (std::is_same_v<detail::underlying_type_t<UnsignedInt>, int128::uint128_t>)
    {
        return boost::int128::has_single_bit(static_cast<underlying_type>(x));
    }
    else
    {
        return cuda::std::has_single_bit(static_cast<underlying_type>(x));
    }

    #endif
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto bit_ceil(const UnsignedInt x) noexcept -> UnsignedInt
{
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    #if !(defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))

    using boost::core::bit_ceil;
    return UnsignedInt{bit_ceil(static_cast<underlying_type>(x))};

    #else

    if constexpr (std::is_same_v<detail::underlying_type_t<UnsignedInt>, int128::uint128_t>)
    {
        return UnsignedInt{boost::int128::bit_ceil(static_cast<underlying_type>(x))};
    }
    else
    {
        return UnsignedInt{cuda::std::bit_ceil(static_cast<underlying_type>(x))};
    }

    #endif
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto bit_floor(const UnsignedInt x) noexcept -> UnsignedInt
{
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    #if !(defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))

    using boost::core::bit_floor;
    return UnsignedInt{bit_floor(static_cast<underlying_type>(x))};

    #else

    if constexpr (std::is_same_v<detail::underlying_type_t<UnsignedInt>, int128::uint128_t>)
    {
        return UnsignedInt{boost::int128::bit_floor(static_cast<underlying_type>(x))};
    }
    else
    {
        return UnsignedInt{cuda::std::bit_floor(static_cast<underlying_type>(x))};
    }

    #endif
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto bit_width(const UnsignedInt x) noexcept -> int
{
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    #if !(defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))

    using boost::core::bit_width;
    return static_cast<int>(bit_width(static_cast<underlying_type>(x)));

    #else

    if constexpr (std::is_same_v<detail::underlying_type_t<UnsignedInt>, int128::uint128_t>)
    {
        return static_cast<int>(boost::int128::bit_width(static_cast<underlying_type>(x)));
    }
    else
    {
        return static_cast<int>(cuda::std::bit_width(static_cast<underlying_type>(x)));
    }

    #endif
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::non_bounded_unsigned_library_type UnsignedInt>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto rotl(const UnsignedInt x, const int s) noexcept -> UnsignedInt
{
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    #if !(defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))

    using boost::core::rotl;
    return UnsignedInt{rotl(static_cast<underlying_type>(x), s)};

    #else

    if constexpr (std::is_same_v<detail::underlying_type_t<UnsignedInt>, int128::uint128_t>)
    {
        return UnsignedInt{boost::int128::rotl(static_cast<underlying_type>(x), s)};
    }
    else
    {
        return UnsignedInt{cuda::std::rotl(static_cast<underlying_type>(x), s)};
    }

    #endif
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::non_bounded_unsigned_library_type UnsignedInt>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto rotr(const UnsignedInt x, const int s) noexcept -> UnsignedInt
{
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    #if !(defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))

    using boost::core::rotr;
    return UnsignedInt{rotr(static_cast<underlying_type>(x), s)};

    #else

    if constexpr (std::is_same_v<detail::underlying_type_t<UnsignedInt>, int128::uint128_t>)
    {
        return UnsignedInt{boost::int128::rotr(static_cast<underlying_type>(x), s)};
    }
    else
    {
        return UnsignedInt{cuda::std::rotr(static_cast<underlying_type>(x), s)};
    }

    #endif
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto countl_zero(const UnsignedInt x) noexcept -> int
{
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    #if !(defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))

    using boost::core::countl_zero;
    return countl_zero(static_cast<underlying_type>(x));

    #else

    if constexpr (std::is_same_v<detail::underlying_type_t<UnsignedInt>, int128::uint128_t>)
    {
        return boost::int128::countl_zero(static_cast<underlying_type>(x));
    }
    else
    {
        return cuda::std::countl_zero(static_cast<underlying_type>(x));
    }

    #endif
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto countl_one(const UnsignedInt x) noexcept -> int
{
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    #if !(defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))

    using boost::core::countl_one;
    return countl_one(static_cast<underlying_type>(x));

    #else

    if constexpr (std::is_same_v<detail::underlying_type_t<UnsignedInt>, int128::uint128_t>)
    {
        return boost::int128::countl_one(static_cast<underlying_type>(x));
    }
    else
    {
        return cuda::std::countl_one(static_cast<underlying_type>(x));
    }

    #endif
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto countr_zero(const UnsignedInt x) noexcept -> int
{
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    #if !(defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))

    using boost::core::countr_zero;
    return countr_zero(static_cast<underlying_type>(x));

    #else

    if constexpr (std::is_same_v<detail::underlying_type_t<UnsignedInt>, int128::uint128_t>)
    {
        return boost::int128::countr_zero(static_cast<underlying_type>(x));
    }
    else
    {
        return cuda::std::countr_zero(static_cast<underlying_type>(x));
    }

    #endif
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto countr_one(const UnsignedInt x) noexcept -> int
{
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    #if !(defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))

    using boost::core::countr_one;
    return countr_one(static_cast<underlying_type>(x));

    #else

    if constexpr (std::is_same_v<detail::underlying_type_t<UnsignedInt>, int128::uint128_t>)
    {
        return boost::int128::countr_one(static_cast<underlying_type>(x));
    }
    else
    {
        return cuda::std::countr_one(static_cast<underlying_type>(x));
    }

    #endif
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto popcount(const UnsignedInt x) noexcept -> int
{
    using underlying_type = detail::underlying_type_t<UnsignedInt>;

    #if !(defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))

    using boost::core::popcount;
    return popcount(static_cast<underlying_type>(x));

    #else

    if constexpr (std::is_same_v<detail::underlying_type_t<UnsignedInt>, int128::uint128_t>)
    {
        return boost::int128::popcount(static_cast<underlying_type>(x));
    }
    else
    {
        return cuda::std::popcount(static_cast<underlying_type>(x));
    }

    #endif
}

// NVCC 12 does not have byteswap builtin, only 13+
#if (defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))

namespace detail {

constexpr auto byteswap_impl(const std::uint8_t x) noexcept
{
    return x;
}

constexpr auto byteswap_impl(const std::uint16_t x) noexcept
{
    return static_cast<std::uint16_t>( x << 8 | x >> 8 );
}

constexpr auto byteswap_impl(const std::uint32_t x) noexcept
{
    const auto step16 = x << 16 | x >> 16;
    return ((step16 << 8) & 0xff00ff00) | ((step16 >> 8) & 0x00ff00ff);
}

constexpr auto byteswap_impl(const std::uint64_t x) noexcept
{
    const auto step32 = x << 32 | x >> 32;
    const auto step16 = (step32 & 0x0000FFFF0000FFFFULL) << 16 | (step32 & 0xFFFF0000FFFF0000ULL) >> 16;
    return (step16 & 0x00FF00FF00FF00FFULL) << 8 | (step16 & 0xFF00FF00FF00FF00ULL) >> 8;
}

} // namespace detail

#endif

BOOST_SAFE_NUMBERS_EXPORT template <detail::non_bounded_integral_library_type Int>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto byteswap(const Int x) noexcept -> Int
{
    using underlying_type = detail::underlying_type_t<Int>;

    #if !(defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))

    using boost::core::byteswap;
    return Int{byteswap(static_cast<underlying_type>(x))};

    #else

    if constexpr (std::is_same_v<detail::underlying_type_t<Int>, int128::uint128_t>)
    {
        return Int{boost::int128::byteswap(static_cast<underlying_type>(x))};
    }
    else
    {
        return Int{detail::byteswap_impl(static_cast<underlying_type>(x))};
    }

    #endif
}

namespace detail {

consteval auto reverse_byte(std::uint8_t b) noexcept -> std::uint8_t
{
    b = static_cast<std::uint8_t>(((b & UINT8_C(0x55)) << 1u) | ((b & UINT8_C(0xAA)) >> 1u));
    b = static_cast<std::uint8_t>(((b & UINT8_C(0x33)) << 2u) | ((b & UINT8_C(0xCC)) >> 2u));
    b = static_cast<std::uint8_t>(((b & UINT8_C(0x0F)) << 4u) | ((b & UINT8_C(0xF0)) >> 4u));

    return b;
}

consteval auto make_byte_reverse_table() -> std::array<std::uint8_t, 256>
{
    std::array<std::uint8_t, 256> table {};

    for (int i {}; i < 256; ++i)
    {
        table[i] = reverse_byte(static_cast<std::uint8_t>(i));
    }

    return table;
}

#if !(defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))

inline constexpr auto reverse_table {make_byte_reverse_table()};

#endif

template <fundamental_unsigned_integral UnsignedInt>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto bitswap_impl(UnsignedInt x) noexcept -> UnsignedInt
{
    #if (defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))

    constexpr auto reverse_table {make_byte_reverse_table()};

    #endif

    if constexpr (sizeof(UnsignedInt) == 1)
    {
        return static_cast<UnsignedInt>(reverse_table[static_cast<std::uint8_t>(x)]);
    }
    else
    {
        constexpr auto n {sizeof(UnsignedInt)};

        UnsignedInt result {};
        for (std::size_t i {}; i < n; ++i)
        {
            result = static_cast<UnsignedInt>(static_cast<UnsignedInt>(result << 8U) |
                     static_cast<UnsignedInt>(reverse_table[static_cast<std::uint8_t>(x & 0xFFU)]));
            x >>= 8U;
        }

        return result;
    }
}

} // namespace detail

BOOST_SAFE_NUMBERS_EXPORT template <detail::non_bounded_integral_library_type Int>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto bitswap(Int x) noexcept -> Int
{
    using underlying_type = detail::underlying_type_t<Int>;
    return static_cast<Int>(detail::bitswap_impl(static_cast<underlying_type>(x)));
}

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_BIT_HPP
// ===== END boost/safe_numbers/bit.hpp =====
// ===== BEGIN boost/safe_numbers/integer_utilities.hpp =====
// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_INTEGER_UTILITIES_HPP
#define BOOST_SAFE_NUMBERS_INTEGER_UTILITIES_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/config.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/type_traits.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/compile_assert.hpp
// ===== BEGIN boost/safe_numbers/detail/rtz.hpp =====
// Copyright 2024 Junekey Jeon
// Copyright 2024 - 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// See: https://github.com/jk-jeon/rtz_benchmark/tree/master

#ifndef BOOST_SAFE_NUMBERS_DETAIL_RTZ_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_RTZ_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/type_traits.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/int128.hpp

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <cstdint>

#endif

namespace boost::safe_numbers::detail {

// n is assumed to be at most of bit_width bits
template <std::size_t bit_width, fundamental_unsigned_integral UInt>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto rotr(UInt n, unsigned int r) noexcept -> UInt
{
    static_assert(bit_width >= std::numeric_limits<UInt>::digits);

    constexpr auto bit_width_m1 {bit_width - 1U};

    r &= bit_width_m1;
    return static_cast<UInt>((n >> r) | (n << ((bit_width - r) & bit_width_m1)));
}

template <fundamental_unsigned_integral UInt>
struct remove_trailing_zeros_return
{
    UInt trimmed_number;
    std::size_t number_of_removed_zeros;
};

BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto remove_trailing_zeros(std::uint8_t n) noexcept -> remove_trailing_zeros_return<std::uint8_t>
{
    std::size_t s {};

    auto r = rotr<8>(static_cast<std::uint8_t>(n * 41U), 2);
    auto b = r < static_cast<std::uint8_t>(3);
    s = s * 2U + static_cast<std::size_t>(b);
    n = b ? r : n;

    r = rotr<8>(static_cast<std::uint8_t>(n * 77U), 1);
    b = r < static_cast<std::uint8_t>(26);
    s = s * 2U + static_cast<std::size_t>(b);
    n = b ? r : n;

    return {n, s};
}

BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto remove_trailing_zeros(std::uint16_t n) noexcept -> remove_trailing_zeros_return<std::uint16_t>
{
    std::size_t s {};

    auto r = rotr<16>(static_cast<std::uint16_t>(n * 3729U), 4);
    auto b = r < static_cast<std::uint16_t>(7);
    s = s * 2U + static_cast<std::size_t>(b);
    n = b ? r : n;

    r = rotr<16>(static_cast<std::uint16_t>(n * 7209U), 2);
    b = r < static_cast<std::uint16_t>(656);
    s = s * 2U + static_cast<std::size_t>(b);
    n = b ? r : n;

    r = rotr<16>(static_cast<std::uint16_t>(n * 19661U), 1);
    b = r < static_cast<std::uint16_t>(6554);
    s = s * 2U + static_cast<std::size_t>(b);
    n = b ? r : n;

    return {n, s};
}

BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto remove_trailing_zeros(std::uint32_t n) noexcept -> remove_trailing_zeros_return<std::uint32_t>
{
    std::size_t s {};

    auto r = rotr<32>(n * UINT32_C(15273505), 8);
    auto b = r < UINT32_C(43);
    s = s * 2U + static_cast<std::size_t>(b);
    n = b ? r : n;

    r = rotr<32>(n * UINT32_C(184254097), 4);
    b = r < UINT32_C(429497);
    s = s * 2U + static_cast<std::size_t>(b);
    n = b ? r : n;

    r = rotr<32>(n * UINT32_C(42949673), 2);
    b = r < UINT32_C(42949673);
    s = s * 2U + static_cast<std::size_t>(b);
    n = b ? r : n;

    r = rotr<32>(n * UINT32_C(1288490189), 1);
    b = r < UINT32_C(429496730);
    s = s * 2U + static_cast<std::size_t>(b);
    n = b ? r : n;

    return {n, s};
}

BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto remove_trailing_zeros(std::uint64_t n) noexcept -> remove_trailing_zeros_return<std::uint64_t>
{
    std::size_t s {};

    auto r = rotr<64>(n * UINT64_C(230079197716545), 16);
    auto b = r < UINT64_C(1845);
    s = s * 2U + static_cast<std::size_t>(b);
    n = b ? r : n;

    r = rotr<64>(n * UINT64_C(28999941890838049), 8);
    b = r < UINT64_C(184467440738);
    s = s * 2U + static_cast<std::size_t>(b);
    n = b ? r : n;

    r = rotr<64>(n * UINT64_C(182622766329724561), 4);
    b = r < UINT64_C(1844674407370956);
    s = s * 2U + static_cast<std::size_t>(b);
    n = b ? r : n;

    r = rotr<64>(n * UINT64_C(10330176681277348905), 2);
    b = r < UINT64_C(184467440737095517);
    s = s * 2U + static_cast<std::size_t>(b);
    n = b ? r : n;

    r = rotr<64>(n * UINT64_C(14757395258967641293), 1);
    b = r < UINT64_C(1844674407370955162);
    s = s * 2U + static_cast<std::size_t>(b);
    n = b ? r : n;

    return {n, s};
}

BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto remove_trailing_zeros(int128::uint128_t n) noexcept -> remove_trailing_zeros_return<int128::uint128_t>
{
    std::size_t s {};

    auto r = rotr<128>(n * boost::int128::uint128_t(UINT64_C(0x62B42691AD836EB1), UINT64_C(0x16590F420A835081)), 32);
    auto b = r < boost::int128::uint128_t {UINT64_C(0x0), UINT64_C(0x33EC48)};
    s = s * 2U + static_cast<std::size_t>(b);
    n = b ? r : n;

    r = rotr<128>(n * boost::int128::uint128_t {UINT64_C(0x3275305C1066), UINT64_C(0xE4A4D1417CD9A041)}, 16);
    b = r < boost::int128::uint128_t {UINT64_C(0x734), UINT64_C(0xACA5F6226F0ADA62)};
    s = s * 2U + static_cast<std::size_t>(b);
    n = b ? r : n;

    r = rotr<128>(n * boost::int128::uint128_t {UINT64_C(0x6B7213EE9F5A78), UINT64_C(0xC767074B22E90E21)}, 8);
    b = r < boost::int128::uint128_t {UINT64_C(0x2AF31DC461), UINT64_C(0x1873BF3F70834ACE)};
    s = s * 2U + static_cast<std::size_t>(b);
    n = b ? r : n;

    r = rotr<128>(n * boost::int128::uint128_t {UINT64_C(0x95182A9930BE0DE), UINT64_C(0xD288CE703AFB7E91)}, 4);
    b = r < boost::int128::uint128_t {UINT64_C(0x68DB8BAC710CB), UINT64_C(0x295E9E1B089A0276)};
    s = s * 2U + static_cast<std::size_t>(b);
    n = b ? r : n;

    r = rotr<128>(n * boost::int128::uint128_t {UINT64_C(0x28F5C28F5C28F5C2), UINT64_C(0x8F5C28F5C28F5C29)}, 2);
    b = r < boost::int128::uint128_t {UINT64_C(0x28F5C28F5C28F5C), UINT64_C(0x28F5C28F5C28F5C3)};
    s = s * 2U + static_cast<std::size_t>(b);
    n = b ? r : n;

    r = rotr<128>(n * boost::int128::uint128_t {UINT64_C(0xCCCCCCCCCCCCCCCC), UINT64_C(0xCCCCCCCCCCCCCCCD)}, 1);
    b = r < boost::int128::uint128_t {UINT64_C(0x1999999999999999), UINT64_C(0x999999999999999A)};
    s = s * 2U + static_cast<std::size_t>(b);
    n = b ? r : n;

    return {n, s};
}

} // namespace boost::safe_numbers::detail

#endif // BOOST_SAFE_NUMBERS_DETAIL_RTZ_HPP
// ===== END boost/safe_numbers/detail/rtz.hpp =====
// ===== BEGIN boost/safe_numbers/detail/num_digits.hpp =====
// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_NUM_DIGITS_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_NUM_DIGITS_HPP

// https://stackoverflow.com/questions/1489830/efficient-way-to-determine-number-of-digits-in-an-integer
// https://graphics.stanford.edu/~seander/bithacks.html

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/bit.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/config.hpp

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <array>
#include <cstdint>
#include <limits>

#if (defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))

#include <cuda/std/bit>

#else

#include <boost/core/bit.hpp>

#endif

#endif

namespace boost::safe_numbers::detail {

// ============================================================================
// Power-of-10 lookup tables (generated at compile time)
// ============================================================================

template <typename T>
consteval auto make_powers_of_10() noexcept
{
    constexpr auto N {static_cast<std::size_t>(std::numeric_limits<T>::digits10 + 1)};

    std::array<T, N> table {};
    table[0] = T{1};
    for (std::size_t i {1}; i < N; ++i)
    {
        table[i] = static_cast<T>(table[i - 1] * T{10});
    }

    return table;
}

#if !(defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))

inline constexpr auto powers_of_10_u32 {make_powers_of_10<std::uint32_t>()};
inline constexpr auto powers_of_10_u64 {make_powers_of_10<std::uint64_t>()};
inline constexpr auto powers_of_10_u128 {make_powers_of_10<int128::uint128_t>()};

#endif

// ============================================================================
// num_digits: counts the number of decimal digits using MSB approximation
//
// Algorithm: log10(x) ~= log2(x) / log2(10) ~= log2(x) / 3.322
// Use (msb * 1000) / 3322 + 1 as an estimate, then refine with a
// table lookup (at most one comparison in each direction).
// ============================================================================

// Overload for types fitting in uint32_t (uint8_t, uint16_t, uint32_t)
template <typename T>
    requires (std::numeric_limits<T>::digits <= 32 && std::is_unsigned_v<T>)
constexpr auto num_digits(const T init_x) noexcept -> int
{
    #if (defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))

    constexpr auto powers_of_10_u32 {make_powers_of_10<std::uint32_t>()};

    #endif

    const auto x {static_cast<std::uint32_t>(init_x)};

    if (x == 0)
    {
        return 1;
    }

    #if !(defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))
    const auto msb {32 - boost::core::countl_zero(x)};
    #else
    const auto msb {32 - cuda::std::countl_zero(x)};
    #endif

    // Approximate log10
    const auto estimated_digits {(msb * 1000) / 3322 + 1};

    if (estimated_digits < 10 && x >= powers_of_10_u32[static_cast<std::size_t>(estimated_digits)])
    {
        return estimated_digits + 1;
    }

    if (estimated_digits > 1 && x < powers_of_10_u32[static_cast<std::size_t>(estimated_digits - 1)])
    {
        return estimated_digits - 1;
    }

    return estimated_digits;
}

// Overload for uint64_t
constexpr auto num_digits(const std::uint64_t x) noexcept -> int
{
    #if (defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))

    constexpr auto powers_of_10_u64 {make_powers_of_10<std::uint64_t>()};

    #endif

    if (x <= UINT32_MAX)
    {
        return num_digits(static_cast<std::uint32_t>(x));
    }

    #if !(defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))
    const auto msb {64 - boost::core::countl_zero(x)};
    #else
    const auto msb {64 - cuda::std::countl_zero(x)};
    #endif

    // Approximate log10
    const auto estimated_digits {(msb * 1000) / 3322 + 1};

    if (estimated_digits < 20 && x >= powers_of_10_u64[static_cast<std::size_t>(estimated_digits)])
    {
        return estimated_digits + 1;
    }

    if (estimated_digits > 1 && x < powers_of_10_u64[static_cast<std::size_t>(estimated_digits - 1)])
    {
        return estimated_digits - 1;
    }

    return estimated_digits;
}

// Overload for uint128_t
constexpr auto num_digits(const boost::int128::uint128_t& x) noexcept -> int
{
    #if (defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))

    constexpr auto powers_of_10_u128 {make_powers_of_10<int128::uint128_t>()};

    #endif

    if (x.high == UINT64_C(0))
    {
        return num_digits(x.low);
    }

    const auto msb {64 + (64 - boost::int128::detail::countl_zero(x.high))};

    // Approximate log10
    const auto estimated_digits {(msb * 1000) / 3322 + 1};

    if (estimated_digits < 39 && x >= powers_of_10_u128[static_cast<std::size_t>(estimated_digits)])
    {
        return estimated_digits + 1;
    }

    // Estimated digits can't be less than 20 (65+ bits)
    if (x < powers_of_10_u128[static_cast<std::size_t>(estimated_digits - 1)])
    {
        return estimated_digits - 1;
    }

    return estimated_digits;
}

} // namespace boost::safe_numbers::detail

#endif // BOOST_SAFE_NUMBERS_DETAIL_NUM_DIGITS_HPP
// ===== END boost/safe_numbers/detail/num_digits.hpp =====
// [amalgamate] skipped duplicate include of boost/safe_numbers/bit.hpp

namespace boost::safe_numbers {

// Newton's method as it can't possibly overflow, and converges rapidly
template <detail::non_bounded_unsigned_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto isqrt(const T val) -> T
{
    using underlying_type = detail::underlying_type_t<T>;

    constexpr auto one {static_cast<underlying_type>(1)};
    constexpr auto two {static_cast<underlying_type>(2)};

    auto n {static_cast<underlying_type>(val)};

    if (n < two)
    {
        return val;
    }

    auto x {n};
    auto y {static_cast<underlying_type>(x / two + (x & one))}; // (x + 1) / 2 without overflow

    while (y < x)
    {
        x = y;
        y = static_cast<underlying_type>(x + n / x) / two;
    }

    return T{x};
}

template <detail::non_bounded_unsigned_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto remove_trailing_zeros(const T n)
{
    using underlying_type = detail::underlying_type_t<T>;

    if (static_cast<underlying_type>(n) == static_cast<underlying_type>(0))
    {
        return detail::remove_trailing_zeros_return<underlying_type>{static_cast<underlying_type>(0), static_cast<std::size_t>(0)};
    }

    return detail::remove_trailing_zeros(static_cast<underlying_type>(n));
}

template <detail::non_bounded_unsigned_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto is_power_10(const T n) -> bool
{
    using underlying_type = detail::underlying_type_t<T>;

    const auto [trimmed_number, _] = detail::remove_trailing_zeros(static_cast<underlying_type>(n));
    return trimmed_number == static_cast<underlying_type>(1);
}

template <detail::non_bounded_unsigned_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto is_power_2(const T n) noexcept -> bool
{
    return has_single_bit(n);
}

// Integer log base 2: floor(log2(n)) == bit_width(n) - 1
template <detail::non_bounded_unsigned_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto ilog2(const T n) -> int
{
    using underlying_type = detail::underlying_type_t<T>;

    if (static_cast<underlying_type>(n) == underlying_type{0})
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "ilog2(0) is undefined");
    }

    return bit_width(n) - 1;
}

// Integer log base 10: floor(ilog10(n)) == num_digits(n) - 1
// Uses MSB-based approximation with power-of-10 table lookup (O(1))
template <detail::non_bounded_unsigned_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto ilog10(const T n) -> int
{
    using underlying_type = detail::underlying_type_t<T>;

    if (static_cast<underlying_type>(n) == underlying_type{0})
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "ilog10(0) is undefined");
    }

    return detail::num_digits(static_cast<underlying_type>(n)) - 1;
}

// Integer log arbitrary base: floor(log_base(n))
// Repeated division: O(log_base(n)) divisions
template <detail::non_bounded_unsigned_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto ilog(const T n, const T base) -> int
{
    using underlying_type = detail::underlying_type_t<T>;

    if (static_cast<underlying_type>(n) == underlying_type{0})
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "ilog(0, base) is undefined");
    }

    if (static_cast<underlying_type>(base) < underlying_type{2})
    {
        BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "ilog(n, base) requires base >= 2");
    }

    auto result {0};
    auto val {static_cast<underlying_type>(n)};
    const auto b {static_cast<underlying_type>(base)};

    while (val >= b)
    {
        val /= b;
        ++result;
    }

    return result;
}

namespace detail {

// Iterative exponentiation by squaring: O(log b) multiplications
template <non_bounded_unsigned_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto ipow_impl(T base, T exp) -> T
{
    using underlying_type = underlying_type_t<T>;

    auto result = T{static_cast<underlying_type>(1)};

    while (exp != T{static_cast<underlying_type>(0)})
    {
        if (static_cast<underlying_type>(exp) & underlying_type{1})
        {
            result = result * base;
        }
        exp = exp / T{static_cast<underlying_type>(2)};
        if (exp != T{static_cast<underlying_type>(0)})
        {
            base = base * base;
        }
    }

    return result;
}

} // namespace detail

template <detail::non_bounded_unsigned_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto ipow(const T a, const T b) -> T
{
    return detail::ipow_impl(a, b);
}

template <detail::integral_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto abs_diff(const T a, const T b) noexcept -> T
{
    return a > b ? a - b : b - a;
}

template <detail::integral_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto div_ceil(const T a, const T b) noexcept -> T
{
    using underlying_type = detail::underlying_type_t<T>;

    // div_ceil divides by b and is noexcept, so a constant zero divisor is always a bug
    #ifdef BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT
    const auto divisor {static_cast<underlying_type>(b)};
    BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(divisor != static_cast<underlying_type>(0), "div_ceil divisor is zero");
    static_cast<void>(divisor); // used only by the assert; keep ENABLE + -O0 warning-clean
    #endif

    const auto d {a / b};
    const auto r {a % b};

    if constexpr (std::numeric_limits<T>::is_signed)
    {
        return d + T{static_cast<underlying_type>(r != static_cast<T>(0) && (r ^ b) > static_cast<T>(0) ? 1 : 0)};
    }
    else
    {
        return d + T{static_cast<underlying_type>(r != static_cast<T>(0) ? 1 : 0)};
    }
}

template <detail::integral_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto next_multiple_of(const T a, const T b) noexcept -> T
{
    return div_ceil(a, b) * b;
}

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_INTEGER_UTILITIES_HPP
// ===== END boost/safe_numbers/integer_utilities.hpp =====
// ===== BEGIN boost/safe_numbers/byte_conversions.hpp =====
// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_BYTE_CONVERSIONS_HPP
#define BOOST_SAFE_NUMBERS_BYTE_CONVERSIONS_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/type_traits.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/throw_exception.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/bit.hpp

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <array>
#include <bit>
#include <cstddef>
#include <span>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers {

template <detail::non_bounded_integral_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto to_be(const T value) noexcept -> T
{
    if constexpr (std::endian::native == std::endian::big)
    {
        return value;
    }
    else
    {
        return byteswap(value);
    }
}

template <detail::non_bounded_integral_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto from_be(const T value) noexcept -> T
{
    // Self-inverse
    return to_be(value);
}

template <detail::non_bounded_integral_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto to_le(const T value) noexcept -> T
{
    if constexpr (std::endian::native == std::endian::little)
    {
        return value;
    }
    else
    {
        return byteswap(value);
    }
}

template <detail::non_bounded_integral_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto from_le(const T value) noexcept -> T
{
    // Self-inverse
    return to_le(value);
}

template <detail::non_bounded_integral_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto to_be_bytes(const T value) noexcept -> std::array<std::byte, sizeof(T)>
{
    const auto be_value {to_be(value)};
    return std::bit_cast<std::array<std::byte, sizeof(T)>>(be_value);
}

template <detail::non_bounded_integral_library_type T, std::size_t N>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto from_be_bytes(const std::span<const std::byte, N> bytes) -> T
{
    using underlying_type = detail::underlying_type_t<T>;

    if constexpr (N == sizeof(T))
    {
        std::array<std::byte, sizeof(T)> arr {};
        for (std::size_t i {}; i < N; ++i)
        {
            arr[i] = bytes[i];
        }
        return from_be(T{std::bit_cast<underlying_type>(arr)});
    }
    else if constexpr (N != std::dynamic_extent)
    {
        static_assert(detail::dependent_false<T>, "The number of bytes provided, and the target type number of bytes do not match");
        return T{}; // LCOV_EXCL_LINE
    }
    else
    {
        if (bytes.size_bytes() != sizeof(T))
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "The number of bytes provided, and the target type number of bytes do not match");
        }

        std::array<std::byte, sizeof(T)> arr {};
        for (std::size_t i {}; i < sizeof(T); ++i)
        {
            arr[i] = bytes[i];
        }
        return from_be(T{std::bit_cast<underlying_type>(arr)});
    }
}

template <detail::non_bounded_integral_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto to_le_bytes(const T value) noexcept -> std::array<std::byte, sizeof(T)>
{
    const auto le_value {to_le(value)};
    return std::bit_cast<std::array<std::byte, sizeof(T)>>(le_value);
}

template <detail::non_bounded_integral_library_type T, std::size_t N>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto from_le_bytes(const std::span<const std::byte, N> bytes) -> T
{
    using underlying_type = detail::underlying_type_t<T>;

    if constexpr (N == sizeof(T))
    {
        std::array<std::byte, sizeof(T)> arr {};
        for (std::size_t i {}; i < N; ++i)
        {
            arr[i] = bytes[i];
        }
        return from_le(T{std::bit_cast<underlying_type>(arr)});
    }
    else if constexpr (N != std::dynamic_extent)
    {
        static_assert(detail::dependent_false<T>, "The number of bytes provided, and the target type number of bytes do not match");
        return T{}; // LCOV_EXCL_LINE
    }
    else
    {
        if (bytes.size_bytes() != sizeof(T))
        {
            BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::domain_error, "The number of bytes provided, and the target type number of bytes do not match");
        }

        std::array<std::byte, sizeof(T)> arr {};
        for (std::size_t i {}; i < sizeof(T); ++i)
        {
            arr[i] = bytes[i];
        }
        return from_le(T{std::bit_cast<underlying_type>(arr)});
    }
}

template <detail::non_bounded_integral_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto to_ne_bytes(const T value) noexcept -> std::array<std::byte, sizeof(T)>
{
    if constexpr (std::endian::native == std::endian::little)
    {
        return to_le_bytes(value);
    }
    else
    {
        return to_be_bytes(value);
    }
}

template <detail::non_bounded_integral_library_type T, std::size_t N>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto from_ne_bytes(const std::span<const std::byte, N> bytes) -> T
{
    if constexpr (std::endian::native == std::endian::little)
    {
        return from_le_bytes<T>(bytes);
    }
    else
    {
        return from_be_bytes<T>(bytes);
    }
}

} // namespace boost::safe_numbers

#endif //BOOST_SAFE_NUMBERS_BYTE_CONVERSIONS_HPP
// ===== END boost/safe_numbers/byte_conversions.hpp =====
// ===== BEGIN boost/safe_numbers/numeric.hpp =====
// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_NUMERIC_HPP
#define BOOST_SAFE_NUMBERS_NUMERIC_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/type_traits.hpp
// ===== BEGIN boost/safe_numbers/detail/int128/numeric.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_NUMERIC_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_NUMERIC_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/bit.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/detail/traits.hpp

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILD_MODULE

#include <limits>
#include <utility>

#endif

namespace boost {
namespace int128 {

namespace detail {

template <typename IntegerType>
struct reduced_integers
{
    static constexpr bool value {std::is_same<IntegerType, signed char>::value ||
                                 std::is_same<IntegerType, unsigned char>::value ||
                                 std::is_same<IntegerType, signed short>::value ||
                                 std::is_same<IntegerType, unsigned short>::value ||
                                 std::is_same<IntegerType, signed int>::value ||
                                 std::is_same<IntegerType, unsigned int>::value ||
                                 std::is_same<IntegerType, signed long>::value ||
                                 std::is_same<IntegerType, unsigned long>::value ||
                                 std::is_same<IntegerType, signed long long>::value ||
                                 std::is_same<IntegerType, unsigned long long>::value ||
                                 std::is_same<IntegerType, int128_t>::value ||
                                 std::is_same<IntegerType, uint128_t>::value};
};

#if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)

template <typename IntegerType>
BOOST_SAFE_NUMBERS_DETAIL_INT128_INLINE_CONSTEXPR bool is_reduced_integer_v {reduced_integers<IntegerType>::value ||
                                              std::is_same<IntegerType, detail::builtin_i128>::value ||
                                              std::is_same<IntegerType, detail::builtin_u128>::value};

#else

template <typename IntegerType>
BOOST_SAFE_NUMBERS_DETAIL_INT128_INLINE_CONSTEXPR bool is_reduced_integer_v {reduced_integers<IntegerType>::value};

#endif // 128-bit

} // namespace detail

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t add_sat(const uint128_t x, const uint128_t y) noexcept
{
    const auto z {x + y};

    if (z < x)
    {
        return (std::numeric_limits<uint128_t>::max)();
    }

    return z;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t sub_sat(const uint128_t x, const uint128_t y) noexcept
{
    const auto z {x - y};

    if (z > x)
    {
        return (std::numeric_limits<uint128_t>::min)();
    }

    return z;
}

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4307) // Addition Overflow
#  pragma warning(disable : 4146) // Unary minus applied to unsigned type
#endif

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t add_sat(const int128_t x, const int128_t y) noexcept
{
    // Detect overflow BEFORE the addition to avoid signed overflow UB.
    // When both are non-negative: overflow iff x > max - y (subtraction safe: max - non_negative >= 0)
    // When both are negative: overflow iff x < min - y (subtraction safe: min - negative > min)
    // Mixed signs: overflow is impossible.

    if (x.high >= 0 && y.high >= 0)
    {
        if (x > (std::numeric_limits<int128_t>::max)() - y)
        {
            return (std::numeric_limits<int128_t>::max)();
        }
    }
    else if (x.high < 0 && y.high < 0)
    {
        if (x < (std::numeric_limits<int128_t>::min)() - y)
        {
            return (std::numeric_limits<int128_t>::min)();
        }
    }

    return x + y;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t sub_sat(const int128_t x, const int128_t y) noexcept
{
    // Detect overflow BEFORE the subtraction to avoid signed overflow UB.
    // Positive overflow: x >= 0 and y < 0 and x > max + y (safe: max + negative < max)
    // Negative overflow: x < 0 and y >= 0 and x < min + y (safe: min + non_negative > min)
    // Same signs: overflow is impossible.

    if (x.high >= 0 && y.high < 0)
    {
        if (x > (std::numeric_limits<int128_t>::max)() + y)
        {
            return (std::numeric_limits<int128_t>::max)();
        }
    }
    else if (x.high < 0 && y.high >= 0)
    {
        if (x < (std::numeric_limits<int128_t>::min)() + y)
        {
            return (std::numeric_limits<int128_t>::min)();
        }
    }

    return x - y;
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t mul_sat(const uint128_t x, const uint128_t y) noexcept
{
    const auto x_bits {bit_width(x)};
    const auto y_bits {bit_width(y)};

    if ((x_bits + y_bits) > std::numeric_limits<uint128_t>::digits)
    {
        return (std::numeric_limits<uint128_t>::max)();
    }

    return x * y;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t mul_sat(const int128_t& x, const int128_t& y) noexcept
{
    const auto x_bits {bit_width(static_cast<uint128_t>(abs(x)))};
    const auto y_bits {bit_width(static_cast<uint128_t>(abs(y)))};

    if ((x_bits + y_bits) > std::numeric_limits<int128_t>::digits)
    {
        if ((x < 0) != (y < 0))
        {
            return (std::numeric_limits<int128_t>::min)();
        }
        else
        {
            return (std::numeric_limits<int128_t>::max)();
        }
    }

    const int128_t res {x * y};
    return res;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t div_sat(const uint128_t x, const uint128_t y) noexcept
{
    return x / y;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t div_sat(const int128_t x, const int128_t y) noexcept
{
    if (BOOST_SAFE_NUMBERS_DETAIL_INT128_UNLIKELY(x == (std::numeric_limits<int128_t>::min)() && y == -1))
    {
        // This is the only possible case of overflow
        return (std::numeric_limits<int128_t>::max)();
    }

    return x / y;
}

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4267)
#endif

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <typename TargetType, std::enable_if_t<detail::is_reduced_integer_v<TargetType>, bool> = true>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr TargetType saturate_cast(const uint128_t value) noexcept
{
    BOOST_SAFE_NUMBERS_DETAIL_INT128_IF_CONSTEXPR (std::is_same<uint128_t, TargetType>::value)
    {
        return static_cast<TargetType>(value);
    }
    else
    {
        if (value > static_cast<uint128_t>((std::numeric_limits<TargetType>::max)()))
        {
            return (std::numeric_limits<TargetType>::max)();
        }

        return static_cast<TargetType>(value);
    }
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

BOOST_SAFE_NUMBERS_DETAIL_INT128_EXPORT template <typename TargetType, std::enable_if_t<detail::is_reduced_integer_v<TargetType>, bool> = true>
BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr TargetType saturate_cast(const int128_t value) noexcept
{
    BOOST_SAFE_NUMBERS_DETAIL_INT128_IF_CONSTEXPR (std::is_same<int128_t, TargetType>::value)
    {
        return static_cast<TargetType>(value);
    }
    #if defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_INT128) || defined(BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_MSVC_INT128)
    else BOOST_SAFE_NUMBERS_DETAIL_INT128_IF_CONSTEXPR (std::is_same<uint128_t, TargetType>::value || std::is_same<detail::builtin_u128, TargetType>::value)
    #else
    else BOOST_SAFE_NUMBERS_DETAIL_INT128_IF_CONSTEXPR (std::is_same<uint128_t, TargetType>::value)
    #endif
    {
        // We can't possibly have overflow in this case
        return value < 0 ? static_cast<TargetType>(0) : static_cast<TargetType>(value);
    }
    else
    {
        if (value > static_cast<int128_t>((std::numeric_limits<TargetType>::max)()))
        {
            return (std::numeric_limits<TargetType>::max)();
        }
        else if (value < static_cast<int128_t>((std::numeric_limits<TargetType>::min)()))
        {
            return (std::numeric_limits<TargetType>::min)();
        }

        return static_cast<TargetType>(value);
    }
}

namespace detail {

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr std::uint64_t gcd64(std::uint64_t x, std::uint64_t y) noexcept
{
    if (x == 0)
    {
        return y;
    }
    if (y == 0)
    {
        return x;
    }

    const auto s {impl::countr_impl(x | y)};
    x >>= impl::countr_impl(x);

    do
    {
        y >>= impl::countr_impl(y);
        if (x > y)
        {
            const auto temp {x};
            x = y;
            y = temp;
        }

        y -= x;
    } while (y);

    return x << s;
}

} // namespace detail

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t gcd(uint128_t a, uint128_t b) noexcept
{
    // Base case
    if (a == 0U)
    {
        return b;
    }
    if (b == 0U)
    {
        return a;
    }

    const auto a_zero {countr_zero(a)};
    const auto b_zero {countr_zero(b)};
    const auto shift {b_zero < a_zero ? b_zero : a_zero};
    a >>= shift;
    b >>= shift;

    do
    {
        b >>= countr_zero(b);

        if (a > b)
        {
            const uint128_t temp {a};
            a = b;
            b = temp;
        }

        b -= a;
    } while (b != 0U && (a.high | b.high) > 0U);

    // Stop doing 128-bit math as soon as we can
    const auto g {detail::gcd64(a.low, b.low)};
    return uint128_t{0, g} << shift;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t gcd(const int128_t a, const int128_t b) noexcept
{
    return static_cast<int128_t>(gcd(static_cast<uint128_t>(abs(a)), static_cast<uint128_t>(abs(b))));
}

// For unknown reasons this implementation fails for MSVC x86 only in release mode
// Directly calculating leads to the same failures, so unfortunately we have a viable,
// but very slow impl that we know works.
#if !(defined(_M_IX86) && !defined(_NDEBUG))

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t lcm(const uint128_t a, const uint128_t b) noexcept
{
    if (a == 0U || b == 0U)
    {
        return static_cast<uint128_t>(0);
    }

    // Calculate GCD first
    const auto g {gcd(a, b)};

    // Compute LCM avoiding overflow: (a/gcd) * b
    return (a / g) * b;
}

#else

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t lcm(uint128_t a, uint128_t b) noexcept
{
    if (a == 0U || b == 0U)
    {
        return uint128_t{0};
    }


    unsigned shift{};
    while ((a & 1U) == 0U && (b & 1U) == 0U) 
    {
        a >>= 1U;
        b >>= 1U;
        shift++;
    }

    // Ensure a >= b
    if (a < b)
    {
        std::swap(a, b);
    }

    uint128_t lcm{a};

    while (lcm % b != 0U) 
    {
        lcm += a;
    }

    return lcm << shift;
}

#endif

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t lcm(const int128_t a, const int128_t b) noexcept
{
    return static_cast<int128_t>(lcm(static_cast<uint128_t>(abs(a)), static_cast<uint128_t>(abs(b))));
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr uint128_t midpoint(const uint128_t a, const uint128_t b) noexcept
{
    // Bit manipulation formula works for unsigned integers
    auto mid {(a & b) + ((a ^ b) >> 1)};

    // std::midpoint rounds towards the first parameter
    if ((a ^ b) & 1U && a > b)
    {
        ++mid;
    }

    return mid;
}

BOOST_SAFE_NUMBERS_DETAIL_INT128_HOST_DEVICE constexpr int128_t midpoint(const int128_t a, const int128_t b) noexcept
{
    // For signed integers, we use a + (b - a) / 2 or a - (a - b) / 2
    // The subtraction is done in unsigned arithmetic to handle overflow correctly
    // Integer division automatically rounds toward the first argument
    //
    // Use direct field access for both the uint128 construction and the
    // comparison to avoid NVCC host compiler issues with operator<= and
    // static_cast on int128_t for large-magnitude values

    const uint128_t ua {static_cast<std::uint64_t>(a.high), a.low};
    const uint128_t ub {static_cast<std::uint64_t>(b.high), b.low};

    const bool a_le_b {a.high == b.high ? a.low <= b.low : a.high < b.high};

    if (a_le_b)
    {
        // diff = b - a (computed in unsigned, handles wrap-around correctly)
        const auto diff {ub - ua};
        return a + static_cast<int128_t>(diff / 2U);
    }
    else
    {
        // diff = a - b (computed in unsigned, handles wrap-around correctly)
        const auto diff {ua - ub};
        return a - static_cast<int128_t>(diff / 2U);
    }
}

} // namespace int128
} // namespace boost

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_NUMERIC_HPP
// ===== END boost/safe_numbers/detail/int128/numeric.hpp =====

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#if (defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))
#include <cuda/std/numeric>
#else
#include <numeric>
#endif

#endif

namespace boost::safe_numbers {

template <detail::non_bounded_integral_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto gcd(const T m, const T n) noexcept -> T
{
    using underlying_type = detail::underlying_type_t<T>;

    if constexpr (std::is_same_v<underlying_type, int128::uint128_t> || std::is_same_v<underlying_type, int128::int128_t>)
    {
        return T{int128::gcd(static_cast<underlying_type>(m), static_cast<underlying_type>(n))};
    }
    else
    {
        #if !(defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))
        return T{static_cast<underlying_type>(std::gcd(static_cast<underlying_type>(m), static_cast<underlying_type>(n)))};
        #else
        return T{static_cast<underlying_type>(cuda::std::gcd(static_cast<underlying_type>(m), static_cast<underlying_type>(n)))};
        #endif
    }
}

template <detail::non_bounded_integral_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto lcm(const T m, const T n) noexcept -> T
{
    using underlying_type = detail::underlying_type_t<T>;

    if constexpr (std::is_same_v<underlying_type, int128::uint128_t> || std::is_same_v<underlying_type, int128::int128_t>)
    {
        return T{int128::lcm(static_cast<underlying_type>(m), static_cast<underlying_type>(n))};
    }
    else
    {
        #if !(defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))
        return T{static_cast<underlying_type>(std::lcm(static_cast<underlying_type>(m), static_cast<underlying_type>(n)))};
        #else
        return T{static_cast<underlying_type>(cuda::std::lcm(static_cast<underlying_type>(m), static_cast<underlying_type>(n)))};
        #endif
    }
}

template <detail::non_bounded_integral_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto midpoint(const T a, const T b) noexcept -> T
{
    using underlying_type = detail::underlying_type_t<T>;

    if constexpr (std::is_same_v<underlying_type, int128::uint128_t> || std::is_same_v<underlying_type, int128::int128_t>)
    {
        return T{int128::midpoint(static_cast<underlying_type>(a), static_cast<underlying_type>(b))};
    }
    else
    {
        #if !(defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))
        return T{static_cast<underlying_type>(std::midpoint(static_cast<underlying_type>(a), static_cast<underlying_type>(b)))};
        #else
        return T{static_cast<underlying_type>(cuda::std::midpoint(static_cast<underlying_type>(a), static_cast<underlying_type>(b)))};
        #endif
    }
}

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_NUMERIC_HPP
// ===== END boost/safe_numbers/numeric.hpp =====
// ===== BEGIN boost/safe_numbers/cmath.hpp =====
// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_CMATH_HPP
#define BOOST_SAFE_NUMBERS_CMATH_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/floats.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/signed_integers.hpp
// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/type_traits.hpp

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#if (defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))
#include <cuda/std/cmath>
#else
#include <cmath>
#endif

#include <cstdint>
#include <type_traits>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

// Namespace that actually provides the <cmath> functions. The wrappers delegate
// to it with an explicit qualification so there is no ADL or self-recursion.
#if (defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA) && defined(__CUDACC__))
#  define BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS cuda::std
#else
#  define BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS std
#endif

namespace boost::safe_numbers {

#define BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(fn)                                                       \
template <detail::non_bounded_float_library_type T>                                                     \
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto fn(const T x) -> T                           \
{                                                                                                       \
    using underlying_type = detail::underlying_type_t<T>;                                               \
    return T{detail::impl::check_cmath_result<underlying_type>(                                          \
        BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS::fn(static_cast<underlying_type>(x)), #fn)};                  \
}

#define BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BINARY(fn)                                                      \
template <detail::non_bounded_float_library_type T>                                                     \
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto fn(const T x, const T y) -> T                \
{                                                                                                       \
    using underlying_type = detail::underlying_type_t<T>;                                               \
    return T{detail::impl::check_cmath_result<underlying_type>(                                          \
        BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS::fn(static_cast<underlying_type>(x),                          \
                                               static_cast<underlying_type>(y)), #fn)};                  \
}

// Trigonometric
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(sin)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(cos)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(tan)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(asin)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(acos)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(atan)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BINARY(atan2)

// Hyperbolic
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(sinh)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(cosh)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(tanh)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(asinh)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(acosh)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(atanh)

// Exponential and logarithmic
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(exp)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(exp2)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(expm1)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(log)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(log2)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(log10)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(log1p)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(logb)

// Power and root
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(sqrt)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(cbrt)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BINARY(pow)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BINARY(hypot)

// Error and gamma
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(erf)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(erfc)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(tgamma)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(lgamma)

// Nearest integer (floating-point result)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(floor)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(ceil)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(trunc)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(round)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(nearbyint)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(rint)

// Absolute value
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(abs)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY(fabs)

// Remainder, difference, min/max, sign manipulation
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BINARY(fmod)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BINARY(remainder)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BINARY(fdim)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BINARY(fmin)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BINARY(fmax)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BINARY(copysign)
BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BINARY(nextafter)

// Fused multiply-add
template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto fma(const T x, const T y, const T z) -> T
{
    using underlying_type = detail::underlying_type_t<T>;
    return T{detail::impl::check_cmath_result<underlying_type>(
        BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS::fma(static_cast<underlying_type>(x),
                                                static_cast<underlying_type>(y),
                                                static_cast<underlying_type>(z)), "fma")};
}

// Scaling by a power of two (integer exponent input, floating-point result)
template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto ldexp(const T x, const int exp) -> T
{
    using underlying_type = detail::underlying_type_t<T>;
    return T{detail::impl::check_cmath_result<underlying_type>(
        BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS::ldexp(static_cast<underlying_type>(x), exp), "ldexp")};
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto scalbn(const T x, const int exp) -> T
{
    using underlying_type = detail::underlying_type_t<T>;
    return T{detail::impl::check_cmath_result<underlying_type>(
        BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS::scalbn(static_cast<underlying_type>(x), exp), "scalbn")};
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto scalbln(const T x, const long exp) -> T
{
    using underlying_type = detail::underlying_type_t<T>;
    return T{detail::impl::check_cmath_result<underlying_type>(
        BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS::scalbln(static_cast<underlying_type>(x), exp), "scalbln")};
}

// ------------------------------
// Classification predicates
// ------------------------------
// These inspect a value and never throw, so they are the escape hatch for code
// that needs to reason about infinities and NANs. They are genuinely constexpr
// because they delegate to the library's own constexpr classification helpers.

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto isnan(const T x) noexcept -> bool
{
    return detail::impl::constexpr_isnan(static_cast<detail::underlying_type_t<T>>(x));
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto isinf(const T x) noexcept -> bool
{
    return detail::impl::constexpr_isinf(static_cast<detail::underlying_type_t<T>>(x));
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto isfinite(const T x) noexcept -> bool
{
    const auto v {static_cast<detail::underlying_type_t<T>>(x)};
    return !detail::impl::constexpr_isinf(v) && !detail::impl::constexpr_isnan(v);
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto isnormal(const T x) noexcept -> bool
{
    return detail::impl::constexpr_isnormal(static_cast<detail::underlying_type_t<T>>(x));
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto signbit(const T x) noexcept -> bool
{
    return detail::impl::constexpr_signbit(static_cast<detail::underlying_type_t<T>>(x));
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto fpclassify(const T x) noexcept -> int
{
    return detail::impl::constexpr_fpclassify(static_cast<detail::underlying_type_t<T>>(x));
}

// Non-signaling comparison predicates: ordered comparisons that return false
// (rather than raising) when an operand is NAN.

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto isunordered(const T x, const T y) noexcept -> bool
{
    using underlying_type = detail::underlying_type_t<T>;
    return detail::impl::constexpr_isnan(static_cast<underlying_type>(x)) ||
           detail::impl::constexpr_isnan(static_cast<underlying_type>(y));
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto isgreater(const T x, const T y) noexcept -> bool
{
    using underlying_type = detail::underlying_type_t<T>;
    return !isunordered(x, y) && static_cast<underlying_type>(x) > static_cast<underlying_type>(y);
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto isgreaterequal(const T x, const T y) noexcept -> bool
{
    using underlying_type = detail::underlying_type_t<T>;
    return !isunordered(x, y) && static_cast<underlying_type>(x) >= static_cast<underlying_type>(y);
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto isless(const T x, const T y) noexcept -> bool
{
    using underlying_type = detail::underlying_type_t<T>;
    return !isunordered(x, y) && static_cast<underlying_type>(x) < static_cast<underlying_type>(y);
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto islessequal(const T x, const T y) noexcept -> bool
{
    using underlying_type = detail::underlying_type_t<T>;
    return !isunordered(x, y) && static_cast<underlying_type>(x) <= static_cast<underlying_type>(y);
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto islessgreater(const T x, const T y) noexcept -> bool
{
    using underlying_type = detail::underlying_type_t<T>;
    const auto a {static_cast<underlying_type>(x)};
    const auto b {static_cast<underlying_type>(y)};
    return !isunordered(x, y) && (a < b || a > b);
}

// ------------------------------
// Integer-returning functions
// ------------------------------
// These return the safe integer type that matches the width of the std return
// type. A non-finite input has an unspecified std result (and raises FE_INVALID),
// so it is reported as a domain error instead. Not constexpr: the underlying std
// functions are not constexpr in C++20/23.

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] auto lround(const T x) -> i64
{
    using underlying_type = detail::underlying_type_t<T>;
    const auto v {static_cast<underlying_type>(x)};
    if (detail::impl::constexpr_isnan(v) || detail::impl::constexpr_isinf(v))
    {
        detail::impl::throw_cmath_domain<underlying_type>("lround");
    }
    return i64{static_cast<std::int64_t>(BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS::lround(v))};
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] auto llround(const T x) -> i64
{
    using underlying_type = detail::underlying_type_t<T>;
    const auto v {static_cast<underlying_type>(x)};
    if (detail::impl::constexpr_isnan(v) || detail::impl::constexpr_isinf(v))
    {
        detail::impl::throw_cmath_domain<underlying_type>("llround");
    }
    return i64{static_cast<std::int64_t>(BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS::llround(v))};
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] auto lrint(const T x) -> i64
{
    using underlying_type = detail::underlying_type_t<T>;
    const auto v {static_cast<underlying_type>(x)};
    if (detail::impl::constexpr_isnan(v) || detail::impl::constexpr_isinf(v))
    {
        detail::impl::throw_cmath_domain<underlying_type>("lrint");
    }
    return i64{static_cast<std::int64_t>(BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS::lrint(v))};
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] auto llrint(const T x) -> i64
{
    using underlying_type = detail::underlying_type_t<T>;
    const auto v {static_cast<underlying_type>(x)};
    if (detail::impl::constexpr_isnan(v) || detail::impl::constexpr_isinf(v))
    {
        detail::impl::throw_cmath_domain<underlying_type>("llrint");
    }
    return i64{static_cast<std::int64_t>(BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS::llrint(v))};
}

template <detail::non_bounded_float_library_type T>
BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] auto ilogb(const T x) -> i32
{
    using underlying_type = detail::underlying_type_t<T>;
    const auto v {static_cast<underlying_type>(x)};
    const auto cls {detail::impl::constexpr_fpclassify(v)};
    if (cls == FP_NAN || cls == FP_INFINITE || cls == FP_ZERO)
    {
        detail::impl::throw_cmath_domain<underlying_type>("ilogb");
    }
    return i32{static_cast<std::int32_t>(BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS::ilogb(v))};
}

#undef BOOST_SAFE_NUMBERS_DETAIL_FLOAT_UNARY
#undef BOOST_SAFE_NUMBERS_DETAIL_FLOAT_BINARY

} // namespace boost::safe_numbers

#undef BOOST_SAFE_NUMBERS_DETAIL_CMATH_NS

#endif // BOOST_SAFE_NUMBERS_CMATH_HPP
// ===== END boost/safe_numbers/cmath.hpp =====
// ===== BEGIN boost/safe_numbers/functional.hpp =====
// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_FUNCTIONAL_HPP
#define BOOST_SAFE_NUMBERS_FUNCTIONAL_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/type_traits.hpp
// ===== BEGIN boost/safe_numbers/detail/int128/hash.hpp =====
// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_HASH_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_HASH_HPP

// [amalgamate] skipped duplicate include of boost/safe_numbers/detail/int128/int128.hpp

#ifndef BOOST_SAFE_NUMBERS_DETAIL_INT128_BUILD_MODULE

#include <cstddef>
#include <cstdint>
#include <functional>

#endif

namespace boost {
namespace int128 {
namespace detail {

// The cast is only useless for 64-bit platforms
// Without we get an implicit conversion warning which is arguably worse
#if defined(__GNUC__) && !defined(__clang__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wuseless-cast"
#endif

// splitmix64 finalizer: mixes all 64 input bits into the result before any narrowing to size_t.
// This is required for correctness on platforms where size_t is 32 bits
inline std::size_t hash_finalize_64(std::uint64_t v) noexcept
{
    v ^= v >> 30;
    v *= UINT64_C(0xbf58476d1ce4e5b9);
    v ^= v >> 27;
    v *= UINT64_C(0x94d049bb133111eb);
    v ^= v >> 31;
    return static_cast<std::size_t>(v);
}

#if defined(__GNUC__) && !defined(__clang__)
#  pragma GCC diagnostic pop
#endif

} // namespace detail
} // namespace int128
} // namespace boost

namespace std {

template <>
struct hash<boost::int128::int128_t>
{
    auto operator()(const boost::int128::int128_t v) const noexcept -> std::size_t
    {
        const std::size_t low_hash {boost::int128::detail::hash_finalize_64(v.low)};
        const std::size_t high_hash {boost::int128::detail::hash_finalize_64(static_cast<std::uint64_t>(v.high))};

        // boost::hash_combine style mixing of the two finalized halves
        return low_hash ^ (high_hash + static_cast<std::size_t>(0x9e3779b9) + (low_hash << 6) + (low_hash >> 2));
    }
};

template <>
struct hash<boost::int128::uint128_t>
{
    auto operator()(const boost::int128::uint128_t v) const noexcept -> std::size_t
    {
        const std::size_t low_hash {boost::int128::detail::hash_finalize_64(v.low)};
        const std::size_t high_hash {boost::int128::detail::hash_finalize_64(v.high)};

        // boost::hash_combine style mixing of the two finalized halves
        return low_hash ^ (high_hash + static_cast<std::size_t>(0x9e3779b9) + (low_hash << 6) + (low_hash >> 2));
    }
};

} // namespace std

#endif // BOOST_SAFE_NUMBERS_DETAIL_INT128_HASH_HPP
// ===== END boost/safe_numbers/detail/int128/hash.hpp =====

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <functional>
#include <cstddef>
#include <cstdint>
#include <type_traits>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

template <boost::safe_numbers::detail::library_type T>
struct std::hash<T>
{
    [[nodiscard]] auto operator()(const T& value) const noexcept -> std::size_t
    {
        using underlying_type = boost::safe_numbers::detail::underlying_type_t<T>;
        const auto raw {static_cast<underlying_type>(value)};

        return std::hash<underlying_type>{}(raw);
    }
};

#endif // BOOST_SAFE_NUMBERS_FUNCTIONAL_HPP
// ===== END boost/safe_numbers/functional.hpp =====

#endif //BOOST_SAFENUMBERS_HPP
// ===== END boost/safe_numbers.hpp =====

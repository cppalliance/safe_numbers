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

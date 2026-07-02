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

#ifdef __GNUC__
#if defined(__OPTIMIZE__) && defined(BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT)
#define BOOST_SAFE_NUMBERS_GCC_COMPILE_ASSERT
#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT_ACTIVE
#endif // defined(__OPTIMIZE__) && defined(BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT)
#endif // __GNU__

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
 * @param message A description of the assertion.
 */
#define BOOST_SAFE_NUMBERS_CA_ASSERT_IMPL(expression, message, fn) \
    do { \
        void fn() __attribute__ ((error(message))); \
        if (!(expression)) { \
            fn(); \
        } \
    } while (0)
#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT(expression, message) \
    BOOST_SAFE_NUMBERS_CA_ASSERT_IMPL(expression, message, BOOST_SAFE_NUMBERS_CA_CAT(_compile_assert_fail_, __COUNTER__))


#define BOOST_SAFE_NUMBERS_CA_CONST_P_IMPL(expression, message, fn) \
    do { \
        if(__builtin_constant_p(expression)) { \
            if (!(expression)) { \
                void fn() __attribute__ ((error(message))); \
                fn(); \
            } \
        } \
    } while (0)
#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(expression, message) \
    BOOST_SAFE_NUMBERS_CA_CONST_P_IMPL(expression, message, BOOST_SAFE_NUMBERS_CA_CAT(_compile_assert_fail_, __COUNTER__))


#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT0(expression) BOOST_SAFE_NUMBERS_COMPILE_ASSERT(expression, NULL)

#else
#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT(condition, description)
#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT0(expression)
#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P(expression, message)
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
#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT_PTR(condition, ptr)
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

// The non-active fallback above already defined these as empty; replace them here.
#undef BOOST_SAFE_NUMBERS_COMPILE_ASSERT
#undef BOOST_SAFE_NUMBERS_COMPILE_ASSERT0
#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT(expr, message) \
do { \
    if (!(expr)) { \
      extern void BOOST_SAFE_NUMBERS_MERGE3(_compile_assert, BOOST_SAFE_NUMBERS_COMPILE_FILE, __LINE__)(); \
      BOOST_SAFE_NUMBERS_MERGE3(_compile_assert, BOOST_SAFE_NUMBERS_COMPILE_FILE, __LINE__)(); \
    } \
} while (0)

#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT0(expression) BOOST_SAFE_NUMBERS_COMPILE_ASSERT(expression, NULL)
#else

#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT(condition, description)
#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT0(expression)
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

#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT(expression, message) \
    do { \
        void _compile_assert_fail(); \
        if (!(expression)) { \
            _compile_assert_fail(); \
        } \
    } while (0)

#define BOOST_SAFE_NUMBERS_COMPILE_ASSERT0(expression) BOOST_SAFE_NUMBERS_COMPILE_ASSERT(expression, NULL)
#endif // !defined(compile_assert)
#endif // defined(BOOST_SAFE_NUMBERS_ENABLE_COMPILE_ASSERT)


#ifndef BOOST_SAFE_NUMBERS_COMPILE_ASSERT
#error "BOOST_SAFE_NUMBERS_COMPILE_ASSERT not defined"
#endif

#ifndef BOOST_SAFE_NUMBERS_COMPILE_ASSERT0
#error "BOOST_SAFE_NUMBERS_COMPILE_ASSERT0 not defined"
#endif

#ifndef BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P
#error "BOOST_SAFE_NUMBERS_COMPILE_ASSERT_CONST_P not defined"
#endif

#endif // BOOST_SAFE_NUMBERS_DETAIL_COMPILE_ASSERT_HPP

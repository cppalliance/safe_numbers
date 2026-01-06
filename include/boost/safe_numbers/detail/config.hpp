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

#ifdef __x86_64__

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

#endif // BOOST_SAFENUMBERS_CONFIG_HPP

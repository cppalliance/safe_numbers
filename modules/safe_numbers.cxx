// Copyright 2024 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Global module fragment required for non-module preprocessing
module;

#include <boost/throw_exception.hpp>
#include <boost/config.hpp>
#include <boost/assert.hpp>

#include <limits>
#include <concepts>
#include <type_traits>
#include <stdexcept>
#include <format>

#include <cstdint>

#if defined(_MSC_VER)
#  include <intrin.h>
#elif defined(__x86_64__)
#  include <x86intrin.h>
#elif defined(__ARM_NEON__)
#  include <arm_neon.h>
#endif

#define BOOST_SAFE_NUMBERS_BUILD_MODULE

export module boost.safe_numbers;

// MSVC wants <boost/safe_numbers.hpp> to be imported but also does not support importing it...
#ifdef _MSC_VER
#  pragma warning( push )
#  pragma warning( disable : 5244 )
#elif defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Winclude-angled-in-module-purview"
#endif

#include <boost/safe_numbers.hpp>

#ifdef _MSC_VER
#  pragma warning( pop )
#elif defined(__clang__)
#  pragma clang diagnostic pop
#endif

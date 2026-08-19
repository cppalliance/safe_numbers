// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_TEST_CVE_CORPUS_UTIL_HPP
#define BOOST_SAFE_NUMBERS_TEST_CVE_CORPUS_UTIL_HPP

#include <cstdint>

// Ground truth for the native-reproduction arm, computed in a wider type so the
// differential assertion never itself overflows and never trips -Wconversion or
// -Wsign-conversion. These are NOT the code under test; they only establish the
// mathematically correct value that the wrapped native result is compared against.
// Files whose root cause width is u64 compute their ground truth inline with a
// 128-bit type (reachable through <boost/safe_numbers.hpp>), since this header
// deliberately depends only on <cstdint>.
namespace cve_corpus {

// The true 64-bit product of two 32-bit values (widening is value-preserving).
[[nodiscard]] constexpr std::uint64_t true_mul(std::uint32_t a, std::uint32_t b) noexcept
{
    return static_cast<std::uint64_t>(a) * static_cast<std::uint64_t>(b);
}

// The true 64-bit sum of two 32-bit values.
[[nodiscard]] constexpr std::uint64_t true_add(std::uint32_t a, std::uint32_t b) noexcept
{
    return static_cast<std::uint64_t>(a) + static_cast<std::uint64_t>(b);
}

// The true signed 64-bit difference of two 32-bit signed values.
[[nodiscard]] constexpr std::int64_t true_sub(std::int32_t a, std::int32_t b) noexcept
{
    return static_cast<std::int64_t>(a) - static_cast<std::int64_t>(b);
}

} // namespace cve_corpus

#endif // BOOST_SAFE_NUMBERS_TEST_CVE_CORPUS_UTIL_HPP

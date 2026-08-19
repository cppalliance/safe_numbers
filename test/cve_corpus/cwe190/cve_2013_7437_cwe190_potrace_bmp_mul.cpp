// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2013-7437
// cwe:                CWE-190
// product:            potrace
// version:            1.11
// summary:            Integer overflow from large BMP dimensions triggers a buffer overflow.
// root-cause:         unsigned multiplication (width * height) of BMP image dimensions
// root-cause-width:   u32
// trigger:            width=100000, height=100000 (product 10000000000 exceeds UINT32_MAX)
// consequence:        undersized bitmap allocation then buffer overflow
// classification:     PREVENTED_RUNTIME
// expected-exception: std::overflow_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2013-7437
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-190&resultsPerPage=200&startIndex=0
// notes:              The BMP reader computes a pixel count from width * height at u32 width.
// notes:              Modeled with representative dimensions whose product wraps below the true size.
// BOOST_SAFE_NUMBERS_CVE_END
// ===========================================================================

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/core/lightweight_test.hpp>

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE
import boost.safe_numbers;
#else
#include <boost/safe_numbers.hpp>
#endif

#include <cstdint>
#include <stdexcept>
#include "../cve_corpus_util.hpp"

using namespace boost::safe_numbers;

void native_reproduction()
{
    const std::uint32_t width  {100000U};
    const std::uint32_t height {100000U};

    const std::uint64_t true_pixels {cve_corpus::true_mul(width, height)}; // 10000000000
    const std::uint32_t pixels {static_cast<std::uint32_t>(width * height)}; // wraps to 1410065408

    BOOST_TEST(true_pixels > static_cast<std::uint64_t>(width));
    BOOST_TEST_EQ(pixels, 1410065408U);                                 // deterministic wrap
    BOOST_TEST(static_cast<std::uint64_t>(pixels) < true_pixels);       // undersized
}

void safe_reproduction()
{
    const u32 width  {100000U};
    const u32 height {100000U};

    BOOST_TEST_THROWS((void)(width * height), std::overflow_error);
    BOOST_TEST(!checked_mul(width, height).has_value());
}

void bounded_reproduction()
{
    // Declaring the input with its documented maximum rejects the oversized
    // value at construction, before the operation can misbehave.
    using bounded_input = bounded_uint<0u, 1000000u>;
    std::uint32_t attacker_input {2000000000U};
    BOOST_TEST_THROWS((bounded_input{u32{attacker_input}}), std::domain_error);
    BOOST_TEST_NO_THROW((void)bounded_input{u32{1000U}});
}

int main()
{
    native_reproduction();
    safe_reproduction();
    bounded_reproduction();
    return boost::report_errors();
}

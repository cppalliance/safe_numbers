// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2006-4519
// cwe:                CWE-190
// product:            GIMP image loader plugins (DICOM)
// version:            before 2.2.16
// summary:            Integer overflow from crafted DICOM length values.
// root-cause:         unsigned multiplication (width * height) of image dimensions
// root-cause-width:   u32
// trigger:            width=80000, height=80000
// consequence:        undersized allocation then code execution
// classification:     PREVENTED_RUNTIME
// expected-exception: std::overflow_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2006-4519
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-190&resultsPerPage=200&startIndex=0
// notes:              The DICOM loader multiplies attacker controlled dimensions at u32 width.
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
    const std::uint32_t width {80000U};
    const std::uint32_t height {80000U};

    const std::uint64_t true_pixels {cve_corpus::true_mul(width, height)};
    const std::uint32_t pixels {static_cast<std::uint32_t>(width * height)};

    BOOST_TEST(true_pixels > static_cast<std::uint64_t>(width));
    BOOST_TEST_EQ(pixels, 2105032704U);
    BOOST_TEST(static_cast<std::uint64_t>(pixels) < true_pixels);
}

void safe_reproduction()
{
    const u32 width {80000U};
    const u32 height {80000U};

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

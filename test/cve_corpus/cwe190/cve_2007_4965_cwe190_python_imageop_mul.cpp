// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2007-4965
// cwe:                CWE-190
// product:            Python (imageop module)
// version:            Python 2.5.1 and earlier
// summary:            Integer overflow in Python (imageop module).
// root-cause:         unsigned multiplication of two values
// root-cause-width:   u32
// trigger:            width=90000, height=90000
// consequence:        undersized allocation then memory corruption
// classification:     PREVENTED_RUNTIME
// expected-exception: std::overflow_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2007-4965
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-190&resultsPerPage=200&startIndex=0
// notes:              Image dimensions overflow the pixel count in imageop.
// notes:              Modeled at the canonical multiplication overflow pattern for this CWE at the disclosed width.
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
    const std::uint32_t width {90000U};
    const std::uint32_t height {90000U};

    const std::uint64_t true_pixels {cve_corpus::true_mul(width, height)};
    const std::uint32_t pixels {static_cast<std::uint32_t>(width * height)};

    BOOST_TEST(true_pixels > static_cast<std::uint64_t>(width));
    BOOST_TEST_EQ(pixels, 3805032704U);
    BOOST_TEST(static_cast<std::uint64_t>(pixels) < true_pixels);
}

void safe_reproduction()
{
    const u32 width {90000U};
    const u32 height {90000U};

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

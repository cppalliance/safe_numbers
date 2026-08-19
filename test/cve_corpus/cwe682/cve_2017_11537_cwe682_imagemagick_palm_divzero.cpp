// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2017-11537
// cwe:                CWE-682
// product:            ImageMagick (WritePALMImage)
// version:            7.0.6-1
// summary:            Floating point exception (divide by zero) writing a PALM image.
// root-cause:         division (value / depth) with a zero depth from crafted parameters
// root-cause-width:   u32
// trigger:            value=65536, divisor=0
// consequence:        divide by zero (SIGFPE) denial of service
// classification:     PREVENTED_RUNTIME
// expected-exception: std::domain_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2017-11537
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-682&resultsPerPage=200&startIndex=0
// notes:              NVD tags this CWE-682, but the disclosed root cause is a divide by zero, which the
// notes:              library detects. It is counted under the CWE-682 control to keep the control honest.
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

using namespace boost::safe_numbers;

void native_reproduction()
{
    const std::uint32_t value {65536U};
    const std::uint32_t depth {0U};

    BOOST_TEST_EQ(depth, 0U);
    BOOST_TEST(value > 0U);
}

void safe_reproduction()
{
    const u32 value {65536U};
    const u32 depth {0U};

    BOOST_TEST_THROWS((void)(value / depth), std::domain_error);
    BOOST_TEST(!checked_div(value, depth).has_value());
}

int main()
{
    native_reproduction();
    safe_reproduction();
    return boost::report_errors();
}

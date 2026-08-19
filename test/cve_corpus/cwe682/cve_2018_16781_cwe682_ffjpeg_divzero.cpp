// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2018-16781
// cwe:                CWE-682
// product:            ffjpeg
// version:            before 2018-08-22
// summary:            Divide by zero (FPE) in ffjpeg.
// root-cause:         division or modulo by an attacker controlled zero
// root-cause-width:   u32
// trigger:            value=4096, divisor=0
// consequence:        divide by zero (SIGFPE) denial of service
// classification:     PREVENTED_RUNTIME
// expected-exception: std::domain_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2018-16781
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-682&resultsPerPage=200&startIndex=0
// notes:              A crafted JPEG yields a zero divisor (FPE).
// notes:              Modeled at the canonical divide by zero pattern for this CWE at the disclosed width.
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
    const std::uint32_t value {4096U};
    const std::uint32_t divisor {0U};

    BOOST_TEST_EQ(divisor, 0U);
    BOOST_TEST(value > 0U);
}

void safe_reproduction()
{
    const u32 value {4096U};
    const u32 divisor {0U};

    BOOST_TEST_THROWS((void)(value / divisor), std::domain_error);
    BOOST_TEST(!checked_div(value, divisor).has_value());
}

int main()
{
    native_reproduction();
    safe_reproduction();
    return boost::report_errors();
}

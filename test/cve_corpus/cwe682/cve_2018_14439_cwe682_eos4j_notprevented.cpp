// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2018-14439
// cwe:                CWE-682
// product:            espritblock eos4j (EOS SDK)
// version:            through 2018-07-12
// summary:            Mishandled numeric calculation in eos4j.
// root-cause:         integer division truncates; the result is in range and well defined
// root-cause-width:   u32
// trigger:            numerator=9, divisor=4
// consequence:        undersized result that the library cannot detect
// classification:     NOT_PREVENTED
// expected-exception: none
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2018-14439
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-682&resultsPerPage=200&startIndex=0
// notes:              In-range calculation error the library cannot detect.
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

using namespace boost::safe_numbers;

void native_reproduction()
{
    const std::uint32_t numerator {9U};
    const std::uint32_t divisor {4U};
    const std::uint32_t quotient {numerator / divisor};
    const std::uint32_t needed {(numerator + divisor - 1U) / divisor};

    BOOST_TEST_EQ(quotient, 2U);
    BOOST_TEST(quotient < needed);
}

void safe_reproduction()
{
    const u32 numerator {9U};
    const u32 divisor {4U};

    BOOST_TEST_NO_THROW((void)(numerator / divisor));
    const u32 quotient {numerator / divisor};
    BOOST_TEST_EQ(static_cast<std::uint32_t>(quotient), 2U);
}

int main()
{
    native_reproduction();
    safe_reproduction();
    return boost::report_errors();
}

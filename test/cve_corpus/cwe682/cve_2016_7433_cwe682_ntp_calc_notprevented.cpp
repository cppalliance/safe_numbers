// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2016-7433
// cwe:                CWE-682
// product:            NTP
// version:            before 4.2.8p9
// summary:            Incorrect initial sync calculation allows an attacker to influence the clock.
// root-cause:         a value is combined with the wrong operand; the arithmetic stays in range
// root-cause-width:   u32
// trigger:            sum=7, count=2 with truncating integer division
// consequence:        a wrong but in range result that skews the computed offset
// classification:     NOT_PREVENTED
// expected-exception: none
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2016-7433
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-682&resultsPerPage=200&startIndex=0
// notes:              Honest limitation. Integer division is well defined and in range, so no fault is
// notes:              raised; the defect is the formula (truncation where a rounded value was intended).
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
    const std::uint32_t sum   {7U};
    const std::uint32_t count {2U};

    const std::uint32_t average    {sum / count};        // 3, truncated (the bug)
    const std::uint32_t rounded    {(sum + count / 2U) / count}; // 4, the intended value

    BOOST_TEST_EQ(average, 3U);
    BOOST_TEST(average != rounded);                       // wrong but in range
}

// safe_numbers does not catch this: division is representable, so no fault, and the safe
// result equals the native truncated value.
void safe_reproduction()
{
    const u32 sum   {7U};
    const u32 count {2U};

    BOOST_TEST_NO_THROW((void)(sum / count));
    const u32 average {sum / count};
    BOOST_TEST_EQ(static_cast<std::uint32_t>(average), 3U); // identical truncated value, not caught
}

int main()
{
    native_reproduction();
    safe_reproduction();
    return boost::report_errors();
}

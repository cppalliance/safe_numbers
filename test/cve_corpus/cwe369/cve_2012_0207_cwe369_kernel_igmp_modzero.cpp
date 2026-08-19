// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2012-0207
// cwe:                CWE-369
// product:            Linux kernel IPv4 IGMP (igmp_heard_query)
// version:            before 3.2.1
// summary:            Divide by zero in IGMP query handling allows remote denial of service.
// root-cause:         modulo (value % divisor) where a crafted query yields a zero divisor
// root-cause-width:   u32
// trigger:            value=1000, divisor=0
// consequence:        divide by zero (SIGFPE) kernel denial of service
// classification:     PREVENTED_RUNTIME
// expected-exception: std::domain_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2012-0207
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-369&resultsPerPage=200&startIndex=0
// notes:              A crafted IGMPv3 query drives a zero divisor into a modulo. Modeled at u32.
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
    const std::uint32_t value   {1000U};
    const std::uint32_t divisor {0U};                // attacker controlled

    BOOST_TEST_EQ(divisor, 0U);
    BOOST_TEST(value > 0U);
}

void safe_reproduction()
{
    const u32 value   {1000U};
    const u32 divisor {0U};

    BOOST_TEST_THROWS((void)(value % divisor), std::domain_error);
    BOOST_TEST(!checked_mod(value, divisor).has_value());
}

void bounded_reproduction()
{
    // A divisor required to be non zero is declared with a minimum of one; the
    // zero divisor is rejected before the division can trap.
    using nonzero_divisor = bounded_uint<1u, 1000000000u>;
    std::uint32_t attacker_divisor {0U};
    BOOST_TEST_THROWS((nonzero_divisor{u32{attacker_divisor}}), std::domain_error);
    BOOST_TEST_NO_THROW((void)nonzero_divisor{u32{7U}});
}

int main()
{
    native_reproduction();
    safe_reproduction();
    bounded_reproduction();
    return boost::report_errors();
}

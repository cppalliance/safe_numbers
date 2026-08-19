// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2016-9112
// cwe:                CWE-369
// product:            OpenJPEG (opj_pi_next_cprl in openjp2/pi.c)
// version:            2.1.2
// summary:            Floating point exception (divide by zero) in progression order iteration.
// root-cause:         division by a zero step derived from crafted component parameters
// root-cause-width:   u32
// trigger:            numerator=65536, step=0
// consequence:        divide by zero (SIGFPE) denial of service
// classification:     PREVENTED_RUNTIME
// expected-exception: std::domain_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2016-9112
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-369&resultsPerPage=200&startIndex=0
// notes:              A crafted component yields a zero step used as a divisor in pi.c. Modeled at u32.
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
    const std::uint32_t numerator {65536U};
    const std::uint32_t step      {0U};              // attacker controlled

    BOOST_TEST_EQ(step, 0U);
    BOOST_TEST(numerator > 0U);
}

void safe_reproduction()
{
    const u32 numerator {65536U};
    const u32 step      {0U};

    BOOST_TEST_THROWS((void)(numerator / step), std::domain_error);
    BOOST_TEST(!checked_div(numerator, step).has_value());
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

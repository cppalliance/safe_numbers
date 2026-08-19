// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2011-1012
// cwe:                CWE-369
// product:            Linux kernel LDM (ldm_parse_vmdb)
// version:            before 2.6.38-rc6-git6
// summary:            Divide by zero from an unvalidated VBLK size in an LDM partition.
// root-cause:         modulo (offset % vblk_size) with a zero vblk_size
// root-cause-width:   u32
// trigger:            offset=512, divisor=0
// consequence:        divide by zero (SIGFPE) kernel crash
// classification:     PREVENTED_RUNTIME
// expected-exception: std::domain_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2011-1012
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-369&resultsPerPage=200&startIndex=0
// notes:              ldm_parse_vmdb does not validate the VBLK size before using it as a divisor.
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
    const std::uint32_t offset {512U};
    const std::uint32_t vblk_size {0U};

    BOOST_TEST_EQ(vblk_size, 0U);
    BOOST_TEST(offset > 0U);
}

void safe_reproduction()
{
    const u32 offset {512U};
    const u32 vblk_size {0U};

    BOOST_TEST_THROWS((void)(offset % vblk_size), std::domain_error);
    BOOST_TEST(!checked_mod(offset, vblk_size).has_value());
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

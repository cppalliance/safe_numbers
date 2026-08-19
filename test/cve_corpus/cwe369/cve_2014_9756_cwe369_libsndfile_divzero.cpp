// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2014-9756
// cwe:                CWE-369
// product:            libsndfile (psf_fwrite)
// version:            through 1.0.25
// summary:            Divide by zero in psf_fwrite from a zero item size crashes the application.
// root-cause:         division (bytes_to_write / item_size) with an attacker controlled zero item size
// root-cause-width:   u32
// trigger:            bytes_to_write=4096, item_size=0
// consequence:        divide by zero (SIGFPE) denial of service
// classification:     PREVENTED_RUNTIME
// expected-exception: std::domain_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2014-9756
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-369&resultsPerPage=200&startIndex=0
// notes:              Native divides without guarding the zero divisor, which traps. The native arm
// notes:              documents the trigger; it does not perform the trapping division.
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

// Native code divides bytes_to_write by item_size with no guard. The division is
// omitted here because dividing by zero is undefined and would trap; the trigger
// condition is asserted instead.
void native_reproduction()
{
    const std::uint32_t bytes_to_write {4096U};
    const std::uint32_t item_size      {0U};         // attacker controlled

    BOOST_TEST_EQ(item_size, 0U);                    // the unguarded divisor is zero
    BOOST_TEST(bytes_to_write > 0U);
}

// The identical division in safe types throws instead of trapping.
void safe_reproduction()
{
    const u32 bytes_to_write {4096U};
    const u32 item_size      {0U};

    BOOST_TEST_THROWS((void)(bytes_to_write / item_size), std::domain_error);
    BOOST_TEST(!checked_div(bytes_to_write, item_size).has_value());
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

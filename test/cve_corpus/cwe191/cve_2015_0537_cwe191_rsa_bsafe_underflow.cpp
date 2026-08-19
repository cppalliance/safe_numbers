// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2015-0537
// cwe:                CWE-191
// product:            EMC RSA BSAFE (base64 decode)
// version:            Micro Edition Suite
// summary:            Integer underflow in EMC RSA BSAFE (base64 decode).
// root-cause:         unsigned subtraction with the minuend too small
// root-cause-width:   u32
// trigger:            available=2, requested=5
// consequence:        wrapped length then out of bounds access
// classification:     PREVENTED_RUNTIME
// expected-exception: std::underflow_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2015-0537
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-191&resultsPerPage=200&startIndex=0
// notes:              Unsigned length subtraction wraps below zero.
// notes:              Modeled at the canonical subtraction underflow pattern for this CWE at the disclosed width.
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
    const std::uint32_t available {2U};
    const std::uint32_t requested {5U};
    const std::uint32_t remaining {available - requested};

    BOOST_TEST_EQ(remaining, 4294967293U);
    BOOST_TEST(remaining > available);
}

void safe_reproduction()
{
    const u32 available {2U};
    const u32 requested {5U};

    BOOST_TEST_THROWS((void)(available - requested), std::underflow_error);
    BOOST_TEST(!checked_sub(available, requested).has_value());
}

void bounded_reproduction()
{
    // A length required to be at least a header size is declared with a minimum
    // bound; a too small length is rejected before the subtraction underflows.
    using bounded_length = bounded_uint<8u, 1000000u>;
    std::uint32_t attacker_length {4U};
    BOOST_TEST_THROWS((bounded_length{u32{attacker_length}}), std::domain_error);
    BOOST_TEST_NO_THROW((void)bounded_length{u32{64U}});
}

int main()
{
    native_reproduction();
    safe_reproduction();
    bounded_reproduction();
    return boost::report_errors();
}
